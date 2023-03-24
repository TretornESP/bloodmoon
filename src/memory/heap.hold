#ifndef _HEAP_H
#define _HEAP_H
#include <stdint.h>
#define HEAP_HEADER_CHECKSUM 0x55
#define HEAP_OVERFLOW_DETECTOR 5000

struct heap_seg_header {
    uint64_t length;
    struct heap_seg_header* next;
    struct heap_seg_header* last;
    uint8_t free;
};

struct heap {
    void* heap_start;
    void* heap_end;
    struct heap_seg_header* last_header;
};

void init_heap();

void * malloc(uint64_t);
void * calloc(uint64_t, uint64_t);
void free(void*);

void debug_heap(struct heap_seg_header *);

void expand_heap(uint64_t);
#endif