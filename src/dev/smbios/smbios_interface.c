#include "smbios_interface.h"
#include "smbios.h"

#include "../../util/printf.h" // Required for printf

#define EXTRACT(x) ((uint8_t)((1 << (x.length)) - 1) & ((*(uint64_t*)x.pointer) >> (x.bit)))

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

uint8_t get_smbios_bios_loaded() {return smbios.bios.loaded;}
uint8_t get_smbios_system_loaded() {return smbios.system.loaded;}
uint8_t get_smbios_cpu_loaded() {return smbios.cpu.loaded;}

void dump_smbios() {

    if (get_smbios_bios_loaded()) {
        printf("[BIOS]\n");
        printf("get_smbios_bios_vendor: %s\n", get_smbios_bios_vendor());
        printf("get_smbios_bios_version: %s\n", get_smbios_bios_version());
        printf("get_smbios_bios_release_date: %s\n", get_smbios_bios_release_date());
        printf("get_smbios_bios_major_release: %d\n", *get_smbios_bios_major_release());
        printf("get_smbios_bios_minor_release: %d\n", *get_smbios_bios_minor_release());
        printf("get_smbios_bios_embedded_controller_major_release: %d\n", *get_smbios_bios_embedded_controller_major_release());
        printf("get_smbios_bios_embedded_controller_minor_release: %d\n", *get_smbios_bios_embedded_controller_minor_release());
        printf("get_smbios_bios_not_supports_characteristics: %d\n", get_smbios_bios_not_supports_characteristics());
        printf("get_smbios_bios_supports_isa: %d\n", get_smbios_bios_supports_isa());
        printf("get_smbios_bios_supports_mca: %d\n", get_smbios_bios_supports_mca());
        printf("get_smbios_bios_supports_pci: %d\n", get_smbios_bios_supports_pci());
        printf("get_smbios_bios_supports_pnp: %d\n", get_smbios_bios_supports_pnp());
        printf("get_smbios_bios_supports_apm: %d\n", get_smbios_bios_supports_apm());
        printf("get_smbios_bios_supports_edd: %d\n", get_smbios_bios_supports_edd());
        printf("get_smbios_bios_supports_escd: %d\n", get_smbios_bios_supports_escd());
        printf("get_smbios_bios_supports_eisa: %d\n", get_smbios_bios_supports_eisa());
        printf("get_smbios_bios_supports_vl_vesa: %d\n", get_smbios_bios_supports_vl_vesa());
        printf("get_smbios_bios_supports_boot_from_cd: %d\n", get_smbios_bios_supports_boot_from_cd());
        printf("get_smbios_bios_supports_selectable_boot: %d\n", get_smbios_bios_supports_selectable_boot());
        printf("get_smbios_bios_supports_acpi: %d\n", get_smbios_bios_supports_acpi());
        printf("get_smbios_bios_supports_usb_legacy: %d\n", get_smbios_bios_supports_usb_legacy()); 
        printf("get_smbios_bios_supports_agp: %d\n", get_smbios_bios_supports_agp());
        printf("get_smbios_bios_supports_bios_boot: %d\n", get_smbios_bios_supports_bios_boot());
        printf("get_smbios_bios_supports_uefi: %d\n", get_smbios_bios_supports_uefi());
        printf("get_smbios_bios_is_virtual_machine: %d\n", get_smbios_bios_is_virtual_machine());
    }

    if (get_smbios_system_loaded()) {
        printf("[System]\n");
        printf("get_smbios_system_manufacturer: %s\n", get_smbios_system_manufacturer());
        printf("get_smbios_system_product_name: %s\n", get_smbios_system_product_name());
        printf("get_smbios_system_version: %s\n", get_smbios_system_version());
        printf("get_smbios_system_serial_number: %s\n", get_smbios_system_serial_number());
        printf("get_smbios_system_sku_number: %s\n", get_smbios_system_sku_number());
        printf("get_smbios_system_family(): %s\n", get_smbios_system_family());
        printf("get_smbios_system_uuid: ");
        for (int i = 0; i < 16; i++)
            printf("%x:", get_smbios_system_uuid()[i]);
        printf("\n");
        printf("get_smbios_system_wake_up_type: %d\n", *get_smbios_system_wake_up_type());
    }

    if (get_smbios_cpu_loaded()) {
        printf("[CPU]\n");
        printf("get_smbios_cpu_socket_designation: %s\n", get_smbios_cpu_socket_designation());
        printf("get_smbios_cpu_processor_type: %d\n", *get_smbios_cpu_processor_type());         
        printf("get_smbios_cpu_processor_family: %d\n", *get_smbios_cpu_processor_family());
        printf("get_smbios_cpu_processor_manufacturer: %s\n", get_smbios_cpu_processor_manufacturer());       
        printf("get_smbios_cpu_processor_id: %llx\n", *get_smbios_cpu_processor_id());
        printf("get_smbios_cpu_processor_version: %s\n", get_smbios_cpu_processor_version());
        printf("get_smbios_cpu_voltage_legacy: %d\n", get_smbios_cpu_voltage_legacy());
        printf("get_smbios_cpu_voltage_5V: %d\n", get_smbios_cpu_voltage_5V());
        printf("get_smbios_cpu_voltage_3_3V: %d\n", get_smbios_cpu_voltage_3_3V());
        printf("get_smbios_cpu_voltage_2_9V: %d\n", get_smbios_cpu_voltage_2_9V());
        printf("get_smbios_cpu_external_clock: %d\n", *get_smbios_cpu_external_clock());
        printf("get_smbios_cpu_max_speed: %d\n", *get_smbios_cpu_max_speed());
        printf("get_smbios_cpu_current_speed: %d\n", *get_smbios_cpu_current_speed());
        printf("get_smbios_cpu_socket_populated: %d\n", get_smbios_cpu_socket_populated());
        printf("get_smbios_cpu_enabled: %d\n", get_smbios_cpu_enabled());
        printf("get_smbios_cpu_disabled_by_user: %d\n", get_smbios_cpu_disabled_by_user());
        printf("get_smbios_cpu_disabled_by_bios: %d\n", get_smbios_cpu_disabled_by_bios());
        printf("get_smbios_cpu_idle: %d\n", get_smbios_cpu_idle());
        printf("get_smbios_cpu_processor_upgrade: %d\n", *get_smbios_cpu_processor_upgrade());
        printf("get_smbios_cpu_serial_number: %s\n", get_smbios_cpu_serial_number());
        printf("get_smbios_cpu_asset_tag: %s\n", get_smbios_cpu_asset_tag());
        printf("get_smbios_cpu_part_number: %s\n", get_smbios_cpu_part_number());     
        printf("get_smbios_cpu_core_count: %d\n", *get_smbios_cpu_core_count());
        printf("get_smbios_cpu_core_enabled: %d\n", *get_smbios_cpu_core_enabled());
        printf("get_smbios_cpu_thread_count: %d\n", *get_smbios_cpu_thread_count());
        printf("get_smbios_cpu_supports_64_bits: %d\n", get_smbios_cpu_supports_64_bits());
        printf("get_smbios_cpu_supports_multicore: %d\n", get_smbios_cpu_supports_multicore());
        printf("get_smbios_cpu_supports_hardware_thread: %d\n", get_smbios_cpu_supports_hardware_thread());
        printf("get_smbios_cpu_supports_execute_protection: %d\n", get_smbios_cpu_supports_execute_protection());
        printf("get_smbios_cpu_supports_enhanced_virtualization: %d\n", get_smbios_cpu_supports_enhanced_virtualization());
        printf("get_smbios_cpu_supports_power_management: %d\n", get_smbios_cpu_supports_power_management());
        printf("get_smbios_cpu_supports_128_bits: %d\n", get_smbios_cpu_supports_128_bits());
        printf("get_smbios_cpu_is_arm_soc: %d\n", get_smbios_cpu_is_arm_soc());
        printf("get_smbios_cpu_processor_family2: %d\n", *get_smbios_cpu_processor_family2());
    }
}

