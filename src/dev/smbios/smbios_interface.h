#ifndef _SMBIOS_INTERFACE_H_
#define _SMBIOS_INTERFACE_H_

#include <stdint.h>

struct smbios_data_entry {
    void * pointer;
    uint64_t length;
};

struct smbios_bitfield_entry {
    void * pointer;
    uint64_t bit;    //Starting bit
    uint64_t length; //Number of bits
};

struct smbios_bios_data {

    uint8_t loaded;

    struct smbios_data_entry vendor;
    struct smbios_data_entry version;
    struct smbios_data_entry release_date;
    struct smbios_data_entry system_bios_major_release;
    struct smbios_data_entry system_bios_minor_release;
    struct smbios_data_entry embedded_controller_major_release;
    struct smbios_data_entry embedded_controller_minor_release;
    
    struct smbios_bitfield_entry supports_characteristics;
    struct smbios_bitfield_entry supports_isa;
    struct smbios_bitfield_entry supports_mca;
    struct smbios_bitfield_entry supports_pci;
    struct smbios_bitfield_entry supports_pnp;
    struct smbios_bitfield_entry supports_apm;
    struct smbios_bitfield_entry supports_edd;
    struct smbios_bitfield_entry supports_escd;
    struct smbios_bitfield_entry supports_eisa;
    struct smbios_bitfield_entry supports_vl_vesa;
    struct smbios_bitfield_entry supports_boot_from_cd;
    struct smbios_bitfield_entry supports_selectable_boot;

    struct smbios_bitfield_entry supports_acpi;
    struct smbios_bitfield_entry supports_usb_legacy;
    struct smbios_bitfield_entry supports_agp;

    struct smbios_bitfield_entry supports_bios_boot;
    struct smbios_bitfield_entry supports_uefi;

    struct smbios_bitfield_entry is_virtual_machine;
};

struct smbios_system_data {

    uint8_t loaded;

    struct smbios_data_entry manufacturer;
    struct smbios_data_entry product_name;
    struct smbios_data_entry version;
    struct smbios_data_entry serial_number;
    struct smbios_data_entry uuid;
    struct smbios_data_entry wake_up_type;
    struct smbios_data_entry sku_number;
    struct smbios_data_entry family;
};

struct smbios_cpu_data {

    uint8_t loaded;

    struct smbios_data_entry socket_designation;
    struct smbios_data_entry processor_type;
    struct smbios_data_entry processor_family;
    struct smbios_data_entry processor_manufacturer;
    struct smbios_data_entry processor_id;
    struct smbios_data_entry processor_version;

    struct smbios_bitfield_entry voltage_legacy;
    struct smbios_bitfield_entry voltage_5V;
    struct smbios_bitfield_entry voltage_3_3V;
    struct smbios_bitfield_entry voltage_2_9V;

    struct smbios_data_entry external_clock;
    struct smbios_data_entry max_speed;
    struct smbios_data_entry current_speed;
  
    struct smbios_bitfield_entry socket_populated;
    struct smbios_bitfield_entry cpu_status;

    struct smbios_data_entry processor_upgrade;
    struct smbios_data_entry serial_number;
    struct smbios_data_entry asset_tag;
    struct smbios_data_entry part_number;
    struct smbios_data_entry core_count;
    struct smbios_data_entry core_enabled;
    struct smbios_data_entry thread_count;

    struct smbios_bitfield_entry supports_64_bits;
    struct smbios_bitfield_entry supports_multicore;
    struct smbios_bitfield_entry supports_hardware_thread;
    struct smbios_bitfield_entry supports_execute_protection;
    struct smbios_bitfield_entry supports_enhanced_virtualization;
    struct smbios_bitfield_entry supports_power_management;
    struct smbios_bitfield_entry supports_128_bits;
    struct smbios_bitfield_entry is_arm_soc;

    struct smbios_data_entry processor_family2;
    struct smbios_data_entry core_count2;
    struct smbios_data_entry core_enabled2;
    struct smbios_data_entry thread_count2;
    struct smbios_data_entry thread_enabled;
};

struct smbios_data {
    struct smbios_bios_data bios;
    struct smbios_system_data system;
    struct smbios_cpu_data cpu;
};

void init_smbios_interface();
void dump_smbios();
void insert_smbios_data(struct smbios_data_entry *, void *, uint16_t);
void insert_smbios_bitfield(struct smbios_bitfield_entry *, void *, uint8_t, uint8_t);

