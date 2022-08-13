#include "heap.h"
#include "paging.h"
#include "memory.h"

struct heap heap;

void expand_heap(uint64_t size) {

}

struct heap_seg_header* split_segment(struct heap_seg_header* current_header, uint64_t splitLength) {
    if (splitLength < 0x10) return 0;
    int64_t splitSegLength = current_header->length - splitLength - sizeof(struct heap_seg_header);
    if (splitSegLength < 0x10) return 0;

    struct heap_seg_header* newSplitHdr = (struct heap_seg_header*) ((uint64_t)current_header + sizeof(struct heap_seg_header) + splitLength); //Not sure of this line's old_header use
    
    current_header->next->last = newSplitHdr;
    newSplitHdr->next = current_header->next;
    current_header->next = newSplitHdr;
    newSplitHdr->last = current_header;
    newSplitHdr->length = splitSegLength;
    newSplitHdr->free = current_header->free;
    current_header->length = splitLength;

    if (heap.last_header == current_header) {
        heap.last_header = newSplitHdr;
    }

    return newSplitHdr;
}

void init_heap(void* heap_address, uint64_t page_count) {
    void* pos = heap_address;

    for (uint64_t i = 0; i < page_count; i++) {
        map_memory(pos, request_page());
        pos = (void*)((uint64_t)pos + PAGESIZE);
    }

    uint64_t heap_length = page_count * PAGESIZE;

    heap.heap_start = heap_address;
    heap.heap_end = (void*)((uint64_t)heap.heap_start + heap_length);
    struct heap_seg_header* start_seg = (struct heap_seg_header*)heap_address;
    start_seg->length = heap_length - sizeof(struct heap_seg_header);
    start_seg->next = 0;
    start_seg->last = 0;
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