void init_smbios_interface() {
    printf("### SMBIOS STARTUP###\n");
    set_smbios_bios_loaded(0);
    set_smbios_system_loaded(0);
    set_smbios_cpu_loaded(0);

    init_smbios(&smbios);
}

char *      get_smbios_bios_vendor() {                           return  (char*)smbios.bios.vendor.pointer;}
char *      get_smbios_bios_version() {                          return  (char*)smbios.bios.version.pointer;}
char *      get_smbios_bios_release_date() {                     return  (char*)smbios.bios.release_date.pointer;}
uint8_t *   get_smbios_bios_major_release() {                    return  (uint8_t*)smbios.bios.system_bios_major_release.pointer;}
uint8_t *   get_smbios_bios_minor_release() {                    return  (uint8_t*)smbios.bios.system_bios_minor_release.pointer;}
uint8_t *   get_smbios_bios_embedded_controller_major_release() {return  (uint8_t*)smbios.bios.embedded_controller_major_release.pointer;}
uint8_t *   get_smbios_bios_embedded_controller_minor_release() {return  (uint8_t*)smbios.bios.embedded_controller_minor_release.pointer;}
uint8_t     get_smbios_bios_not_supports_characteristics() {     return  EXTRACT(smbios.bios.supports_characteristics);}
uint8_t     get_smbios_bios_supports_isa() {                     return  EXTRACT(smbios.bios.supports_isa);}
uint8_t     get_smbios_bios_supports_mca() {                     return  EXTRACT(smbios.bios.supports_mca);}
uint8_t     get_smbios_bios_supports_pci() {                     return  EXTRACT(smbios.bios.supports_pci);}
uint8_t     get_smbios_bios_supports_pnp() {                     return  EXTRACT(smbios.bios.supports_pnp);}
uint8_t     get_smbios_bios_supports_apm() {                     return  EXTRACT(smbios.bios.supports_apm);}
uint8_t     get_smbios_bios_supports_edd() {                     return  EXTRACT(smbios.bios.supports_edd);}
uint8_t     get_smbios_bios_supports_escd() {                    return  EXTRACT(smbios.bios.supports_escd);}
uint8_t     get_smbios_bios_supports_eisa() {                    return  EXTRACT(smbios.bios.supports_eisa);}
uint8_t     get_smbios_bios_supports_vl_vesa() {                 return  EXTRACT(smbios.bios.supports_vl_vesa);}
uint8_t     get_smbios_bios_supports_boot_from_cd() {            return  EXTRACT(smbios.bios.supports_boot_from_cd);}
uint8_t     get_smbios_bios_supports_selectable_boot() {         return  EXTRACT(smbios.bios.supports_selectable_boot);}
uint8_t     get_smbios_bios_supports_acpi() {                    return  EXTRACT(smbios.bios.supports_acpi);}
uint8_t     get_smbios_bios_supports_usb_legacy() {              return  EXTRACT(smbios.bios.supports_usb_legacy);}
uint8_t     get_smbios_bios_supports_agp() {                     return  EXTRACT(smbios.bios.supports_agp);}
uint8_t     get_smbios_bios_supports_bios_boot() {               return  EXTRACT(smbios.bios.supports_bios_boot);}
uint8_t     get_smbios_bios_supports_uefi() {                    return  EXTRACT(smbios.bios.supports_uefi);}
uint8_t     get_smbios_bios_is_virtual_machine() {               return  EXTRACT(smbios.bios.is_virtual_machine);}

