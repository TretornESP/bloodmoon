//------------------------------------------------------------------------------
// MIT license
// Copyright (c) 2020 StrawberryHacker
//------------------------------------------------------------------------------

#ifndef DISK_INTERFACE_H
#define DISK_INTERFACE_H

#include <stdint.h>

/// Returns the status of the MSD (mass storage device)
uint8_t disk_get_status(const char * disk);

/// Initializes at disk intrface
uint8_t disk_initialize(const char * disk);

/// Read a number of sectors from the MSD
uint8_t disk_read(const char * disk, uint8_t* buffer, uint32_t lba, uint32_t count);

/// Write a number of sectors to the MSD
uint8_t disk_write(const char * disk, uint8_t* buffer, uint32_t lba, uint32_t count);

#endif