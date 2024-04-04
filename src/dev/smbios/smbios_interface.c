#include "smbios_interface.h"
#include "smbios.h"

#include "../../util/printf.h" // Required for printf

#define BITFIELD(t, x) ((t)((1 << (x.length)) - 1) & ((*(uint64_t*)x.pointer) >> (x.bit)))
#define VALUE(t, x) ((*(t*)x.pointer))
#define VALUEPTR(t, x) (((t)x.pointer))

struct smbios_data smbios;

void insert_smbios_data(struct smbios_data_entry * data, void* pointer, uint16_t length) {
    data->pointer = pointer;
    data->length = length;
}

void insert_smbios_bitfield(struct smbios_bitfield_entry * bitfield, void * pointer, uint8_t bit, uint8_t length) {
    bitfield->pointer = pointer;
    bitfield->bit = bit;
    bitfield->length = length;
}

void set_smbios_bios_loaded(uint8_t value) {smbios.bios.loaded = value;}
void set_smbios_system_loaded(uint8_t value) {smbios.system.loaded = value;}
void set_smbios_cpu_loaded(uint8_t value) {smbios.cpu.loaded = value;}
void set_smbios_oem_loaded(uint8_t value) {smbios.oem.loaded = value;}

uint8_t get_smbios_bios_loaded() {return smbios.bios.loaded;}
uint8_t get_smbios_system_loaded() {return smbios.system.loaded;}
uint8_t get_smbios_cpu_loaded() {return smbios.cpu.loaded;}
uint8_t get_smbios_oem_loaded() {return smbios.oem.loaded;}

void init_smbios_interface() {
    printf("### SMBIOS STARTUP###\n");
    set_smbios_bios_loaded(0);
    set_smbios_system_loaded(0);
    set_smbios_cpu_loaded(0);
    set_smbios_oem_loaded(0);

    init_smbios(&smbios);
}

