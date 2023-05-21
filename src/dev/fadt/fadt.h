#ifndef _FADT_H
#define _FADT_H
//https://wiki.osdev.org/FADT
#include <stdint.h>
#include "../acpi/acpi.h"

void register_fadt(struct fadt_header*, char* (*cb)(void*, uint8_t, uint64_t));

#endif