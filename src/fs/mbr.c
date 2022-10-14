#include "mbr.h"
#include "vfs.h"
#include "disk_interface.h"
#include "../util/string.h"
#include "../util/printf.h"
#include "../memory/heap.h"
#include "../util/dbgprinter.h"

void translate_partition(struct mbr_partition* native, struct partition* target) {
    target->lba = native->lba_partition_start;
    target->size = native->number_of_sectors;
    target->type = native->partition_type;
    target->status = 0;
}

uint32_t read_mbr(const char* disk, struct partition* partitions) {
uint8_t mount_buffer[512];
	memset(mount_buffer, 0, 512);
	
	// Read MBR sector at LBA address zero
	if (!disk_read(disk, mount_buffer, 0, 1)) return 0;
    
	// Check the boot signature in the MBR
	if (load16(mount_buffer + MBR_BOOT_SIG) != MBR_BOOT_SIG_VALUE) {
		return 0;
	}

    printf("MBR compatible found on %s\n", disk);
    partitions = (struct partition*)malloc(sizeof(struct partition) * 4);

    struct mbr_partition* mbr_partitions = (struct mbr_partition*)(mount_buffer + MBR_PARTITION);
    uint32_t valid_partitions = 0;

    for (int i = 0; i < 4; i++) {
        if (mbr_partitions[i].attributes == 0x80) {
            printf("Partition %d find \n", i);
            translate_partition(&mbr_partitions[i], &partitions[i]);
            valid_partitions++;
        } else if (mbr_partitions[i].attributes == 0x00) {
            printf("Discarding partition %d as inactive\n", i);
        }
    }

    return valid_partitions;
}