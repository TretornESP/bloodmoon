#ifndef _MEMORY_H
#define _MEMORY_H
#include "stdint.h"

uint64_t get_total_memory();
uint64_t get_free_memory();

void init_memory();

void reserve_page(void*);
void unreserve_page(void*);
void lock_page(void*);
void unlock_page(void*);

#endif // _MEMORY_H