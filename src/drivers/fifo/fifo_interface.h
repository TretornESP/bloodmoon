#ifndef _FIFO_INTERFACE_H
#define _FIFO_INTERFACE_H
#include <stdint.h>
char * create_fifo(uint64_t size);
void destroy_fifo(const char * device);
uint64_t fifo_read(const char * device, uint8_t * buffer, uint32_t skip, uint32_t size);
uint64_t fifo_write(const char * device, uint8_t * buffer, uint32_t skip, uint32_t size);
uint64_t fifo_identify(const char* device);
uint64_t fifo_dev_flush(const char* device);
uint64_t fifo_get_size(const char* device);
#endif