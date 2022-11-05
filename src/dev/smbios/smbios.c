#include "smbios.h"
#include "smbios_interface.h"

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

void process_cpu_information(uint16_t version, void* cpu_information, struct smbios_cpu_data * data) {
    (void)data;
    if (version < 20) {
        printf("Cpu Information table version %d not supported\n", version);
        return;   
    }
    if (version < 21) {
        printf("Cpu Information table version %d not supported\n", version);
        return;
    }
    if (version < 23) {
        printf("Cpu Information table version %d not supported\n", version);
        return;
    }
    if (version < 24) {
        printf("Cpu Information table version %d not supported\n", version);
        return;
    }
    if (version < 25) {
        printf("Cpu Information table version %d not supported\n", version);
        return;        
    }
    if (version < 26) {
        printf("Cpu Information table version %d not supported\n", version);
        return;
    }
    if (version < 30) {
        struct smbios26_processor_information * cpu = (struct smbios26_processor_information *)cpu_information;
        void * strings_addr = (void*)((uint64_t)cpu + sizeof(struct smbios26_processor_information));
        printf("CPU v%d Information table at [%p]: Socket: %s Manufacturer: %s, Version: %s Serial: %s, Asset: %s, Part: %s\n",
            version,
            cpu,
            get_smbios_string(strings_addr, cpu->socket_designation),
            get_smbios_string(strings_addr, cpu->processor_manufacturer),
            get_smbios_string(strings_addr, cpu->processor_version),
            get_smbios_string(strings_addr, cpu->serial_number),
            get_smbios_string(strings_addr, cpu->asset_tag),
            get_smbios_string(strings_addr, cpu->part_number)
        );

        char * socket_designation_string = get_smbios_string(strings_addr, cpu->socket_designation);
        char * processor_manufacturer_string = get_smbios_string(strings_addr, cpu->processor_manufacturer);
        char * processor_version_string = get_smbios_string(strings_addr, cpu->processor_version);
        char * serial_number_string = get_smbios_string(strings_addr, cpu->serial_number);
        char * asset_tag_string = get_smbios_string(strings_addr, cpu->asset_tag);
        char * part_number_string = get_smbios_string(strings_addr, cpu->part_number);

        insert_smbios_data(&data->socket_designation, socket_designation_string, strlen(socket_designation_string));
        insert_smbios_data(&data->processor_manufacturer, processor_manufacturer_string, strlen(processor_manufacturer_string));
        insert_smbios_data(&data->processor_version, processor_version_string, strlen(processor_version_string));
        insert_smbios_data(&data->serial_number, serial_number_string, strlen(serial_number_string));
        insert_smbios_data(&data->asset_tag, asset_tag_string, strlen(asset_tag_string));
        insert_smbios_data(&data->part_number, part_number_string, strlen(part_number_string));

        insert_smbios_data(&data->processor_type, &cpu->processor_type, 1);
        insert_smbios_data(&data->processor_family, &cpu->processor_family, 1);
        insert_smbios_data(&data->processor_id, &cpu->processor_id, 8);
        
        insert_smbios_bitfield(&data->voltage_legacy, &cpu->voltage, 7, 1);
        insert_smbios_bitfield(&data->voltage_5V, &cpu->voltage, 0, 1);
        insert_smbios_bitfield(&data->voltage_3_3V, &cpu->voltage, 1, 1);
        insert_smbios_bitfield(&data->voltage_2_9V, &cpu->voltage, 2, 1);

        insert_smbios_data(&data->external_clock, &cpu->external_clock, 2);
        insert_smbios_data(&data->max_speed, &cpu->max_speed, 2);
        insert_smbios_data(&data->current_speed, &cpu->current_speed, 2);

        insert_smbios_bitfield(&data->socket_populated, &cpu->status, 6, 1);
        insert_smbios_bitfield(&data->cpu_status, &cpu->status, 0, 2);

        insert_smbios_data(&data->processor_upgrade, &cpu->processor_upgrade, 1);
        insert_smbios_data(&data->core_count, &cpu->core_count, 1);
        insert_smbios_data(&data->core_enabled, &cpu->core_enabled, 1);
        insert_smbios_data(&data->thread_count, &cpu->thread_count, 1);

        insert_smbios_bitfield(&data->supports_64_bits, &cpu->characteristics, 2, 1);
        insert_smbios_bitfield(&data->supports_multicore, &cpu->characteristics, 3, 1);
        insert_smbios_bitfield(&data->supports_hardware_thread, &cpu->characteristics, 4, 1);
        insert_smbios_bitfield(&data->supports_execute_protection, &cpu->characteristics, 5, 1);
        insert_smbios_bitfield(&data->supports_enhanced_virtualization, &cpu->characteristics, 6, 1);
        insert_smbios_bitfield(&data->supports_power_management, &cpu->characteristics, 7, 1);
        insert_smbios_bitfield(&data->supports_128_bits, &cpu->characteristics, 8, 1);
        insert_smbios_bitfield(&data->is_arm_soc, &cpu->characteristics, 9, 1);

        insert_smbios_data(&data->processor_family2, &cpu->processor_family2, 2);
        
        set_smbios_cpu_loaded(1);
        return;
    }
    if (version < 36) {
        printf("Cpu Information table version %d not supported\n", version);
        return;
    } else {
        printf("Cpu Information table version %d not supported\n", version);
    }
}

