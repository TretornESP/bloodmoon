#include "mbr.h"
#include "vfs.h"
#include "disk_interface.h"
#include "../util/string.h"
#include "../util/printf.h"
#include "../memory/heap.h"
#include "../util/dbgprinter.h"

uint32_t read_mbr(const char* disk, struct partition* partitions) {
    uint8_t mount_buffer[512];
	memset(mount_buffer, 0, 512);
	
	// Read MBR sector at LBA address zero
	if (!disk_read(disk, mount_buffer, 0, 1)) return 0;
    
	// Check the boot signature in the MBR
	if (load16(mount_buffer + MBR_BOOT_SIG) != MBR_BOOT_SIG_VALUE) {
		return 0;
	}
    
    struct mbr_header *mbr_header = (struct mbr_header *)mount_buffer;
    printf("MBR compatible found on %s [sig: %llx, boot sig: %lx\n", disk, mbr_header->disk_signature, mbr_header->signature);
    partitions = (struct partition*)malloc(sizeof(struct partition) * 4);
    memset(partitions, 0, sizeof(struct partition) * 4);
    
    uint32_t valid_partitions = 0;

    for (int i = 0; i < 4; i++) {
        printf("Checking partition %d, RAW: ", i);
        for (int j = 0; j < 16; j++)
            printf("%x ", *((uint8_t*)(&mbr_header->partitions[i]+j)));
        printf("\nAttributes: %x\n", mbr_header->partitions[i].attributes);
        if (mbr_header->partitions[i].attributes == 0x80) {
            partitions[i].status = 1;
            partitions[i].type = mbr_header->partitions[i].partition_type;
            partitions[i].lba = mbr_header->partitions[i].lba_partition_start;
            partitions[i].size = mbr_header->partitions[i].number_of_sectors;
            printf("Partition %d find \n", i);
            valid_partitions++;
        } else if (mbr_header->partitions[i].attributes == 0x00) {
            printf("Discarding partition %d as inactive, attributes were: %x\n", i, mbr_header->partitions[i].attributes);
        }
    }

    return valid_partitions;
}