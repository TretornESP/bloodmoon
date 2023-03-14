#pragma GCC diagnostic ignored "-Wvariadic-macros"
#include "ext2_partition.h"
#include "ext2_integrity.h"

#include "../../../drivers/disk/disk_interface.h"

#include "../../../util/string.h"
#include "../../../util/printf.h"

void ext2_dump_partition(struct ext2_partition* partition) {
    printf("ext2 partition:\n");
    printf("name: %s\n", partition->name);
    printf("disk: %s\n", partition->disk);
    printf("lba: %u\n", partition->lba);
    printf("group_number: %u\n", partition->group_number);
    printf("sector_size: %u\n", partition->sector_size);
    printf("bgdt_block: %u\n", partition->bgdt_block);
    printf("sb_block: %u\n", partition->sb_block);
    printf("flush_required: %u\n", partition->flush_required);
    printf("sb: %p\n", (void*)partition->sb);
    printf("gd: %p\n", (void*)partition->gd);
    printf("next: %p\n", (void*)partition->next);
}

void ext2_disk_from_partition(char * destination, const char * partition) {
    uint32_t partition_name = strlen(partition);
    //iterate partition backwards to find the last 'p' character
    for (int i = partition_name - 1; i >= 0; i--) {
        if (partition[i] == 'p') {
            //copy the partition name to the destination
            memcpy(destination, partition, i);
            destination[i] = 0;
            return;
        }
    }
}

uint8_t ext2_check_status(const char* disk) {
    int status = get_disk_status(disk);
    EXT2_DEBUG("Checking disk %s status", disk);
    if (status != STATUS_READY) {
        if (init_disk(disk) != OP_SUCCESS) {
            EXT2_WARN("Failed to initialize disk");
            return 0;
        }
        if (get_disk_status(disk) != STATUS_READY) {
            EXT2_WARN("Disk not ready");
            return 0;
        }
    }
    return 1;
}

struct ext2_partition * get_partition(struct ext2_partition* partition, const char * partno) {
    while (partition != 0) {
        if (strcmp(partition->name, partno) == 0) {
            return partition;
        }
        partition = partition->next;
    }
    return 0;
}
