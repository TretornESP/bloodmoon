#include "generic_f32_exp.h"
#include "../../../drivers/disk/disk_interface.h"
#include "../../../util/string.h"
#include "../../../memory/heap.h"

struct fat32_compat_device f32_device_list[MAX_F32_DEVICES] = {0};

char fat32_register_partition(const char* device, uint32_t lba) {
    uint8_t i;
    for (i = 0; i < MAX_F32_DEVICES; i++) {
        if (f32_device_list[i].name[0] == 0) {
            strncpy(f32_device_list[i].name, device, 32);
            f32_device_list[i].lba = lba;
            f32_device_list[i].partition = i;
            break;
        }
    }

    //TODO: Call mount
    return i;
}

uint8_t get_enabled_f32_devices() {
    uint8_t i;
    uint8_t count = 0;
    for (i = 0; i < MAX_F32_DEVICES; i++) {
        if (f32_device_list[i].name[0] != 0) {
            count++;
        }
    }
    return count;
}

struct fat32_compat_device *get_device_at_index(uint8_t index) {
    return (struct fat32_compat_device*)(f32_device_list + (index * sizeof(struct fat32_compat_device)));
}

uint8_t fat32_unregister_partition(char partition) {
    memset(&f32_device_list[(uint8_t)partition], 0, sizeof(struct fat32_compat_device));
    return 1;
}

uint8_t fat32_detect_partition(const char* name, uint32_t lba) {
	uint8_t bpb[512];
    if (!disk_read(name, bpb, lba, 1)) return 0;
	
	// Check the BPB boot signature
	if (load16(bpb + 510) != 0xAA55) return 0;
	
	// A valid FAT file system will have the "FAT" string in either the FAT16
	// boot sector, or in the FAT32 boot sector. This does NOT indicate the
	// FAT file system fype
	if (!memcmp(bpb + BPB_32_FSTYPE, "FAT", 3)) {
		if (!memcmp(bpb + BPB_16_FSTYPE, "FAT", 3)) {
			return 0;
		}
	}
	
	// A FAT12, FAT16 or FAT32 file system is present. The type is determined 
	// by the count of data cluster.
	uint32_t root_sectors = ((load16(bpb + BPB_ROOT_ENT_CNT) * 32) + 
		(load16(bpb + BPB_SECTOR_SIZE) - 1)) / 
		(load16(bpb + BPB_SECTOR_SIZE) - 1);
	
	uint32_t fat_size = (load16(bpb + BPB_FAT_SIZE_16)) ? 
		(uint32_t)load16(bpb + BPB_FAT_SIZE_16) : 
		load32(bpb + BPB_32_FAT_SIZE);
		
	uint32_t tot_sect = (load16(bpb + BPB_TOT_SECT_16)) ? 
		(uint32_t)load16(bpb + BPB_TOT_SECT_16) :
		load32(bpb + BPB_TOT_SECT_32);
		
	uint32_t data_sectors = tot_sect - (load16(bpb + BPB_RSVD_CNT) + 
		(bpb[BPB_NUM_FATS] * fat_size) + root_sectors);
		
	uint32_t data_clusters = data_sectors / bpb[BPB_CLUSTER_SIZE];
	
	// Only FAT32 is supported
	if (data_clusters < 65525) {
		return 0;
	}
	return 1;
}

uint64_t file_open(const char* path, int mode, int flags) {
    FIL* file = malloc(sizeof(FIL));
    FRESULT res = f_open(file, path, mode);
    if (res != FR_OK) {
        free(file);
        return -1;
    }
    return (uint64_t)file;
}

struct vfs_compatible fat32_exp_register = {
    .name = "FAT32",

    .register_partition = fat32_register_partition,
    .unregister_partition = fat32_unregister_partition,
    .detect = fat32_detect_partition,
};

struct vfs_compatible * get_f32_exp_driver() {
    return &fat32_exp_register;
}