char *   get_smbios_bios_vendor() {return VALUEPTR(char*, smbios.bios.vendor);}
char *   get_smbios_bios_version() {return VALUEPTR(char*, smbios.bios.version);}
uint16_t get_smbios_bios_start_address_segment() {return VALUE(uint16_t, smbios.bios.start_address_segment);}
char *   get_smbios_bios_release_date() {return VALUEPTR(char*, smbios.bios.release_date);}
uint8_t  get_smbios_bios_rom_size() {return VALUE(uint8_t, smbios.bios.rom_size);}
uint8_t  get_smbios_bios_supports_characteristics() {return BITFIELD(uint8_t, smbios.bios.supports_characteristics);}
uint8_t  get_smbios_bios_supports_isa() {return BITFIELD(uint8_t, smbios.bios.supports_isa);}
uint8_t  get_smbios_bios_supports_mca() {return BITFIELD(uint8_t, smbios.bios.supports_mca);}
uint8_t  get_smbios_bios_supports_eisa() {return BITFIELD(uint8_t, smbios.bios.supports_eisa);}
uint8_t  get_smbios_bios_supports_pci() {return BITFIELD(uint8_t, smbios.bios.supports_pci);}
uint8_t  get_smbios_bios_supports_pcmcia() {return BITFIELD(uint8_t, smbios.bios.supports_pcmcia);}
uint8_t  get_smbios_bios_supports_pnp() {return BITFIELD(uint8_t, smbios.bios.supports_pnp);}
uint8_t  get_smbios_bios_supports_apm() {return BITFIELD(uint8_t, smbios.bios.supports_apm);}
uint8_t  get_smbios_bios_supports_flash_upgrade() {return BITFIELD(uint8_t, smbios.bios.supports_flash_upgrade);}
uint8_t  get_smbios_bios_supports_bios_shadowing() {return BITFIELD(uint8_t, smbios.bios.supports_bios_shadowing);}
uint8_t  get_smbios_bios_supports_vl_vesa() {return BITFIELD(uint8_t, smbios.bios.supports_vl_vesa);}
uint8_t  get_smbios_bios_supports_escd() {return BITFIELD(uint8_t, smbios.bios.supports_escd);}
uint8_t  get_smbios_bios_supports_boot_from_cd() {return BITFIELD(uint8_t, smbios.bios.supports_boot_from_cd);}
uint8_t  get_smbios_bios_supports_selectable_boot() {return BITFIELD(uint8_t, smbios.bios.supports_selectable_boot);}
uint8_t  get_smbios_bios_supports_bios_rom_socketed() {return BITFIELD(uint8_t, smbios.bios.supports_bios_rom_socketed);}
uint8_t  get_smbios_bios_supports_boot_from_pc_card() {return BITFIELD(uint8_t, smbios.bios.supports_boot_from_pc_card);}
uint8_t  get_smbios_bios_supports_edd() {return BITFIELD(uint8_t, smbios.bios.supports_edd);}
uint8_t  get_smbios_bios_supports_int_13h_japanese_floppy_nec9800() {return BITFIELD(uint8_t, smbios.bios.supports_int_13h_japanese_floppy_nec9800);}
uint8_t  get_smbios_bios_supports_int_13h_japanese_floppy_toshiba() {return BITFIELD(uint8_t, smbios.bios.supports_int_13h_japanese_floppy_toshiba);}
uint8_t  get_smbios_bios_supports_int_13h_525_360_kb_floppy() {return BITFIELD(uint8_t, smbios.bios.supports_int_13h_525_360_kb_floppy);}
uint8_t  get_smbios_bios_supports_int_13h_525_1200_kb_floppy() {return BITFIELD(uint8_t, smbios.bios.supports_int_13h_525_1200_kb_floppy);}
uint8_t  get_smbios_bios_supports_int_13h_35_720_kb_floppy() {return BITFIELD(uint8_t, smbios.bios.supports_int_13h_35_720_kb_floppy);}
uint8_t  get_smbios_bios_supports_int_13h_35_2880_kb_floppy() {return BITFIELD(uint8_t, smbios.bios.supports_int_13h_35_2880_kb_floppy);}
uint8_t  get_smbios_bios_supports_int_5h_print_screen() {return BITFIELD(uint8_t, smbios.bios.supports_int_5h_print_screen);}
uint8_t  get_smbios_bios_supports_int_9h_842_keyboad() {return BITFIELD(uint8_t, smbios.bios.supports_int_9h_842_keyboad);}
uint8_t  get_smbios_bios_supports_int_14h_serial_services() {return BITFIELD(uint8_t, smbios.bios.supports_int_14h_serial_services);}
uint8_t  get_smbios_bios_supports_int_17h_printer_services() {return BITFIELD(uint8_t, smbios.bios.supports_int_17h_printer_services);}
uint8_t  get_smbios_bios_supports_int_10h_cga_mono_video() {return BITFIELD(uint8_t, smbios.bios.supports_int_10h_cga_mono_video);}
uint8_t  get_smbios_bios_supports_nec_pc_98() {return BITFIELD(uint8_t, smbios.bios.supports_nec_pc_98);}
uint16_t get_smbios_bios_reserved_bios_vendor() {return BITFIELD(uint16_t, smbios.bios.reserved_bios_vendor);}
uint16_t get_smbios_bios_reserved_system_vendor() {return BITFIELD(uint16_t, smbios.bios.reserved_system_vendor);}
uint8_t  get_smbios_bios_supports_acpi() {return BITFIELD(uint8_t, smbios.bios.supports_acpi);}
uint8_t  get_smbios_bios_supports_usb_legacy() {return BITFIELD(uint8_t, smbios.bios.supports_usb_legacy);}
uint8_t  get_smbios_bios_supports_agp() {return BITFIELD(uint8_t, smbios.bios.supports_agp);}
uint8_t  get_smbios_bios_supports_i2o_boot() {return BITFIELD(uint8_t, smbios.bios.supports_i2o_boot);}
uint8_t  get_smbios_bios_supports_ls_120_boot() {return BITFIELD(uint8_t, smbios.bios.supports_ls_120_boot);}
uint8_t  get_smbios_bios_supports_atapi_zip_drive_boot() {return BITFIELD(uint8_t, smbios.bios.supports_atapi_zip_drive_boot);}
uint8_t  get_smbios_bios_supports_1394_boot() {return BITFIELD(uint8_t, smbios.bios.supports_1394_boot);}
uint8_t  get_smbios_bios_supports_smart_battery() {return BITFIELD(uint8_t, smbios.bios.supports_smart_battery);}
uint8_t  get_smbios_bios_supports_bios_boot() {return BITFIELD(uint8_t, smbios.bios.supports_bios_boot);}
uint8_t  get_smbios_bios_fnkey_initiated_netboot() {return BITFIELD(uint8_t, smbios.bios.fnkey_initiated_netboot);}
uint8_t  get_smbios_bios_supports_targeted_content_distribution() {return BITFIELD(uint8_t, smbios.bios.supports_targeted_content_distribution);}
uint8_t  get_smbios_bios_supports_uefi() {return BITFIELD(uint8_t, smbios.bios.supports_uefi);}
uint8_t  get_smbios_bios_is_virtual_machine() {return BITFIELD(uint8_t, smbios.bios.is_virtual_machine);}
uint8_t  get_smbios_bios_manufacturing_mode_supported() {return BITFIELD(uint8_t, smbios.bios.manufacturing_mode_supported);}
uint8_t  get_smbios_bios_manufacturing_mode_enabled() {return BITFIELD(uint8_t, smbios.bios.manufacturing_mode_enabled);}
uint8_t  get_smbios_bios_system_bios_major_release() {return VALUE(uint8_t, smbios.bios.system_bios_major_release);}
uint8_t  get_smbios_bios_system_bios_minor_release() {return VALUE(uint8_t, smbios.bios.system_bios_minor_release);}
uint8_t  get_smbios_bios_embedded_controller_major_release() {return VALUE(uint8_t, smbios.bios.embedded_controller_major_release);}
uint8_t  get_smbios_bios_embedded_controller_minor_release() {return VALUE(uint8_t, smbios.bios.embedded_controller_minor_release);}
uint16_t get_smbios_bios_extended_bios_rom_size() {return VALUE(uint16_t, smbios.bios.extended_bios_rom_size);}

