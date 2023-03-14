//Supress -Waddress-of-packed-member warning
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
//Supress  warning: ISO C99 requires at least one argument for the "..." in a variadic macro
#pragma GCC diagnostic ignored "-Wvariadic-macros"

#include "ext2.h"
#include "ext2_block.h"
#include "ext2_util.h"
#include "ext2_inode.h"
#include "ext2_bg.h"
#include "ext2_sb.h"
#include "ext2_dentry.h"
#include "ext2_integrity.h"

#include "../../../drivers/disk/disk_interface.h"

#include "../../../memory/heap.h"
#include "../../../util/string.h"
#include "../../../util/printf.h"

struct ext2_partition * ext2_partition_head = 0x0;
uint8_t flushing_required = 0;

#define EXT2_DENTRY_TRANSLATOR_INDEX 0
#define EXT2_INODE_TRANSLATOR_INDEX 1
const uint32_t file_type_translator[8][2] = {
    {EXT2_DIR_TYPE_UNKNOWN, 0},
    {EXT2_DIR_TYPE_REGULAR, INODE_TYPE_FILE},
    {EXT2_DIR_TYPE_DIRECTORY, INODE_TYPE_DIR},
    {EXT2_DIR_TYPE_CHARDEV, INODE_TYPE_CHARDEV},
    {EXT2_DIR_TYPE_BLOCKDEV, INODE_TYPE_BLOCKDEV},
    {EXT2_DIR_TYPE_FIFO, INODE_TYPE_FIFO},
    {EXT2_DIR_TYPE_SOCKET, INODE_TYPE_SOCKET},
    {EXT2_DIR_TYPE_SYMLINK, INODE_TYPE_SYMLINK}
};
char * ext2_file_type_names[8] = {
    "unknown",
    "regular",
    "directory",
    "chardev",
    "blockdev",
    "fifo",
    "socket",
    "symlink"
};

#define EXT2_TRANSLATE_UNIT(native, unit) ((file_type_translator[native][unit]))
#define EXT2_TRANSLATE_DENTRY(native) (EXT2_TRANSLATE_UNIT(native, EXT2_DENTRY_TRANSLATOR_INDEX))
#define EXT2_TRANSLATE_INODE(native) (EXT2_TRANSLATE_UNIT(native, EXT2_INODE_TRANSLATOR_INDEX))

//Returns EXT2_RESULT_ERROR on error, struct ext2_partition * on success
struct ext2_partition * ext2_register_partition(const char* disk, uint32_t lba) {
    EXT2_INFO("Registering partition on disk %s at LBA %d", disk, lba);

    if (!ext2_check_status(disk)) {
        EXT2_WARN("Disk is not ready");
        return EXT2_RESULT_ERROR;
    }

    uint32_t sector_size;
    ioctl_disk(disk, IOCTL_GET_SECTOR_SIZE, &sector_size);

    EXT2_DEBUG("Disk %s is ready, sector size is %d", disk, sector_size);

    uint8_t superblock_buffer[1024];
    if (read_disk(disk, superblock_buffer, lba+SB_OFFSET_LBA, 2)) {
        EXT2_ERROR("Failed to read superblock");
        return EXT2_RESULT_ERROR;
    }

    struct ext2_superblock * superblock = (struct ext2_superblock*)superblock_buffer;
    if (superblock->s_magic != EXT2_SUPER_MAGIC) {
        EXT2_ERROR("Invalid superblock magic");
        return EXT2_RESULT_ERROR;
    }

