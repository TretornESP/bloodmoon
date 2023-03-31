//------------------------------------------------------------------------------
// MIT license
// Copyright (c) 2020 StrawberryHacker
//------------------------------------------------------------------------------

#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

/// Most of the FAT32 file system functions returns one of these status codes
typedef enum {
	FSTATUS_OK,
	FSTATUS_ERROR,
	FSTATUS_NO_VOLUME,
	FSTATUS_PATH_ERR,
	FSTATUS_EOF
} fstatus;

struct volume_s {
	struct volume_s* next;
	
	// The first label is 11-bytes and located in the BPB, while the sectondary
	// label is introduced in the root directory. The BPB label contains 13 
	// characters while the root label can contain 13 characters.
	char label[13];
	char letter;
	
	// FAT32 info
	uint16_t sector_size;
	uint8_t cluster_size;
	uint32_t total_size;
	uint32_t fat_lba;
	uint32_t fsinfo_lba;
	uint32_t data_lba;
	uint32_t root_lba;
	
	// All file system operations require a 512-byte buffer for storing the
	// current sector.
	uint8_t buffer[512];
	uint32_t buffer_lba;
	char disk[32];
	uint32_t buffer_dirty;
	
	char lfn[256];
	uint8_t lfn_size;
	
};

struct dir_s {
	uint32_t sector;
	uint32_t cluster;
	uint32_t rw_offset;
	
	uint32_t start_sect;
	uint32_t size;
	struct volume_s* vol;
};

struct file_s {
	uint32_t sector;
	uint32_t cluster;
	uint32_t rw_offset;
	uint32_t size;
	uint32_t start_sect;
	uint32_t glob_offset;
	struct volume_s* vol;
};

/// This structure will contain all information needed for a file or a folder. 
/// It is mainly used to read directory entries from a path
struct info_s {
	
	// By default this code supports long file name entries (LFN) up to 
	// 256 characters. The same buffer will be used for LFN and SFN entries.
	char name[256];
	uint8_t name_length;
	
	// The attribute field apply to a file or a folder
	// 
	// Bit 0 - Read-only
	// Bit 1 - Hidden
	// Bit 2 - System (do not mess with these directories)
	// Bit 3 - Volume label
	// Bit 4 - Subdirectory
	// Bit 5 - Archeive
	// Bit 6 - Device
	uint8_t	attribute;
	
	// Time and date properties
	uint8_t	c_time_tenth;
	uint16_t c_time;
	uint16_t c_date;
	uint16_t a_date;
	uint16_t w_time;
	uint16_t w_date;
	
	// Contains the total size of a file or a folder
	// A folder has 
	uint32_t size;
};

/// The classical generic MBR located at sector zero at a MSD contains four 
/// partition fields. This structure describe one partition. 
struct partition_s {
	uint32_t lba;
	uint32_t size;
	uint8_t status;
	uint8_t type;
};

/// Format structure
struct fat_fmt_s {
	uint32_t allocation_size;
	uint32_t allignment;
	uint32_t quick_format;
};

//------------------------------------------------------------------------------
// Microsoft FAT32 spesification
// Due to Microsoft releasing the licensing on the FAT LFN usage, this code will 
// use LFN instead of SFN. It will not have SFN support since it is not meant
// for smaller systems.
//------------------------------------------------------------------------------

/// Old BPB and BS
#define BPB_JUMP_BOOT		0
#define BPB_OEM				3
#define BPB_SECTOR_SIZE		11
#define BPB_CLUSTER_SIZE	13
#define BPB_RSVD_CNT		14
#define BPB_NUM_FATS		16
#define BPB_ROOT_ENT_CNT	17
#define BPB_TOT_SECT_16		19
#define BPB_MEDIA			21
#define BPB_FAT_SIZE_16		22
#define BPB_SEC_PER_TRACK	24
#define BPB_NUM_HEADS		26
#define BPB_HIDD_SECT		28
#define BPB_TOT_SECT_32		32

/// New BPB and BS applying for FAT12 and FAT16
#define BPB_16_DRV_NUM		36
#define BPB_16_RSVD1		37
#define BPB_16_BOOT_SIG		38
#define BPB_16_VOL_ID		39
#define BPB_16_VOL_LABEL	43
#define BPB_16_FSTYPE		54

// New BPB and BS applying for FAT32
#define BPB_32_FAT_SIZE		36
#define BPB_32_EXT_FLAGS	40
#define BPB_32_FSV			42
#define BPB_32_ROOT_CLUST	44
#define BPB_32_FSINFO		48
#define BPB_32_BOOT_SECT	50
#define BPB_32_RSVD			52
#define BPB_32_DRV_NUM		64
#define BPB_32_RSVD1		65
#define BPB_32_BOOT_SIG		66
#define BPB_32_VOL_ID		67
#define BPB_32_VOL_LABEL	71
#define BPB_32_FSTYPE		82

/// Directory entry defines
#define SFN_NAME			0
#define SFN_ATTR			11
#define SFN_NTR				12
#define SFN_CTIME_TH		13
#define SFN_CTIME			14
#define SFN_CDATE			16
#define SFN_ADATE			18
#define SFN_CLUSTH			20
#define SFN_WTIME			22
#define SFN_WDATE			24
#define SFN_CLUSTL			26
#define SFN_FILE_SIZE		28

#define LFN_SEQ				0
#define LFN_SEQ_MSK			0x1F
#define LFN_NAME_1			1
#define LFN_ATTR			11
#define LFN_TYPE			12
#define LFN_CRC				13
#define LFN_NAME_2			14
#define LFN_NAME_3			28

#define ATTR_RO				0x01
#define ATTR_HIDD			0x02
#define ATTR_SYS			0x04
#define ATTR_VOL_LABEL		0x08
#define ATTR_DIR			0x10
#define ATTR_ARCH			0x20
#define ATTR_LFN			0x0F

/// FSinfo structure
#define INFO_CLUST_CNT		488
#define INFO_NEXT_FREE		492

/// File system thread
void fat32_debug(const char* disk);

/// Disk functions
char 	register_fat32_partition(const char* disk, uint32_t lba, const char* mountpoint);
uint8_t unregstr_fat32_partition(char letter);
uint8_t disk_eject(const char* disk);
uint8_t fat_search(const char*, uint32_t);

/// Volume functions
struct volume_s* volume_get_first(void);
struct volume_s* volume_get(char letter);
fstatus volume_set_label(struct volume_s* vol, const char* name, uint8_t length);
fstatus volume_get_label(struct volume_s* vol, char* name);
fstatus volume_format(struct volume_s* vol, struct fat_fmt_s* fmt);

/// Directory actions
fstatus fat_dir_open(struct dir_s* dir, const char* path, uint16_t length);
fstatus fat_dir_close(struct dir_s* dir);
fstatus fat_dir_read(struct dir_s* dir, struct info_s* info);
fstatus fat_dir_make(const char* path);

/// File actions
fstatus fat_file_open(struct file_s* file, const char* path, uint16_t length);
fstatus fat_file_close(struct file_s* file);
fstatus fat_file_read(struct file_s* file, uint8_t* buffer, uint32_t count, uint32_t* status);
fstatus fat_file_write(struct file_s* file, const uint8_t* buffer, uint32_t count);
fstatus fat_file_jump(struct file_s* file, uint32_t offset);
fstatus fat_file_flush(struct file_s* file);

/// Directory and file actions
fstatus fat_dir_rename(struct dir_s* dir, const char* name, uint8_t length);
fstatus fat_dir_delete(struct dir_s* dir);
fstatus fat_dir_chmod(struct dir_s* dir, const char* mod);

#endif