#include "pci.h"
#include "../../memory/paging.h"
#include "../../util/printf.h"
#include "../../util/dbgprinter.h"
#include "../../drivers/disk/ahci/ahci.h"

struct pci_device_header* global_device_header = {0};

const char* pci_device_classes[] = {
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge Device",
    "Simple Communication Controller",
    "Base System Peripheral",
    "Input Device Controller",
    "Docking Station", 
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Processing Accelerator",
    "Non Essential Instrumentation"
};

const char* get_vendor_name(uint16_t vendor_id) {
    switch (vendor_id) {
        case 0x8086:
            return "Intel";
        case 0x10de:
            return "nVidia";
        case 0x1022:
            return "AMD";
        case 0x1234:
            return "Bochs";
    }
    return itoa(vendor_id, 16);
}

const char* get_device_class(uint8_t class) {
    return pci_device_classes[class];
}


const char* get_device_name(uint16_t vendor_id, uint16_t device_id) {
    switch (vendor_id) {
        case 0x8086: {
            switch (device_id) {
                case 0x29c0:
                    return "Express DRAM Controller";
                case 0x2918:
                    return "LPC Interface Controller";
                case 0x2922:
                    return "SATA Controller [AHCI]";
                case 0x2930:
                    return "SMBus Controller";
                case 0x10d3:
                    return "Gigabit Network Connection";
            }
            break;
        }
        case 0x1234: {
            switch (device_id) {
                case 0x1111:
                    return "Virtual VGA Controller";
            }
            break;
        }
    }
    
    return itoa(device_id, 16);
}

const char * mass_storage_controller_subclass_name(uint8_t subclass_code) {
    switch (subclass_code) {
        case 0x00:
            return "SCSI Bus Controller";
        case 0x01:
            return "IDE Controller";
        case 0x02:
            return "Floppy Disk Controller";
        case 0x03:
            return "IPI Bus Controller";
        case 0x04:
            return "RAID Controller";
        case 0x05:
            return "ATA Controller";
        case 0x06:
            return "Serial ATA";
        case 0x07:
            return "Serial Attached SCSI";
        case 0x08:
            return "Non-Volatile Memory Controller";
        case 0x80:
            return "Other";
    }

    return itoa(subclass_code, 16);
}

const char* serial_bus_controller_subclass_name(uint8_t subclass_code){
    switch (subclass_code){
        case 0x00:
            return "FireWire (IEEE 1394) Controller";
        case 0x01:
            return "ACCESS Bus";
        case 0x02:
            return "SSA";
        case 0x03:
            return "USB Controller";
        case 0x04:
            return "Fibre Channel";
        case 0x05:
            return "SMBus";
        case 0x06:
            return "Infiniband";
        case 0x07:
            return "IPMI Interface";
        case 0x08:
            return "SERCOS Interface (IEC 61491)";
        case 0x09:
            return "CANbus";
        case 0x80:
            return "SerialBusController - Other";
    }
    return itoa(subclass_code, 16);
}

const char* bridge_device_subclass_name(uint8_t subclass_code){
    switch (subclass_code){
        case 0x00:
            return "Host Bridge";
        case 0x01:
            return "ISA Bridge";
        case 0x02:
            return "EISA Bridge";
        case 0x03:
            return "MCA Bridge";
        case 0x04:
            return "PCI-to-PCI Bridge";
        case 0x05:
            return "PCMCIA Bridge";
        case 0x06:
            return "NuBus Bridge";
        case 0x07:
            return "CardBus Bridge";
        case 0x08:
            return "RACEway Bridge";
        case 0x09:
            return "PCI-to-PCI Bridge";
        case 0x0a:
            return "InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Other";
    }

    return itoa(subclass_code, 16);
}

const char * get_subclass_name(uint8_t class_code, uint8_t subclass_code) {
    switch (class_code) {
        case 0x01:
            return mass_storage_controller_subclass_name(subclass_code);
        case 0x03: {
            switch (subclass_code) {
                case 0x00:
                    return "VGA Compatible Controller";
            }
            break;
        }
        case 0x06:
            return bridge_device_subclass_name(subclass_code);
        case 0x0c:
            return serial_bus_controller_subclass_name(subclass_code);
    }
    return itoa(subclass_code, 16);
}

