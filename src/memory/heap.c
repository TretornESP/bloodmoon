#include "heap.h"
#include "paging.h"
#include "memory.h"

#define HEAP_START 0x0000100000000000
#define PAGE_COUNT 0x100

#include "../util/printf.h"
#include "../util/string.h"

struct heap heap;

struct heap_seg_header* split_segment(struct heap_seg_header* current_header, uint64_t splitLength) {
    if (splitLength < 0x10) return 0;
    int64_t splitSegLength = current_header->length - splitLength - sizeof(struct heap_seg_header);
    if (splitSegLength < 0x10) return 0;

    struct heap_seg_header* newSplitHdr = (struct heap_seg_header*) ((uint64_t)current_header + sizeof(struct heap_seg_header) + splitLength); //Not sure of this line's old_header use

    if (current_header->next)
        current_header->next->prev = newSplitHdr;

    newSplitHdr->next = current_header->next;

    newSplitHdr->prev = current_header->prev;
    newSplitHdr->length = splitSegLength;
    newSplitHdr->free = 1;

    current_header->length = splitLength;
    current_header->next = newSplitHdr;

    if (heap.last_header == current_header) {
        heap.last_header = newSplitHdr;
    }

    return newSplitHdr;
}

void debug_heap() {
    struct heap_seg_header* current_segment = (struct heap_seg_header*)heap.heap_start;

    int no = 0;
    while (1) {
        printf("[SEGMENT %d] Address: %p Prev: %p Next: %p\n", no, current_segment, current_segment->prev, current_segment->next);
        printf("[SEGMENT %d]. Free: %d. Length: %llx\n", no, current_segment->free, current_segment->length);
        if (current_segment->next == 0) break;
        current_segment = current_segment->next;
        no++;
    }
}

void init_heap() {
    printf("### HEAP STARTUP ###\n");
    void* pos = (void*)(uint64_t)HEAP_START;

    for (uint64_t i = 0; i < PAGE_COUNT; i++) {
        map_memory(pos, request_page());
        pos = (void*)((uint64_t)pos + PAGESIZE);
    }

    uint64_t heap_length = PAGE_COUNT * PAGESIZE;

    heap.heap_start = (void*)(uint64_t)HEAP_START;
    heap.heap_end = (void*)((uint64_t)heap.heap_start + heap_length);
    struct heap_seg_header* start_seg = (struct heap_seg_header*)HEAP_START;
    start_seg->length = heap_length - sizeof(struct heap_seg_header);
    start_seg->next = 0;
    start_seg->prev = 0;
    start_seg->free = 1;
    
    heap.last_header = start_seg;
}

void* malloc(uint64_t size) {

    if ((size % 10) > 0) {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return 0;

    struct heap_seg_header* current_segment = (struct heap_seg_header*)heap.heap_start;

    while (1) {
        if (current_segment->free) {
            if (current_segment->length > size) {
                split_segment(current_segment, size);
                current_segment->free = 0;
                return (void*)((uint64_t)current_segment + sizeof(struct heap_seg_header));
            }
            if (current_segment->length == size) {
                current_segment->free = 0;
                return (void*)((uint64_t)current_segment + sizeof(struct heap_seg_header));
            }
        }
        if (current_segment->next == 0) break;
        
        current_segment = current_segment->next;
    }

    expand_heap(size);
    return malloc(size);
}

void * calloc(uint64_t value, uint64_t size) {
    void* ptr = malloc(size);
    memset(ptr, value, size);
    return ptr;
}

void combine_forward(struct heap_seg_header* current_header) {
    if (current_header->next == 0) return;
    if (current_header->next->free == 0) return;

    if (current_header->next == heap.last_header) {
        heap.last_header = current_header;
    }

    if (current_header->next != 0) {
        current_header->next->prev = current_header;
    }

    current_header->length += current_header->next->length + sizeof(struct heap_seg_header);
}

void combine_backward(struct heap_seg_header* current_header) {
    if (current_header->prev != 0 && current_header->free)
        combine_forward(current_header->prev);
}

void expand_heap(uint64_t length) {
    if (length % PAGESIZE) {
        length -= (length % PAGESIZE);
        length += PAGESIZE;
    }

    uint64_t page_count = length / PAGESIZE;
    struct heap_seg_header* newSegment = (struct heap_seg_header*)heap.heap_end;

    for (uint64_t i = 0; i < page_count; i++) {
        map_memory(heap.heap_end, request_page());
        heap.heap_end = (void*)((uint64_t)heap.heap_end + PAGESIZE);
    }

    newSegment->free = 1;
    newSegment->prev = heap.last_header;

    heap.last_header->next = newSegment;
    heap.last_header = newSegment;

    newSegment->next = 0;
    newSegment->length = length - sizeof(struct heap_seg_header);
    combine_backward(newSegment);
}

void free(void * address) {
    struct heap_seg_header* segment = (struct heap_seg_header*)((uint64_t)address - sizeof(struct heap_seg_header));
    segment->free = 1;
    combine_forward(segment);
    combine_backward(segment);
}