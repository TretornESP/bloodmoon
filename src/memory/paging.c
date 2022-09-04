#include "paging.h"
#include "../bootservices/bootservices.h"
#include "../util/dbgprinter.h"
#include "../util/printf.h"
#include "../util/string.h"

struct page_directory *pml4;

void address_to_map(uint64_t address, struct page_map_index* map) {
    address >>= 12;
    map->P_i = address & 0x1ff;
    address >>= 9;
    map->PT_i = address & 0x1ff;
    address >>= 9;
    map->PD_i = address & 0x1ff;
    address >>= 9;    
    map->PDP_i = address & 0x1ff;
}

void print_pde(struct page_directory_entry * pde) {
    printf("p: %d w: %d u: %d wt: %d c: %d a: %d i3: %d s: %d i2: %d [pp: 0x%x] r: %d i1: %d n: %d\n",
        pde->present, pde->writeable, pde->user_access, pde->write_through, pde->cache_disabled,
        pde->accessed, pde->ignored_3, pde->size, pde->ignored_2, pde->page_ppn, pde->reserved_1,
        pde->ignored_1, pde->execution_disabled
    );
}

void print_pte(struct page_table_entry * pte) {
    printf("p: %d w: %d u: %d wt: %d c: %d a: %d d: %d s: %d g: %d i2: %d [pp: 0x%x] r: %d i1: %d n: %d\n",
        pte->present, pte->writeable, pte->user_access, pte->write_through, pte->cache_disabled,
        pte->accessed, pte->dirty, pte->size, pte->global, pte->ignored_2, pte->page_ppn, pte->reserved_1,
        pte->ignored_1, pte->execution_disabled
    );
}

uint64_t virtual_to_physical(void* virtual_memory) {
    struct page_map_index map;
    address_to_map((uint64_t)virtual_memory, &map);

    struct page_directory_entry pde;
    struct page_directory *pd;

    pde = pml4->entries[map.PDP_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PD_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PT_i];
    
    struct page_table *pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    struct page_table_entry pte = pt->entries[map.P_i];
    
    uint64_t physical = ((uint64_t)pte.page_ppn << 12) | ((uint64_t)virtual_memory & 0xfff);
    return physical;
}

void debug_memory_map(void* virtual_memory, void* physical_memory) {
    struct page_map_index map;
    address_to_map((uint64_t)virtual_memory, &map);

    struct page_directory_entry pde;
    struct page_directory *pd;
    printf("[MAP DEBUG] Virtual memory: 0x%llx Physical target: 0x%llx\n", (uint64_t)virtual_memory, (uint64_t)physical_memory);
    printf("MAP: PDP_i: %x PD_i: %x PT_i: %x P_i: %x\n", map.PDP_i, map.PD_i, map.PT_i, map.P_i);
    pde = pml4->entries[map.PDP_i];
    printf("PDP entry at: %p\n", &pml4->entries[map.PDP_i]);
    print_pde(&pde);
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PD_i];
    printf("PD entry at: %p\n", &pd->entries[map.PD_i]);
    print_pde(&pde);
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);

    pde = pd->entries[map.PT_i];
    printf("PT entry at: %p\n", &pd->entries[map.PT_i]);
    print_pde(&pde);
    
    struct page_table *pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    struct page_table_entry pte = pt->entries[map.P_i];
    printf("P entry at: %p\n", &pt->entries[map.P_i]);
    print_pte(&pte);
    
    uint64_t physical = ((uint64_t)pte.page_ppn << 12) | ((uint64_t)virtual_memory & 0xfff);
    printf("Points to physical: %llx\n", physical);
    printf("I said it was:      %llx\n", physical_memory);

}

void init_paging() {
     __asm__("movq %%cr3, %0" : "=r"(pml4));
     __asm__("movq %0, %%cr3" : : "r" (pml4)); //This should invalidate TLB    

    uint64_t virtual_start = get_kernel_address_virtual();
    uint64_t linker_kstart = (uint64_t)&KERNEL_START;

    if (linker_kstart != virtual_start) {
        printf("Crashing: KERNEL_START: %llx VIRT_ADDR: %p\n", linker_kstart, virtual_start);
        panic("init_paging: kernel virtual address does not match KERNEL_START");
    }
}

