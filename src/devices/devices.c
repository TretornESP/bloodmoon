#include "devices.h"
#include "../memory/memory.h"
#include "../memory/paging.h"
#include "../memory/heap.h"
#include "../util/dbgprinter.h"
#include "../util/string.h"
#include "../util/printf.h"
#include "../arch/getcpuid.h"

struct device_driver char_device_drivers[256] = {0};
struct device_driver block_device_drivers[256] = {0};
struct device_driver net_device_drivers[256] = {0};

uint32_t irq_handlers[256] = {
    [0 ... 100] = 0x1,
    [101 ... 255] = 0x0
};

struct device* device_header = 0;

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
    "sd",   // c
    "none", // d
    "tty", // e
    "none", // f
    "mouse", // 10
    "net", // 11
    "fb", // 12
    "none", // 13 
    [0x8d] = "serial", // 80
    [0x8e] = "fifo",
    [0x8f] = "kbd", // 81
};

char * device_create(void * device_control_structure, uint8_t major, uint64_t id) {
    
    char name[32];
    struct device* device = device_header;
    uint8_t minor = 0;
    while (device->next != 0) {
        if ((uint8_t)(device->bc << 7 | device->major) == major)
            minor++;
        device = device->next;
    }

    sprintf(name, "%s%x", device_identifiers[major], minor+0xa);

    printf("Registering device: %s [MAJ: %x MIN: %x ID: %x CTRL: %x]\n", name, major, minor, id, device_control_structure);

    device->next = (struct device*)kmalloc(sizeof(struct device));
    memset(device->next, 0, sizeof(struct device));
    device->bc = ((major & 0x80) >> 7);
    device->major = major & 0x7F;
    device->minor = minor;
    strncpy(device->name, name, strlen(name));
    device->device_control_structure = device_control_structure;
    device->internal_id = id;
    device->valid = 1;
    
    return device->name;
}

char* insert_device_cb(void* device_control_structure, uint8_t major, uint64_t id) {
    return device_create(device_control_structure, major, id);
}

//TOD: Untested
void device_destroy(const char * device_name) {
    struct device* device = device_header;
    struct device* prev = 0;
    while (device->valid) {
        if (memcmp((void*)device->name, (void*)device, strlen(device_name)) == 0) {
            device->valid = 0;
            if (prev != 0) {
                prev->next = device->next;
                free_page(device);
            } else {
                free_page(device);
            }
            return;
        }
        prev = device;
        device = device->next;
    }
}


void register_char(uint8_t major, const char* name, struct file_operations* fops) {
    uint8_t major_id = major & 0x7f;
    strncpy(char_device_drivers[major_id].name, name, strlen(name));
    char_device_drivers[major_id].registered = 1;
    char_device_drivers[major_id].fops = fops;
    char_device_drivers[major_id].nops = 0x0;

    printf("Registered char device: %s [MAJ: %x]\n", name, major);
}

void register_block(uint8_t major, const char* name, struct file_operations* fops) {
    strncpy(block_device_drivers[major].name, name, strlen(name));
    block_device_drivers[major].registered = 1;
    block_device_drivers[major].fops = fops;
    block_device_drivers[major].nops = 0x0;
    printf("Registered block device: %s [MAJ: %x]\n", name, major);
}

void register_network(uint8_t major, const char * name, struct network_operations* nops) {
    strncpy(net_device_drivers[major].name, name, strlen(name));
    net_device_drivers[major].registered = 1;
    net_device_drivers[major].nops = nops;
    net_device_drivers[major].fops = 0x0;
    printf("Registered network device: %s [IDX: %x]\n", name, major);
}

void unregister_block(uint8_t major) {
    block_device_drivers[major].registered = 0;
}

void unregister_char(uint8_t major) {
    char_device_drivers[major].registered = 0;
}

void unregister_network(uint8_t major) {
    net_device_drivers[major].registered = 0;
}

void init_devices() {
    printf("### DEVICES STARTUP ###\n");
    device_header = (struct device*)kmalloc(sizeof(struct device));
    memset(device_header, 0, sizeof(struct device));

    struct fadt_header* fadt = get_acpi_fadt();
    if (fadt != 0) {
        register_fadt(fadt, insert_device_cb);
    }

    struct mcfg_header* mcfg = get_acpi_mcfg();
    if (mcfg != 0) {
        register_pci(mcfg, insert_device_cb);
    }
    
    register_comm(insert_device_cb);
}