const char* get_prog_interface(uint8_t class_code, uint8_t subclass_code, uint8_t prog_interface){
    switch (class_code){
        case 0x01: {
            switch (subclass_code){
                case 0x06: {
                    switch (prog_interface){
                        case 0x00:
                            return "Vendor Specific Interface";
                        case 0x01:
                            return "AHCI 1.0";
                        case 0x02:
                            return "Serial Storage Bus";
                    }
                    break;
                }
            }
            break;
        }
        case 0x03: {
            switch (subclass_code) {
                case 0x00: {
                    switch (prog_interface){
                        case 0x00:
                            return "VGA Controller";
                        case 0x01:
                            return "8514-Compatible Controller";
                    }
                    break;
                }
            }
            break;
        }
        case 0x0C: {
            switch (subclass_code){
                case 0x03: {
                    switch (prog_interface){
                        case 0x00:
                            return "UHCI Controller";
                        case 0x10:
                            return "OHCI Controller";
                        case 0x20:
                            return "EHCI (USB2) Controller";
                        case 0x30:
                            return "XHCI (USB3) Controller";
                        case 0x80:
                            return "Unspecified";
                        case 0xFE:
                            return "USB Device (Not a Host Controller)";
                    }
                    break;
                }
            }    
            break;
        }
    }
    return itoa(prog_interface, 16);
}

void register_pci_device(struct pci_device_header* pci, char* (*cb)(void*, uint8_t, uint64_t)) {
    printf("Device detected: %x, %x, %x\n", pci->class_code, pci->subclass, pci->prog_if);
    switch (pci->class_code){
        case 0x01: {
            switch (pci->subclass){
                case 0x06: {
                    switch (pci->prog_if){
                        case 0x01:
                            init_ahci(((struct pci_device_header_0*)pci)->bar5);//TODO: Careful. This is a dynamic driver startup.
                            for (int i = 0; i < get_port_count(); i++) {
                                switch (get_port_type(i)) {
                                    case PORT_TYPE_SATA:
                                        cb((void*)pci, 0x8, i);
                                        break;
                                    case PORT_TYPE_SATAPI:
                                        cb((void*)pci, 0x9, i);
                                        break;
                                    case PORT_TYPE_SEMB:
                                        cb((void*)pci, 0xa, i);
                                        break;
                                    case PORT_TYPE_PM:
                                        cb((void*)pci, 0xb, i);
                                        break;
                                    case PORT_TYPE_NONE:
                                        cb((void*)pci, 0xc, i);
                                        break;
                                }
                            }
                            break;
                        default:
                            printf("Unknown AHCI interface: %x\n", pci->prog_if);
                    }
                    break;
                }
            }
            break;
        }
    }
}

void enumerate_function(uint64_t device_address, uint64_t function, char* (*cb)(void*, uint8_t, uint64_t)) {

    uint64_t offset = function << 12;

    uint64_t function_address = device_address + offset;
    map_memory((void*)function_address, (void*)function_address);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)function_address;
    global_device_header = pci_device_header;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    printf("%s %s: %s %s / %s\n",
        get_device_class(pci_device_header->class_code),
        get_subclass_name(pci_device_header->class_code, pci_device_header->subclass),
        get_vendor_name(pci_device_header->vendor_id),
        get_device_name(pci_device_header->vendor_id, pci_device_header->device_id),
        get_prog_interface(pci_device_header->class_code, pci_device_header->subclass, pci_device_header->prog_if)
    );

    register_pci_device(pci_device_header, cb);
}

void enumerate_device(uint64_t bus_address, uint64_t device, char* (*cb)(void*, uint8_t, uint64_t)) {
    uint64_t offset = device << 15;

    uint64_t device_address = bus_address + offset;
    map_memory((void*)device_address, (void*)device_address);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)device_address;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    for (uint64_t function = 0; function < 8; function++) {
        enumerate_function(device_address, function, cb);
    }
}

void enumerate_bus(uint64_t base_address, uint64_t bus, char* (*cb)(void*, uint8_t, uint64_t)) {
    uint64_t offset = bus << 20;

    uint64_t bus_address = base_address + offset;
    map_memory((void*)bus_address, (void*)bus_address);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)bus_address;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    for (uint64_t device = 0; device < 32; device++) {
        enumerate_device(bus_address, device, cb);
    }
}

void register_pci(struct mcfg_header *mcfg, char* (*cb)(void*, uint8_t, uint64_t)) {
    uint64_t entries = ((mcfg->header.length) - sizeof(struct mcfg_header)) / sizeof(struct device_config);

    for (uint64_t i = 0; i < entries; i++) {
        struct device_config * new_device_config = (struct device_config*)((uint64_t)mcfg + sizeof(struct mcfg_header) + (sizeof(struct device_config) * i));	
        for (uint64_t bus = new_device_config->start_bus; bus < new_device_config->end_bus; bus++) {
            enumerate_bus(new_device_config->base_address, bus, cb);
        }
    }
}