void map_memory(void* virtual_memory, void* physical_memory) {
    if ((uint64_t)virtual_memory & 0xfff) {
        printf("Crashing: virtual_memory: %p\n", virtual_memory);
        panic("map_memory: virtual_memory must be aligned to 0x1000");
    }

    if ((uint64_t)physical_memory & 0xfff) {
        printf("Crashing: physical_memory: %p\n", physical_memory);
        panic("map_memory: physical_memory must be aligned to 0x1000");
    }

    struct page_map_index map;
    address_to_map((uint64_t)virtual_memory, &map);
    struct page_directory_entry pde;

    pde = pml4->entries[map.PDP_i];
    struct page_directory* pdp;
    if (!pde.present) {
        pdp = (struct page_directory*)request_page();
        memset(pdp, 0, PAGESIZE);
        pde.page_ppn = (uint64_t) pdp >> 12;
        pde.present = 1;
        pde.writeable = 1;
        pml4->entries[map.PDP_i] = pde;
    } else {
        pdp = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    }

    pde = pdp->entries[map.PD_i];
    struct page_directory* pd;
    if (!pde.present) {
        pd = (struct page_directory*)request_page();
        memset(pd, 0, PAGESIZE);
        pde.page_ppn = (uint64_t) pd >> 12;
        pde.present = 1;
        pde.writeable = 1;
        pdp->entries[map.PD_i] = pde;
    } else {
        pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    }

    pde = pd->entries[map.PT_i];
    struct page_table* pt;
    if (!pde.present) {
        pt = (struct page_table*)request_page();
        memset(pt, 0, PAGESIZE);
        pde.page_ppn = (uint64_t) pt >> 12;
        pde.present = 1;
        pde.writeable = 1;
        pd->entries[map.PT_i] = pde;
    } else {
        pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    }

    struct page_table_entry pte = pt->entries[map.P_i];
    pte.page_ppn = (uint64_t)physical_memory >> 12;
    pte.present = 1;
    pte.writeable = 1;
    pt->entries[map.P_i] = pte;
}

void set_page_perms(void* address, uint8_t permissions) {
    struct page_map_index map;
    address_to_map((uint64_t)address, &map);

    struct page_directory_entry pde;
    struct page_directory *pd;

    pde = pml4->entries[map.PDP_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PD_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PT_i];
    
    struct page_table *pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    struct page_table_entry *pte = &pt->entries[map.P_i];

    pte->writeable = (permissions & 1);
    pte->user_access = ((permissions & 2) >> 1);
    pte->execution_disabled = ((permissions & 4) >> 2);
    pte->cache_disabled = ((permissions & 8) >> 3);
}

uint8_t get_page_perms(void* address) {
    struct page_map_index map;
    address_to_map((uint64_t)address, &map);

    struct page_directory_entry pde;
    struct page_directory *pd;

    pde = pml4->entries[map.PDP_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PD_i];
    pd = (struct page_directory*)((uint64_t)pde.page_ppn << 12);
    pde = pd->entries[map.PT_i];
    
    struct page_table *pt = (struct page_table*)((uint64_t)pde.page_ppn << 12);
    struct page_table_entry pte = pt->entries[map.P_i];

    uint8_t result = pte.writeable;
    result |= (pte.user_access << 1);
    result |= (pte.execution_disabled << 2);
    result |= (pte.cache_disabled << 3);

    return result;
}

void page_set(void* address, uint8_t field) {
    uint8_t perms = get_page_perms(address);
    perms |= field;
    set_page_perms(address, perms);
}

void page_clear(void* address, uint8_t field) {
    uint8_t perms = get_page_perms(address);
    perms &= ~field;
    set_page_perms(address, perms);
}

void * request_page_identity() {
    void * result = request_page();
    map_memory(result, result);
    return result;
}