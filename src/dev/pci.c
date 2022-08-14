#include "pci.h"
#include "../memory/paging.h"
#include "../util/printf.h"

void enumerate_function(uint64_t device_address, uint64_t function) {

    uint64_t offset = function << 12;

    uint64_t function_address = device_address + offset;
    map_memory((void*)function_address, (void*)function_address);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)function_address;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    printf("Dev: %x Vend: %x\n", pci_device_header->device_id, pci_device_header->vendor_id);
}

void enumerate_device(uint64_t bus_address, uint64_t device) {
    uint64_t offset = device << 15;

    uint64_t device_address = bus_address + offset;
    map_memory((void*)device_address, (void*)device_address);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)device_address;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    for (uint64_t function = 0; function < 8; function++) {
        enumerate_function(device_address, function);
    }
}

void enumerate_bus(uint64_t base_address, uint64_t bus) {
    uint64_t offset = bus << 20;

    uint64_t bus_address = base_address + offset;
    map_memory((void*)bus_address, (void*)bus_address);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)bus_address;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    for (uint64_t device = 0; device < 32; device++) {
        enumerate_device(bus_address, device);
    }
}

void enumerate_pci(struct mcfg_header *mcfg) {
    uint64_t entries = ((mcfg->header.length) - sizeof(struct mcfg_header)) / sizeof(struct device_config);

    for (uint64_t i = 0; i < entries; i++) {
        struct device_config * new_device_config = (struct device_config*)((uint64_t)mcfg + sizeof(struct mcfg_header) + (sizeof(struct device_config) * i));	
        for (uint64_t bus = new_device_config->start_bus; bus < new_device_config->end_bus; bus++) {
            enumerate_bus(new_device_config->base_address, bus);
        }
    }
}

