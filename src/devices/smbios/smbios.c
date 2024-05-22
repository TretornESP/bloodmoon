#include "smbios.h"
#include "smbios_interface.h"

#include "../../memory/paging.h"               //Required for: map_current_memory
#include "../../util/printf.h"               //Required for: printf
#include "../../util/string.h"               //Required for: memcmp, strlen
#include "../../bootservices/bootservices.h" //Required for: get_smbios32_address, get_smbios64_address
#include "../../util/dbgprinter.h"           //Required for: panic

char * get_smbios_string(void * first_addr, uint8_t index) {
    char * addr = first_addr;

    for (int i = 0; i < (index-1); i++) {
        addr += strlen(addr) + 1;
    }

    return addr;
}

uint64_t get_smbios_string_length(void * first_addr) {
    char * addr = first_addr;

    while (*addr != '\0' || *(addr+1) != '\0') {
        addr++;
    }

    return ((uint64_t)(addr+2) - (uint64_t)first_addr);
}


uint8_t process_bios_information(uint16_t version, void* bios_information, struct smbios_bios_data* data) {

    //You can change this to implement more versions pretty easily!
    void * strings_addr;
    if (version >= 20) {
        strings_addr = (void*)((uint64_t)bios_information + sizeof(struct smbios2_bios_information));
    }
    if (version >= 24) {
        strings_addr = (void*)((uint64_t)bios_information + sizeof(struct smbios24_bios_information));
    }
    if (version >= 31) {
        strings_addr = (void*)((uint64_t)bios_information + sizeof(struct smbios31_bios_information));
    }

    if (version >= 20) {
            struct smbios2_bios_information * bios = (struct smbios2_bios_information *)bios_information;

            char * vendor_string = get_smbios_string(strings_addr, bios->vendor);
            char * version_string = get_smbios_string(strings_addr, bios->version);
            char * release_date_string = get_smbios_string(strings_addr, bios->release_date);

            insert_smbios_data(&data->vendor, vendor_string, strlen(vendor_string));
            insert_smbios_data(&data->version, version_string, strlen(version_string));
            insert_smbios_data(&data->start_address_segment, &bios->start_address_segment, 2);
            insert_smbios_data(&data->release_date, release_date_string, strlen(release_date_string));
            insert_smbios_data(&data->rom_size, &bios->rom_size, 1);
            insert_smbios_bitfield(&data->supports_characteristics, &bios->characteristics, 3, 1);
            insert_smbios_bitfield(&data->supports_isa, &bios->characteristics, 4, 1);
            insert_smbios_bitfield(&data->supports_mca, &bios->characteristics, 5, 1);
            insert_smbios_bitfield(&data->supports_eisa, &bios->characteristics, 6, 1);
            insert_smbios_bitfield(&data->supports_pci, &bios->characteristics, 7, 1);
            insert_smbios_bitfield(&data->supports_pcmcia, &bios->characteristics, 8, 1);
            insert_smbios_bitfield(&data->supports_pnp, &bios->characteristics, 9, 1);
            insert_smbios_bitfield(&data->supports_apm, &bios->characteristics, 10, 1);
            insert_smbios_bitfield(&data->supports_flash_upgrade, &bios->characteristics, 11, 1);
            insert_smbios_bitfield(&data->supports_bios_shadowing, &bios->characteristics, 12, 1);
            insert_smbios_bitfield(&data->supports_vl_vesa, &bios->characteristics, 13, 1);
            insert_smbios_bitfield(&data->supports_escd, &bios->characteristics, 14, 1);
            insert_smbios_bitfield(&data->supports_boot_from_cd, &bios->characteristics, 15, 1);
            insert_smbios_bitfield(&data->supports_selectable_boot, &bios->characteristics, 16, 1);
            insert_smbios_bitfield(&data->supports_bios_rom_socketed, &bios->characteristics, 17, 1);
            insert_smbios_bitfield(&data->supports_boot_from_pc_card, &bios->characteristics, 18, 1);
            insert_smbios_bitfield(&data->supports_edd, &bios->characteristics, 19, 1);
            insert_smbios_bitfield(&data->supports_int_13h_japanese_floppy_nec9800, &bios->characteristics, 20, 1);
            insert_smbios_bitfield(&data->supports_int_13h_japanese_floppy_toshiba, &bios->characteristics, 21, 1);
            insert_smbios_bitfield(&data->supports_int_13h_525_360_kb_floppy, &bios->characteristics, 22, 1);
            insert_smbios_bitfield(&data->supports_int_13h_525_1200_kb_floppy, &bios->characteristics, 23, 1);
            insert_smbios_bitfield(&data->supports_int_13h_35_720_kb_floppy, &bios->characteristics, 24, 1);
            insert_smbios_bitfield(&data->supports_int_13h_35_2880_kb_floppy, &bios->characteristics, 25, 1);
            insert_smbios_bitfield(&data->supports_int_5h_print_screen, &bios->characteristics, 26, 1);
            insert_smbios_bitfield(&data->supports_int_9h_842_keyboad, &bios->characteristics, 27, 1);
            insert_smbios_bitfield(&data->supports_int_14h_serial_services, &bios->characteristics, 28, 1);
            insert_smbios_bitfield(&data->supports_int_17h_printer_services, &bios->characteristics, 29, 1);
            insert_smbios_bitfield(&data->supports_int_10h_cga_mono_video, &bios->characteristics, 30, 1);
            insert_smbios_bitfield(&data->supports_nec_pc_98, &bios->characteristics, 31, 1);
            insert_smbios_bitfield(&data->reserved_bios_vendor, &bios->characteristics, 32, 16);
            insert_smbios_bitfield(&data->reserved_system_vendor, &bios->characteristics, 48, 16);
    } else {
        printf("BIOS Information table version %d not supported\n", version);
        return 0;
    }


    if (version >= 24) {

        struct smbios24_bios_information * bios = (struct smbios24_bios_information *)bios_information;

        insert_smbios_bitfield(&data->supports_acpi, &(bios->characteristics_extension_bytes[0]), 0, 1);
        insert_smbios_bitfield(&data->supports_usb_legacy, &(bios->characteristics_extension_bytes[0]), 1, 1);
        insert_smbios_bitfield(&data->supports_agp, &(bios->characteristics_extension_bytes[0]), 2, 1);
        insert_smbios_bitfield(&data->supports_i2o_boot, &(bios->characteristics_extension_bytes[0]), 3, 1);
        insert_smbios_bitfield(&data->supports_ls_120_boot, &(bios->characteristics_extension_bytes[0]), 4, 1);
        insert_smbios_bitfield(&data->supports_atapi_zip_drive_boot, &(bios->characteristics_extension_bytes[0]), 5, 1);
        insert_smbios_bitfield(&data->supports_1394_boot, &(bios->characteristics_extension_bytes[0]), 6, 1);
        insert_smbios_bitfield(&data->supports_smart_battery, &(bios->characteristics_extension_bytes[0]), 7, 1);

        insert_smbios_bitfield(&data->supports_bios_boot, &(bios->characteristics_extension_bytes[1]), 0, 1);
        insert_smbios_bitfield(&data->fnkey_initiated_netboot, &(bios->characteristics_extension_bytes[1]), 1, 1);
        insert_smbios_bitfield(&data->supports_targeted_content_distribution, &(bios->characteristics_extension_bytes[1]), 2, 1);
        insert_smbios_bitfield(&data->supports_uefi, &(bios->characteristics_extension_bytes[1]), 3, 1);
        insert_smbios_bitfield(&data->is_virtual_machine, &(bios->characteristics_extension_bytes[1]), 4, 1);
        insert_smbios_bitfield(&data->manufacturing_mode_supported, &(bios->characteristics_extension_bytes[1]), 5, 1);
        insert_smbios_bitfield(&data->manufacturing_mode_enabled, &(bios->characteristics_extension_bytes[1]), 6, 1);
    
        insert_smbios_data(&data->system_bios_major_release, &(bios->system_bios_major_release), 1);
        insert_smbios_data(&data->system_bios_minor_release, &(bios->system_bios_minor_release), 1);
        insert_smbios_data(&data->embedded_controller_major_release, &(bios->embedded_controller_major_release), 1);
        insert_smbios_data(&data->embedded_controller_minor_release, &(bios->embedded_controller_minor_release), 1);
    }

    if (version >= 31) {
        struct smbios31_bios_information * bios = (struct smbios31_bios_information *)bios_information;

        insert_smbios_data(&data->extended_bios_rom_size, &(bios->extended_bios_rom_size), 2);
    }

    set_smbios_bios_loaded(1);
    return 1;
}