    uint32_t block_size = 1024 << superblock->s_log_block_size;
    uint32_t sectors_per_block = DIVIDE_ROUNDED_UP(block_size, sector_size);
    EXT2_DEBUG("First superblock found at LBA %d", lba+SB_OFFSET_LBA);
    EXT2_DEBUG("Superblock magic valid, ext2 version: %d", superblock->s_rev_level);
    EXT2_DEBUG("Block size is %d", block_size);
    EXT2_DEBUG("Sectors per block: %d", sectors_per_block);
    EXT2_DEBUG("Blocks count: %d", superblock->s_blocks_count);
    EXT2_DEBUG("Inodes count: %d", superblock->s_inodes_count);
    EXT2_DEBUG("Blocks per group: %d", superblock->s_blocks_per_group);
    EXT2_DEBUG("Inodes per group: %d", superblock->s_inodes_per_group);

    uint8_t bgdt_block = (block_size == 1024) ? 2 : 1;

    uint32_t block_groups_first  = DIVIDE_ROUNDED_UP(superblock->s_blocks_count, superblock->s_blocks_per_group);
    uint32_t block_groups_second = DIVIDE_ROUNDED_UP(superblock->s_inodes_count, superblock->s_inodes_per_group);

    if (block_groups_first != block_groups_second) {
        EXT2_ERROR("block_groups_first != block_groups_second");
        return EXT2_RESULT_ERROR;
    }

    EXT2_DEBUG("Block groups: %d", block_groups_first);
    EXT2_DEBUG("Checking if sb is valid in all block groups...");
    uint32_t blocks_per_group = superblock->s_blocks_per_group;
    uint32_t sectors_per_group = blocks_per_group * sectors_per_block;
    uint8_t dummy_sb_buffer[1024];
    for (uint32_t i = 0; i < block_groups_first; i++) {
        if (read_disk(disk, dummy_sb_buffer, lba+(i*sectors_per_group)+SB_OFFSET_LBA, 2)) {
            EXT2_ERROR("Failed to read dummy superblock");
            return EXT2_RESULT_ERROR;
        }
        struct ext2_superblock * dummy_sb = (struct ext2_superblock*)dummy_sb_buffer;
        if (dummy_sb->s_magic != EXT2_SUPER_MAGIC) {
            EXT2_ERROR("Invalid dummy superblock magic");
            return EXT2_RESULT_ERROR;
        }
    }
    EXT2_DEBUG("All %d superblocks are valid", block_groups_first);
    //TODO: Delete this sanity check
    uint32_t block_group_descriptors_size = DIVIDE_ROUNDED_UP(block_groups_first * sizeof(struct ext2_block_group_descriptor), sector_size);
    EXT2_DEBUG("Block group descriptors size: %d", block_group_descriptors_size);
    //TODO: End of sanity check

    void * block_group_descriptor_buffer = malloc(block_group_descriptors_size * sector_size);
    
    if (read_disk(disk, (uint8_t*)block_group_descriptor_buffer, lba+(sectors_per_block*bgdt_block), block_group_descriptors_size)) {
        EXT2_ERROR("Failed to read block group descriptor table");
        return EXT2_RESULT_ERROR;
    }
    struct ext2_block_group_descriptor * block_group_descriptor = (struct ext2_block_group_descriptor*)block_group_descriptor_buffer;

    EXT2_DEBUG("Registering partition %s:%d", disk, lba);

    struct ext2_partition * partition = ext2_partition_head;
    uint32_t partition_id = 0;

    if (partition == 0) {
        ext2_partition_head = malloc(sizeof(struct ext2_partition));
        partition = ext2_partition_head;
    } else {
        while (partition->next != 0) {
            partition = partition->next;
            partition_id++;
        }
        partition->next = malloc(sizeof(struct ext2_partition));
        partition = partition->next;
    }

    snprintf(partition->name, 32, "%sp%d", disk, partition_id);
    snprintf(partition->disk, 32, "%s", disk);
    partition->group_number = block_groups_first;
    partition->lba = lba;
    partition->sector_size = sector_size;
    partition->sb = malloc(1024);
    partition->flush_required = 0;
    partition->sb_block = SB_OFFSET_LBA;
    partition->bgdt_block = bgdt_block; 
    memcpy(partition->sb, superblock, 1024);
    partition->gd = malloc(block_group_descriptors_size * sector_size);
    memcpy(partition->gd, block_group_descriptor, block_group_descriptors_size * sector_size);

