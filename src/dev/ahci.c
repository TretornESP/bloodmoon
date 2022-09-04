#include "ahci.h"
#include "../util/printf.h"
#include "../util/string.h"
#include "../memory/paging.h"
#include "../memory/memory.h"
#include "../util/dbgprinter.h"

struct hba_memory* abar = 0;
struct ahci_port ahci_ports[32];
uint8_t port_count = 0;

void init_ahci(struct pci_device_header* pci_header) {
    if ((uint64_t)abar != 0)
        panic("AHCI already initialized\n");

    abar = (struct hba_memory*)(uint64_t)(((struct pci_device_header_0*)pci_header)->bar5);

    map_memory(abar, abar);
    PAGE_DISABLE_CACHE(abar);

    probe_ports(abar);

    for (int i = 0; i < port_count; i++) {
        struct ahci_port* port = &ahci_ports[i];
        configure_port(port);
        port->buffer = (uint8_t*)request_page_identity();
        memset(port->buffer, 0, 4096);
    }

}

uint8_t get_port_count() {
    return port_count;
}

int8_t find_cmd_slot(struct ahci_port* port) {
    uint32_t slots = (port->hba_port->sata_active | port->hba_port->command_issue);
    int cmd_slots = (abar->host_capabilities & 0x0f00) >> 8;
    for (int i = 0; i < cmd_slots; i++) {
        if ((slots & 1) == 0) {
            return i;
        }
        slots >>= 1;
    }

    printf("Cannot find free command list entry\n");
    return -1;
}

uint8_t read_port(uint8_t port_no, uint64_t sector, uint32_t sector_count) {
    struct ahci_port* port = &ahci_ports[port_no];

    uint32_t sector_low = (uint32_t)sector;
    uint32_t sector_high = (uint32_t)(sector >> 32);

    port->hba_port->interrupt_status = (uint32_t)-1;
    int spin = 0;
    int slot = (int)find_cmd_slot(port);
    if (slot == -1) {
        printf("No free command slots\n");
        return 0;
    }

    struct hba_command_header* command_header = (struct hba_command_header*)(uint64_t)(port->hba_port->command_list_base);
    command_header += slot;
    command_header->command_fis_length = sizeof(struct hba_command_fis) / sizeof(uint32_t);
    command_header->write = 0;
    command_header->prdt_length = (uint16_t)((sector_count - 1) >> 4) + 1;

    struct hba_command_table* command_table = (struct hba_command_table*)(uint64_t)(command_header->command_table_base_address);
    memset(command_table, 0, sizeof(struct hba_command_table) + (command_header->prdt_length - 1) * sizeof(struct hba_prdt_entry));
    void* buffer = port->buffer;
    int i;
    for (i = 0; i < command_header->prdt_length - 1; i++) {
        command_table->prdt_entry[i].data_base_address = (uint32_t)(uint64_t)buffer;
        command_table->prdt_entry[i].data_base_address_upper = (uint32_t)((uint64_t)buffer >> 32);
        command_table->prdt_entry[i].byte_count = 8 * 1024 - 1;
        command_table->prdt_entry[i].interrupt_on_completion = 1;
        buffer = (void*)((uint64_t*)buffer+0x1000);
        sector_count -= 16;
    }

    command_table->prdt_entry[i].data_base_address = (uint32_t)(uint64_t)buffer;
    command_table->prdt_entry[i].data_base_address_upper = (uint32_t)((uint64_t)buffer >> 32);
    command_table->prdt_entry[i].byte_count = (sector_count << 9) - 1;
    command_table->prdt_entry[i].interrupt_on_completion = 1;

    struct hba_command_fis* command_fis = (struct hba_command_fis*)command_table->command_fis;

    command_fis->fis_type = FIS_TYPE_REG_H2D;
    command_fis->command_control = 1;
    command_fis->command = ATA_CMD_READ_DMA_EX;

    command_fis->lba0 = (uint8_t)sector_low;
    command_fis->lba1 = (uint8_t)(sector_low >> 8);
    command_fis->lba2 = (uint8_t)(sector_low >> 16);

    command_fis->device_register = 1 << 6;

    command_fis->lba3 = (uint8_t)(sector_low >> 24);
    command_fis->lba4 = (uint8_t)(sector_high);
    command_fis->lba5 = (uint8_t)(sector_high >> 8);

    command_fis->count_low = sector_count & 0xFF;
    command_fis->count_high = (sector_count >> 8);

    while (port->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ) && spin < 1000000) {
        spin++;
    };
    if (spin == 1000000) {
        printf("Port is hung\n");
        return 0;
    }

    port->hba_port->command_issue = (1 << slot);

    while(1) {
        if ((port->hba_port->command_issue & (1<<slot)) == 0) break;
        if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
            return 0;
        }
    }

    if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
        return 0;
    }

    return 1;
}