uint8_t process_system_information(uint16_t version, void* system_information, struct smbios_system_data* data) {

    void * strings_addr;
    if (version >= 20) {
        strings_addr = (void*)((uint64_t)system_information + sizeof(struct smbios20_system_information));
    }
    if (version >= 21) {
        strings_addr = (void*)((uint64_t)system_information + sizeof(struct smbios21_system_information));
    }
    if (version >= 24) {
        strings_addr = (void*)((uint64_t)system_information + sizeof(struct smbios24_system_information));
    }

    if (version >= 20) {
        struct smbios20_system_information * system = (struct smbios20_system_information *)system_information;

        char * manufacturer_string = get_smbios_string(strings_addr, system->manufacturer);
        char * product_name_string = get_smbios_string(strings_addr, system->product_name);
        char * version_string = get_smbios_string(strings_addr, system->version);
        char * serial_number_string = get_smbios_string(strings_addr, system->serial_number);

        insert_smbios_data(&data->manufacturer, manufacturer_string, strlen(manufacturer_string));
        insert_smbios_data(&data->product_name, product_name_string, strlen(product_name_string));
        insert_smbios_data(&data->version, version_string, strlen(version_string));
        insert_smbios_data(&data->serial_number, serial_number_string, strlen(serial_number_string));
    } else {
        printf("System Information table version %d not supported\n", version);
        return 0;
    }

    if (version >= 21) {

        struct smbios21_system_information * system = (struct smbios21_system_information *)system_information;

        insert_smbios_data(&data->uuid, system->uuid, 16);
        insert_smbios_data(&data->wake_up_type, &system->wake_up_type, 1);
    }
    
    if (version >= 24) {
        struct smbios24_system_information * system = (struct smbios24_system_information *)system_information;
       
        char * sku_number_string = get_smbios_string(strings_addr, system->sku_number);
        char * family_string = get_smbios_string(strings_addr, system->family);

        insert_smbios_data(&data->sku_number, sku_number_string, strlen(sku_number_string));
        insert_smbios_data(&data->family, family_string, strlen(family_string));

    }

    set_smbios_system_loaded(1);
    return 1;
}

