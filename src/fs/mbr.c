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

        //For some reason, fdisk initialices attributes as 0, so we have to check 
        //Validity by sector size. I'm guessing a zero size partition must be bogus.
        if (mbr_header->partitions[i].number_of_sectors != 0x0) {
            partitions[i].status = 1;
            partitions[i].type = mbr_header->partitions[i].partition_type;
            partitions[i].lba = mbr_header->partitions[i].lba_partition_start;
            partitions[i].size = mbr_header->partitions[i].number_of_sectors;
            valid_partitions++;
        }
    }

    if (valid_partitions) {
        printf("Found %d valid partitions on %s\n", valid_partitions, disk);
    }

    return valid_partitions;
}