    EXT2_DEBUG("Partition %s has: %d groups", partition->name, block_groups_first);

    return partition;
}

uint8_t ext2_sync(struct ext2_partition * partition) {
    EXT2_INFO("Syncing partition %s", partition->name);
    ext2_flush_partition(partition);
    return EXT2_RESULT_OK;
}

//Returns number of partitions, cannot fail
uint32_t ext2_count_partitions() {
    EXT2_INFO("Counting partitions");
    uint32_t count = 0;
    struct ext2_partition * partition = ext2_partition_head;

    while (partition != 0) {
        count++;
        partition = partition->next;
    }
    return count;
}

//Returns struct ext2_partition *, on error returns EXT2_RESULT_ERROR
struct ext2_partition * ext2_get_partition_by_index(uint32_t index) {
    EXT2_INFO("Getting partition by index %d", index);
    struct ext2_partition * partition = ext2_partition_head;
    uint32_t partition_id = 0;

    while (partition != 0) {
        if (partition_id == index) {
            return partition;
        }
        partition = partition->next;
        partition_id++;
    }

    return EXT2_RESULT_ERROR;
}

uint8_t ext2_search(const char* name, uint32_t lba) {
    EXT2_INFO("Searching for ext2 partition %s:%d", name, lba);
    uint8_t bpb[1024];
    if (read_disk(name, bpb, lba+2, 2)) return EXT2_RESULT_ERROR;

    struct ext2_superblock *sb = (struct ext2_superblock *)bpb;
    return (sb->s_magic == EXT2_SUPER_MAGIC) ? EXT2_RESULT_OK : EXT2_RESULT_ERROR;
}

uint8_t ext2_unregister_partition(char letter) {
    EXT2_INFO("Unregistering partition %c", letter);
    (void)letter;
    EXT2_WARN("unregstr_ext2_partition Not implemented");
    return EXT2_RESULT_ERROR;
}

//Returns uint64, on error returns EXT2_RESULT_ERROR
uint64_t ext2_get_file_size(struct ext2_partition* partition, const char* path) {
    EXT2_INFO("Getting file size of %s", path);
    uint32_t inode_number = ext2_path_to_inode(partition, path);
    if (inode_number == EXT2_RESULT_ERROR) {
        return EXT2_RESULT_ERROR;
    }

    struct ext2_inode_descriptor_generic * inode = (struct ext2_inode_descriptor_generic *)ext2_read_inode(partition, inode_number);
    if (inode == EXT2_RESULT_ERROR) {
        return EXT2_RESULT_ERROR;
    }

    return inode->i_size;
}

//Returns partition name, cannot fail
const char * ext2_get_partition_name(struct ext2_partition * partition) {
    EXT2_INFO("Getting partition name");
    return partition->name;
}

uint8_t ext2_set_debug_base(const char* base) {
    EXT2_INFO("Setting debug base path to %s", base);
    ext2_set_debug_base_path(base);
    return EXT2_RESULT_OK;
}

uint8_t ext2_list_directory(struct ext2_partition* partition, const char * path) {
    EXT2_INFO("Listing directory %s", path);
    ext2_list_dentry(partition, path);
    return EXT2_RESULT_OK;
}

