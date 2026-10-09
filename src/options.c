#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "options.h"

void print_usage(const char *progname)
{
    fprintf(stderr,
            "usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n",
            progname);
}

int parse_options(int argc, char *argv[], options_t *opts, char ***files)
{
    int opt;

    memset(opts, 0, sizeof(*opts));

    /* Default: -q when output is a terminal (we approximate by isatty) */
    if (isatty(STDOUT_FILENO))
        opts->q = true;
    else
        opts->w = true;

    /* Note: getopt stops at first non-option; we collect remaining argv */
    while ((opt = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (opt) {
        case 'A': opts->A = true; break;
        case 'a': opts->a = true; break;
        case 'c': opts->c = true; opts->u = false; break;
        case 'd': opts->d = true; break;
        case 'F': opts->F = true; break;
        case 'f': opts->f = true; opts->a = true; /* -f implies -a in many implementations */
                  break;
        case 'h': opts->h = true; opts->k = false; break;
        case 'i': opts->i = true; break;
        case 'k': opts->k = true; opts->h = false; break;
        case 'l': opts->l = true; opts->n = false; break;
        case 'n': opts->n = true; opts->l = true; break; /* -n implies long */
        case 'q': opts->q = true; opts->w = false; break;
        case 'R': opts->R = true; break;
        case 'r': opts->r = true; break;
        case 'S': opts->S = true; opts->t = false; break;
        case 's': opts->s = true; break;
        case 't': opts->t = true; opts->S = false; break;
        case 'u': opts->u = true; opts->c = false; break;
        case 'w': opts->w = true; opts->q = false; break;
        case '?':
        default:
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* Mutual exclusion notes from man page (last one wins) already handled
     * by the assignments above. */

    /* -f disables sorting; we still honor other flags */
    if (opts->f) {
        /* no special action needed beyond the flag itself */
    }

    *files = &argv[optind];
    return argc - optind;   /* number of operands */
}
