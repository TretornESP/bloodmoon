#include "devices.h"
#include "../memory/memory.h"
#include "../util/dbgprinter.h"
#include "../util/string.h"
#include "../util/printf.h"
#include "comm/comm.h"

struct device_driver char_device_drivers[256] = {0};
struct device_driver block_device_drivers[256] = {0};
struct device* devices;

const char* device_identifiers[] = {
    "none", // 0
    "none", // 1
    "none", // 2
    "none", // 3
    "none", // 4
    "none", // 5
    "none", // 6
    "none", // 7
    "hd",   // 8
    "cd",   // 9
    "semb", // a
    "pm",   // b
    "umsd", // c
    "none", // d
    "tty", // e
    "kbd", // f
    "mouse", // 10
    "none", // 11
    "none", // 12
    "none", // 13
    [0x8d] = "serial", // 80
};

char* insert_device(uint8_t major, void* device_control_structure, const char * prefix, uint64_t id) {
    char name[32];
    memset(name, 0, 32);

    struct device* device = devices;
    uint8_t minor = 0;
    while (device->next != 0) {
        if (device->major == (major & 0x7f))
            minor++;
        device = device->next;
    }


    snprintf(name, 32, "%s%x", prefix, minor+0xa);

    printf("Registering device: %s [MAJ: %d MIN: %d ID: %d CTRL: %p]\n", name, major, minor, id, device_control_structure);

    device->next = (struct device*) request_page();
    memset(device->next, 0, sizeof(struct device));
    device->valid = 1;
    device->bc = ((major & 0x80) >> 7);
    device->major = major & 0x7f;
    device->minor = minor;
    strncpy(device->name, name, strlen(name));
    device->device_control_structure = device_control_structure;
    device->internal_id = id;

    return device->name;
}

char* insert_device_cb(void* device_control_structure, uint8_t major, uint64_t id) {
    return insert_device(major, device_control_structure, device_identifiers[major], id);
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
        register_pci(header, insert_device_cb);
    }
    register_comm(insert_device_cb);
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

uint8_t device_identify(const char* device, char* driver_name) {
    struct device* dev = devices;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                if (!block_device_drivers[dev->major].registered)
                    return 0;
                return !strcmp(block_device_drivers[dev->major].name, driver_name);
            } else {
                if (!char_device_drivers[dev->major].registered)
                    return 0;
                return !strcmp(char_device_drivers[dev->major].name, driver_name);
            }
        }
        dev = dev->next;
    }

    return 0;
}

uint64_t device_read(const char * device, uint64_t size, uint64_t offset, uint8_t* buffer) {
    struct device* dev = devices;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            if (dev->bc == 0) {
                if (!block_device_drivers[dev->major].registered)
                    return 0;
                return block_device_drivers[dev->major].fops->read(dev->internal_id, size, offset, buffer);
            } else {
                if (!char_device_drivers[dev->major].registered)
                    return 0;
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
                if (!block_device_drivers[dev->major].registered)
                    return 0;
                return block_device_drivers[dev->major].fops->write(dev->internal_id, size, offset, buffer);
            } else {
                if (!char_device_drivers[dev->major].registered)
                    return 0;
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
                if (!block_device_drivers[dev->major].registered)
                    return 0;
                return block_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
            } else {
                if (!char_device_drivers[dev->major].registered)
                    return 0;
                return char_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
            }
        }
        dev = dev->next;
    } while (dev->valid);
    return 0; 
}