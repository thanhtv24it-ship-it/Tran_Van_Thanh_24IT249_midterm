#ifndef PRINT_H
#define PRINT_H

#include "options.h"
#include "util.h"

/* Print a single entry according to the active options.
 * If long format, also needs the total blocks for the directory
 * (printed separately by the caller). */
void print_entry(const entry_t *e, const options_t *opts);

/* Print the "total N" line used in long format for a directory */
void print_total(blkcnt_t total_blocks, const options_t *opts);

/* Print just the name (with optional -F indicator and sanitization) */
void print_name(const entry_t *e, const options_t *opts);

#endif /* PRINT_H */
