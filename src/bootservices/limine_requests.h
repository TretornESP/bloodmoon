#ifndef _LIMINE_REQUESTS_H
#define _LIMINE_REQUESTS_H

#define BOOTLOADER  bootloader_info_request
#define TERMINAL    terminal_request
#define HHDM        hhdm_request
#define MEMMAP      memmap_request

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};
static volatile struct limine_bootloader_info_request bootloader_info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0
};
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
#endif // _LIMINE_REQUESTS_H