char *    get_smbios_system_manufacturer() {return VALUEPTR(char*, smbios.system.manufacturer);}
char *    get_smbios_system_product_name() {return VALUEPTR(char*, smbios.system.product_name);}
char *    get_smbios_system_version() {return VALUEPTR(char*, smbios.system.version);}
char *    get_smbios_system_serial_number() {return VALUEPTR(char*, smbios.system.serial_number);}
uint8_t * get_smbios_system_uuid() {return VALUEPTR(uint8_t*, smbios.system.uuid);}
uint8_t   get_smbios_system_wake_up_type() {return VALUE(uint8_t, smbios.system.wake_up_type);}
char *    get_smbios_system_sku_number() {return VALUEPTR(char*, smbios.system.sku_number);}
char *    get_smbios_system_family() {return VALUEPTR(char*, smbios.system.family);}

uint8_t get_smbios_baseboard_deviceno() {return smbios.baseboard_devices;}

char * get_smbios_cpu_socket_designation() {return VALUEPTR(char*, smbios.cpu.socket_designation);}
uint8_t get_smbios_cpu_processor_type() {return VALUE(uint8_t, smbios.cpu.processor_type);}
uint8_t get_smbios_cpu_processor_family() {return VALUE(uint8_t, smbios.cpu.processor_family);}
char * get_smbios_cpu_processor_manufacturer() {return VALUEPTR(char*, smbios.cpu.processor_manufacturer);}
uint64_t get_smbios_cpu_processor_id() {return VALUE(uint64_t, smbios.cpu.processor_id);}
char * get_smbios_cpu_processor_version() {return VALUEPTR(char*, smbios.cpu.processor_version);}

