#ifndef _CRC_H
#define _CRC_H
#include <stdint.h>


uint32_t crc32_byte(uint8_t *p, uint32_t bytelength);
void crc32_fill(uint32_t *table);
#endif