void process_system_information(uint16_t version, void* system_information, struct smbios_system_data* data) {
    (void)data;
    if (version < 20) {
        printf("System Information table version %d not supported\n", version);
        return;   
    }
    if (version < 21) {
        printf("System Information table version %d not supported\n", version);
        return;
    }
    if (version < 24) {
        printf("System Information table version %d not supported\n", version);
        return;
    } else {
        struct smbios24_system_information * system = (struct smbios24_system_information *)system_information;
        void * strings_addr = (void*)((uint64_t)system + sizeof(struct smbios24_system_information));
        printf("System v%d Information table at [%p]: Manufacturer: %s Product Name: %s, Version: %s Serial: %s, SKU: %s, Family: %s\n",
            version,
            system,
            get_smbios_string(strings_addr, system->manufacturer),
            get_smbios_string(strings_addr, system->product_name),
            get_smbios_string(strings_addr, system->version),
            get_smbios_string(strings_addr, system->serial_number),
            get_smbios_string(strings_addr, system->sku_number),
            get_smbios_string(strings_addr, system->family)
        );

        char * manufacturer_string = get_smbios_string(strings_addr, system->manufacturer);
        char * product_name_string = get_smbios_string(strings_addr, system->product_name);
        char * version_string = get_smbios_string(strings_addr, system->version);
        char * serial_number_string = get_smbios_string(strings_addr, system->serial_number);
        char * sku_number_string = get_smbios_string(strings_addr, system->sku_number);
        char * family_string = get_smbios_string(strings_addr, system->family);

        insert_smbios_data(&data->manufacturer, manufacturer_string, strlen(manufacturer_string));
        insert_smbios_data(&data->product_name, product_name_string, strlen(product_name_string));
        insert_smbios_data(&data->version, version_string, strlen(version_string));
        insert_smbios_data(&data->serial_number, serial_number_string, strlen(serial_number_string));
        insert_smbios_data(&data->sku_number, sku_number_string, strlen(sku_number_string));
        insert_smbios_data(&data->family, family_string, strlen(family_string));

        insert_smbios_data(&data->uuid, system->uuid, 16);
        insert_smbios_data(&data->wake_up_type, &system->wake_up_type, 1);
        
        set_smbios_system_loaded(1);
        return;
    }
}

