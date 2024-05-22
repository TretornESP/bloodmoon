#ifndef _SMBIOS_H
#define _SMBIOS_H

#define SMBIOS_BIOS 0
#define SMBIOS_SYSTEM 1
#define SMBIOS_BASEBOARD 2
#define SMBIOS_SYSTEM_ENCLOSURE 3
#define SMBIOS_PROCESSOR 4
#define SMBIOS_CACHE 7
#define SMBIOS_SLOTS 9
#define SMBIOS_OEM_STRINGS 11
#define SMBIOS_PHYSICAL_MEMORY_ARRAY 16
#define SMBIOS_MEMORY_DEVICE 17
#define SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS 19
#define SMBIOS_SYSTEM_BOOT_INFORMATION 32

#include <stdint.h>

struct smbios2_entry_point {
 	char entry_point_string[4];    //This is _SM_
 	uint8_t checksum;              //This value summed with all the values of the table, should be 0 (overflow)
 	uint8_t length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
 	uint8_t major_version;          //Major Version of SMBIOS
 	uint8_t minor_version;          //Minor Version of SMBIOS
 	uint16_t max_structure_size;     //Maximum size of a SMBIOS Structure (we will se later)
 	uint8_t entry_point_revision;    //...
 	char formatted_area[5];       //...
 	char entry_point_string2[5];   //This is _DMI_
 	uint8_t checksum2;             //Checksum for values from EntryPointString2 to the end of table
 	uint16_t table_length;          //Length of the Table containing all the structures
 	uint32_t table_address;	     //Address of the Table
 	uint16_t number_of_structures;   //Number of structures in the table
 	uint8_t bcd_revision;           //Unused
} __attribute__((packed));

 
struct smbios3_entry_point {
 	char entry_point_string[5];    //This is _SM3_
 	uint8_t checksum;              //This value summed with all the values of the table, should be 0 (overflow)
 	uint8_t length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
 	uint8_t major_version;          //Major Version of SMBIOS
 	uint8_t minor_version;          //Minor Version of SMBIOS+
    uint8_t doc_rev;               //Document Revision
 	uint8_t entry_point_revision;    //...
 	uint8_t reserved;             //Checksum for values from EntryPointString2 to the end of table
 	uint16_t max_structure_size;     //Maximum size of a SMBIOS Structure (we will se later)
    uint64_t table_address;         //Address of the Table
} __attribute__((packed));

struct smbios_header {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
} __attribute__((packed));

struct smbios2_bios_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t vendor;
    uint8_t version;
    uint16_t start_address_segment;
    uint8_t release_date;
    uint8_t rom_size;
    uint64_t characteristics;
} __attribute__((packed));

struct smbios24_bios_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle; //4 bytes

    uint8_t vendor;
    uint8_t version;
    uint16_t start_address_segment; //8 bytes
    uint8_t release_date;
    uint8_t rom_size; //10bytes
    uint64_t characteristics; //14bytes

    uint8_t characteristics_extension_bytes[2]; //16bytes
    uint8_t system_bios_major_release;
    uint8_t system_bios_minor_release;
    uint8_t embedded_controller_major_release;
    uint8_t embedded_controller_minor_release; //20bytes
    
} __attribute__((packed));

struct smbios31_bios_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t vendor;
    uint8_t version;
    uint16_t start_address_segment;
    uint8_t release_date;
    uint8_t rom_size;
    uint64_t characteristics;

    uint8_t characteristics_extension_bytes[2];
    uint8_t system_bios_major_release;
    uint8_t system_bios_minor_release;
    uint8_t embedded_controller_major_release;
    uint8_t embedded_controller_minor_release;

    uint16_t extended_bios_rom_size;
} __attribute__((packed));

struct smbios20_system_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t manufacturer;
    uint8_t product_name;
    uint8_t version;
    uint8_t serial_number;
} __attribute__((packed));

struct smbios21_system_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t manufacturer;
    uint8_t product_name;
    uint8_t version;
    uint8_t serial_number;
    uint8_t uuid[16];
    uint8_t wake_up_type;
} __attribute__((packed));

struct smbios24_system_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t manufacturer;
    uint8_t product_name;
    uint8_t version;
    uint8_t serial_number;
    uint8_t uuid[16];
    uint8_t wake_up_type;
    uint8_t sku_number;
    uint8_t family;
} __attribute__((packed));

struct smbios_baseboard_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t manufacturer;
    uint8_t product_name;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t feature_flags;
    uint8_t location_in_chassis;
    uint16_t chassis_handle;
    uint8_t board_type;
    uint8_t number_of_contained_handles;
} __attribute__((packed));
//After this comes the contained elements themselves