uint8_t get_smbios_cpu_voltage_legacy() {return BITFIELD(uint8_t, smbios.cpu.voltage_legacy);}
uint8_t get_smbios_cpu_voltage_5V() {return BITFIELD(uint8_t, smbios.cpu.voltage_5V);}
uint8_t get_smbios_cpu_voltage_3_3V() {return BITFIELD(uint8_t, smbios.cpu.voltage_3_3V);}
uint8_t get_smbios_cpu_voltage_2_9V() {return BITFIELD(uint8_t, smbios.cpu.voltage_2_9V);}

uint16_t get_smbios_cpu_external_clock() {return VALUE(uint16_t, smbios.cpu.external_clock);}
uint16_t get_smbios_cpu_max_speed() {return VALUE(uint16_t, smbios.cpu.max_speed);}
uint16_t get_smbios_cpu_current_speed() {return VALUE(uint16_t, smbios.cpu.current_speed);}

uint8_t get_smbios_cpu_socket_populated() {return BITFIELD(uint8_t, smbios.cpu.socket_populated);}
uint8_t get_smbios_cpu_status() {return BITFIELD(uint8_t, smbios.cpu.cpu_status);}

uint8_t get_smbios_cpu_processor_upgrade() {return VALUE(uint8_t, smbios.cpu.processor_upgrade);}
uint16_t get_smbios_cpu_l1_cache_handle() {return VALUE(uint16_t, smbios.cpu.l1_cache_handle);}
uint16_t get_smbios_cpu_l2_cache_handle() {return VALUE(uint16_t, smbios.cpu.l2_cache_handle);}
uint16_t get_smbios_cpu_l3_cache_handle() {return VALUE(uint16_t, smbios.cpu.l3_cache_handle);}
char * get_smbios_cpu_serial_number() {return VALUEPTR(char*, smbios.cpu.serial_number);}
char * get_smbios_cpu_asset_tag() {return VALUEPTR(char*, smbios.cpu.asset_tag);}
char * get_smbios_cpu_part_number() {return VALUEPTR(char*, smbios.cpu.part_number);}
uint8_t get_smbios_cpu_core_count() {return VALUE(uint8_t, smbios.cpu.core_count);}
uint8_t get_smbios_cpu_core_enabled() {return VALUE(uint8_t, smbios.cpu.core_enabled);}
uint8_t get_smbios_cpu_thread_count() {return VALUE(uint8_t, smbios.cpu.thread_count);}

uint8_t get_smbios_cpu_supports_64_bits() {return BITFIELD(uint8_t, smbios.cpu.supports_64_bits);}
uint8_t get_smbios_cpu_supports_multicore() {return BITFIELD(uint8_t, smbios.cpu.supports_multicore);}
uint8_t get_smbios_cpu_supports_hardware_thread() {return BITFIELD(uint8_t, smbios.cpu.supports_hardware_thread);}
uint8_t get_smbios_cpu_supports_execute_protection() {return BITFIELD(uint8_t, smbios.cpu.supports_execute_protection);}
uint8_t get_smbios_cpu_supports_enhanced_virtualization() {return BITFIELD(uint8_t, smbios.cpu.supports_enhanced_virtualization);}
uint8_t get_smbios_cpu_supports_power_management() {return BITFIELD(uint8_t, smbios.cpu.supports_power_management);}
uint8_t get_smbios_cpu_supports_128_bits() {return BITFIELD(uint8_t, smbios.cpu.supports_128_bits);}
uint8_t get_smbios_cpu_is_arm_soc() {return BITFIELD(uint8_t, smbios.cpu.is_arm_soc);}
uint16_t get_smbios_cpu_processor_family2() {return VALUE(uint16_t, smbios.cpu.processor_family2);}
uint16_t get_smbios_cpu_core_count2() {return VALUE(uint16_t, smbios.cpu.core_count2);}
uint16_t get_smbios_cpu_core_enabled2() {return VALUE(uint16_t, smbios.cpu.core_enabled2);}
uint16_t get_smbios_cpu_thread_count2() {return VALUE(uint16_t, smbios.cpu.thread_count2);}
uint16_t get_smbios_cpu_thread_enabled() {return VALUE(uint16_t, smbios.cpu.thread_enabled);}


