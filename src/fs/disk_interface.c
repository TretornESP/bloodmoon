//------------------------------------------------------------------------------
// MIT license
// Copyright (c) 2020 StrawberryHacker
//------------------------------------------------------------------------------

#include "disk_interface.h"
#include "../dev/devices.h"

uint8_t disk_get_status(disk_e disk) {
	switch (disk) {
		case DISK_SATA_DRIVE:
			return 1;
		case DISK_USB_DRIVE:
			return 0;
		case DISK_SD_CARD:
			return 0;
		break;
	}
	return 0;
}

uint8_t disk_initialize(disk_e disk) {
	switch (disk) {
		case DISK_SATA_DRIVE:
			return 1;
		case DISK_USB_DRIVE:
			return 0;
		case DISK_SD_CARD:
			return 0;
		break;
	}
	return 0;
}

uint8_t disk_read(disk_e disk, uint8_t* buffer, uint32_t lba, uint32_t count) {
	switch (disk) {
		case DISK_SATA_DRIVE:
			return device_read("/dev/sda", count, lba, buffer);
		case DISK_USB_DRIVE:
			return 0;
		case DISK_SD_CARD:
			return 0;
		break;
	}
	return 0;
}

uint8_t disk_write(disk_e disk, uint8_t* buffer, uint32_t lba, uint32_t count) {
	switch (disk) {
		case DISK_SATA_DRIVE:
			return device_write("/dev/sda", count, lba, buffer);
		case DISK_USB_DRIVE:
			return 0;
		case DISK_SD_CARD:
			return 0;
		break;
	}
	return 0;
}