uint8_t process_baseboard_information(uint16_t version, void* baseboard_information, struct smbios_baseboard_data * data) {
}

uint8_t process_cpu_information(uint16_t version, void* cpu_information, struct smbios_cpu_data * data) {
    
    void * strings_addr;
    if (version >= 20) {
        strings_addr = (void*)((uint64_t)cpu_information + sizeof(struct smbios20_processor_information));
    }
    if (version >= 21) {
        strings_addr = (void*)((uint64_t)cpu_information + sizeof(struct smbios21_processor_information));
    }
    if (version >= 23) {
        strings_addr = (void*)((uint64_t)cpu_information + sizeof(struct smbios23_processor_information));
    }
    if (version >= 25) {
        strings_addr = (void*)((uint64_t)cpu_information + sizeof(struct smbios25_processor_information));
    }
    if (version >= 26) {
        strings_addr = (void*)((uint64_t)cpu_information + sizeof(struct smbios26_processor_information));
    }
    if (version >= 30) {
        strings_addr = (void*)((uint64_t)cpu_information + sizeof(struct smbios30_processor_information));
    }

    if (version >= 20) {
        struct smbios20_processor_information * processor = (struct smbios20_processor_information *)cpu_information;

        char * socket_designation = get_smbios_string(strings_addr, processor->socket_designation);
        char * processor_manufacturer = get_smbios_string(strings_addr, processor->processor_manufacturer);
        char * processor_version = get_smbios_string(strings_addr, processor->processor_version);

        insert_smbios_data(&data->socket_designation, socket_designation, strlen(socket_designation));
        insert_smbios_data(&data->processor_type, &processor->processor_type, 1);
        insert_smbios_data(&data->processor_family, &processor->processor_family, 1);
        insert_smbios_data(&data->processor_manufacturer, processor_manufacturer, strlen(processor_manufacturer));
        insert_smbios_data(&data->processor_id, &processor->processor_id, 8);
        insert_smbios_data(&data->processor_version, processor_version, strlen(processor_version));
        insert_smbios_bitfield(&data->voltage_legacy, &processor->voltage, 7, 1);
        insert_smbios_bitfield(&data->voltage_5V, &processor->voltage, 0, 1);
        insert_smbios_bitfield(&data->voltage_3_3V, &processor->voltage, 1, 1);
        insert_smbios_bitfield(&data->voltage_2_9V, &processor->voltage, 2, 1);
        insert_smbios_data(&data->external_clock, &processor->external_clock, 2);
        insert_smbios_data(&data->max_speed, &processor->max_speed, 2);
        insert_smbios_data(&data->current_speed, &processor->current_speed, 2);
        insert_smbios_bitfield(&data->socket_populated, &processor->status, 6, 1);
        insert_smbios_bitfield(&data->cpu_status, &processor->status, 0, 2);
        insert_smbios_data(&data->processor_upgrade, &processor->processor_upgrade, 1);
    } else {
        printf("Cpu Information table version %d not supported\n", version);
        return 0;
    }

    if (version >= 21) {
        struct smbios21_processor_information * processor = (struct smbios21_processor_information *)cpu_information;

        insert_smbios_data(&data->l1_cache_handle, &processor->l1_cache_handle, 2);
        insert_smbios_data(&data->l2_cache_handle, &processor->l2_cache_handle, 2);
        insert_smbios_data(&data->l3_cache_handle, &processor->l3_cache_handle, 2);
    }

    if (version >= 23) {
        struct smbios23_processor_information * processor = (struct smbios23_processor_information *)cpu_information;

        char * serial_number = get_smbios_string(strings_addr, processor->serial_number);
        char * asset_tag = get_smbios_string(strings_addr, processor->asset_tag);
        char * part_number = get_smbios_string(strings_addr, processor->part_number);

        insert_smbios_data(&data->serial_number, serial_number, strlen(serial_number));
        insert_smbios_data(&data->asset_tag, asset_tag, strlen(asset_tag));
        insert_smbios_data(&data->part_number, part_number, strlen(part_number));
    }

    if (version >= 25) {
        struct smbios25_processor_information * processor = (struct smbios25_processor_information *)cpu_information;

        insert_smbios_data(&data->core_count, &processor->core_count, 1);
        insert_smbios_data(&data->core_enabled, &processor->core_enabled, 1);
        insert_smbios_data(&data->thread_count, &processor->thread_count, 1);

        insert_smbios_bitfield(&data->supports_64_bits, &processor->characteristics, 2, 1);
        insert_smbios_bitfield(&data->supports_multicore, &processor->characteristics, 3, 1);
        insert_smbios_bitfield(&data->supports_hardware_thread, &processor->characteristics, 4, 1);
        insert_smbios_bitfield(&data->supports_execute_protection, &processor->characteristics, 5, 1);
        insert_smbios_bitfield(&data->supports_enhanced_virtualization, &processor->characteristics, 6, 1);
        insert_smbios_bitfield(&data->supports_power_management, &processor->characteristics, 7, 1);
        insert_smbios_bitfield(&data->supports_128_bits, &processor->characteristics, 8, 1);
        insert_smbios_bitfield(&data->is_arm_soc, &processor->characteristics, 9, 1);
    }

    if (version >= 26) {
        struct smbios26_processor_information * processor = (struct smbios26_processor_information *)cpu_information;

        insert_smbios_data(&data->processor_family2, &processor->processor_family2, 2);
    }

    if (version >= 30) {
        struct smbios30_processor_information * processor = (struct smbios30_processor_information *)cpu_information;

        insert_smbios_data(&data->core_count2, &processor->core_count2, 2);
        insert_smbios_data(&data->core_enabled2, &processor->core_enabled2, 2);
        insert_smbios_data(&data->thread_count2, &processor->thread_count2, 2);
    }

    if (version >= 36) {
        struct smbios36_processor_information * processor = (struct smbios36_processor_information *)cpu_information;

        insert_smbios_data(&data->thread_enabled, &processor->thread_enabled, 2);
    }

    set_smbios_cpu_loaded(1);
    return 1;
}