uint8_t get_smbios_slot_deviceno() {return smbios.slot_devices;}
char * get_smbios_slot_designation(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUEPTR(char*, smbios.slot[slot].slot_designation);} else {return 0;}}
uint8_t get_smbios_slot_type(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].slot_type);} else {return 0;}}
uint8_t get_smbios_slot_data_bus_width(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].slot_data_bus_width);} else {return 0;}}
uint8_t get_smbios_slot_current_usage(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].current_usage);} else {return 0;}}
uint8_t get_smbios_slot_length(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].slot_length);} else {return 0;}}
uint16_t get_smbios_slot_id(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint16_t, smbios.slot[slot].slot_id);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_unknown(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_unknown);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_provides_5V(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_provides_5V);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_provides_3_3V(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_provides_3_3V);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_opening_is_shared(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_opening_is_shared);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_supports_card_16(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_supports_card_16);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_supports_cardbus(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_supports_cardbus);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_supports_zv(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_supports_zv);} else {return 0;}}
uint8_t get_smbios_slot_characteristics1_supports_modem_ring_resume(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics1_supports_modem_ring_resume);} else {return 0;}}
uint8_t get_smbios_slot_characteristics2_supports_pme_signal(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics2_supports_pme_signal);} else {return 0;}}
uint8_t get_smbios_slot_characteristics2_supports_hot_plug_devices(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics2_supports_hot_plug_devices);} else {return 0;}}
uint8_t get_smbios_slot_characteristics2_supports_smbus_signal(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics2_supports_smbus_signal);} else {return 0;}}
uint8_t get_smbios_slot_characteristics2_supports_pcie_bifurcation(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics2_supports_pcie_bifurcation);} else {return 0;}}
uint8_t get_smbios_slot_characteristics2_supports_surprise_removal(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics2_supports_surprise_removal);} else {return 0;}}
uint8_t get_smbios_slot_characteristics2_supports_flexbus_cxl_1_0(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics2_supports_flexbus_cxl_1_0);} else {return 0;}}
uint8_t get_smbios_slot_characteristics2_supports_flexbus_cxl_2_0(uint8_t slot) {if (smbios.slot_devices > slot) {return BITFIELD(uint8_t, smbios.slot[slot].slot_characteristics2_supports_flexbus_cxl_2_0);} else {return 0;}}
uint8_t get_smbios_slot_segment_group_number(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].segment_group_number);} else {return 0;}}
uint8_t get_smbios_slot_bus_number(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].bus_number);} else {return 0;}}
uint8_t get_smbios_slot_function_number(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].function_number);} else {return 0;}}
uint8_t get_smbios_slot_device_number(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].device_number);} else {return 0;}}
uint8_t get_smbios_slot_peer_grouping_n(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].peer_grouping_n);} else {return 0;}}
uint8_t get_smbios_slot_peer_group(uint8_t slot, uint8_t peer_group) {if (smbios.slot_devices > slot) {if (peer_group < 8) {return VALUE(uint8_t, smbios.slot[slot].peer_group[peer_group]);} else {return 0;}} else {return 0;}}
uint8_t get_smbios_slot_information(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].slot_information);} else {return 0;}}
uint8_t get_smbios_slot_physical_width(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].slot_physical_width);} else {return 0;}}
uint8_t get_smbios_slot_pitch(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].slot_pitch);} else {return 0;}}
uint8_t get_smbios_slot_height(uint8_t slot) {if (smbios.slot_devices > slot) {return VALUE(uint8_t, smbios.slot[slot].slot_height);} else {return 0;}}