uint8_t* get_buffer(uint8_t port_no) {
    return ahci_ports[port_no].buffer;
}

void port_stop_command(struct ahci_port* port) {
    port->hba_port->command_status &= ~HBA_PxCMD_ST;
    port->hba_port->command_status &= ~HBA_PxCMD_FRE;

    while (1) {
        if (port->hba_port->command_status & HBA_PxCMD_FR) continue;
        if (port->hba_port->command_status & HBA_PxCMD_CR) continue;
        break;
    }
}

void port_start_command(struct ahci_port* port) {
    while (port->hba_port->command_status & HBA_PxCMD_CR);
    port->hba_port->command_status |= HBA_PxCMD_FRE;
    port->hba_port->command_status |= HBA_PxCMD_ST;
}

void configure_port(struct ahci_port* port) {
    port_stop_command(port);

    void * new_base = request_page_identity();
    port->hba_port->command_list_base = (uint32_t)(uint64_t)new_base;
    port->hba_port->command_list_base_upper = (uint32_t)((uint64_t)new_base >> 32);
    memset(new_base, 0, 1024);

    void * new_fis_base = request_page_identity();
    port->hba_port->fis_base_address = (uint32_t)(uint64_t)new_fis_base;
    port->hba_port->fis_base_address_upper = (uint32_t)((uint64_t)new_fis_base >> 32);
    memset(new_fis_base, 0, 256);

    struct hba_command_header* command_header = (struct hba_command_header*)((uint64_t)port->hba_port->command_list_base + ((uint64_t) port->hba_port->command_list_base_upper << 32));

    for (int i = 0; i < 32; i++) {
        command_header[i].prdt_length = 8;

        void * cmd_table_address = request_page_identity();
        uint64_t address = (uint64_t)cmd_table_address + (i << 8);
        command_header[i].command_table_base_address = (uint32_t)(uint64_t)address;
        command_header[i].command_table_base_address_upper = (uint32_t)((uint64_t)address >> 32);
        memset(cmd_table_address, 0, 256);
    }

    port_start_command(port);
}

enum port_type check_port_type(struct hba_port* port) {
    uint32_t stata_status  = port->sata_status;
    uint8_t interface_power_management = (stata_status >> 8) & 0x7;
    uint8_t device_detection = stata_status & 0x7;

    if (device_detection != 0x03) return PORT_TYPE_NONE;
    if (interface_power_management != 0x01) return PORT_TYPE_NONE;

    switch(port->signature) {
        case SATA_SIG_ATAPI:
            return PORT_TYPE_SATAPI;
        case SATA_SIG_SEMB:
            return PORT_TYPE_SEMB;
        case SATA_SIG_PM:
            return PORT_TYPE_PM;
        case SATA_SIG_ATA:
            return PORT_TYPE_SATA;
        default:
            return PORT_TYPE_NONE;
    }
}

void probe_ports(struct hba_memory* abar) {
    uint32_t ports_implemented = abar->ports_implemented;

    for (int i = 0; i < 32; i++) { 
        if (ports_implemented & (1 << i)) {
            enum port_type port_type = check_port_type(&abar->ports[i]);
            if (port_type == PORT_TYPE_SATA) { //TODO: Add support for SATAPI
                ahci_ports[port_count].port_type = port_type;
                ahci_ports[port_count].hba_port = &abar->ports[i];
                ahci_ports[port_count].port_number = port_count;
                port_count++;
            }
        }
    }
}