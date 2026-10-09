#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

/* Flags corresponding to ls(1) options from the NetBSD man page */
typedef struct {
    bool A;   /* -A: all except . and .. */
    bool a;   /* -a: include dot files */
    bool c;   /* -c: use ctime */
    bool d;   /* -d: directories as plain files */
    bool F;   /* -F: append indicators */
    bool f;   /* -f: no sorting */
    bool h;   /* -h: human-readable sizes */
    bool i;   /* -i: print inode */
    bool k;   /* -k: sizes in kilobytes */
    bool l;   /* -l: long format */
    bool n;   /* -n: numeric uid/gid */
    bool q;   /* -q: non-printable as ? */
    bool R;   /* -R: recursive */
    bool r;   /* -r: reverse order */
    bool S;   /* -S: sort by size */
    bool s;   /* -s: show block count */
    bool t;   /* -t: sort by time */
    bool u;   /* -u: use atime */
    bool w;   /* -w: raw non-printable */
} options_t;

/* Parse command-line arguments. Returns number of non-option arguments
 * (file/directory operands). Sets *files to point to the first operand. */
int parse_options(int argc, char *argv[], options_t *opts, char ***files);

/* Print a short usage message */
void print_usage(const char *progname);

#endif /* OPTIONS_H */
