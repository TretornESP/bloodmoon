#include "devices.h"
#include "../drivers/ahci/ahci.h"
#include "../memory/memory.h"
#include "../util/dbgprinter.h"
#include "../util/string.h"
#include "../util/printf.h"

struct device_driver char_device_drivers[256] = {0};
struct device_driver block_device_drivers[256] = {0};
struct device* devices;

void insert_device(uint8_t major, struct pci_device_header* pci, const char * prefix, uint8_t id) {
    char name[32];
    memset(name, 0, 32);

    struct device* device = devices;
    uint8_t minor = 0;
    while (device->next != 0) {
        if (device->major == major)
            minor++;
        device = device->next;
    }


    snprintf(name, 32, "%s%x", prefix, minor+0xa);

    printf("Registering device: %s [MAJ: %d MIN: %d ID: %d PCI: %p]\n", name, major, minor, id, pci);

    device->next = (struct device*) request_page();
    memset(device->next, 0, sizeof(struct device));
    device->valid = 1;
    device->bc = ((major & 0x80) >> 7);
    device->major = major & 0x7f;
    device->minor = minor;
    strncpy(device->name, name, strlen(name));
    device->pci = pci;
    device->internal_id = id;
}

void register_device(struct pci_device_header* pci) {
    printf("Device detected: %x, %x, %x\n", pci->class_code, pci->subclass, pci->prog_if);
    switch (pci->class_code){
        case 0x01: {
            switch (pci->subclass){
                case 0x06: {
                    switch (pci->prog_if){
                        case 0x01:
                            init_ahci(pci);
                            for (int i = 0; i < get_port_count(); i++) {
                                switch (get_port_type(i)) {
                                    case PORT_TYPE_SATA:
                                        insert_device(0x8, pci, "/dev/hd", i);
                                        break;
                                    case PORT_TYPE_SATAPI:
                                        insert_device(0x9, pci, "/dev/cd", i);
                                        break;
                                    case PORT_TYPE_SEMB:
                                        insert_device(0xa, pci, "/dev/semb", i);
                                        break;
                                    case PORT_TYPE_PM:
                                        insert_device(0xb, pci, "/dev/pm", i);
                                        break;
                                    case PORT_TYPE_NONE:
                                        insert_device(0xc, pci, "/dev/unknown", i);
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

void register_char(uint8_t major, const char* name, struct file_operations* fops) {
    strncpy(char_device_drivers[major].name, name, strlen(name));
    char_device_drivers[major].registered = 1;
    char_device_drivers[major].fops = fops;
}

void register_block(uint8_t major, const char* name, struct file_operations* fops) {
    strncpy(block_device_drivers[major].name, name, strlen(name));
    block_device_drivers[major].registered = 1;
    block_device_drivers[major].fops = fops;
    printf("Registered block device: %s [MAJ: %d]\n", name, major);
}

void unregister_block(uint8_t major) {
    block_device_drivers[major].registered = 0;
}

void unregister_char(uint8_t major) {
    char_device_drivers[major].registered = 0;
}

void init_devices() {
    printf("### DEVICES STARTUP ###\n");
    devices = (struct device*)request_page();
    memset(devices, 0, sizeof(struct device));

    struct mcfg_header* header = get_acpi_mcfg();
    if (header != 0) {
        register_pci(header);
    }
}

void device_list() {
    struct device* dev = devices;
    while (dev->valid) {
        printf("Device: %s [MAJ: %d MIN: %d]\n", dev->name, dev->major, dev->minor);
        dev = dev->next;
    }
}

struct device* get_device_head() {
    if (devices->valid)
        return devices;
    return (struct device*)0;
}

uint32_t get_device_count() {
    uint32_t count = 0;
    struct device* dev = devices;
    while (dev->valid) {
        count++;
        dev = dev->next;
    }
    return count;
}

uint32_t get_device_count_by_major(uint8_t major) {
    uint32_t count = 0;
    struct device* dev = devices;
    while (dev->valid) {
        if (dev->major == major)
            count++;
        dev = dev->next;
    }
    return count;
}

struct device* get_next_device(struct device* dev) {
    if (dev->next->valid)
        return dev->next;
    return (struct device*)0;
}

struct device* device_search(const char* device) {
    struct device* dev = devices;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            return dev;
        }
        dev = dev->next;
    }
    
    return (struct device*)0;
}

uint64_t device_read(const char * device, uint64_t size, uint64_t offset, uint8_t* buffer) {
    struct device* dev = devices;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                return block_device_drivers[dev->major].fops->read(dev->internal_id, size, offset, buffer);
            } else {
                return char_device_drivers[dev->major].fops->read(dev->internal_id, size, offset, buffer);
            }
        }
        dev = dev->next;
    }
    
    return 0;
}

uint64_t device_write(const char * device, uint64_t size, uint64_t offset, uint8_t* buffer) {
    struct device* dev = devices;
    do {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                return block_device_drivers[dev->major].fops->write(dev->internal_id, size, offset, buffer);
            } else {
                return char_device_drivers[dev->major].fops->write(dev->internal_id, size, offset, buffer);
            }
        }
        dev = dev->next;
    } while (dev->valid);
    return 0;
}

uint64_t device_ioctl (const char * device, uint32_t op, void* buffer) {
    struct device* dev = devices;
    do {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                return block_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
            } else {
                return char_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
            }
        }
        dev = dev->next;
    } while (dev->valid);
    return 0; 
}