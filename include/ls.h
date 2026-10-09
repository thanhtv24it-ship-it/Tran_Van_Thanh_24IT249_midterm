#ifndef LS_H
#define LS_H

#include "options.h"
#include "util.h"

/* List a single path (file or directory).
 * For directories, if -d is not set, list contents.
 * If -R is set, recurse into subdirectories. */
void list_path(const char *path, const options_t *opts, bool is_top_level);

/* Compare two entries for sorting. Returns <0, 0, >0 like strcmp.
 * Sorting key depends on options (-t, -S, -r, default name). */
int compare_entries(const void *a, const void *b, const options_t *opts);

/* Wrapper for qsort that carries options via a global (simple approach) */
void sort_entries(entry_t *entries, size_t n, const options_t *opts);

#endif /* LS_H */
