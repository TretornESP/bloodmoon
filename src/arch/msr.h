#ifndef _MSR_H
#define _MSR_H

#define MSR_EFER                0xc0000080
#define MSR_FS_BASE             0xc0000100
#define MSR_GS_BASE             0xc0000101
#define MSG_GS_KERNEL_BASE      0xc0000102

#include <stdint.h>
void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi);
void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi);
void cpuGetMSR64(uint32_t msr, uint64_t *val);
void cpuSetMSR64(uint32_t msr, uint64_t val);
#endif