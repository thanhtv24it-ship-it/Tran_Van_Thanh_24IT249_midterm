#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysmacros.h>   /* major / minor on Linux */
#include "print.h"


void print_total(blkcnt_t total_blocks, const options_t *opts)
{
    char buf[32];
    format_blocks(total_blocks, opts, buf, sizeof(buf));
    printf("total %s\n", buf);
}

void print_name(const entry_t *e, const options_t *opts)
{
    char sname[MAX_PATH];
    sanitize_name(e->name, sname, sizeof(sname), opts);
    fputs(sname, stdout);

    if (opts->F) {
        char ind = get_indicator(&e->st);
        if (ind != '\0')
            putchar(ind);
    }
}

void print_entry(const entry_t *e, const options_t *opts)
{
    /* inode */
    if (opts->i)
        printf("%llu ", (unsigned long long)e->st.st_ino);

    /* blocks */
    if (opts->s) {
        char bbuf[32];
        format_blocks(e->st.st_blocks, opts, bbuf, sizeof(bbuf));
        printf("%s ", bbuf);
    }

    if (opts->l || opts->n) {
        char mode[12];
        char owner[64];
        char group[64];
        char sizebuf[32];
        char timebuf[32];
        struct tm *tm;
        time_t when;

        format_mode(e->st.st_mode, mode);

        get_owner(e->st.st_uid, opts->n, owner, sizeof(owner));
        get_group(e->st.st_gid, opts->n, group, sizeof(group));

        /* size or major/minor for devices */
        if (S_ISCHR(e->st.st_mode) || S_ISBLK(e->st.st_mode)) {
            snprintf(sizebuf, sizeof(sizebuf), "%u, %u",
                     major(e->st.st_rdev), minor(e->st.st_rdev));
        } else if (opts->h) {
            humanize_size(e->st.st_size, sizebuf, sizeof(sizebuf));
        } else {
            snprintf(sizebuf, sizeof(sizebuf), "%lld",
                     (long long)e->st.st_size);
        }

        /* choose time field */
        if (opts->c)
            when = e->st.st_ctime;
        else if (opts->u)
            when = e->st.st_atime;
        else
            when = e->st.st_mtime;

        tm = localtime(&when);
        if (tm == NULL) {
            snprintf(timebuf, sizeof(timebuf), "??? ?? ??:??");
        } else {
            /* Classic ls style: "Mon DD HH:MM" if recent, else "Mon DD  YYYY" */
            time_t now = time(NULL);
            if (now - when < 15552000 && when <= now) { /* ~6 months */
                strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", tm);
            } else {
                strftime(timebuf, sizeof(timebuf), "%b %e  %Y", tm);
            }
        }

        printf("%s %3lu %-8s %-8s %8s %s ",
               mode,
               (unsigned long)e->st.st_nlink,
               owner,
               group,
               sizebuf,
               timebuf);

        print_name(e, opts);

        /* symlink target */
        if (S_ISLNK(e->st.st_mode)) {
            char target[MAX_PATH];
            ssize_t len = readlink(e->fullpath, target, sizeof(target) - 1);
            if (len >= 0) {
                target[len] = '\0';
                printf(" -> %s", target);
            }
        }
        putchar('\n');
    } else {
        /* short format */
        print_name(e, opts);
        putchar('\n');
    }
}
