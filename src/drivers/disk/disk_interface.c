//------------------------------------------------------------------------------
// MIT license
// Copyright (c) 2020 StrawberryHacker
//------------------------------------------------------------------------------

#include "disk_interface.h"
#include "disk.h"
#include "../../dev/devices.h"

uint8_t disk_get_status(const char * disk) {
	return (device_search(disk) != 0);
}

uint8_t disk_initialize(const char * disk) {
	return (device_ioctl(disk, IOCTL_INIT, 0) != 0);
}

uint8_t disk_read(const char * disk, uint8_t* buffer, uint32_t lba, uint32_t count) {
	return device_read(disk, count, lba, buffer);
}

uint8_t disk_write(const char * disk, uint8_t* buffer, uint32_t lba, uint32_t count) {
	return device_write(disk, count, lba, buffer);
}
