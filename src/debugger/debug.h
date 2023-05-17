#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>
#include <stdint.h>

#define MSG_MAX_LEN 1024
#define MSG_BUFFER_LEN 4096

struct buffer {
    char* data;
    int size;
    int pos;
    struct buffer* next;
};

void vdbg(const char* format, va_list va);
uint8_t dbg(const char* format, ...);
void dbg_init(const char* device);
int dbg_is_present();
const char * dbg_get_device();
uint8_t dbg_flush();
#endif