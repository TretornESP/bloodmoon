#ifndef _SYS_SWAP_H
#define _SYS_SWAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define SWAP_FLAG_PREFER 0x8000
#define SWAP_FLAG_PRIO_MASK 0x7fff
#define SWAP_FLAG_PRIO_SHIFT 0
#define SWAP_FLAG_DISCARD 0x10000

#ifndef __MLIBC_ABI_ONLY

int swapon(const char *, int);
int swapoff(const char *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SWAP_H */