struct smbios20_enclosure_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t manufacturer;
    uint8_t type2;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
} __attribute__((packed));

struct smbios21_enclosure_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t manufacturer;
    uint8_t type2;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t boot_up_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
} __attribute__((packed));

struct smbios22_enclosure_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t manufacturer;
    uint8_t type2;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t boot_up_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
    uint32_t oem_defined;
    uint8_t height;
    uint8_t number_of_power_cords;
    uint8_t contained_element_count;
    uint8_t contained_element_record_length;
} __attribute__((packed));
//After this comes the contained elements and SKU number (only 2.7+)

struct smbios_enclosure_contained_element {
    uint8_t type;
    uint8_t minimum;
    uint8_t maximum;
} __attribute__((packed));

struct smbios20_processor_information {
    uint8_t type;                      //Common: 4 bytes
    uint8_t length;
    uint16_t handle;                   

    uint8_t socket_designation;        //2.0+   18 bytes
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;          
} __attribute__((packed));

struct smbios21_processor_information {
    uint8_t type;                      //Common: 4 bytes
    uint8_t length;
    uint16_t handle;                   

    uint8_t socket_designation;        //2.0+   18 bytes
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;          

    uint16_t l1_cache_handle;           //2.1+   6 bytes
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;           
} __attribute__((packed));

struct smbios23_processor_information {
    uint8_t type;                      //Common: 4 bytes
    uint8_t length;
    uint16_t handle;                   

    uint8_t socket_designation;        //2.0+   18 bytes
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;          

    uint16_t l1_cache_handle;           //2.1+   6 bytes
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;           

    uint8_t serial_number;              //2.3+   3 bytes
    uint8_t asset_tag;
    uint8_t part_number;                
} __attribute__((packed));

struct smbios25_processor_information {
    uint8_t type;                      //Common: 4 bytes
    uint8_t length;
    uint16_t handle;                   

    uint8_t socket_designation;        //2.0+   18 bytes
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;          

    uint16_t l1_cache_handle;           //2.1+   6 bytes
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;           

    uint8_t serial_number;              //2.3+   3 bytes
    uint8_t asset_tag;
    uint8_t part_number;                

    uint8_t core_count;                 //2.5+   5 bytes
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t characteristics;           
} __attribute__((packed));

struct smbios26_processor_information {
    uint8_t type;                      //Common: 4 bytes
    uint8_t length;
    uint16_t handle;                   

    uint8_t socket_designation;        //2.0+   18 bytes
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;          

    uint16_t l1_cache_handle;           //2.1+   6 bytes
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;           

    uint8_t serial_number;              //2.3+   3 bytes
    uint8_t asset_tag;
    uint8_t part_number;                

    uint8_t core_count;                 //2.5+   5 bytes
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t characteristics;           

    uint16_t processor_family2;          //2.6+    2 byte
} __attribute__((packed));

struct smbios30_processor_information {
    uint8_t type;                      //Common: 4 bytes
    uint8_t length;
    uint16_t handle;                   

    uint8_t socket_designation;        //2.0+   18 bytes
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;          

    uint16_t l1_cache_handle;           //2.1+   6 bytes
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;           

    uint8_t serial_number;              //2.3+   3 bytes
    uint8_t asset_tag;
    uint8_t part_number;                

    uint8_t core_count;                 //2.5+   5 bytes
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t characteristics;           

    uint16_t processor_family2;          //2.6+    2 byte

    uint16_t core_count2;                //3.0+    6 bytes
    uint16_t core_enabled2;
    uint16_t thread_count2;             

} __attribute__((packed));

struct smbios36_processor_information {
    uint8_t type;                      //Common: 4 bytes
    uint8_t length;
    uint16_t handle;                   

    uint8_t socket_designation;        //2.0+   18 bytes
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;          

    uint16_t l1_cache_handle;           //2.1+   6 bytes
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;           

    uint8_t serial_number;              //2.3+   3 bytes
    uint8_t asset_tag;
    uint8_t part_number;                

    uint8_t core_count;                 //2.5+   5 bytes
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t characteristics;           

    uint16_t processor_family2;          //2.6+    2 byte

    uint16_t core_count2;                //3.0+    6 bytes
    uint16_t core_enabled2;
    uint16_t thread_count2;             

    uint16_t thread_enabled;             //3.6+    2 bytes
} __attribute__((packed));

