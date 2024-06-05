#ifndef _WORDEXP_H
#define _WORDEXP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>

#define WRDE_APPEND 1
#define WRDE_DOOFFS 2
#define WRDE_NOCMD 4
#define WRDE_REUSE 8
#define WRDE_SHOWERR 16
#define WRDE_UNDEF 32

#define WRDE_SUCCESS 1
#define WRDE_BADCHAR 1
#define WRDE_BADVAL 2
#define WRDE_CMDSUB 3
#define WRDE_NOSPACE 4
#define WRDE_SYNTAX 5

typedef struct {
	size_t we_wordc;
	char **we_wordv;
	size_t we_offs;
	char *we_strings;
	size_t we_nbytes;
} wordexp_t;

int wordexp(const char *s, wordexp_t *p, int flags);
void wordfree(wordexp_t *p);

#ifdef __cplusplus
}
#endif

#endif