void device_list(uint8_t mode) {
    struct device* dev = device_header;
    while (dev->valid) {
        if (mode == MODE_BLOCK && dev->bc == 0)
            printf("Device: %s [MAJ: %x MIN: %x]\n", dev->name, dev->major, dev->minor);
        else if (mode == MODE_CHAR && dev->bc == 1) {
            printf("Device: %s [MAJ: %x MIN: %x]\n", dev->name, dev->major, dev->minor);
        }
        dev = dev->next;
    }
}

struct device* get_device_head() {
    if (device_header->valid)
        return device_header;
    return (struct device*)0;
}

uint32_t get_device_count() {
    uint32_t count = 0;
    struct device* dev = device_header;
    while (dev->valid) {
        count++;
        dev = dev->next;
    }
    return count;
}

uint32_t get_device_count_by_major(uint8_t major) {
    uint32_t count = 0;
    struct device* dev = device_header;
    while (dev->valid) {
        if ((uint8_t)(dev->bc << 7 | dev->major) == major)
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
    struct device* dev = device_header;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)device, strlen(device)) == 0) {
            return dev;
        }
        dev = dev->next;
    }
    
    return (struct device*)0;
}

uint8_t device_identify(const char* device, char* driver_name) {
    struct device* dev = device_header;
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

uint64_t device_read(const char * name, uint64_t size, uint64_t offset, uint8_t* buffer) {
    struct device* dev = device_header;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)name, strlen(name)) == 0) {
            if (dev->bc == 0) {
                if (!block_device_drivers[dev->major].registered && !net_device_drivers[dev->major].registered)
                    continue;

                if (net_device_drivers[dev->major].nops != 0 && net_device_drivers[dev->major].nops->recv != 0)
                    return net_device_drivers[dev->major].nops->recv(dev->device_control_structure, buffer, size);
                
                if (block_device_drivers[dev->major].fops != 0 && block_device_drivers[dev->major].fops->read != 0)
                    return block_device_drivers[dev->major].fops->read(dev->internal_id, size, offset, buffer);
                
            } else {
                if (!char_device_drivers[dev->major].registered)
                    continue;

                if (char_device_drivers[dev->major].fops != 0 && char_device_drivers[dev->major].fops->read != 0)
                    return char_device_drivers[dev->major].fops->read(dev->internal_id, size, offset, buffer);
            }
        }
        dev = dev->next;
    }

    printf("Device %s not found\n", name);
    return 0;
}

uint64_t device_write(const char * name, uint64_t size, uint64_t offset, uint8_t* buffer) {
    struct device* dev = device_header;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)name, strlen(name)) == 0) {
            if (dev->bc == 0) {
                if (!block_device_drivers[dev->major].registered && !net_device_drivers[dev->major].registered)
                    continue;

                if (net_device_drivers[dev->major].nops != 0 && net_device_drivers[dev->major].nops->send != 0)
                    return net_device_drivers[dev->major].nops->send(dev->device_control_structure, buffer, size);
                
                if (block_device_drivers[dev->major].fops != 0 && block_device_drivers[dev->major].fops->write != 0)
                    return block_device_drivers[dev->major].fops->write(dev->internal_id, size, offset, buffer);
                
            } else {
                if (!char_device_drivers[dev->major].registered)
                    continue;

                if (char_device_drivers[dev->major].fops != 0 && char_device_drivers[dev->major].fops->write != 0)
                    return char_device_drivers[dev->major].fops->write(dev->internal_id, size, offset, buffer);
            }
        }
        dev = dev->next;
    }

    printf("Device %s not found\n", name);
    return 0;
}


uint64_t device_ioctl(const char * name, uint64_t op, void * buffer) {
    struct device* dev = device_header;
    while (dev->valid) {
        if (memcmp((void*)dev->name, (void*)name, strlen(name)) == 0) {
            if (dev->bc == 0) {
                if (!block_device_drivers[dev->major].registered && !net_device_drivers[dev->major].registered)
                    continue;

                if (net_device_drivers[dev->major].nops != 0 && net_device_drivers[dev->major].nops->ioctl != 0)
                    return net_device_drivers[dev->major].nops->ioctl(dev->device_control_structure, op, buffer);
                
                if (block_device_drivers[dev->major].fops != 0 && block_device_drivers[dev->major].fops->ioctl != 0)
                    return block_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
                
            } else {
                if (!char_device_drivers[dev->major].registered)
                    continue;

                if (char_device_drivers[dev->major].fops != 0 && char_device_drivers[dev->major].fops->ioctl != 0)
                    return char_device_drivers[dev->major].fops->ioctl(dev->internal_id, op, buffer);
            }
        }
        dev = dev->next;
    }

    printf("Device %s not found\n", name);
    return 0;
}
