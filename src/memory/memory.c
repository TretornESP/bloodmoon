#include "memory.h"
#include "../bootservices/bootservices.h"
uint64_t get_total_memory() {
    static uint64_t total_memory;
    if (total_memory > 0) return total_memory;

    uint64_t memory_entries = get_memory_map_entries();

    for (uint64_t index = 0; index < memory_entries; index++) {
        total_memory += get_memory_map_length(index);
    }

    return total_memory;
}

uint64_t get_free_memory() {
    static uint64_t free_memory;
    if (free_memory > 0) return free_memory;

    uint64_t memory_entries = get_memory_map_entries();

    for (uint64_t index = 0; index < memory_entries; index++) {
        if (get_memory_map_type(index) == MEMMAP_USABLE) {
            free_memory += get_memory_map_length(index);
        }
    }

    return free_memory;
}