uint8_t ext2_create_file(struct ext2_partition * partition, const char* path, uint32_t type, uint32_t permissions) {
    EXT2_INFO("Creating file %s of type %s", path, ext2_file_type_names[type]);
    uint32_t block_size = 1024 << (((struct ext2_superblock*)partition->sb)->s_log_block_size);

    char * name;
    char * parent_path;
    if (!ext2_path_to_parent_and_name(path, &parent_path, &name)) {
        EXT2_WARN("Invalid path");
        return EXT2_RESULT_ERROR;
    }

    uint32_t parent_inode_index = ext2_path_to_inode(partition, parent_path);
    if (!parent_inode_index) {
        EXT2_WARN("Parent directory doesn't exist");
        return EXT2_RESULT_ERROR;
    }

    struct ext2_inode_descriptor_generic * parent_inode = (struct ext2_inode_descriptor_generic *)ext2_read_inode(partition, parent_inode_index);
    if (!(parent_inode->i_mode & INODE_TYPE_DIR)) {
        EXT2_WARN("Parent inode is not a directory");
        return EXT2_RESULT_ERROR;
    }

    EXT2_DEBUG("Parent inode is a directory, ready to allocate");

    uint32_t new_inode_index = ext2_allocate_inode(partition);
    if (!new_inode_index) {
        EXT2_ERROR("Failed to allocate inode");
        return EXT2_RESULT_ERROR;
    }

    EXT2_DEBUG("Allocated inode %d", new_inode_index);
    struct ext2_inode_descriptor * inode = ext2_initialize_inode(partition, new_inode_index, EXT2_TRANSLATE_INODE(type), permissions);
    EXT2_DEBUG("Initialized inode %d", new_inode_index);

    if (ext2_write_inode(partition, new_inode_index, inode)) {
        EXT2_ERROR("Failed to write inode");
        return EXT2_RESULT_ERROR;
    }

    if (type == EXT2_FILE_TYPE_DIRECTORY) {
        EXT2_DEBUG("Creating directory");
        if (ext2_resize_file(partition, new_inode_index, block_size) != EXT2_RESULT_OK) {
            EXT2_ERROR("Failed to resize file");
            return EXT2_RESULT_ERROR;
        }

        ext2_flush_partition(partition);

        if (ext2_initialize_directory(partition, new_inode_index, parent_inode_index)) {
            EXT2_ERROR("Failed to initialize directory");
            return EXT2_RESULT_ERROR;
        }
    }

    EXT2_DEBUG("Creating directory entry for inode %d", new_inode_index);
    if (ext2_create_directory_entry(partition, parent_inode_index, new_inode_index, name, EXT2_TRANSLATE_DENTRY(type))) {
        EXT2_ERROR("Failed to create directory entry");
        return EXT2_RESULT_ERROR;
    }
    ext2_list_directory(partition, path);

    return EXT2_RESULT_OK;
}

uint8_t ext2_resize_file(struct ext2_partition* partition, uint32_t inode_index, uint32_t new_size) {
    EXT2_INFO("Resizing file %d to %d", inode_index, new_size);
    struct ext2_inode_descriptor_generic * inode = (struct ext2_inode_descriptor_generic *)ext2_read_inode(partition, inode_index);
    uint32_t block_size = 1024 << (((struct ext2_superblock*)partition->sb)->s_log_block_size);
    if (!inode) {
        EXT2_ERROR("Failed to read inode");
        return EXT2_RESULT_ERROR;
    }

    if (inode->i_size == new_size) {
        EXT2_WARN("File is already of the requested size");
        return EXT2_RESULT_OK;
    }

    if (inode->i_size > new_size) {
        uint32_t blocks_to_deallocate = (inode->i_size - new_size) / block_size;

        EXT2_DEBUG("Resizing file to %d bytes, deallocating %d blocks", new_size, blocks_to_deallocate);
        uint32_t * blocks = ext2_load_block_list(partition, inode_index);
        if (!blocks) {
            EXT2_ERROR("Failed to load block list");
            return EXT2_RESULT_ERROR;
        }

        if (ext2_deallocate_blocks(partition, blocks, blocks_to_deallocate) != blocks_to_deallocate) {
            EXT2_ERROR("Failed to deallocate blocks");
            return EXT2_RESULT_ERROR;
        }
    } else {
        uint32_t blocks_to_allocate = (new_size - inode->i_size) / block_size;
        if ((new_size - inode->i_size) % block_size) blocks_to_allocate++;

        EXT2_DEBUG("Resizing file to %d bytes, allocating %d blocks", new_size, blocks_to_allocate);
        if (ext2_allocate_blocks(partition, inode, blocks_to_allocate)) {
            EXT2_ERROR("Failed to allocate blocks");
            return EXT2_RESULT_ERROR;
        }   
    }

    inode->i_size = new_size;
    inode->i_sectors = DIVIDE_ROUNDED_UP(new_size, partition->sector_size);

    if (ext2_write_inode(partition, inode_index, (struct ext2_inode_descriptor*)inode)) {
        EXT2_ERROR("Failed to write inode");
        return EXT2_RESULT_ERROR;
    }

    ext2_flush_partition(partition);
    return EXT2_RESULT_OK;
}

