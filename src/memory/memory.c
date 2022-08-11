#include "memory.h"
#include "memory_cb.h"

#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "../bootservices/bootservices.h"
#define PAGESIZE 0x1000

#define ALIGN_ADDR(x) (((PAGESIZE-1)&(x)) ? ((x+PAGESIZE) & ~(PAGESIZE-1)):(x))
#define IS_ALIGNED(x) ((((uint64_t)(x))&(PAGESIZE-1)) == 0)

#define ADDR_TO_PAGE(x) ((uint64_t)(x) >> 12)

#define SET_PAGE_BIT(x) (memory->bitfield[(x) >> 3] |= (1 << ((x) % 8)));
#define CLEAR_PAGE_BIT(x) (memory->bitfield[(x) >> 3] &= ~(1 << ((x) % 8)));
#define GET_PAGE_BIT(x) (memory->bitfield[(x) >> 3] >> ((x) % 8) & 1);

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

void set_page_bit(void* address) {
    if (ADDR_TO_PAGE(address) > memory->total_pages) {
        dbg_print("\nCrashing at: ");
        dbg_print(itoa(memory->total_pages, 16));
        dbg_print("\n");
        panic("set_page_bit: address out of bounds");
    }
    SET_PAGE_BIT(ADDR_TO_PAGE(address));
}

void clear_page_bit(void* address) {
    if (ADDR_TO_PAGE(address) > memory->total_pages) {
        dbg_print("\nCrashing at: ");
        dbg_print(itoa(memory->total_pages, 16));
        dbg_print("\n");
        panic("clear_page_bit: address out of bounds");
    }
    CLEAR_PAGE_BIT(ADDR_TO_PAGE(address));
}

uint8_t get_page_bit(void* address) {
    if (ADDR_TO_PAGE(address) > memory->total_pages) {
        dbg_print("\nCrashing addr: 0x");
        dbg_print(itoa((uint64_t)address, 16));
        dbg_print(" against: 0x");
        dbg_print(itoa(memory->total_pages, 16));
        dbg_print("\n");
        panic("get_page_bit: address out of bounds");
    }
    return (uint8_t)GET_PAGE_BIT(ADDR_TO_PAGE(address));
}

void init_memory() {
    static struct chunk_data biggest_avail_chunk;
    if (biggest_avail_chunk.size > 0) return;

    uint64_t total_memory = get_total_memory(); //Divide by pagesize and by byte size (12+3)
    uint64_t total_pages = ((total_memory % 0x100) == 0) ? (total_memory >> 12) : (total_memory >> 12) + 1;
    uint64_t bitfield_entries = ((total_pages % 0x8) == 0) ? (total_pages >> 3) : (total_pages >> 3) + 1;

    iterate_memory(init_memory_cb, (void*)&biggest_avail_chunk);
    
    //We are subtracting: size of the bitfield, size of the memory descriptor struct, up to three pages per alignment
    if (biggest_avail_chunk.size < (bitfield_entries + sizeof(struct system_memory) + (3 << 12)))
        panic("Not enough memory for startup!");

    uint64_t first_addr = ALIGN_ADDR(biggest_avail_chunk.addr);
    uint64_t last_addr = biggest_avail_chunk.addr + biggest_avail_chunk.size;
    memory = (struct system_memory*)ALIGN_ADDR(first_addr + bitfield_entries + 1);

    memory->bitfield = (uint8_t*)(first_addr);
    memory->total_pages = total_pages;
    memory->first_available_page_addr = ALIGN_ADDR((uint64_t)memory + sizeof(struct system_memory) + 1);
    memory->total_available_pages = ((last_addr - memory->first_available_page_addr) >> 12) - 1;

    memset(memory->bitfield, 0, bitfield_entries);

    memory->free_memory = memory->total_available_pages << 12;
    memory->locked_memory = 0;
    memory->reserved_memory = 0;

    if (!IS_ALIGNED(memory) || !IS_ALIGNED(memory->bitfield) || !IS_ALIGNED(memory->first_available_page_addr))
        panic("Memory alignment error!");

    dbg_print("\nBITFIELD ENTRIES: ");
    dbg_print(itoa(memory->total_pages, 16));
    dbg_print("\n");
    iterate_memory(reserve_memory_cb, (void*)reserve_pages);

    uint64_t kernel_pages = ADDR_TO_PAGE((uint64_t)KERNEL_END - (uint64_t)KERNEL_START);
    //lock_pages((void*)KERNEL_START, kernel_pages);
    //Lock global variables!!
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

void reserve_page(void* address) {
    if (get_page_bit(address) == 1)
        panic("Page already reserved!"); //Deleteme

    set_page_bit(address);

    memory->free_memory -= PAGESIZE;
    memory->reserved_memory += PAGESIZE;
}

void unreserve_page(void* address) {
    if (get_page_bit(address) == 0)
        panic("Page not reserved!"); //Deleteme
    
    clear_page_bit(address);

    memory->free_memory += PAGESIZE;
    memory->reserved_memory -= PAGESIZE;
}

void lock_page(void* address) {
    if (get_page_bit(address) == 1)
        panic("Page already locked!"); //Deleteme
    
    set_page_bit(address);

    memory->free_memory -= PAGESIZE;
    memory->locked_memory += PAGESIZE;
}

void unlock_page(void* address) {
    if (get_page_bit(address) == 0)
        panic("Page not locked!"); //Deleteme
    
    clear_page_bit(address);

    memory->free_memory += PAGESIZE;
    memory->locked_memory -= PAGESIZE;
}

void reserve_pages(void* address, uint64_t pages) {
    if ((int64_t)address < 0) {
        panic("NEGATIVE");
    }
    for (uint64_t i = 0; i < pages; i++) {
        reserve_page((void*)((uint64_t)address + (i << 12)));
    }
}

void unreserve_pages(void* address, uint64_t pages) {
    for (uint64_t i = 0; i < pages; i++)
        unreserve_page((void*)((uint64_t)address + (i << 12)));
}

void lock_pages(void* address, uint64_t pages) {
    for (uint64_t i = 0; i < pages; i++)
        lock_page((void*)((uint64_t)address + (i << 12)));
}

void unlock_pages(void* address, uint64_t pages) {
    for (uint64_t i = 0; i < pages; i++)
        unlock_page((void*)((uint64_t)address + (i << 12)));
}