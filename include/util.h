#ifndef UTIL_H
#define UTIL_H

#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include "options.h"

/* Maximum length for a single path component or display name */
#define MAX_PATH 4096

/* Entry representing one filesystem object to list */
typedef struct {
    char name[MAX_PATH];      /* basename or relative name as given */
    char fullpath[MAX_PATH];  /* full path used for stat/lstat */
    struct stat st;           /* result of lstat */
    bool is_dir;              /* S_ISDIR after following? no, from lstat */
} entry_t;

/* Safe path join: dest = dir + "/" + name (handles trailing slash) */
void path_join(char *dest, size_t destsize, const char *dir, const char *name);

/* Return true if name starts with '.' */
bool is_dotfile(const char *name);

/* Return true if name is "." or ".." */
bool is_dot_or_dotdot(const char *name);

/* Convert size to human-readable string (B, K, M, G, T).
 * Writes into buf (must be at least 16 bytes). */
void humanize_size(off_t size, char *buf, size_t bufsize);

/* Format block count according to -h / -k / BLOCKSIZE */
void format_blocks(blkcnt_t blocks, const options_t *opts, char *buf, size_t bufsize);

/* Get owner name or numeric uid */
void get_owner(uid_t uid, bool numeric, char *buf, size_t bufsize);

/* Get group name or numeric gid */
void get_group(gid_t gid, bool numeric, char *buf, size_t bufsize);

/* Build the 10-character mode string (e.g. "drwxr-xr-x") */
void format_mode(mode_t mode, char *buf);

/* Return the indicator character for -F, or '\0' if none */
char get_indicator(const struct stat *st);

/* Sanitize non-printable characters according to -q / -w */
void sanitize_name(const char *src, char *dest, size_t destsize, const options_t *opts);

#endif /* UTIL_H */