uint8_t ext2_read_file(struct ext2_partition * partition, const char * path, uint8_t * destination_buffer, uint64_t size, uint64_t skip) {
    EXT2_INFO("Reading file %s", path);

    uint32_t inode_index = ext2_path_to_inode(partition, path);

    if (!inode_index) {
        EXT2_WARN("Failed to find inode");
        return EXT2_RESULT_ERROR;
    }

    struct ext2_inode_descriptor_generic * inode = (struct ext2_inode_descriptor_generic *)ext2_read_inode(partition, inode_index);
    if (inode->i_mode & INODE_TYPE_DIR) {
        EXT2_WARN("Trying to read a directory");
        return EXT2_RESULT_ERROR;
    }

    if ((skip+size) > inode->i_size) {
        EXT2_WARN("Trying to read past the end of the file[skip=%d, size=%d, file_size=%d]", skip, size, inode->i_size);
        return EXT2_RESULT_ERROR;
    }


    int64_t read_bytes = ext2_read_inode_bytes(partition, inode_index, destination_buffer, size, skip);
    if (read_bytes == EXT2_READ_FAILED) {
        EXT2_ERROR("File read failed");
        return EXT2_RESULT_ERROR;
    }

    if (read_bytes <= 0) {
        EXT2_ERROR("file read failed");
        return EXT2_RESULT_ERROR;
    }

    if ((uint64_t)read_bytes != size) {
        EXT2_ERROR("Read %d bytes, expected %d", read_bytes, size);
    }

    return EXT2_RESULT_OK;
}

uint32_t ext2_get_inode_index(struct ext2_partition* partition, const char* path) {
    EXT2_INFO("Getting inode index for %s", path);
    uint32_t inode_index = ext2_path_to_inode(partition, path);
    if (!inode_index) {
        EXT2_WARN("Failed to find inode");
        return EXT2_RESULT_ERROR;
    }

    struct ext2_inode_descriptor_generic * inode = (struct ext2_inode_descriptor_generic *)ext2_read_inode(partition, inode_index);
    if (inode == 0) {
        EXT2_ERROR("Failed to read inode");
        return EXT2_RESULT_ERROR;
    }

    return inode_index;
}

uint8_t ext2_write_file(struct ext2_partition * partition, const char * path, uint8_t * source_buffer, uint64_t size, uint64_t skip) {
    EXT2_INFO("Writing file %s", path);

    uint32_t inode_index = ext2_path_to_inode(partition, path);
    if (!inode_index) {
        EXT2_WARN("File doesn't exist");
        return EXT2_RESULT_ERROR;
    }
    
    EXT2_DEBUG("File created, trying to get inode");
    inode_index = ext2_path_to_inode(partition, path);
    if (!inode_index) {
        EXT2_ERROR("Failed to get file inode");
        return EXT2_RESULT_ERROR;
    }

    struct ext2_inode_descriptor_generic * inode = (struct ext2_inode_descriptor_generic *)ext2_read_inode(partition, inode_index);
    if (inode->i_mode & INODE_TYPE_DIR) {
        EXT2_ERROR("Trying to write to a directory");
        return EXT2_RESULT_ERROR;
    }

    if ((size + skip) > inode->i_size) {
        EXT2_DEBUG("File %s is too small, resizing", path);
        if (ext2_resize_file(partition, inode_index, size + skip) != EXT2_RESULT_OK) {
            EXT2_ERROR("Failed to resize file");
            return EXT2_RESULT_ERROR;
        }
    }

    int64_t write_bytes = ext2_write_inode_bytes(partition, inode_index, source_buffer, size, skip);
    if (write_bytes == EXT2_WRITE_FAILED) {
        EXT2_ERROR("File write failed");
        return EXT2_RESULT_ERROR;
    }

    if (write_bytes <= 0) {
        EXT2_ERROR("file write failed");
        return EXT2_RESULT_ERROR;
    }

    return EXT2_RESULT_OK;
}

