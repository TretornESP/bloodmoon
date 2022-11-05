#ifndef _SMBIOS_H
#define _SMBIOS_H

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

void init_smbios();

#endif