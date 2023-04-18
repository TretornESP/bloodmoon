#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>

#define MSG_MAX_LEN 1024
#define MSG_BUFFER_LEN 4096

struct buffer {
    char* data;
    int size;
    int pos;
    struct buffer* next;
};

void dbg(const char* format, ...);
void dbg_init(const char* device);
int dbg_is_present();
const char * dbg_get_device();
void dbg_flush();
#endif