uint8_t get_smbios_memory_array_deviceno() {return smbios.memory_array_devices;}
uint8_t get_smbios_memory_array_location(uint8_t device) {if (smbios.memory_array_devices > device) {return VALUE(uint8_t, smbios.memory_array[device].location);} else {return 0;}}
uint8_t get_smbios_memory_array_use(uint8_t device) {if (smbios.memory_array_devices > device) {return VALUE(uint8_t, smbios.memory_array[device].use);} else {return 0;}}
uint8_t get_smbios_memory_array_error_correction(uint8_t device) {if (smbios.memory_array_devices > device) {return VALUE(uint8_t, smbios.memory_array[device].error_correction);} else {return 0;}}
uint64_t get_smbios_memory_array_maximum_capacity(uint8_t device) {if (smbios.memory_array_devices > device) {return VALUE(uint64_t, smbios.memory_array[device].maximum_capacity);} else {return 0;}}
uint16_t get_smbios_memory_array_error_information_handle(uint8_t device) {if (smbios.memory_array_devices > device) {return VALUE(uint16_t, smbios.memory_array[device].error_information_handle);} else {return 0;}}
uint8_t get_smbios_memory_array_number_of_memory_devices(uint8_t device) {if (smbios.memory_array_devices > device) {return VALUE(uint8_t, smbios.memory_array[device].number_of_memory_devices);} else {return 0;}}
uint64_t get_smbios_memory_array_extended_maximum_capacity(uint8_t device) {if (smbios.memory_array_devices > device) {return VALUE(uint64_t, smbios.memory_array[device].extended_maximum_capacity);} else {return 0;}}