uint8_t process_memory_array_information(uint16_t version, void* memory_array_information, struct smbios_memory_array_data* data) {

    if (version >= 21) {
        struct smbios21_memory_array_information * memory_array = (struct smbios21_memory_array_information *)memory_array_information;

        insert_smbios_data(&data->location, &memory_array->location, 1);
        insert_smbios_data(&data->use, &memory_array->use, 1);
        insert_smbios_data(&data->error_correction, &memory_array->error_correction, 1);
        insert_smbios_data(&data->maximum_capacity, &memory_array->maximum_capacity, 4);
        insert_smbios_data(&data->error_information_handle, &memory_array->error_information_handle, 2);
        insert_smbios_data(&data->number_of_memory_devices, &memory_array->number_of_memory_devices, 2);
    } else {
        printf("Memory array table version %d not supported\n", version);
        return 0;
    }

    if (version >= 27) {

        struct smbios27_memory_array_information * system = (struct smbios27_memory_array_information *)memory_array_information;

        insert_smbios_data(&data->extended_maximum_capacity, &system->extended_maximum_capacity, 8);
    }
    
    return 1;
}

uint8_t process_memory_device_information(uint16_t version, void* memory_device_information, struct smbios_memory_device_data * data) {
    
    void * strings_addr;
    if (version >= 21) {
        strings_addr = (void*)((uint64_t)memory_device_information + sizeof(struct smbios21_memory_device_information));
    }
    if (version >= 23) {
        strings_addr = (void*)((uint64_t)memory_device_information + sizeof(struct smbios23_memory_device_information));
    }
    if (version >= 26) {
        strings_addr = (void*)((uint64_t)memory_device_information + sizeof(struct smbios26_memory_device_information));
    }
    if (version >= 27) {
        strings_addr = (void*)((uint64_t)memory_device_information + sizeof(struct smbios27_memory_device_information));
    }
    if (version >= 28) {
        strings_addr = (void*)((uint64_t)memory_device_information + sizeof(struct smbios28_memory_device_information));
    }
    if (version >= 32) {
        strings_addr = (void*)((uint64_t)memory_device_information + sizeof(struct smbios32_memory_device_information));
    }

    if (version >= 21) {
        struct smbios21_memory_device_information * memory_device = (struct smbios21_memory_device_information *)memory_device_information;

        char * device_locator = get_smbios_string(strings_addr, memory_device->device_locator);
        char * bank_locator = get_smbios_string(strings_addr, memory_device->bank_locator);

        insert_smbios_data(&data->physical_memory_array_handle, &memory_device->physical_memory_array_handle, 2);
        insert_smbios_data(&data->memory_error_information_handle, &memory_device->memory_error_information_handle, 2);
        insert_smbios_data(&data->total_width, &memory_device->total_width, 1);
        insert_smbios_data(&data->data_width, &memory_device->data_width, 1);
        insert_smbios_data(&data->size, &memory_device->size, 1);
        insert_smbios_data(&data->form_factor, &memory_device->form_factor, 1);
        insert_smbios_data(&data->device_set, &memory_device->device_set, 1);
        insert_smbios_data(&data->device_locator, device_locator, strlen(device_locator));
        insert_smbios_data(&data->bank_locator, bank_locator, strlen(bank_locator));
        insert_smbios_data(&data->memory_type, &memory_device->memory_type, 1);
        insert_smbios_bitfield(&data->type_detail_other, &memory_device->type_detail, 1, 1);
        insert_smbios_bitfield(&data->type_detail_unknown, &memory_device->type_detail, 2, 1);
        insert_smbios_bitfield(&data->type_detail_fast_paged, &memory_device->type_detail, 3, 1);
        insert_smbios_bitfield(&data->type_detail_static_column, &memory_device->type_detail, 4, 1);
        insert_smbios_bitfield(&data->type_detail_pseudo_static, &memory_device->type_detail, 5, 1);
        insert_smbios_bitfield(&data->type_detail_ram_bus, &memory_device->type_detail, 6, 1);
        insert_smbios_bitfield(&data->type_detail_synchronous, &memory_device->type_detail, 7, 1);
        insert_smbios_bitfield(&data->type_detail_cmos, &memory_device->type_detail, 8, 1);
        insert_smbios_bitfield(&data->type_detail_edo, &memory_device->type_detail, 9, 1);
        insert_smbios_bitfield(&data->type_detail_window_dram, &memory_device->type_detail, 10, 1);
        insert_smbios_bitfield(&data->type_detail_cache_dram, &memory_device->type_detail, 11, 1);
        insert_smbios_bitfield(&data->type_detail_non_volatile, &memory_device->type_detail, 12, 1);
        insert_smbios_bitfield(&data->type_detail_registered_buffered, &memory_device->type_detail, 13, 1);
        insert_smbios_bitfield(&data->type_detail_unbuffered, &memory_device->type_detail, 14, 1);
        insert_smbios_bitfield(&data->type_detail_lr_dimm, &memory_device->type_detail, 15, 1);
    } else {
        printf("Cpu Information table version %d not supported\n", version);
        return 0;
    }
    if (version >= 23) {
        struct smbios23_memory_device_information * memory_device = (struct smbios23_memory_device_information *)memory_device_information;

        char * manufacturer = get_smbios_string(strings_addr, memory_device->manufacturer);
        char * serial_number = get_smbios_string(strings_addr, memory_device->serial_number);
        char * asset_tag = get_smbios_string(strings_addr, memory_device->asset_tag);
        char * part_number = get_smbios_string(strings_addr, memory_device->part_number);

        insert_smbios_data(&data->speed, &memory_device->speed, 2);
        insert_smbios_data(&data->manufacturer, manufacturer, strlen(manufacturer));
        insert_smbios_data(&data->serial_number, serial_number, strlen(serial_number));
        insert_smbios_data(&data->asset_tag, asset_tag, strlen(asset_tag));
        insert_smbios_data(&data->part_number, part_number, strlen(part_number));
    }
    if (version >= 26) {
        struct smbios26_memory_device_information * memory_device = (struct smbios26_memory_device_information *)memory_device_information;
    
        insert_smbios_data(&data->attributes, &memory_device->attributes, 1);
    }
    if (version >= 27) {
        struct smbios27_memory_device_information * memory_device = (struct smbios27_memory_device_information *)memory_device_information;
    
        insert_smbios_data(&data->extended_size, &memory_device->extended_size, 4);
        insert_smbios_data(&data->configured_memory_clock_speed, &memory_device->configured_memory_clock_speed, 2);
    }
    if (version >= 28) {
        struct smbios28_memory_device_information * memory_device = (struct smbios28_memory_device_information *)memory_device_information;
    
        insert_smbios_data(&data->minimum_voltage, &memory_device->minimum_voltage, 2);
        insert_smbios_data(&data->maximum_voltage, &memory_device->maximum_voltage, 2);
        insert_smbios_data(&data->configured_voltage, &memory_device->configured_voltage, 2);
    }
    if (version >= 32) {
        struct smbios32_memory_device_information * memory_device = (struct smbios32_memory_device_information *)memory_device_information;

        char * furmware_version = get_smbios_string(strings_addr, memory_device->firmware_version);

        insert_smbios_data(&data->memory_technology, &memory_device->memory_technology, 2);
        insert_smbios_bitfield(&data->memory_operating_mode_capability_other, &memory_device->memory_operating_mode_capability, 1, 1);
        insert_smbios_bitfield(&data->memory_operating_mode_capability_unknown, &memory_device->memory_operating_mode_capability, 2, 1);
        insert_smbios_bitfield(&data->memory_operating_mode_capability_volatile, &memory_device->memory_operating_mode_capability, 3, 1);
        insert_smbios_bitfield(&data->memory_operating_mode_capability_byte_access, &memory_device->memory_operating_mode_capability, 4, 1);
        insert_smbios_bitfield(&data->memory_operating_mode_capability_block_access, &memory_device->memory_operating_mode_capability, 5, 1);
        insert_smbios_data(&data->firmware_version, furmware_version, strlen(furmware_version));
        insert_smbios_data(&data->module_manufacturer_id, &memory_device->module_manufacturer_id, 2);
        insert_smbios_data(&data->module_product_id, &memory_device->module_product_id, 2);
        insert_smbios_data(&data->memory_subsystem_controller_manufacturer_id, &memory_device->memory_subsystem_controller_manufacturer_id, 2);
        insert_smbios_data(&data->memory_subsystem_controller_product_id, &memory_device->memory_subsystem_controller_product_id, 2);
        insert_smbios_data(&data->non_volatile_size, &memory_device->non_volatile_size, 8);
        insert_smbios_data(&data->volatile_size, &memory_device->volatile_size, 8);
        insert_smbios_data(&data->cache_size, &memory_device->cache_size, 8);
        insert_smbios_data(&data->logical_size, &memory_device->logical_size, 8);
    }

    return 1;
}

