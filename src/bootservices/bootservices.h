#ifndef _BOOTSERVICES_H
#define _BOOTSERVICES_H
#include "limine.h"
#include <stdint.h>

#define MEMMAP_USABLE                 0x0
#define MEMMAP_RESERVED               0x1
#define MEMMAP_ACPI_RECLAIMABLE       0x2
#define MEMMAP_ACPI_NVS               0x3
#define MEMMAP_BAD_MEMORY             0x4
#define MEMMAP_BOOTLOADER_RECLAIMABLE 0x5
#define MEMMAP_KERNEL_AND_MODULES     0x6
#define MEMMAP_FRAMEBUFFER            0x7

void   (*get_terminal_writer())(const char*, uint64_t);

char*    get_bootloader_name();
char*    get_bootloader_version();

uint64_t get_hhdm_offset();
uint64_t get_terminal_count();
uint64_t get_current_terminal();

uint64_t get_memory_map_entries();
uint64_t get_memory_map_base(uint64_t);
uint64_t get_memory_map_length(uint64_t);
uint64_t get_memory_map_type(uint64_t);

void     set_terminal_extra_handler(); //TODO
void     set_terminal_writer(uint64_t terminal);
void     set_stack_size(uint64_t size);

#endif