struct smbios21_memory_array_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t location;
    uint8_t use;
    uint8_t error_correction;
    uint32_t maximum_capacity;
    uint16_t error_information_handle;
    uint16_t number_of_memory_devices;
} __attribute__((packed));

struct smbios27_memory_array_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t location;
    uint8_t use;
    uint8_t error_correction;
    uint32_t maximum_capacity;
    uint16_t error_information_handle;
    uint16_t number_of_memory_devices;

    uint64_t extended_maximum_capacity; //2.7+
} __attribute__((packed));

struct smbios21_memory_device_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;
} __attribute__((packed));

struct smbios23_memory_device_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;

    uint16_t speed; //2.3+
    uint8_t manufacturer; 
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
} __attribute__((packed));

struct smbios26_memory_device_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;

    uint16_t speed; //2.3+
    uint8_t manufacturer; 
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;

    uint8_t attributes; //2.6+
} __attribute__((packed));

struct smbios27_memory_device_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;

    uint16_t speed; //2.3+
    uint8_t manufacturer; 
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;

    uint8_t attributes; //2.6+

    uint32_t extended_size; //2.7+
    uint16_t configured_memory_clock_speed;
} __attribute__((packed));

struct smbios28_memory_device_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;

    uint16_t speed; //2.3+
    uint8_t manufacturer; 
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;

    uint8_t attributes; //2.6+

    uint32_t extended_size; //2.7+
    uint16_t configured_memory_clock_speed;

    uint16_t minimum_voltage; //2.8+
    uint16_t maximum_voltage;
    uint16_t configured_voltage;
} __attribute__((packed));

struct smbios32_memory_device_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;

    uint16_t speed; //2.3+
    uint8_t manufacturer; 
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;

    uint8_t attributes; //2.6+

    uint32_t extended_size; //2.7+
    uint16_t configured_memory_clock_speed;

    uint16_t minimum_voltage; //2.8+
    uint16_t maximum_voltage;
    uint16_t configured_voltage;

    uint8_t memory_technology; //3.2+
    uint16_t memory_operating_mode_capability;
    uint8_t firmware_version;
    uint16_t module_manufacturer_id;
    uint16_t module_product_id;
    uint16_t memory_subsystem_controller_manufacturer_id;
    uint16_t memory_subsystem_controller_product_id;
    uint64_t non_volatile_size;
    uint64_t volatile_size;
    uint64_t cache_size;
    uint64_t logical_size;
} __attribute__((packed));

struct smbios20_system_slots_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t slot_designation;
    uint8_t slot_type;
    uint8_t slot_data_bus_width;
    uint8_t current_usage;
    uint8_t slot_length;
    uint16_t slot_id;
    uint8_t slot_characteristics1;
} __attribute__((packed));

struct smbios21_system_slots_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t slot_designation;
    uint8_t slot_type;
    uint8_t slot_data_bus_width;
    uint8_t current_usage;
    uint8_t slot_length;
    uint16_t slot_id;
    uint8_t slot_characteristics1;

    uint8_t slot_characteristics2; //2.1+
} __attribute__((packed));

struct smbios26_system_slots_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t slot_designation;
    uint8_t slot_type;
    uint8_t slot_data_bus_width;
    uint8_t current_usage;
    uint8_t slot_length;
    uint16_t slot_id;
    uint8_t slot_characteristics1;

    uint8_t slot_characteristics2; //2.1+

    uint16_t segment_group_number; //2.6+
    uint8_t bus_number;
    uint8_t device_function_number;
} __attribute__((packed));

struct smbios32_system_slots_information {
    uint8_t type;
    uint8_t length;
    uint16_t handle;

    uint8_t slot_designation;
    uint8_t slot_type;
    uint8_t slot_data_bus_width;
    uint8_t current_usage;
    uint8_t slot_length;
    uint16_t slot_id;
    uint8_t slot_characteristics1;

    uint8_t slot_characteristics2; //2.1+

    uint16_t segment_group_number; //2.6+
    uint8_t bus_number;
    uint8_t device_function_number;

    uint8_t data_bus_width; //3.2+
    uint8_t peer_grouping_n;

} __attribute__((packed));

struct smbios_system_slots_groups {
    uint8_t peer_groups;
} __attribute__((packed));

struct smbios34_system_slots_information_trail {
    uint8_t slot_information;
    uint8_t slot_physical_width;
    uint16_t slot_pitch;
} __attribute__((packed));

struct smbios35_system_slots_information_trail {
    uint8_t slot_height;
} __attribute__((packed));

struct smbios_oem_strings {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t count;
} __attribute__((packed));

void init_smbios();

#endif