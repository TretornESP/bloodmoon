#include "heap.h"
#include "paging.h"
#include "memory.h"

#define HEAP_START 0x0000100000000000
#define PAGE_COUNT 0x100

#include "../util/printf.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"

struct heap heap;

struct heap_seg_header* split_segment(struct heap_seg_header* current_header, uint64_t splitLength) {
    printf("SPLIT\n");
    if (splitLength < 0x10) {printf("Split aborting\n"); return 0;}
    int64_t splitSegLength = current_header->length - splitLength - sizeof(struct heap_seg_header);
    if (splitSegLength < 0x10) {printf("Split aborting\n"); return 0;}

    struct heap_seg_header* newSplitHdr = (struct heap_seg_header*) ((uint64_t)current_header + sizeof(struct heap_seg_header) + splitLength); //Not sure of this line's old_header use

    if (current_header->next != 0) {
        printf("LINE 26\n");
        current_header->next->last = newSplitHdr;
    }

    newSplitHdr->next = current_header->next;
    current_header->next = newSplitHdr;

    newSplitHdr->last = current_header;
    newSplitHdr->length = splitLength;
    newSplitHdr->free = current_header->free;

    if (heap.last_header == current_header) {
        printf("LINE 37\n");
        heap.last_header = newSplitHdr;
    }

    return newSplitHdr;
}

void debug_heap(struct heap_seg_header* crashing_header) {
    struct heap_seg_header* current_segment = (struct heap_seg_header*)heap.heap_start;

    int no = 0;
    while (no <= HEAP_OVERFLOW_DETECTOR) {
        printf("current_segment: %p, len: %llx, free: %d, next: %p, last: %p\n", current_segment, current_segment->length, current_segment->free, current_segment->next, current_segment->last);
        if (current_segment->next == 0) break;
        if (crashing_header == current_segment) break;
        if (current_segment->next == current_segment) break;
        current_segment = current_segment->next;
        no++;
    }
    if (no > HEAP_OVERFLOW_DETECTOR) {
        printf("current_segment: %p, len: %llx, free: %d, next: %p, last: %p\n", current_segment, current_segment->length, current_segment->free, current_segment->next, current_segment->last);
        panic("Heap overflow detected\n");
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
    memset(heap.heap_start, 0, heap_length);
    heap.heap_end = (void*)((uint64_t)heap.heap_start + heap_length);
    struct heap_seg_header* start_seg = (struct heap_seg_header*)HEAP_START;
    start_seg->length = heap_length - sizeof(struct heap_seg_header);
    start_seg->next = 0;
    start_seg->last = 0;
    start_seg->free = 1;
    heap.last_header = start_seg;
}

void* malloc(uint64_t size) {
    printf("MALLOC %d\n", size);
    if ((size % 10) > 0) {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return 0;

    struct heap_seg_header* current_segment = (struct heap_seg_header*)heap.heap_start;

    int no = 0;
    while (no++ < HEAP_OVERFLOW_DETECTOR) {

        if (current_segment->free) {
            printf("LINE 97\n");
            if (current_segment->length > size) {
                printf("LINE 99\n");
                split_segment(current_segment, size);
                current_segment->free = 0;
                return (void*)((uint64_t)current_segment + sizeof(struct heap_seg_header));
            }
            if (current_segment->length == size) {
                printf("LINE 105\n");
                current_segment->free = 0;
                return (void*)((uint64_t)current_segment + sizeof(struct heap_seg_header));
            }
            printf("Insufficient space\n");
        }
        if (current_segment->next == 0) break;
        if (current_segment->next == current_segment) { //This is a bug. It should never happen but it does.
            printf("Bug detected\n");
            current_segment->next = 0;
            break;
        }
        current_segment = current_segment->next;
    }
    if (no >= HEAP_OVERFLOW_DETECTOR) {
        printf("current_segment: %p, len: %llx, free: %d, next: %p, last: %p\n", current_segment, current_segment->length, current_segment->free, current_segment->next, current_segment->last);
        debug_heap(current_segment);
        panic("Heap overflow\n");
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
    printf("Combine forward\n");
    if (current_header->next == 0) {printf("LINE 135\n"); return;}
    if (current_header->next->free == 0) {printf("LINE 136\n"); return;}
    if (current_header->next == heap.last_header) {
        printf("LINE 138\n");
        heap.last_header = current_header;
    }

    if (current_header->next != 0) {
        printf("LINE 143\n");
        current_header->next->last = current_header;
    }

    current_header->length += current_header->next->length + sizeof(struct heap_seg_header);
}

void combine_backward(struct heap_seg_header* current_header) {
    printf("Combine backward\n");
    if (current_header->last != 0 && current_header->last->free) {
        printf("LINE 153\n");
        combine_forward(current_header->last);
    }
}

void expand_heap(uint64_t length) {
    printf("Expanding heap by %llx bytes\n", length);
    if (length % PAGESIZE) {
        length -= (length % PAGESIZE);
        length += PAGESIZE;
    }

    uint64_t page_count = length / PAGESIZE;
    printf("page_count: %llx\n", page_count);
    
    struct heap_seg_header* newSegment = (struct heap_seg_header*)heap.heap_end;

    for (uint64_t i = 0; i < page_count; i++) {
        map_memory(heap.heap_end, request_page());
        heap.heap_end = (void*)((uint64_t)heap.heap_end + PAGESIZE);
    }

    newSegment->free = 1;
    newSegment->last = heap.last_header;

    heap.last_header->next = newSegment;
    heap.last_header = newSegment;

    newSegment->next = 0;
    newSegment->length = length - sizeof(struct heap_seg_header);
    combine_backward(newSegment);
}

void free(void * address) {
    struct heap_seg_header* segment = (struct heap_seg_header*)((uint64_t)address - sizeof(struct heap_seg_header));
    if (segment->free) panic("Double free\n");
    segment->free = 1;
    combine_forward(segment);
    combine_backward(segment);
}