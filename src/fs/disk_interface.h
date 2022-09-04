//------------------------------------------------------------------------------
// MIT license
// Copyright (c) 2020 StrawberryHacker
//------------------------------------------------------------------------------

#ifndef DISK_INTERFACE_H
#define DISK_INTERFACE_H

#include <stdint.h>

/// Add new physical disk here
typedef enum {
	DISK_SD_CARD,
	DISK_SATA_DRIVE,
	DISK_USB_DRIVE
} disk_e;

/// Returns the status of the MSD (mass storage device)
uint8_t disk_get_status(disk_e disk);

/// Initializes at disk intrface
uint8_t disk_initialize(disk_e disk);

/// Read a number of sectors from the MSD
uint8_t disk_read(disk_e disk, uint8_t* buffer, uint32_t lba, uint32_t count);

/// Write a number of sectors to the MSD
uint8_t disk_write(disk_e disk, uint8_t* buffer, uint32_t lba, uint32_t count);

#endif