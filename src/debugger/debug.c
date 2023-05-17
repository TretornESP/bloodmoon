#include "debug.h"
#include "../util/printf.h"
#include "../memory/heap.h"
#include "../util/string.h"
#include "../dev/devices.h"

struct buffer * dbg_buffer = 0;
char dbg_device[32];
int started = 0;

struct buffer* allocate() {
    struct buffer* buffer = (struct buffer*)malloc(sizeof(struct buffer));
    buffer->data = malloc(MSG_BUFFER_LEN);
    buffer->size = MSG_BUFFER_LEN;
    buffer->pos = 0;
    buffer->next = 0;
    return buffer;
}

void add_message(char * msg) {
    if (msg == 0) {
        return;
    }

    if (dbg_buffer == 0) {
        dbg_buffer = allocate();
        strncpy(dbg_buffer->data, "Starting debug session...\n", 26);
        dbg_buffer->pos = 26;
    }

    struct buffer* current = dbg_buffer;
 
    while (current->next != 0) {
        current = current->next;
    }

    if ((uint64_t)current->pos + strlen(msg) >= (uint64_t)current->size) {
        current->next = allocate();
        current = current->next;
    }

    strcpy(current->data + current->pos, msg);
    current->pos += strlen(msg);
}

void clean() {
    struct buffer* current = dbg_buffer;
    while (current != 0) {
        struct buffer* next = current->next;
        if (current->data) free(current->data);
        free(current);
        current = next;
    }
    dbg_buffer = 0;
}

void vdbg(const char* format, va_list va) {
    if (!heap_safeguard()) return;
    char* buffer = (char*)calloc(1, MSG_MAX_LEN);
    if (!buffer) return;

    vsnprintf(buffer, MSG_MAX_LEN, format, va);

    add_message(buffer);

    free(buffer);
}

uint8_t dbg(const char* format, ...) {
    if (!heap_safeguard()) return 0;
    char* buffer = (char*)calloc(1, MSG_MAX_LEN);
    if (!buffer) return 0;

    va_list va;
    va_start(va, format);
    vsnprintf(buffer, MSG_MAX_LEN, format, va);
    va_end(va);

    add_message(buffer);

    free(buffer);
    return 1;
}

const char * dbg_get_device() {
    return dbg_device;
}

int dbg_is_present() {
    return started;
}

void dbg_init(const char* device) {
    if (device == 0) {
        return;
    }

    struct device * dev = device_search(device);
    if (dev == 0) {
        return;
    }
    strncpy(dbg_device, device, 32);
    
    started = 1;
}

uint8_t dbg_flush() {
    if (!heap_safeguard()) return 0;

    if (dbg_buffer == 0) {
        return 0;
    }

    struct buffer* current = dbg_buffer;
    while (current != 0) {
        if (current->pos > 0) {
            device_write(dbg_device, current->pos, 0, (uint8_t*)current->data);
            device_ioctl(dbg_device, 0x5, 0); //Flush
        }
        current = current->next;
    }

    clean();
    return 1;
}