#ifndef _MEMORY_H
#define _MEMORY_H
#include "stdint.h"

struct system_memory {
    uint64_t total_available_pages;
    uint64_t first_available_page_addr;
    uint8_t * bitfield;

    uint64_t free_memory;
    uint64_t locked_memory;
    uint64_t reserved_memory;

} __attribute__((packed));

uint64_t get_total_memory();
uint64_t get_free_memory();

void init_memory();

void reserve_page(void*);
void unreserve_page(void*);
void lock_page(void*);
void unlock_page(void*);

#endif // _MEMORY_H