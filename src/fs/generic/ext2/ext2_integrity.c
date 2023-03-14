#pragma GCC diagnostic ignored "-Wvariadic-macros"

#include "ext2_integrity.h"
#include "ext2_util.h"
#include "ext2_sb.h"
#include "ext2_bg.h"

#include "../../../memory/heap.h"
#include "../../../util/string.h"
#include "../../../util/printf.h"

#define EXT2_MAX_ERRORS 1024
#define EXT2_DELETE_CYCLE 128

struct error_message {
    char message[ERROR_MESSAGE_SIZE];
    char function[ERROR_FUNC_SIZE];
    char file[ERROR_FILE_SIZE];
    uint32_t line;
    uint8_t type;
    uint64_t id;
    struct error_message * next;
};

char debug_base_path[ERROR_FILE_SIZE] = {0};

uint32_t error_count = 0;
uint64_t error_deletion_counter = 0;
uint64_t error_id = 0;

const char* error_type_names[] = {
    "DEBUG",
    "\x1B[34mINFO\x1B[0m ",
    "\x1B[33mWARN\x1B[0m ",
    "\x1B[31mERROR\x1B[0m"
};

struct error_message * error_messages = 0;

void ext2_set_debug_base_path(const char* path) {
    if (strlen(path) > ERROR_FILE_SIZE) {
        printf("[EXT2] Path too long for debug base path\n");
        return;
    }

    strcpy(debug_base_path, path);
}

char* ext2_format_message(const char* error, ...) {
    va_list args;
    va_start(args, error);
    char * buffer = malloc(ERROR_MESSAGE_SIZE);
    if (buffer == 0) {
        printf("[EXT2] Failed to allocate memory for error message\n");
        return 0;
    }
    vsnprintf(buffer, ERROR_MESSAGE_SIZE, error, args);
    va_end(args);
    return buffer;
}

void ext2_subtract_base_path(char* path, char* result) {
    if (strlen(path) <= strlen(debug_base_path)) {
        return;
    }

    if (strncmp(path, debug_base_path, strlen(debug_base_path)) == 0) {
        strcpy(result, path+strlen(debug_base_path));
    }
}

void ext2_clear_oldest_error() {
    if (error_messages == 0) {
        return;
    }

    struct error_message * message = error_messages;
    struct error_message * prev = 0;
    while (message->next != 0) {
        prev = message;
        message = message->next;
    }

    if (prev == 0) {
        error_messages = 0;
    } else {
        prev->next = 0;
    }

    free(message);
}

void ext2_make_space() {
    
    for (uint32_t i = 0; i < EXT2_DELETE_CYCLE; i++) {
        ext2_clear_oldest_error();
    }

    error_count -= EXT2_DELETE_CYCLE;
    error_deletion_counter++;
    EXT2_WARN("Error count exceeded %d, deleting oldest %d errors", EXT2_MAX_ERRORS, EXT2_DELETE_CYCLE);

    if (error_deletion_counter >= 0xFFFFFFFFFFFFFFFF/EXT2_DELETE_CYCLE) {
        EXT2_WARN("Error deletion counter overflowed, resetting to 0");
        error_deletion_counter = 0;
    }
}

uint64_t ext2_get_error_deletion_counter() {
    return error_deletion_counter*EXT2_DELETE_CYCLE;
}

void ext2_add_error(char * error, const char* function, char* file, uint32_t line, uint8_t type) {

    if (error_count >= EXT2_MAX_ERRORS) {
        ext2_make_space();
    }

    struct error_message * message = malloc(sizeof(struct error_message));
    if (message == 0) {
        printf("[EXT2] Failed to allocate memory for error message\n");
        free(error);
        return;
    }

    if (strlen(error) > ERROR_MESSAGE_SIZE) {
        strcpy(message->message, "Error message too long");
    } else {
        strcpy(message->message, error);
    }

    if (strlen(function) > ERROR_FUNC_SIZE) {
        strcpy(message->function, "Function name too long");
    } else {
        strcpy(message->function, function);
    }


    if ((strlen(file) - strlen(debug_base_path)) > ERROR_FILE_SIZE) {
        strcpy(message->file, "File name too long");
    } else {
        ext2_subtract_base_path(file, message->file);
    }    

    message->type = type;
    message->line = line;
    message->next = error_messages;
    message->id = error_id;
    error_id++;
    error_messages = message;

    free(error);
}

uint8_t ext2_has_errors(uint8_t min_level) {
    if (error_messages == 0) {
        return 0;
    }

    struct error_message * message = error_messages;
    while (message != 0) {
        if (message->type >= min_level) {
            return 1;
        }
        message = message->next;
    }

    return 0;
}

void ext2_print_errors(uint8_t min_level) {
    if (error_messages == 0) {
        return;
    }

    struct error_message * message = error_messages;
    while (message != 0) {
        if (message->type >= min_level) {
            printf("[EXT2] [%s] %-128s [%s@%u:%s]\n", error_type_names[message->type], message->message, message->file, message->line, message->function);
        }
        message = message->next;
    }
}

void ext2_clear_errors() {
    struct error_message * message = error_messages;
    while (message != 0) {
        struct error_message * next = message->next;
        free(message);
        message = next;
    }
    error_messages = 0;
}

void ext2_flush_required(struct ext2_partition * partition) {
    partition->flush_required = 1;
}

void ext2_flush_partition(struct ext2_partition * partition) {
    EXT2_DEBUG("Flushing structures for partition %s", partition->name);
    ext2_operate_on_bg(partition, ext2_flush_bg);
    ext2_operate_on_bg(partition, ext2_flush_sb);
    EXT2_DEBUG("Flushed structures for partition %s", partition->name);
    partition->flush_required = 0;
}

void ext2_flush_all_partitions() {
    uint32_t partitions = ext2_count_partitions();
    for (uint32_t i = 0; i < partitions; i++) {
        struct ext2_partition * partition = ext2_get_partition_by_index(i);
        if (partition->flush_required) {
            ext2_flush_partition(partition);
        }
    }
}

uint8_t ext2_is_flush_required(struct ext2_partition * partition) {
    return partition->flush_required;
}

uint8_t ext2_is_any_flush_required() {
    uint32_t partitions = ext2_count_partitions();
    for (uint32_t i = 0; i < partitions; i++) {
        struct ext2_partition * partition = ext2_get_partition_by_index(i);
        if (partition->flush_required) {
            return 1;
        }
    }
    return 0;
}