uint8_t ext2_delete_file(struct ext2_partition* partition, const char * path) {
    EXT2_INFO("Deleting file %s", path);
    uint32_t inode_index = ext2_path_to_inode(partition, path);
    EXT2_DEBUG("Deleting file %s, inode %d", path, inode_index);
    if (!inode_index) {
        EXT2_WARN("File doesn't exist");
        return EXT2_RESULT_ERROR;
    }

    EXT2_DEBUG("Deleting file blocks");
    if (ext2_delete_file_blocks(partition, inode_index)) {
        EXT2_ERROR("Failed to deallocate blocks");
        return EXT2_RESULT_ERROR;
    }

    EXT2_DEBUG("Deleting file inode");
    if (ext2_delete_inode(partition, inode_index)) {
        EXT2_ERROR("Failed to delete inode");
        return EXT2_RESULT_ERROR;
    }

    EXT2_DEBUG("Deleting file dentry");
    if (ext2_delete_dentry(partition, path)) {
        EXT2_ERROR("Failed to delete dentry");
        return EXT2_RESULT_ERROR;
    }

    EXT2_DEBUG("Flushing partition");
    ext2_flush_partition(partition);

    return EXT2_RESULT_OK;
}

uint8_t ext2_debug(struct ext2_partition* partition) {
    //ext2_dump_partition(partition);
    //ext2_dump_sb(partition);
    //ext2_dump_all_bgs(partition);
    ext2_dump_all_inodes(partition, "/"); //TODO: Get the root inode name from somewhere
    return EXT2_RESULT_OK;
}

uint8_t ext2_stacktrace() {
    EXT2_INFO("Printing stacktrace");
    if (ext2_has_errors(EXT2_ERROR_DEBUG)) {
        printf("[EXT2] Stacktrace requested\n");
        ext2_print_errors(EXT2_ERROR_DEBUG);
        ext2_clear_errors();
    } else {
        printf("[EXT2] No errors\n");
    }

    return EXT2_RESULT_OK;
}

uint8_t ext2_errors() {
    return ext2_has_errors(EXT2_ERROR_ERROR);
}

uint16_t ext2_get_file_permissions(struct ext2_partition* partition, const char* path) {
    uint32_t inode_index = ext2_path_to_inode(partition, path);
    if (!inode_index) {
        EXT2_WARN("File doesn't exist");
        return EXT2_RESULT_ERROR;
    }

    struct ext2_inode_descriptor_generic * inode = (struct ext2_inode_descriptor_generic *)ext2_read_inode(partition, inode_index);
    if (inode->i_mode & INODE_TYPE_DIR) {
        EXT2_ERROR("Trying to get permissions of a directory");
        return EXT2_RESULT_ERROR;
    }

    return (inode->i_mode & 0x1FF);
}

/*
Required functions:

create_file ok
read_file ok (no skip)
write_file ok (no skip) (no shrinking)
delete_file ok
modify_file_info (copy + delete)
create_directory ok
delete_directory ok
read_directory ok
modify_directory_info (copy + delete)

create_link
delete_link
*/