uint8_t get_smbios_memory_device_deviceno() {return smbios.memory_device_devices;}
uint16_t get_smbios_memory_device_physical_memory_array_handle(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].physical_memory_array_handle);} else {return 0;}}
uint16_t get_smbios_memory_device_memory_error_information_handle(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].memory_error_information_handle);} else {return 0;}}
uint16_t get_smbios_memory_device_total_width(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].total_width);} else {return 0;}}
uint16_t get_smbios_memory_device_data_width(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].data_width);} else {return 0;}}
uint16_t get_smbios_memory_device_size(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].size);} else {return 0;}}
uint8_t get_smbios_memory_device_form_factor(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint8_t, smbios.memory_device[device].form_factor);} else {return 0;}}
uint8_t get_smbios_memory_device_device_set(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint8_t, smbios.memory_device[device].device_set);} else {return 0;}}
char * get_smbios_memory_device_device_locator(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUEPTR(char*, smbios.memory_device[device].device_locator);} else {return 0;}}
char * get_smbios_memory_device_bank_locator(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUEPTR(char*, smbios.memory_device[device].bank_locator);} else {return 0;}}
uint8_t get_smbios_memory_device_memory_type(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint8_t, smbios.memory_device[device].memory_type);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_other(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_other);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_unknown(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_unknown);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_fast_paged(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_fast_paged);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_static_column(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_static_column);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_pseudo_static(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_pseudo_static);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_ram_bus(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_ram_bus);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_synchronous(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_synchronous);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_cmos(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_cmos);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_edo(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_edo);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_window_dram(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_window_dram);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_cache_dram(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_cache_dram);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_non_volatile(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_non_volatile);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_registered_buffered(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_registered_buffered);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_unbuffered(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_unbuffered);} else {return 0;}}
uint8_t get_smbios_memory_device_type_detail_lr_dimm(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].type_detail_lr_dimm);} else {return 0;}}
uint16_t get_smbios_memory_device_speed(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].speed);} else {return 0;}}
char * get_smbios_memory_device_manufacturer(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUEPTR(char*, smbios.memory_device[device].manufacturer);} else {return 0;}}
char * get_smbios_memory_device_serial_number(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUEPTR(char*, smbios.memory_device[device].serial_number);} else {return 0;}}
char * get_smbios_memory_device_asset_tag(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUEPTR(char*, smbios.memory_device[device].asset_tag);} else {return 0;}}
char * get_smbios_memory_device_part_number(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUEPTR(char*, smbios.memory_device[device].part_number);} else {return 0;}}
uint8_t get_smbios_memory_device_attributes(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint8_t, smbios.memory_device[device].attributes);} else {return 0;}}
uint64_t get_smbios_memory_device_extended_size(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint64_t, smbios.memory_device[device].extended_size);} else {return 0;}}
uint16_t get_smbios_memory_device_configured_memory_clock_speed(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].configured_memory_clock_speed);} else {return 0;}}
uint16_t get_smbios_memory_device_minimum_voltage(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].minimum_voltage);} else {return 0;}}
uint16_t get_smbios_memory_device_maximum_voltage(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].maximum_voltage);} else {return 0;}}
uint16_t get_smbios_memory_device_configured_voltage(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].configured_voltage);} else {return 0;}}
uint8_t get_smbios_memory_device_memory_technology(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint8_t, smbios.memory_device[device].memory_technology);} else {return 0;}}
uint8_t get_smbios_memory_device_memory_operating_mode_capability_other(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].memory_operating_mode_capability_other);} else {return 0;}}
uint8_t get_smbios_memory_device_memory_operating_mode_capability_unknown(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].memory_operating_mode_capability_unknown);} else {return 0;}}
uint8_t get_smbios_memory_device_memory_operating_mode_capability_volatile(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].memory_operating_mode_capability_volatile);} else {return 0;}}
uint8_t get_smbios_memory_device_memory_operating_mode_capability_byte_access(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].memory_operating_mode_capability_byte_access);} else {return 0;}}
uint8_t get_smbios_memory_device_memory_operating_mode_capability_block_access(uint8_t device) {if (smbios.memory_device_devices > device) {return BITFIELD(uint8_t, smbios.memory_device[device].memory_operating_mode_capability_block_access);} else {return 0;}}
char * get_smbios_memory_device_firmware_version(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUEPTR(char*, smbios.memory_device[device].firmware_version);} else {return 0;}}
uint16_t get_smbios_memory_device_module_manufacturer_id(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].module_manufacturer_id);} else {return 0;}}
uint16_t get_smbios_memory_device_module_product_id(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].module_product_id);} else {return 0;}}
uint16_t get_smbios_memory_device_memory_subsystem_controller_manufacturer_id(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].memory_subsystem_controller_manufacturer_id);} else {return 0;}}
uint16_t get_smbios_memory_device_memory_subsystem_controller_product_id(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint16_t, smbios.memory_device[device].memory_subsystem_controller_product_id);} else {return 0;}}
uint64_t get_smbios_memory_device_non_volatile_size(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint64_t, smbios.memory_device[device].non_volatile_size);} else {return 0;}}
uint64_t get_smbios_memory_device_volatile_size(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint64_t, smbios.memory_device[device].volatile_size);} else {return 0;}}
uint64_t get_smbios_memory_device_cache_size(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint64_t, smbios.memory_device[device].cache_size);} else {return 0;}}
uint64_t get_smbios_memory_device_logical_size(uint8_t device) {if (smbios.memory_device_devices > device) {return VALUE(uint64_t, smbios.memory_device[device].logical_size);} else {return 0;}}