uint8_t process_oem_strings(uint16_t version, void* oem_strings, struct smbios_oem_data* data) {
    (void)data;
    if (version < 20) {
        printf("OEM Strings table version %d not supported\n", version);
        return;   
    }
    if (version < 21) {
        printf("OEM Strings table version %d not supported\n", version);
        return;
    }
    if (version < 24) {
        printf("OEM Strings table version %d not supported\n", version);
        return;
    } else {
        struct smbios_oem_strings * oem = (struct smbios_oem_strings *)oem_strings;
        uint8_t count = oem->count;
        void * strings_addr = (void*)((uint64_t)oem + sizeof(struct smbios_oem_strings));
        printf("OEM v%d Strings table at [%p]: %s\n",
            version,
            oem,
            (char*)strings_addr
        );

        insert_smbios_data(&data->count, &count, 1);

        if (count > 0)
            insert_smbios_data(&data->strings, (char*)strings_addr, strlen((char*)strings_addr));
        else
            insert_smbios_data(&data->strings, "", 0);

        set_smbios_oem_loaded(1);

        return;
    }
}

uint8_t process_slot_information(uint16_t version, void* header, struct smbios_slot_data* data) {
    uint8_t groupings = 0;
    if (version >= 32) {
        struct smbios32_system_slots_information * slot = (struct smbios32_system_slots_information *)header;
        groupings = slot->peer_grouping_n;
    }

    void * strings_addr;
    if (version >= 20) {
        strings_addr = (void*)((uint64_t)header + sizeof(struct smbios20_system_slots_information));
    }
    if (version >= 21) {
        strings_addr = (void*)((uint64_t)header + sizeof(struct smbios21_system_slots_information));
    }
    if (version >= 26) {
        strings_addr = (void*)((uint64_t)header + sizeof(struct smbios26_system_slots_information));
    }
    if (version >= 32) {
        strings_addr = (void*)((uint64_t)header + sizeof(struct smbios32_system_slots_information));
        strings_addr += (groupings * sizeof(struct smbios_system_slots_groups));
    }
    if (version >= 34) {
        strings_addr = (void*)((uint64_t)header + sizeof(struct smbios32_system_slots_information));
        strings_addr += (groupings * sizeof(struct smbios_system_slots_groups));
        strings_addr += sizeof(struct smbios34_system_slots_information_trail);
    }
    if (version >= 35) {
        strings_addr = (void*)((uint64_t)header + sizeof(struct smbios32_system_slots_information));
        strings_addr += (groupings * sizeof(struct smbios_system_slots_groups));
        strings_addr += sizeof(struct smbios34_system_slots_information_trail);
        strings_addr += sizeof(struct smbios35_system_slots_information_trail);
    }

    if (version >= 20) {
        struct smbios20_system_slots_information * slot = (struct smbios20_system_slots_information *)header;

        char * slot_designation = get_smbios_string(strings_addr, slot->slot_designation);

        insert_smbios_data(&data->slot_designation, slot_designation, strlen(slot_designation));
        insert_smbios_data(&data->slot_type, &slot->slot_type, 1);
        insert_smbios_data(&data->slot_data_bus_width, &slot->slot_data_bus_width, 1);
        insert_smbios_data(&data->current_usage, &slot->current_usage, 1);
        insert_smbios_data(&data->slot_length, &slot->slot_length, 1);
        insert_smbios_data(&data->slot_id, &slot->slot_id, 2);
        insert_smbios_bitfield(&data->slot_characteristics1_unknown, &slot->slot_characteristics1, 0, 1);
        insert_smbios_bitfield(&data->slot_characteristics1_provides_5V, &slot->slot_characteristics1, 1, 1);
        insert_smbios_bitfield(&data->slot_characteristics1_provides_3_3V, &slot->slot_characteristics1, 2, 1);
        insert_smbios_bitfield(&data->slot_characteristics1_opening_is_shared, &slot->slot_characteristics1, 3, 1);
        insert_smbios_bitfield(&data->slot_characteristics1_supports_card_16, &slot->slot_characteristics1, 4, 1);
        insert_smbios_bitfield(&data->slot_characteristics1_supports_cardbus, &slot->slot_characteristics1, 5, 1);
        insert_smbios_bitfield(&data->slot_characteristics1_supports_zv, &slot->slot_characteristics1, 6, 1);
        insert_smbios_bitfield(&data->slot_characteristics1_supports_modem_ring_resume, &slot->slot_characteristics1, 7, 1);    
    }

    if (version >= 21) {
        struct smbios21_system_slots_information * slot = (struct smbios21_system_slots_information *)header;

        insert_smbios_bitfield(&data->slot_characteristics2_supports_pme_signal, &slot->slot_characteristics2, 0, 1);
        insert_smbios_bitfield(&data->slot_characteristics2_supports_hot_plug_devices, &slot->slot_characteristics2, 1, 1);
        insert_smbios_bitfield(&data->slot_characteristics2_supports_smbus_signal, &slot->slot_characteristics2, 2, 1);
        insert_smbios_bitfield(&data->slot_characteristics2_supports_pcie_bifurcation, &slot->slot_characteristics2, 3, 1);
        insert_smbios_bitfield(&data->slot_characteristics2_supports_surprise_removal, &slot->slot_characteristics2, 4, 1);
        insert_smbios_bitfield(&data->slot_characteristics2_supports_flexbus_cxl_1_0, &slot->slot_characteristics2, 5, 1);
        insert_smbios_bitfield(&data->slot_characteristics2_supports_flexbus_cxl_2_0, &slot->slot_characteristics2, 6, 1);
    
    }

    if (version >= 26) {
        struct smbios26_system_slots_information * slot = (struct smbios26_system_slots_information *)header;

        insert_smbios_data(&data->segment_group_number, &slot->segment_group_number, 2);
        insert_smbios_data(&data->bus_number, &slot->bus_number, 2);
        insert_smbios_bitfield(&data->function_number, &slot->device_function_number, 0, 3);
        insert_smbios_bitfield(&data->device_number, &slot->device_function_number, 3, 5);
    }


    if (version >= 32) {
        struct smbios32_system_slots_information * slot = (struct smbios32_system_slots_information *)header;

        insert_smbios_data(&data->data_bus_width, &slot->data_bus_width, 1);
        insert_smbios_data(&data->peer_grouping_n, &slot->peer_grouping_n, 1);

        struct smbios_system_slots_groups * slot_groups = (void*)((uint64_t)header + sizeof(struct smbios32_system_slots_information));

        for (uint8_t i = 0; i < groupings; i++) {
            insert_smbios_data(&data->peer_group[i], &slot_groups[i], 1);
        }
    }

    void * endaddr = (void*)((uint64_t)header + sizeof(struct smbios32_system_slots_information)) + (groupings * sizeof(struct smbios_system_slots_groups));

    if (version >= 34) {
        struct smbios34_system_slots_information_trail * trail = (struct smbios34_system_slots_information_trail*) endaddr;

        insert_smbios_data(&data->slot_information, &trail->slot_information, 1);
        insert_smbios_data(&data->slot_physical_width, &trail->slot_physical_width, 1);
        insert_smbios_data(&data->slot_pitch, &trail->slot_pitch, 2);
    }

    endaddr += sizeof(struct smbios34_system_slots_information_trail);

    if (version >= 35) {
        struct smbios35_system_slots_information_trail * trail = (struct smbios35_system_slots_information_trail*) endaddr;

        insert_smbios_data(&data->slot_height, &trail->slot_height, 1);
    }
}