char *      get_smbios_system_manufacturer() {                   return (char*)smbios.system.manufacturer.pointer;}
char *      get_smbios_system_product_name() {                   return (char*)smbios.system.product_name.pointer;}
char *      get_smbios_system_version() {                        return (char*)smbios.system.version.pointer;}
char *      get_smbios_system_serial_number() {                  return (char*)smbios.system.serial_number.pointer;}
char *      get_smbios_system_sku_number() {                     return (char*)smbios.system.sku_number.pointer;}
char *      get_smbios_system_family() {                         return (char*)smbios.system.family.pointer;}
uint8_t *   get_smbios_system_uuid() {                           return (uint8_t*)smbios.system.uuid.pointer;}
uint8_t *   get_smbios_system_wake_up_type() {                   return (uint8_t*)smbios.system.wake_up_type.pointer;}

char *      get_smbios_cpu_socket_designation() {                return (char*)smbios.cpu.socket_designation.pointer;}
uint8_t *   get_smbios_cpu_processor_type() {                    return (uint8_t*)smbios.cpu.processor_type.pointer;}
uint8_t *   get_smbios_cpu_processor_family() {                  return (uint8_t*)smbios.cpu.processor_family.pointer;}
char *      get_smbios_cpu_processor_manufacturer() {            return (char*)smbios.cpu.processor_manufacturer.pointer;}
uint64_t *  get_smbios_cpu_processor_id() {                      return (uint64_t*)smbios.cpu.processor_id.pointer;}
char *      get_smbios_cpu_processor_version() {                 return (char*)smbios.cpu.processor_version.pointer;}
uint8_t     get_smbios_cpu_voltage_legacy() {                    return EXTRACT(smbios.cpu.voltage_legacy);}
uint8_t     get_smbios_cpu_voltage_5V() {                        return EXTRACT(smbios.cpu.voltage_5V);}
uint8_t     get_smbios_cpu_voltage_3_3V() {                      return EXTRACT(smbios.cpu.voltage_3_3V);}
uint8_t     get_smbios_cpu_voltage_2_9V() {                      return EXTRACT(smbios.cpu.voltage_2_9V);}
uint16_t *  get_smbios_cpu_external_clock() {                    return (uint16_t*)smbios.cpu.external_clock.pointer;}
uint16_t *  get_smbios_cpu_max_speed() {                         return (uint16_t*)smbios.cpu.max_speed.pointer;}
uint16_t *  get_smbios_cpu_current_speed() {                     return (uint16_t*)smbios.cpu.current_speed.pointer;}
uint8_t     get_smbios_cpu_socket_populated() {                  return EXTRACT(smbios.cpu.socket_populated);}
uint8_t     get_smbios_cpu_enabled() {                           return EXTRACT(smbios.cpu.cpu_status);}
uint8_t     get_smbios_cpu_disabled_by_user() {                  return EXTRACT(smbios.cpu.cpu_status);}
uint8_t     get_smbios_cpu_disabled_by_bios() {                  return EXTRACT(smbios.cpu.cpu_status);}
uint8_t     get_smbios_cpu_idle() {                              return EXTRACT(smbios.cpu.cpu_status);}
uint8_t *   get_smbios_cpu_processor_upgrade() {                 return (uint8_t*)smbios.cpu.processor_upgrade.pointer;}
char *      get_smbios_cpu_serial_number() {                     return (char*)smbios.cpu.serial_number.pointer;}
char *      get_smbios_cpu_asset_tag() {                         return (char*)smbios.cpu.asset_tag.pointer;}
char *      get_smbios_cpu_part_number() {                       return (char*)smbios.cpu.part_number.pointer;}
uint8_t *   get_smbios_cpu_core_count() {                        return (uint8_t*)smbios.cpu.core_count.pointer;}
uint8_t *   get_smbios_cpu_core_enabled() {                      return (uint8_t*)smbios.cpu.core_enabled.pointer;}
uint8_t *   get_smbios_cpu_thread_count() {                      return (uint8_t*)smbios.cpu.thread_count.pointer;}
uint8_t     get_smbios_cpu_supports_64_bits() {                  return EXTRACT(smbios.cpu.supports_64_bits);}
uint8_t     get_smbios_cpu_supports_multicore() {                return EXTRACT(smbios.cpu.supports_multicore);}
uint8_t     get_smbios_cpu_supports_hardware_thread() {          return EXTRACT(smbios.cpu.supports_hardware_thread);}
uint8_t     get_smbios_cpu_supports_execute_protection() {       return EXTRACT(smbios.cpu.supports_execute_protection);}
uint8_t     get_smbios_cpu_supports_enhanced_virtualization() {  return EXTRACT(smbios.cpu.supports_enhanced_virtualization);}
uint8_t     get_smbios_cpu_supports_power_management() {         return EXTRACT(smbios.cpu.supports_power_management);}
uint8_t     get_smbios_cpu_supports_128_bits() {                 return EXTRACT(smbios.cpu.supports_128_bits);}
uint8_t     get_smbios_cpu_is_arm_soc() {                        return EXTRACT(smbios.cpu.is_arm_soc);}
uint16_t *  get_smbios_cpu_processor_family2() {                 return (uint16_t*)smbios.cpu.processor_family2.pointer;}
uint16_t *  get_smbios_cpu_core_count2() {                       return (uint16_t*)smbios.cpu.core_count2.pointer;}
uint16_t *  get_smbios_cpu_core_enabled2() {                     return (uint16_t*)smbios.cpu.core_enabled2.pointer;}
uint16_t *  get_smbios_cpu_thread_count2() {                     return (uint16_t*)smbios.cpu.thread_count2.pointer;}