#ifndef MLIBC_BITS_SIGSET_T_H
#define MLIBC_BITS_SIGSET_T_H

#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

// functions to manage sigset_t
int sigemptyset(sigset_t *);
int sigfillset(sigset_t *);
int sigaddset(sigset_t *, int);
int sigdelset(sigset_t *, int);
int sigismember(const sigset_t *set, int sig);

#ifdef __cplusplus
}
#endif

#endif //MLIBC_BITS_SIGSET_T_H