char *    get_smbios_bios_vendor();
char *    get_smbios_bios_version();
char *    get_smbios_bios_release_date();

void set_smbios_bios_loaded(uint8_t);
void set_smbios_system_loaded(uint8_t);
void set_smbios_cpu_loaded(uint8_t);

uint8_t get_smbios_bios_loaded();
uint8_t get_smbios_system_loaded();
uint8_t get_smbios_cpu_loaded();

uint8_t * get_smbios_bios_major_release();
uint8_t * get_smbios_bios_minor_release();
uint8_t * get_smbios_bios_embedded_controller_major_release();
uint8_t * get_smbios_bios_embedded_controller_minor_release();

uint8_t get_smbios_bios_not_supports_characteristics();
uint8_t get_smbios_bios_supports_isa();
uint8_t get_smbios_bios_supports_mca();
uint8_t get_smbios_bios_supports_pci();
uint8_t get_smbios_bios_supports_pnp();
uint8_t get_smbios_bios_supports_apm();
uint8_t get_smbios_bios_supports_edd();
uint8_t get_smbios_bios_supports_escd();
uint8_t get_smbios_bios_supports_eisa();
uint8_t get_smbios_bios_supports_vl_vesa();
uint8_t get_smbios_bios_supports_boot_from_cd();
uint8_t get_smbios_bios_supports_selectable_boot();
uint8_t get_smbios_bios_supports_acpi();
uint8_t get_smbios_bios_supports_usb_legacy();
uint8_t get_smbios_bios_supports_agp();
uint8_t get_smbios_bios_supports_bios_boot();
uint8_t get_smbios_bios_supports_uefi();
uint8_t get_smbios_bios_is_virtual_machine();

char *   get_smbios_system_manufacturer();
char *   get_smbios_system_product_name();
char *   get_smbios_system_version();
char *   get_smbios_system_serial_number();
char *   get_smbios_system_sku_number();
char *   get_smbios_system_family();
uint8_t * get_smbios_system_uuid();
uint8_t * get_smbios_system_wake_up_type();

char * get_smbios_cpu_socket_designation();
uint8_t * get_smbios_cpu_processor_type();
uint8_t * get_smbios_cpu_processor_family();
char * get_smbios_cpu_processor_manufacturer();
uint64_t * get_smbios_cpu_processor_id();
char * get_smbios_cpu_processor_version();

uint8_t get_smbios_cpu_voltage_legacy();
uint8_t get_smbios_cpu_voltage_5V();
uint8_t get_smbios_cpu_voltage_3_3V();
uint8_t get_smbios_cpu_voltage_2_9V();

uint16_t * get_smbios_cpu_external_clock();
uint16_t * get_smbios_cpu_max_speed();
uint16_t * get_smbios_cpu_current_speed();

uint8_t get_smbios_cpu_socket_populated();
uint8_t get_smbios_cpu_enabled();
uint8_t get_smbios_cpu_disabled_by_user();
uint8_t get_smbios_cpu_disabled_by_bios();
uint8_t get_smbios_cpu_idle();

uint8_t * get_smbios_cpu_processor_upgrade();
char * get_smbios_cpu_serial_number();
char * get_smbios_cpu_asset_tag();
char * get_smbios_cpu_part_number();
uint8_t * get_smbios_cpu_core_count();
uint8_t * get_smbios_cpu_core_enabled();
uint8_t * get_smbios_cpu_thread_count();

uint8_t get_smbios_cpu_supports_64_bits();
uint8_t get_smbios_cpu_supports_multicore();
uint8_t get_smbios_cpu_supports_hardware_thread();
uint8_t get_smbios_cpu_supports_execute_protection();
uint8_t get_smbios_cpu_supports_enhanced_virtualization();
uint8_t get_smbios_cpu_supports_power_management();
uint8_t get_smbios_cpu_supports_128_bits();
uint8_t get_smbios_cpu_is_arm_soc();

uint16_t * get_smbios_cpu_processor_family2(); 
uint16_t * get_smbios_cpu_core_count2();
uint16_t * get_smbios_cpu_core_enabled2();
uint16_t * get_smbios_cpu_thread_count2();

uint16_t * get_smbios_cpu_thread_enabled();
#endif