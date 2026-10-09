#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "options.h"
#include "ls.h"


int main(int argc, char *argv[])
{
    options_t opts;
    char **files;
    int nfiles;
    int i;
    int status = 0;

    nfiles = parse_options(argc, argv, &opts, &files);

    if (nfiles == 0) {
        /* No operands: list current directory */
        list_path(".", &opts, true);
    } else {
        /*
         * Multiple operands: non-directory operands first (already in
         * argv order), then directories. For simplicity we process in
         * the order given; a more complete implementation would separate
         * them and sort each group. The man page says they are sorted
         * separately in lexicographical order.
         */
        /* Simple approach: first pass non-dirs, second pass dirs */
        for (i = 0; i < nfiles; i++) {
            struct stat st;
            if (lstat(files[i], &st) < 0) {
                fprintf(stderr, "ls: cannot access '%s': %s\n",
                        files[i], strerror(errno));
                status = 1;
                continue;
            }
            if (!S_ISDIR(st.st_mode) || opts.d)
                list_path(files[i], &opts, true);
        }
        for (i = 0; i < nfiles; i++) {
            struct stat st;
            if (lstat(files[i], &st) < 0)
                continue;   /* already reported */
            if (S_ISDIR(st.st_mode) && !opts.d) {
                if (nfiles > 1)
                    printf("%s:\n", files[i]);
                /* is_top_level = true so no extra header inside */
                list_path(files[i], &opts, true);
            }
        }

    }

    return status;
}
