#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include "util.h"

void path_join(char *dest, size_t destsize, const char *dir, const char *name)
{
    size_t len = strlen(dir);
    if (len == 0) {
        snprintf(dest, destsize, "%s", name);
        return;
    }
    /* Avoid double slash */
    if (dir[len - 1] == '/')
        snprintf(dest, destsize, "%s%s", dir, name);
    else
        snprintf(dest, destsize, "%s/%s", dir, name);
}

bool is_dotfile(const char *name)
{
    return name[0] == '.';
}

bool is_dot_or_dotdot(const char *name)
{
    return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0);
}

void humanize_size(off_t size, char *buf, size_t bufsize)
{
    const char *units[] = {"B", "K", "M", "G", "T", "P"};
    double val = (double)size;
    int u = 0;

    while (val >= 1024.0 && u < 5) {
        val /= 1024.0;
        u++;
    }

    if (u == 0)
        snprintf(buf, bufsize, "%lld", (long long)size);
    else if (val < 10.0)
        snprintf(buf, bufsize, "%.1f%s", val, units[u]);
    else
        snprintf(buf, bufsize, "%.0f%s", val, units[u]);
}

void format_blocks(blkcnt_t blocks, const options_t *opts, char *buf, size_t bufsize)
{
    /* On Linux st_blocks is in 512-byte units (same as traditional UNIX).
     * -k forces 1K units, -h humanizes. */
    long long b = (long long)blocks;

    if (opts->h) {
        /* Convert 512-byte blocks to bytes then humanize */
        humanize_size(b * 512, buf, bufsize);
        return;
    }

    if (opts->k) {
        /* Round up to next kilobyte */
        b = (b + 1) / 2;   /* 512 -> 1024 */
        snprintf(buf, bufsize, "%lld", b);
        return;
    }

    /* Default: 512-byte blocks (or BLOCKSIZE if set, but we keep simple) */
    const char *bs = getenv("BLOCKSIZE");
    if (bs != NULL) {
        long blocksize = strtol(bs, NULL, 10);
        if (blocksize > 0) {
            /* Convert from 512-byte units to user blocksize */
            long long bytes = b * 512;
            long long units = (bytes + blocksize - 1) / blocksize;
            snprintf(buf, bufsize, "%lld", units);
            return;
        }
    }

    snprintf(buf, bufsize, "%lld", b);
}

void get_owner(uid_t uid, bool numeric, char *buf, size_t bufsize)
{
    if (!numeric) {
        struct passwd *pw = getpwuid(uid);
        if (pw != NULL) {
            snprintf(buf, bufsize, "%s", pw->pw_name);
            return;
        }
    }
    snprintf(buf, bufsize, "%u", (unsigned)uid);
}

void get_group(gid_t gid, bool numeric, char *buf, size_t bufsize)
{
    if (!numeric) {
        struct group *gr = getgrgid(gid);
        if (gr != NULL) {
            snprintf(buf, bufsize, "%s", gr->gr_name);
            return;
        }
    }
    snprintf(buf, bufsize, "%u", (unsigned)gid);
}

void format_mode(mode_t mode, char *buf)
{
    /* Entry type */
    if (S_ISREG(mode))       buf[0] = '-';
    else if (S_ISDIR(mode))  buf[0] = 'd';
    else if (S_ISLNK(mode))  buf[0] = 'l';
    else if (S_ISCHR(mode))  buf[0] = 'c';
    else if (S_ISBLK(mode))  buf[0] = 'b';
    else if (S_ISFIFO(mode)) buf[0] = 'p';
    else if (S_ISSOCK(mode)) buf[0] = 's';
    else                     buf[0] = '?';

    /* Owner */
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID)
        buf[3] = (mode & S_IXUSR) ? 's' : 'S';
    else
        buf[3] = (mode & S_IXUSR) ? 'x' : '-';

    /* Group */
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID)
        buf[6] = (mode & S_IXGRP) ? 's' : 'S';
    else
        buf[6] = (mode & S_IXGRP) ? 'x' : '-';

    /* Other */
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX)
        buf[9] = (mode & S_IXOTH) ? 't' : 'T';
    else
        buf[9] = (mode & S_IXOTH) ? 'x' : '-';

    buf[10] = '\0';
}

char get_indicator(const struct stat *st)
{
    mode_t m = st->st_mode;
    if (S_ISDIR(m))  return '/';
    if (S_ISLNK(m))  return '@';
    if (S_ISFIFO(m)) return '|';
    if (S_ISSOCK(m)) return '=';
    /* executable? */
    if ((m & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0)
        return '*';
    return '\0';
}

void sanitize_name(const char *src, char *dest, size_t destsize, const options_t *opts)
{
    size_t i, j = 0;
    if (opts->w) {
        /* raw: copy as-is (truncate if needed) */
        snprintf(dest, destsize, "%s", src);
        return;
    }

    /* -q or default terminal: replace non-printable with '?' */
    for (i = 0; src[i] != '\0' && j + 1 < destsize; i++) {
        unsigned char c = (unsigned char)src[i];
        if (isprint(c))
            dest[j++] = (char)c;
        else
            dest[j++] = '?';
    }
    dest[j] = '\0';
}
