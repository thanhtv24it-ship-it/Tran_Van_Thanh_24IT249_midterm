#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ls.h"
#include "print.h"

/* Global pointer used by the qsort comparison wrapper.
 * Simple and sufficient for a single-threaded program. */
static const options_t *g_opts = NULL;

int compare_entries(const void *a, const void *b, const options_t *opts)
{
    const entry_t *ea = (const entry_t *)a;
    const entry_t *eb = (const entry_t *)b;
    int cmp = 0;

    if (opts->S) {
        /* largest first */
        if (ea->st.st_size < eb->st.st_size) cmp = 1;
        else if (ea->st.st_size > eb->st.st_size) cmp = -1;
        else cmp = strcmp(ea->name, eb->name);
    } else if (opts->t) {
        time_t ta, tb;
        if (opts->c) {
            ta = ea->st.st_ctime;
            tb = eb->st.st_ctime;
        } else if (opts->u) {
            ta = ea->st.st_atime;
            tb = eb->st.st_atime;
        } else {
            ta = ea->st.st_mtime;
            tb = eb->st.st_mtime;
        }
        /* most recent first */
        if (ta < tb) cmp = 1;
        else if (ta > tb) cmp = -1;
        else cmp = strcmp(ea->name, eb->name);
    } else {
        cmp = strcmp(ea->name, eb->name);
    }

    if (opts->r)
        cmp = -cmp;
    return cmp;
}

static int qsort_cmp(const void *a, const void *b)
{
    return compare_entries(a, b, g_opts);
}

void sort_entries(entry_t *entries, size_t n, const options_t *opts)
{
    if (opts->f || n < 2)
        return;
    g_opts = opts;
    qsort(entries, n, sizeof(entry_t), qsort_cmp);
}

/* Collect directory entries into a dynamically grown array */
static entry_t *collect_entries(const char *dirpath, const options_t *opts,
                                size_t *out_count, blkcnt_t *out_total)
{
    DIR *dir;
    struct dirent *de;
    entry_t *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    blkcnt_t total = 0;

    dir = opendir(dirpath);
    if (dir == NULL) {
        fprintf(stderr, "ls: cannot open directory '%s': %s\n",
                dirpath, strerror(errno));
        *out_count = 0;
        *out_total = 0;
        return NULL;
    }

    while ((de = readdir(dir)) != NULL) {
        const char *name = de->d_name;

        /* Filtering according to -a / -A */
        if (!opts->a && !opts->A) {
            if (is_dotfile(name))
                continue;
        } else if (opts->A && !opts->a) {
            if (is_dot_or_dotdot(name))
                continue;
        }
        /* if -a is set, show everything including . and .. */

        if (count >= capacity) {
            size_t newcap = (capacity == 0) ? 32 : capacity * 2;
            entry_t *tmp = realloc(list, newcap * sizeof(entry_t));
            if (tmp == NULL) {
                fprintf(stderr, "ls: out of memory\n");
                free(list);
                closedir(dir);
                *out_count = 0;
                *out_total = 0;
                return NULL;
            }
            list = tmp;
            capacity = newcap;
        }

        entry_t *e = &list[count];
        snprintf(e->name, sizeof(e->name), "%s", name);
        path_join(e->fullpath, sizeof(e->fullpath), dirpath, name);

        if (lstat(e->fullpath, &e->st) < 0) {
            fprintf(stderr, "ls: cannot access '%s': %s\n",
                    e->fullpath, strerror(errno));
            continue;   /* skip this entry */
        }

        e->is_dir = S_ISDIR(e->st.st_mode);
        total += e->st.st_blocks;
        count++;
    }

    closedir(dir);
    *out_count = count;
    *out_total = total;
    return list;
}

/* Forward declaration for recursion */
static void list_directory(const char *path, const options_t *opts, bool print_header);

void list_path(const char *path, const options_t *opts, bool is_top_level)
{
    struct stat st;
    entry_t single;

    if (lstat(path, &st) < 0) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", path, strerror(errno));
        return;
    }

    /* If -d or not a directory, treat as a single file entry */
    if (opts->d || !S_ISDIR(st.st_mode)) {
        memset(&single, 0, sizeof(single));
        /* Use the original path as the display name when it is an argument */
        snprintf(single.name, sizeof(single.name), "%s", path);
        snprintf(single.fullpath, sizeof(single.fullpath), "%s", path);
        single.st = st;
        single.is_dir = S_ISDIR(st.st_mode);

        if (opts->l || opts->n || opts->s || opts->i)
            print_entry(&single, opts);
        else {
            print_name(&single, opts);
            putchar('\n');
        }
        return;
    }

    /* Directory: list its contents */
    list_directory(path, opts, !is_top_level || opts->R);
}

static void list_directory(const char *path, const options_t *opts, bool print_header)
{
    size_t count = 0;
    blkcnt_t total = 0;
    entry_t *entries = collect_entries(path, opts, &count, &total);

    if (entries == NULL && count == 0)
        return;

    if (print_header)
        printf("\n%s:\n", path);

    if (opts->l || opts->n)
        print_total(total, opts);

    sort_entries(entries, count, opts);

    for (size_t i = 0; i < count; i++)
        print_entry(&entries[i], opts);

    /* Recursion */
    if (opts->R && !opts->d) {
        for (size_t i = 0; i < count; i++) {
            if (entries[i].is_dir && !is_dot_or_dotdot(entries[i].name)) {
                list_directory(entries[i].fullpath, opts, true);
            }
        }
    }

    free(entries);
}