void process_bios_information(uint16_t version, void* bios_information, struct smbios_bios_data* data) {

    //You can change this to implement more versions pretty easily!
    if (version < 20) {
        printf("BIOS Information table version %d not supported\n", version);
        return;
    }
    if (version < 24) {
        printf("BIOS Information table version %d not supported\n", version);
        return;
    }
    if (version < 30) {
        struct smbios24_bios_information * bios = (struct smbios24_bios_information *)bios_information;
        void * strings_addr = (void*)((uint64_t)bios + sizeof(struct smbios24_bios_information));
        
        char * vendor_string = get_smbios_string(strings_addr, bios->vendor);
        char * version_string = get_smbios_string(strings_addr, bios->version);
        char * release_date_string = get_smbios_string(strings_addr, bios->release_date);

        insert_smbios_data(&data->vendor, vendor_string, strlen(vendor_string));
        insert_smbios_data(&data->version, version_string, strlen(version_string));
        insert_smbios_data(&data->release_date, release_date_string, strlen(release_date_string));

        insert_smbios_data(&data->system_bios_major_release, &bios->system_bios_major_release, 1);
        insert_smbios_data(&data->system_bios_minor_release, &bios->system_bios_minor_release, 1);
        insert_smbios_data(&data->embedded_controller_major_release, &bios->embedded_controller_major_release, 1);
        insert_smbios_data(&data->embedded_controller_minor_release, &bios->embedded_controller_minor_release, 1);

        insert_smbios_bitfield(&data->supports_characteristics, &bios->characteristics, 3, 1);

        insert_smbios_bitfield(&data->supports_isa, &bios->characteristics, 4, 1);
        insert_smbios_bitfield(&data->supports_mca, &bios->characteristics, 5, 1);
        insert_smbios_bitfield(&data->supports_pci, &bios->characteristics, 7, 1);
        insert_smbios_bitfield(&data->supports_pnp, &bios->characteristics, 9, 1);
        insert_smbios_bitfield(&data->supports_apm, &bios->characteristics, 10, 1);
        insert_smbios_bitfield(&data->supports_edd, &bios->characteristics, 19, 1);
        insert_smbios_bitfield(&data->supports_escd, &bios->characteristics, 14, 1);
        insert_smbios_bitfield(&data->supports_eisa, &bios->characteristics, 6, 1);
        insert_smbios_bitfield(&data->supports_vl_vesa, &bios->characteristics, 13, 1);
        insert_smbios_bitfield(&data->supports_boot_from_cd, &bios->characteristics, 15, 1);
        insert_smbios_bitfield(&data->supports_selectable_boot, &bios->characteristics, 16, 1);

        insert_smbios_bitfield(&data->supports_acpi, &(bios->characteristics_extension_bytes[0]), 0, 1);
        insert_smbios_bitfield(&data->supports_usb_legacy, &(bios->characteristics_extension_bytes[0]), 1, 1);
        insert_smbios_bitfield(&data->supports_agp, &(bios->characteristics_extension_bytes[0]), 2, 1);

        insert_smbios_bitfield(&data->supports_bios_boot, &(bios->characteristics_extension_bytes[1]), 0, 1);
        insert_smbios_bitfield(&data->supports_uefi, &(bios->characteristics_extension_bytes[1]), 3, 1);
        insert_smbios_bitfield(&data->is_virtual_machine, &(bios->characteristics_extension_bytes[1]), 4, 1);
        set_smbios_bios_loaded(1);

        return;
    } else {
        printf("BIOS Information table version %d not supported\n", version);
        return;        
    }

}

void process_smbios_table(uint8_t major, uint8_t minor, struct smbios_header * header, struct smbios_data* data) {

    if (header->handle == 0xffff) return;

    uint16_t version = (major*10) + minor;

    switch (header->type) {
        case 0x0:
            process_bios_information(version, (void*)header, &(data->bios));
            break;
        case 0x1:
            process_system_information(version, (void*)header, &(data->system));
            break;
        case 0x4:
            process_cpu_information(version, (void*)header, &(data->cpu));
            break;
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
    (void)entry;
    (void)data;
    panic("SMBIOSv3 Not supported\n");
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