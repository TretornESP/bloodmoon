#include "memory.h"
#include "memory_cb.h"

#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "../bootservices/bootservices.h"
#define PAGESIZE 0x1000
#define MAX_PAGES 0x1000000
#define ALIGN_ADDR(x) (((PAGESIZE-1)&(x)) ? ((x+PAGESIZE) & ~(PAGESIZE-1)):(x))
#define IS_ALIGNED(x) ((((uint64_t)x)&(PAGESIZE-1)) == 0)

struct system_memory * memory;

void iterate_memory(void (*callback)(void*, uint64_t, uint64_t, uint64_t), void *global_override) {
    uint64_t memory_entries = get_memory_map_entries();

    for (uint64_t index = 0; index < memory_entries; index++) {
        uint64_t base = get_memory_map_base(index);
        uint64_t length = get_memory_map_length(index);
        uint64_t type = get_memory_map_type(index);
        callback(global_override, base, length, type);
    }
}

void init_memory() {
    static struct chunk_data biggest_avail_chunk;
    if (biggest_avail_chunk.size > 0) return;

    uint64_t bitfield_entries = get_total_memory() >> 15; //Divide by pagesize and by byte size (12+3)

    iterate_memory(init_memory_cb, (void*)&biggest_avail_chunk);
    
    //We are subtracting: size of the bitfield, size of the memory descriptor struct, up to three pages per alignment
    if (biggest_avail_chunk.size < (bitfield_entries + sizeof(struct system_memory) + (3 << 12)))
        panic("Not enough memory for startup!");

    uint64_t first_addr = ALIGN_ADDR(biggest_avail_chunk.addr);
    uint64_t last_addr = biggest_avail_chunk.addr + biggest_avail_chunk.size;
    memory = (struct system_memory*)ALIGN_ADDR(first_addr + bitfield_entries + 1);

    memory->bitfield = (uint8_t*)(first_addr);
    memory->first_available_page_addr = ALIGN_ADDR((uint64_t)memory + sizeof(struct system_memory) + 1);
    memory->total_available_pages = ((last_addr - memory->first_available_page_addr) >> 12) - 1;

    memset(memory->bitfield, 0, bitfield_entries);

    memory->free_memory = memory->total_available_pages << 12;
    memory->locked_memory = 0;
    memory->reserved_memory = 0;

    if (!IS_ALIGNED(memory) || !IS_ALIGNED(memory->bitfield) || !IS_ALIGNED(memory->first_available_page_addr))
        panic("Memory alignment error!");

    return;

}

uint64_t get_total_memory() {
    static uint64_t total_memory;
    if (total_memory > 0) return total_memory;
    iterate_memory(get_total_memory_cb, (void*)&total_memory);
    return total_memory;
}

uint64_t get_free_memory() {
    static uint64_t free_memory;
    if (free_memory > 0) return free_memory;
    iterate_memory(get_free_memory_cb, (void*)&free_memory);
    return free_memory;
}

void get_biggest_memory_chunk() {

}