void process_smbios_table(uint8_t major, uint8_t minor, struct smbios_header * header, struct smbios_data* data) {

    if (header->handle == 0xffff) return;

    uint16_t version = (major*10) + minor;

    switch (header->type) {
        case SMBIOS_BIOS:
            process_bios_information(version, (void*)header, &(data->bios));
            break;
        case SMBIOS_SYSTEM:
            process_system_information(version, (void*)header, &(data->system));
            break;
        case SMBIOS_BASEBOARD:
            process_baseboard_information(version, (void*)header, &(data->baseboard[data->baseboard_devices++]));
            break;
        case SMBIOS_PROCESSOR:
            process_cpu_information(version, (void*)header, &(data->cpu));
            break;
        case SMBIOS_SLOTS:
            process_slot_information(version, (void*)header, &(data->slot[data->slot_devices++]));
            break;
        case SMBIOS_OEM_STRINGS:
            process_oem_strings(version, (void*)header, &(data->oem));
            break;
        case SMBIOS_PHYSICAL_MEMORY_ARRAY:
            process_memory_array_information(version, (void*)header, &(data->memory_array[data->memory_array_devices++]));
            break;
        case SMBIOS_MEMORY_DEVICE:
            process_memory_device_information(version, (void*)header, &(data->memory_device[data->memory_device_devices++]));
        default:
            break;
    }
}

