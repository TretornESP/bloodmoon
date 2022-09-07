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
#include "util/string.h"
#include "fs/fat32.h"

void _start(void) {
    init_memory();
    init_paging();
    init_heap();
    init_pit();
    init_interrupts(1);
    init_smbios_interface();
    init_devices();
    init_drive();

    uint8_t * buffer = malloc(0x1000);
    memset(buffer, 0, 0x1000);
    
    device_ioctl("/dev/hda", IOCTL_ATAPI_IDENTIFY, buffer);
    struct sata_ident_test* dev = (struct sata_ident_test*) buffer;

    uint8_t sum = 0;
    printf("Struct size: %x\n", sizeof(struct sata_ident_test));

    for (uint64_t i = 0; i < sizeof(struct sata_ident_test); i++) {
        sum += buffer[i];
    }

    printf("SUM: %x\n", sum);
    
    for (int i = 0; i < 0x200; i++) {
        printf("%c ", buffer[i]);
    }

    printf("\nSATA IDENT LBA SIZE: %lx\n", dev->lba_capacity);
    printf("\nDMA CAPABILITIES1: %x\n", dev->capability1);
    printf("\nDMA CAPABILITIES2: %x\n", dev->capability2);

    printf("Serial: ");
    for (int i = 0; i < 20; i++)
        printf("%c", dev->serial_no[i]);
    printf("\n");
    printf("Model: ");
    for (int i = 0; i < 40; i++)
        printf("%c", dev->model[i]);
    printf("\n");
    
    //fat32_debug();
    printf("VIVO\n");
    while(1);

}