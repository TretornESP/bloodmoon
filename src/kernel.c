#include "bootservices/bootservices.h"

#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "scheduling/pit.h"
#include "io/interrupts.h"
#include "dev/smbios_interface.h"
#include "dev/devices.h"
#include "drivers/disk.h"
#include "util/printf.h"
#include "drivers/ahci.h"
#include "util/string.h"
#include "fs/vfs.h"

void _start(void) {
    init_memory();
    init_paging();
    init_heap();
    //init_pit();
    init_interrupts(1);
    //init_smbios_interface();
    //init_devices();
    //device_list();
    //init_drive();
    //init_vfs();
    
    void * target = request_page();
    memset(target, 0, 4096);
    map_memory((void*)0xffffffffdeadb000, target);

    uint64_t* ptr1 = (uint64_t*)0xffffffffdeadb000;	
    PAGE_RESTRICT_WRITE((void*)ptr1);

    *ptr1 = 0xdeadbeef;

    printf("VIVO\n");
    while(1);

}