uint8_t smbios_checksum(uint8_t* entry, uint64_t len)
{
    uint8_t sum = 0;
 
    for (uint32_t i = 0; i < len; i++)
    {
        sum += entry[i];
    }
 
    return (sum == 0);
}


void init_smbios32(struct smbios2_entry_point * entry, struct smbios_data* data) {
    
    struct smbios_header* header = (struct smbios_header *)(uint64_t)entry->table_address;
    for (uint16_t i = 0; i < entry->number_of_structures; i++) {
        process_smbios_table(entry->major_version, entry->minor_version, header, data);
        header = (struct smbios_header *)((uint64_t)header+header->length);
        header = (struct smbios_header *)((uint64_t)header+(uint64_t)get_smbios_string_length((void*)header));
    }
}

void init_smbios64(struct smbios3_entry_point * entry, struct smbios_data* data) {
    uint8_t *table_ptr = (uint8_t *)((uintptr_t)entry->table_address >> 16);

    uint8_t *end_of_table = table_ptr + entry->max_structure_size;
    
    while (table_ptr < end_of_table) {
        struct smbios_header *header = (struct smbios_header *)table_ptr;
        
        // Call the process_smbios_table function for each structure
        process_smbios_table(entry->major_version, entry->minor_version, header, data);
        
        // Move to the next structure
        table_ptr += header->length;
        table_ptr += get_smbios_string_length(table_ptr);
    }
}

void init_smbios(struct smbios_data* data) {
    struct smbios2_entry_point * smbios32 = (struct smbios2_entry_point*)get_smbios32_address();
    struct smbios3_entry_point * smbios64 = (struct smbios3_entry_point*)get_smbios64_address();

    if (smbios64 && !memcmp(smbios64->entry_point_string, "_SM3_", 5) && smbios_checksum((uint8_t*)smbios64, smbios64->length)) {
        init_smbios64(smbios64, data);
        return;
    }

    if (smbios32 && !memcmp(smbios32->entry_point_string, "_SM_", 4) && smbios_checksum((uint8_t*)smbios32, smbios32->length)) {
        init_smbios32(smbios32, data);
    }
}