const char * smbios_slot_names[] = {
    "Invalid", //0x0
    "Other", //0x1
    "Unknown", //0x2
    "ISA",  //0x3
    "MCA",  //0x4
    "EISA", //0x5
    "PCI",  //0x6
    "PCMCIA",       //0x7
    "VL-VESA",      //0x8
    "Proprietary",  //0x9
    "Processor Card Slot",  //0xA
    "Proprietary Memory Card Slot", //0xB
    "IORiser Card Slot",    //0xC
    "NuBus",        //0xD
    "PCI-66MHz Capable",            //0xE
    "AGP",        //0xF
    "AGP 2X",       //0x10
    "AGP 4X",       //0x11
    "PCI-X",        //0x12
    "AGP 8X",       //0x13
    "M.2 Socket 1-DP (Mechanical Key A)",   //0x14
    "M.2 Socket 1-SD (Mechanical Key E)",   //0x15
    "M.2 Socket 2 (Mechanical Key B)",      //0x16
    "M.2 Socket 3 (Mechanical Key M)",      //0x17
    "MXM Type I",   //0x18
    "MXM Type II",  //0x19
    "MXM Type III (Standard Connector)",    //0x1A
    "MXM Type III (HE Connector)",  //0x1B
    "MXM Type IV",  //0x1C
    "MXM 3.0 Type A",       //0x1D
    "MXM 3.0 Type B",       //0x1E
    "PCI Express Gen 2 SFF-8639",   //0x1F
    "PCI Express Gen 3 SFF-8639",   //0x20
    "PCI Express Mini 52-pin (With bottom-side keep-outs)",      //0x21  
    "PCI Express Mini 52-pin (Without bottom-side keep-outs)",   //0x22
    "PCI Express Mini 76-pin",      //0x23
    "PCI Express Gen 4 SFF-8639",   //0x24
    "PCI Express Gen 5 SFF-8639",   //0x25
    "OCP NIC 3.0 Small form factor",  //0x26
    "OCP NIC 3.0 Large form factor",  //0x27
    "OCP NIC Prior to 3.0",           //0x28
    "UNUSED",      //0x29
    "UNUSED",      //0x2A
    "UNUSED",      //0x2B
    "UNUSED",      //0x2C
    "UNUSED",      //0x2D
    "UNUSED",      //0x2E
    "UNUSED",      //0x2F
    "CXL Flexbus 1.0"  //0x30
};

const char * smbios_slot_names2[] = {
    "PC-98/C20", //0xA0
    "PC-98/C24", //0xA1
    "PC-98/E",   //0xA2
    "PC-98/Local Bus",  //0xA3
    "PC-98/Card",       //0xA4
    "PCI Express",      //0xA5
    "PCI Express x1",   //0xA6
    "PCI Express x2",   //0xA7
    "PCI Express x4",   //0xA8
    "PCI Express x8",   //0xA9
    "PCI Express x16",  //0xAA
    "PCI Express Gen 2",    //0xAB
    "PCI Express Gen 2 x1", //0xAC
    "PCI Express Gen 2 x2", //0xAD
    "PCI Express Gen 2 x4", //0xAE
    "PCI Express Gen 2 x8", //0xAF
    "PCI Express Gen 2 x16",//0xB0
    "PCI Express Gen 3",    //0xB1
    "PCI Express Gen 3 x1", //0xB2
    "PCI Express Gen 3 x2", //0xB3
    "PCI Express Gen 3 x4", //0xB4
    "PCI Express Gen 3 x8", //0xB5
    "PCI Express Gen 3 x16",//0xB6
    "UNUSED",               //0xB7
    "PCI Express Gen 4",    //0xB8
    "PCI Express Gen 4 x1", //0xB9
    "PCI Express Gen 4 x2", //0xBA
    "PCI Express Gen 4 x4", //0xBB
    "PCI Express Gen 4 x8", //0xBC
    "PCI Express Gen 4 x16",//0xBD
    "PCI Express Gen 5",    //0xBE
    "PCI Express Gen 5 x1", //0xBF
    "PCI Express Gen 5 x2", //0xC0
    "PCI Express Gen 5 x4", //0xC1
    "PCI Express Gen 5 x8", //0xC2
    "PCI Express Gen 5 x16",//0xC3
    "PCI Express Gen 6 and beyond",    //0xC4
    "Enterprise and datacenter 1U E1", //0xC5
    "Enterprise and datacenter 3\" E3", //0xC6
};

const char * get_smbios_slot_type_name(uint8_t slot_type) {
    if (slot_type > 0 && slot_type < 0x31) {
        return smbios_slot_names[slot_type];
    } else if (slot_type > 0x9F && slot_type < 0xC7) {
        return smbios_slot_names2[slot_type - 0xA0];
    } else {
        return "Unknown";
    }
}