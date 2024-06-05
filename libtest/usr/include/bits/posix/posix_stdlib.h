
#ifndef MLIBC_POSIX_STDLIB_H
#define MLIBC_POSIX_STDLIB_H

#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

long random(void);
double drand48(void);
void srand48(long int);
char *initstate(unsigned int, char *, size_t);
char *setstate(char *);
void srandom(unsigned int);

// ----------------------------------------------------------------------------
// Environment.
// ----------------------------------------------------------------------------

int putenv(char *);
int setenv(const char *, const char *, int);
int unsetenv(const char *);

// ----------------------------------------------------------------------------
// Path handling.
// ----------------------------------------------------------------------------

int mkstemp(char *);
int mkostemp(char *, int flags);
int mkostemps(char *pattern, int suffixlen, int flags);
char *mkdtemp(char *path);

char *realpath(const char *__restrict, char *__restrict);

// ----------------------------------------------------------------------------
// Pseudoterminals
// ----------------------------------------------------------------------------

int posix_openpt(int flags);
int grantpt(int fd);
int unlockpt(int fd);
char *ptsname(int fd);
int ptsname_r(int fd, char *buf, size_t len);

double strtod_l(const char *__restrict__ nptr, char ** __restrict__ endptr, locale_t loc);
long double strtold_l(const char *__restrict__ nptr, char ** __restrict__ endptr, locale_t loc);
float strtof_l(const char *__restrict string, char **__restrict end, locale_t loc);

int getloadavg(double *, int);

// GNU extension
char *secure_getenv(const char *);
char *canonicalize_file_name(const char *);

// BSD extension
void *reallocarray(void *, size_t, size_t);

int clearenv(void);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STDLIB_H

