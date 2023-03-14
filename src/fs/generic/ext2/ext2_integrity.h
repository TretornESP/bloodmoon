#ifndef _EXT2_INTEGRITY_H
#define _EXT2_INTEGRITY_H

#include "ext2_partition.h"
#include <stdint.h>
#include <stdarg.h>

#define EXT2_ERROR_DEBUG 0
#define EXT2_ERROR_INFO 1
#define EXT2_ERROR_WARN 2
#define EXT2_ERROR_ERROR 3

#define ERROR_FUNC_SIZE 64
#define ERROR_MESSAGE_SIZE 128
#define ERROR_FILE_SIZE 64

#define EXT2_ERROR(error, ...) ext2_add_error(ext2_format_message(error, ##__VA_ARGS__), __func__, __FILE__, __LINE__, EXT2_ERROR_ERROR)
#define EXT2_WARN(error, ...) ext2_add_error(ext2_format_message(error, ##__VA_ARGS__), __func__, __FILE__, __LINE__, EXT2_ERROR_WARN)
#define EXT2_INFO(error, ...) ext2_add_error(ext2_format_message(error, ##__VA_ARGS__), __func__, __FILE__, __LINE__, EXT2_ERROR_INFO)
#define EXT2_DEBUG(error, ...) ext2_add_error(ext2_format_message(error, ##__VA_ARGS__), __func__, __FILE__, __LINE__, EXT2_ERROR_DEBUG)

char* ext2_format_message(const char* error, ...);
void ext2_set_debug_base_path(const char* path);

void ext2_add_error(char * error, const char* function, char* file, uint32_t line, uint8_t type);
uint8_t ext2_has_errors(uint8_t min_level);
void ext2_print_errors(uint8_t min_level);
void ext2_clear_errors();
uint64_t ext2_get_error_deletion_counter();

void ext2_flush_required(struct ext2_partition * partition);
void ext2_flush_partition(struct ext2_partition * partition);
void ext2_flush_all_partitions();
uint8_t ext2_requires_flush(struct ext2_partition * partition);
uint8_t ext2_something_requires_flush();

#endif /* _EXT2_INTEGRITY_H */