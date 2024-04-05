#ifndef _BOOTSERVICES_H
#define _BOOTSERVICES_H
#include "limine.h"
#include <stdint.h>

struct bmoon_smp_info;

typedef void (*bmoon_goto_address)(struct bmoon_smp_info *);

struct bmoon_smp_info {
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t reserved;
    bmoon_goto_address goto_address;
    uint64_t extra_argument;
};

struct bmoon_video_mode {
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

struct bmoon_framebuffer {
    void *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp; // Bits per pixel
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
    uint64_t edid_size;
    void *edid;

    /* Response revision 1 */
    uint64_t mode_count;
    struct bmoon_video_mode **modes;
};

void   (*get_terminal_writer())(const char*, uint64_t);

char*    get_bootloader_name();
char*    get_bootloader_version();

uint64_t get_terminal_count();
uint64_t get_current_terminal();

int64_t get_boot_time();

uint64_t get_memory_map_entries();
uint64_t get_memory_map_base(uint64_t);
uint64_t get_memory_map_length(uint64_t);
uint64_t get_memory_map_type(uint64_t);

uint64_t get_kernel_address_physical();
uint64_t get_kernel_address_virtual();

uint64_t get_rsdp_address();

uint64_t get_smbios32_address();
uint64_t get_smbios64_address();

uint32_t get_smp_flags();
uint32_t get_smp_bsp_lapic_id();
uint64_t get_smp_cpu_count();
struct bmoon_smp_info ** get_smp_cpus();

uint64_t get_framebuffer_count();
struct bmoon_framebuffer ** get_framebuffers();

void     set_terminal_extra_handler(); //TODO
void     set_terminal_writer(uint64_t terminal);

#endif