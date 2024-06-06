#include "dbgshell.h"
#include "../devices/devices.h"
#include "../util/printf.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "../sched/scheduler.h"
#include "../sched/pit.h"
#include "../vfs/vfs_interface.h"
#include "../vfs/vfs.h"
#include "../memory/heap.h"
#include "../proc/loader.h"
#include "../drivers/net/e1000/e1000c.h"
#include "../drivers/ps2/ps2.h"
#include "../proc/process.h"
#include "../proc/raw.h"
#include "../devices/net/netstack.h"
#include "../drivers/gui/gui.h"
#include "../drivers/tty/tty.h"
#include "../drivers/fifo/fifo_interface.h"
#include "../devices/fb/framebuffer.h"
#include "../net/arp.h"
#include "../net/eth.h"
#include "../programs/terminal/terminal.h"
#include "debug.h"

struct command {
    char keyword[32];
    void (*handler)(int argc, char* argv[]);
};

void handler(void* ttyb, uint8_t event);

const char root[] = "hdap2";
char cwd[256] = {0};
char workpath[256] = {0};
char devno[32] = {0};

void test(int argc, char *argv[]) {
    printf("%s: %d arguments\n", argv[0], argc);
    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
}

void dump(int argc, char* argv[]) {
    dbg_flush();
}

void tty_device_handler(void* ttyb, uint8_t event) {
    int pid = get_current_task()->pid;
    add_signal(pid, DBGSHELL_SIGNAL_TTY, ttyb, (uint64_t)event);
}

char * find_suitable_keyboard() {
    struct device* current = get_device_head();
    while (current != 0) {
        if (current->major == DEVICE_KEYBOARD) {
            return current->name;
        }
        current = get_next_device(current);
    }
}

char * find_suitable_mouse() {
    struct device* current = get_device_head();
    while (current != 0) {
        if (current->major == DEVICE_MOUSE) {
            return current->name;
        }
        current = get_next_device(current);
    }
}

char * find_suitable_framebuffer() {
    struct device* current = get_device_head();
    while (current != 0) {
        if (current->major == FRAMEBUFFER_MAJOR) {
            return current->name;
        }
        current = get_next_device(current);
    }
}

void startx_cmd(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Starts the GUI\n");
        printf("Usage: startx\n");
        return;
    }

    char * keyboard = find_suitable_keyboard();
    char * mouse = find_suitable_mouse();
    char * framebuffer = find_suitable_framebuffer();
    
    struct gui_device * devices = kmalloc(sizeof(struct gui_device) * 3);
    memset(devices, 0, sizeof(struct gui_device) * 3);
    strcpy(devices[0].name, keyboard);
    devices[0].type = GUI_KEYBOARD_DEVICE;
    strcpy(devices[1].name, mouse);
    devices[1].type = GUI_MOUSE_DEVICE;
    strcpy(devices[2].name, framebuffer);
    devices[2].type = GUI_FB_DEVICE;

    startx(devices, 3);
}

void spawn_terminal(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Spawns a terminal\n");
        printf("Usage: terminal\n");
        return;
    }

    add_task(create_task((void*)init_terminal, "ttya", KERNEL_TASK, 0x0, 0x0, 0x0, 0x0, 0x0));
}

void lsdsk(int argc, char* argv[]) {
    vfs_lsdisk();
}

void lsblk(int argc, char* argv[]) {
    device_list(MODE_BLOCK);
}

void lschr(int argc, char* argv[]) {
    device_list(MODE_CHAR);
}

void attach(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Sets the output device for the debugger shell\n");
        printf("Usage: attach <tty>\n");
        return;
    }

    //device_ioctl(devno, 0x2, handler); //REMOVE SUBSCRIBER
    memset(devno, 0, 32);
    strcpy(devno, argv[1]);
    device_ioctl(devno, 0x1, handler); //ADD SUBSCRIBER
    
    set_current_tty(argv[1]);
    printf("Attached to %s\n", argv[1]);

}

void detach(int argc, char *argv[]) {
    reset_current_tty();
    printf("Reverted to default tty\n");
}

void readelf(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Reads the ELF header of a file\n");
        printf("Usage: readelf <file>\n");
        return;
    }

    int fd = vfs_file_open(argv[1], 0, 0);
    if (fd < 0) {
        printf("Could not open file %s\n", argv[1]);
        return;
    }

    vfs_file_seek(fd, 0, 0x2); //SEEK_END
    uint64_t size = vfs_file_tell(fd);
    vfs_file_seek(fd, 0, 0x0); //SEEK_SET

    uint8_t* buf = kmalloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    elf_readelf(buf, size);
    kfree(buf);
}

void loadelf(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Loads an ELF file into memory\n");
        printf("Usage: loadelf <file> <arg0> <arg1> ...\n");
        return;
    }

    int fd = vfs_file_open(argv[1], 0, 0);
    if (fd < 0) {
        printf("Could not open file %s\n", argv[1]);
        return;
    }

    vfs_file_seek(fd, 0, 0x2); //SEEK_END
    uint64_t size = vfs_file_tell(fd);
    vfs_file_seek(fd, 0, 0x0); //SEEK_SET

    uint8_t* buf = kmalloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    elf_load_elf(buf, size, argc - 1, argv + 1, 0);
    kfree(buf);
}

void loadraw(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Loads raw shellcode file into memory at address\n");
        printf("Usage: loadraw <file> <address:hex>\n");
        return;
    }

    int fd = vfs_file_open(argv[1], 0, 0);
    if (fd < 0) {
        printf("Could not open file %s\n", argv[1]);
        return;
    }

    vfs_file_seek(fd, 0, 0x2); //SEEK_END
    uint64_t size = vfs_file_tell(fd);
    vfs_file_seek(fd, 0, 0x0); //SEEK_SET

    uint8_t* buf = kmalloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);


    uint64_t addr = atou64(argv[2]);

    load_and_execute(buf, addr, size);
    kfree(buf);
}

//This function receives a relative path and returns an absolute path
void apply_cd(char* path) {
    memset(workpath, 0, 256);
    if (path[0] == '/') {
        strcpy(workpath, path+1);
    } else {
        strcpy(workpath, cwd);
        strcat(workpath, "/");
        strcat(workpath, path);
    }
}

void ls(int argc, char*argv[]) {
    if (argc < 2) {
        printf("Lists the contents of a directory\n");
        printf("Usage: ls <directory>\n");
        return;
    }

    apply_cd(argv[1]);
    printf("Listing directory %s\n", workpath);
    vfs_dir_list(workpath);
}

void sched(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Calls the scheduler\n");
        printf("Usage: sched\n");
        return;
    }

    yield();
}

void spawn(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Spawns a process\n");
        printf("Usage: spawn <nice> <addr of init> <privilege>\n");
        return;
    }

    //convert string to uint64_t
    char* endptr;
    long nice = atoi(argv[1]);
    (void)nice;
    uint64_t addr = strtoull(argv[2], &endptr, 16);
    uint64_t privilege = atoi(argv[3]);
    add_task(create_task((void*)addr, get_current_tty(), privilege, 0x0, 0x0, 0x0, 0x0, 0x0));
}

void kill(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Kills a process\n");
        printf("Usage: kill <pid>\n");
        return;
    }

    int16_t pid = atoi(argv[1]);
    kill_task(pid);
}

void ps(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Lists processes\n");
        printf("Usage: ps\n");
        return;
    }

    pseudo_ps();
}

void ptoggle(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Toggles preemption\n");
        printf("Usage: preempt\n");
        return;
    }

    preempt_toggle();
}

void mouse_signal_handler(int signo, void * signal_data, uint64_t signal_data_size) {
    struct ps2_mouse_status* status = (struct ps2_mouse_status*)signal_data;
    //static char * tty;
    //if (!tty) tty = get_current_tty();
    //if (tty == 0) {
    //    printf("No tty attached\n");
    //    return;
    //}

    //set_current_tty("default");
    printf("Mouse B:%x X:%d Y:%d F:%x\n", status->buttons, status->x, status->y, status->flags);
    //set_current_tty(tty);
}

void mouse_device_handler(struct ps2_mouse_status* status) {
    int pid = get_current_task()->pid;
    add_signal(pid, DBGSHELL_SIGNAL_MOUSE, (void*)status, sizeof(struct ps2_mouse_status));
}

void mousea(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Attaches the mouse\n");
        printf("Usage: mousea <device> [event]\n");
        return;
    }

    if (argc == 3) {
        device_ioctl(argv[1], IOCTL_MOUSE_SUBSCRIBE_EVENT, mouse_device_handler);
    } else {
        device_ioctl(argv[1], IOCTL_MOUSE_SUBSCRIBE, mouse_device_handler);
    }
}

void mouseda(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Detaches the mouse\n");
        printf("Usage: mouseda <device>x\n");
        return;
    }

    device_ioctl(argv[1], IOCTL_MOUSE_UNSUBSCRIBE, mouse_device_handler);
}

void writed(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Writes to a device\n");
        printf("Usage: write device text\n");
        return;
    }

    int fd = vfs_file_open(argv[1], 0, 0);
    if (fd < 0) {
        printf("Could not open device %s\n", argv[1]);
        return;
    }

    printf("Writing to %s\n", argv[1]);
    for (int i = 2; i < argc; i++) {
        vfs_file_write(fd, argv[i], strlen(argv[i]));
        vfs_file_write(fd, " ", 1);
    }

    vfs_file_close(fd);
}

void readd(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Reads n characters of a device\n");
        printf("Usage: read device [size] [offset]\n");
        return;
    }

    uint64_t size = 0;
    uint64_t offset = 0;

    int fd = vfs_file_open(argv[1], 0, 0);
    if (fd < 0) {
        printf("Could not open device %s\n", argv[1]);
        return;
    }

    if (argc > 3) {
        offset = atou64(argv[3]);
    }

    if (argc > 2) {
        size = atou64(argv[2]);
    } else {
        vfs_file_seek(fd, 0, 0x2); //SEEK_END
        size = vfs_file_tell(fd);
        vfs_file_seek(fd, 0, 0x0); //SEEK_SET
    }

    printf("Reading %d bytes from %s offset: %d\n", size, argv[1], offset);

    vfs_file_seek(fd, offset, 0x0); //SEEK_SET
    uint8_t* buf = kmalloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);
    buf[size] = '\0';    
    printf("str: %s\n", buf);
    printf("hex: ");
    for (uint64_t i = 0; i < size; i++) {
        printf("%02x ", buf[i]);
        if (i > 0 && i % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    kfree(buf);
}

void read(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Reads n characters of a file\n");
        printf("Usage: read <file> [size] [offset]\n");
        return;
    }

    uint64_t size = 0;
    uint64_t offset = 0;

    apply_cd(argv[1]);
    int fd = vfs_file_open(workpath, 0, 0);
    if (fd < 0) {
        printf("Could not open file %s\n", workpath);
        return;
    }

    if (argc > 3) {
        offset = atou64(argv[3]);
    }

    if (argc > 2) {
        size = atou64(argv[2]);
    } else {
        vfs_file_seek(fd, 0, 0x2); //SEEK_END
        size = vfs_file_tell(fd);
        vfs_file_seek(fd, 0, 0x0); //SEEK_SET
    }

    printf("Reading %d bytes from %s offset: %d\n", size, workpath, offset);

    vfs_file_seek(fd, offset, 0x0); //SEEK_SET
    uint8_t* buf = kmalloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    printf("%s\n", buf);
    kfree(buf);
}

void write(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Writes n characters to a file\n");
        printf("Usage: write <file>[:offset] <text...>\n");
        return;
    }

    uint64_t offset = 0;
    //Do not use strchr
    for (uint64_t i = 0; i < strlen(argv[1]); i++) {
        if (argv[1][i] == ':') {
            argv[1][i] = '\0';
            offset = atou64(argv[1] + i + 1);
            break;
        }
    }

    apply_cd(argv[1]);
    
    int fd = vfs_file_open(workpath, 0, 0);
    if (fd < 0) {
        printf("Could not open file %s\n", workpath);
        return;
    }

    printf("Writing to %s offset: %d\n", workpath, offset);
    printf("Text: ");
    for (int i = 2; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    vfs_file_seek(fd, offset, 0x0); //SEEK_SET
    for (int i = 2; i < argc; i++) {
        vfs_file_write(fd, argv[i], strlen(argv[i]));
        vfs_file_write(fd, " ", 1);
    }

    vfs_file_close(fd);
}

void create(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Creates a file\n");
        printf("Usage: create <file>\n");
        return;
    }

    apply_cd(argv[1]);
    int result = vfs_file_creat(workpath, 0);
    if (result < 0) {
        printf("Could not create file %s\n", workpath);
    }
}

void delete(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Deletes a file\n");
        printf("Usage: delete <file> [-force]\n");
        return;
    }

    uint8_t force = 0;
    if (argc > 2) {
        if (strcmp(argv[2], "-force") == 0)
            force = 1;
    }

    apply_cd(argv[1]);
    int result = vfs_remove(workpath, force);
    if (result < 0) {
        printf("Could not delete file %s\n", workpath);
    }
}

void mkdir(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Creates a directory\n");
        printf("Usage: mkdir <dir>\n");
        return;
    }

    apply_cd(argv[1]);
    int result = vfs_mkdir(workpath, 0);
    if (result < 0) {
        printf("Could not create directory %s\n", workpath);
    }
}

void normalizePath(char *path) {
    if (path == NULL || strlen(path) == 0) {
        return;
    }

    char normalizedPath[strlen(path) + 1];
    normalizedPath[0] = '\0';  // Initialize the normalizedPath

    char *token = strtok(path, "/");
    int depth = 0;

    while (token != NULL) {
        if (strcmp(token, "..") == 0) {
            if (depth > 0) {
                char *lastSlash = normalizedPath + strlen(normalizedPath) - 1;
                while (lastSlash > normalizedPath && *lastSlash != '/') {
                    lastSlash--;
                }
                if (lastSlash != normalizedPath) {
                    *lastSlash = '\0';
                    depth--;
                }
            }
        } else if (strcmp(token, ".") != 0 && strlen(token) > 0) {
            if (depth > 0) {
                strcat(normalizedPath, "/");
            }
            strcat(normalizedPath, token);
            depth++;
        }

        token = strtok(NULL, "/");
    }

    if (strlen(normalizedPath) > 0 && normalizedPath[strlen(normalizedPath) - 1] == '/') {
        normalizedPath[strlen(normalizedPath) - 1] = '\0';
    }

    strcpy(path, normalizedPath);
}

void nict(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Tests the network interface card\n");
        printf("Usage: nic <nic name> <network card device name>\n");
        return;
    }

    struct device * dev = device_search(argv[2]);
    if (dev == 0) {
        printf("Could not find device %s\n", argv[2]);
        return;
    }
    struct nic * nic = create_nic(argv[1], argv[2]);
    if (nic == 0) {
        printf("Could not create NIC\n");
        return;
    }

    printf("NIC created\n");
    dump_nics();
}

void ndump(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Dumps the packets of a NIC\n");
        printf("Usage: nicdump <nic name>\n");
        return;
    }

    struct nic * nic = get_nic(argv[1]);
    if (nic == 0) {
        printf("Could not find NIC %s\n", argv[1]);
        return;
    }

    dump_packets(nic);
}

void ioctl(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Creates a buffer of size <bsize> and calls <device>'s\n");
        printf("ioctl code <code> with the buffer as argument\n");
        printf("Then prints the buffer. If content is specified\n");
        printf("it will be copied to the buffer\n");
        printf("Usage: ioctl <device> <code> <bsize> <content...>\n");
        return;
    }

    uint64_t bsize = atou64(argv[3]);
    uint8_t* buffer = kmalloc(bsize);
    memset(buffer, 0, bsize);
    if (argc > 4) {
        for (int i = 4; i < argc; i++) {
            buffer[i-4] = atou8(argv[i]);
        }
        printf("Sending: %s\n", argv[4]);
        printf("Sending hex: ");
        for (int i = 0; i < argc - 4; i++) {
            printf("%02x ", buffer[i]);
            if (i > 0 && i % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    }

    uint64_t result = device_ioctl(argv[1], atou64(argv[2]), buffer);
    printf("Ioctl complete, Result: %d\n", result);
    printf("Buffer (str): %s\n", buffer);
    printf("Buffer hex: ");
    for (uint64_t i = 0; i < bsize; i++) {
        printf("%02x ", buffer[i]);
        if (i > 0 && i % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
    kfree(buffer);
}

void nicw(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Forces the nic worker to run on all nics\n");
        printf("Usage: nicw\n");
        return;
    }

    force_network_worker();
}

void nicadd(int argc, char* argv[]) { 
    if (argc < 2) {
        printf("Adds a dummy packet to the nic' tx\n");
        printf("Usage: nicadd <nic name>\n");
        return;
    }

    struct nic * nic = get_nic(argv[1]);
    if (nic == 0) {
        printf("Could not find NIC %s\n", argv[1]);
        return;
    }

    struct packet * pkt = create_tx_packet(nic, 2048);
    uint8_t * bufferExample = pkt->data;
    uint8_t mac2[6];
    memcpy(mac2, nic->mac, 6);

    memset(bufferExample, 0, 2048);

    uint32_t ipsrc = 0x54400001;
    uint32_t ipdest = 0x0A000101;

    uint8_t arp_buffer[42];

    // Ethernet header
    memcpy((void*)&arp_buffer[0], (void*)"\xff\xff\xff\xff\xff\xff", 6); // Destination MAC address (broadcast)
    memcpy((void*)&arp_buffer[6], (void*)mac2, 6); // Source MAC address
    memcpy((void*)&arp_buffer[12], (void*)"\x08\x06", 2); // Ethernet Type: ARP

    // ARP header
    memcpy((void*)&arp_buffer[14], (void*)"\x00\x01", 2); // Hardware Type: Ethernet
    memcpy((void*)&arp_buffer[16], (void*)"\x08\x00", 2); // Protocol Type: IPv4
    arp_buffer[18] = 6; // Hardware Address Length: 6 (Ethernet MAC address)
    arp_buffer[19] = 4; // Protocol Address Length: 4 (IPv4 address)
    memcpy((void*)&arp_buffer[20], (void*)"\x00\x01", 2); // Operation: ARP Request
    memcpy((void*)&arp_buffer[22], (void*)mac2, 6); // Sender Hardware Address: Source MAC address
    memcpy((void*)&arp_buffer[28], (void*)&ipsrc, 4); // Sender Protocol Address: Source IP address
    memcpy((void*)&arp_buffer[32], (void*)"\x00\x00\x00\x00\x00\x00", 6); // Target Hardware Address: zero (unknown)
    memcpy((void*)&arp_buffer[38], (void*)&ipdest, 4); // Target Protocol Address: Destination IP address

    memcpy(bufferExample, arp_buffer, 42);
    printf("NIC PACKET DUMP: \n");
    dump_packets(nic);
}

void nicflush(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Flushes the packets of a NIC\n");
        printf("Usage: nicflush <nic name>\n");
        return;
    }

    struct nic * nic = get_nic(argv[1]);
    if (nic == 0) {
        printf("Could not find NIC %s\n", argv[1]);
        return;
    }

    flush_tx(nic);
}

void arp_request(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Sends an ARP request and prints the resulting MAC\n");
        printf("Usage: arpr <nic name> <ip>\n");
        return;
    }

    struct nic * nic = get_nic(argv[1]);
    if (nic == 0) {
        printf("Could not find NIC %s\n", argv[1]);
        return;
    }

    struct arp arp;
    uint16_t length;
    uint8_t type[2];
    init_arp(&arp, nic->mac, "10.0.1.2", argv[2]);
    length = size_arp(&arp);
    ethertype_arp(type);

    uint8_t * arp_data = kmalloc(length); //No se si este cast falla para numeros mas grandes que 2^8 idk
    data_arp(&arp, arp_data);

    printf("ARP initialized\n");
    dump_arp(&arp);
    //Ethernet initialization
    struct eth eth;
    eth_create(&eth, nic->mac, (uint8_t*)"\xff\xff\xff\xff\xff\xff", arp_data, type, length);


    uint64_t size = eth_get_packet_size(&eth);
    uint8_t * buffer = kmalloc(size);
    memset(buffer, 0, size);

    uint16_t eth_packet_size = eth_to_packet(&eth, buffer, size);
    if (eth_packet_size == 0) {
        printf("Could not create ethernet packet\n");
        return;
    }

    struct packet * pkt = create_tx_packet(nic, eth_packet_size);
    memcpy(pkt->data, buffer, eth_packet_size);

    flush_tx(nic);

    printf("Packet sent\n");

    //Freeing resources
    kfree(arp_data);
    if (arp.sha == 0) {
        printf("SHA\n");
    } else {
        //print first 6 elements of arp.sha
        printf("SHA: %02x:%02x:%02x:%02x:%02x:%02x\n", arp.sha[0], arp.sha[1], arp.sha[2], arp.sha[3], arp.sha[4], arp.sha[5]);
    }
    destroy_arp(&arp);

    printf("Resources freed\n");

}

void nicc(int argc, char* argv[]) {
    if (argc < 7) {
        printf("Configures a nic\n");
        printf("Usage: nicc <nic> <ip> <subnet> <gw> <dns> <dhcp>\n");
        printf("Note: Value 0 means 0.0.0.0, on dhcp means disable (1 enable)\n");
        return;
    }

    struct nic * nic = get_nic(argv[1]);
    if (nic == 0) {
        printf("Could not find NIC %s\n", argv[1]);
        return;
    }

    uint8_t ip[4] = {0};
    uint8_t subnet[4] = {0};
    uint8_t gw[4] = {0};
    uint8_t dns[4] = {0};
    uint8_t dhcp = 0;

    if (strcmp(argv[2], "0") != 0)
        inet_pton(AF_INET, argv[2], ip);

    if (strcmp(argv[3], "0") != 0) 
        inet_pton(AF_INET, argv[3], subnet);

    if (strcmp(argv[4], "0") != 0)
        inet_pton(AF_INET, argv[4], gw);

    if (strcmp(argv[5], "0") != 0)
        inet_pton(AF_INET, argv[5], dns);

    if (strcmp(argv[6], "0") != 0)
        dhcp = 1;

    nic_configure(nic, ip, subnet, gw, dns, dhcp);
    print_nic(nic);
}

void nicp(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Prints the configuration of a nic\n");
        printf("Usage: nicp <nic name>\n");
        return;
    }

    struct nic * nic = get_nic(argv[1]);
    if (nic == 0) {
        printf("Could not find NIC %s\n", argv[1]);
        return;
    }

    print_nic(nic);
}

void mouse(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Prints the mouse status\n");
        printf("Usage: mouse <device>\n");
        return;
    }

    struct ps2_mouse_status mouse;
    uint64_t result = device_ioctl(argv[1], IOCTL_MOUSE_GET_STATUS, &mouse);
    if (result == 0) {
        printf("Could not get mouse status\n");
        return;
    }

    printf("Mouse status\n");
    printf("Buttons: %x\n", mouse.buttons);
    printf("X: %d\n", mouse.x);
    printf("Y: %d\n", mouse.y);
    printf("Flags: %x\n", mouse.flags);
}

void cd(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Changes the current directory\n");
        printf("Usage: cd <dir>\n");
        return;
    }

    apply_cd(argv[1]);
    memset(cwd, 0, 256);
    strcpy(cwd, workpath);
    //Make sure cwd ends in \0
    cwd[strlen(cwd)] = '\0';
    normalizePath(cwd);
}

void ppm(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Prints a ppm image\n");
        printf("Usage: ppm <filename> <x> <y>\n");
        return;
    }

    uint64_t x = atou64(argv[2]);
    uint64_t y = atou64(argv[3]);

    draw_image(argv[1], x, y);
}

void fb(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Tests the framebuffer\n");
        printf("Usage: fb <fbindex>\n");
        return;
    }

    uint8_t index = atou8(argv[1]);
    uint32_t red = 0x00FF0000;
    uint32_t green = 0x0000FF00;
    uint32_t blue = 0x000000FF;

    struct framebuffer * fb = get_framebuffer(index);
    if (fb == 0) {
        printf("Could not find framebuffer %d\n", index);
        return;
    }

    printf("Framebuffer %d\n", index);
    printf("Width: %d\n", fb->width);
    printf("Height: %d\n", fb->height);

    //Draw a red square
    for (uint64_t i = 0; i < fb->width; i++) {
        for (uint64_t j = 0; j < fb->height; j++) {
            if (i < fb->width / 2 && j < fb->height / 2) {
                draw_pixel(index, i, j, red);
            } else if (i >= fb->width / 2 && j < fb->height / 2) {
                draw_pixel(index, i, j, green);
            } else if (i < fb->width / 2 && j >= fb->height / 2) {
                draw_pixel(index, i, j, blue);
            }
        }
    }
}

void ticks(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Prints the number of ticks since boot\n");
        printf("Usage: ticks\n");
        return;
    }

    printf("Ticks: %ld\n", get_ticks_since_boot());
}

void dc(int argc, char* argv[]);
void help(int argc, char* argv[]);

struct command cmdlist[] = {
    {
        .keyword = "test",
        .handler = test
    },
    {
        .keyword = "attach",
        .handler = attach
    },
    {
        .keyword = "detach",
        .handler = detach
    },
    {
        .keyword = "startx",
        .handler = startx_cmd
    },
    {
        .keyword = "dmesg",
        .handler = dump
    },
    {
        .keyword = "readelf",
        .handler = readelf
    },
    {
        .keyword = "ppm",
        .handler = ppm
    },
    {
        .keyword = "nic",
        .handler = nict
    },
    {
        .keyword = "nicc",
        .handler = nicc
    },
    {
        .keyword = "fb",
        .handler = fb
    },
    {
        .keyword = "ls",
        .handler = ls
    },
    {
        .keyword = "nicw",
        .handler = nicw
    },
    {
        .keyword = "mouseda",
        .handler = mouseda
    },
    {
        .keyword = "mousea",
        .handler = mousea
    },
    {
        .keyword = "arpr",
        .handler = arp_request
    },
    {
        .keyword = "lsblk",
        .handler = lsblk
    },
    {
        .keyword = "lschr",
        .handler = lschr
    },
    {
        .keyword = "readd",
        .handler = readd
    },
    {
        .keyword = "writed",
        .handler = writed
    },
    {
        .keyword = "nicdump",
        .handler = ndump
    },
    {
        .keyword = "lsdsk",
        .handler = lsdsk
    },
    {
        .keyword = "loadelf",
        .handler = loadelf
    },
    {
        .keyword = "loadraw",
        .handler = loadraw
    },
    {
        .keyword = "ps",
        .handler = ps
    },
    {
        .keyword = "d",
        .handler = dc
    },
    {
        .keyword = "sterminal",
        .handler = spawn_terminal
    },
    {
        .keyword = "spawn",
        .handler = spawn
    },
    {
        .keyword = "sched",
        .handler = sched
    },
    {
        .keyword = "kill",
        .handler = kill
    },
    {
        .keyword = "nicp",
        .handler = nicp
    },
    {
        .keyword = "ioctl",
        .handler = ioctl
    },
    {
        .keyword = "nicadd",
        .handler = nicadd
    },
    {
        .keyword = "nicflush",
        .handler = nicflush
    },
    {
        .keyword = "read",
        .handler = read
    },
    {
        .keyword = "write",
        .handler = write
    },
    {
        .keyword = "create",
        .handler = create
    },
    {
        .keyword = "delete",
        .handler = delete
    },
    {
        .keyword = "mkdir",
        .handler = mkdir
    },
    {
        .keyword = "cd",
        .handler = cd
    },
    {
        .keyword = "preempt",
        .handler = ptoggle
    },
    {
        .keyword = "ticks",
        .handler = ticks
    },
    {
        .keyword = "mouse",
        .handler = mouse
    },
    {
        .keyword = "help",
        .handler = help
    }
};

void help(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Available commands:\n");
        for (long unsigned int i = 0; i < sizeof(cmdlist) / sizeof(struct command); i++) {
            printf("%s ", cmdlist[i].keyword);
            if (i > 1 && i % 5 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    } else {
        //Search for command, print all matches, even if partial
        for (long unsigned int i = 0; i < sizeof(cmdlist) / sizeof(struct command); i++) {
            if (strstr(cmdlist[i].keyword, argv[1]) != 0) {
                printf("%s\n", cmdlist[i].keyword);
                if (i > 1 && i % 5 == 0) {
                    printf("\n");
                }
            }
        }
    }
}

//TODO: Remove this, it's utterly stupid
void dc(int argc, char* argv[]) {
    char *av1[] = {"attach", "ttya"};
    int ac1 = 2;
    attach(ac1, av1);
    char *av2[] = {"loadelf", "hdap2/export/main.elf"};
    int ac2 = 2;
    loadelf(ac2, av2);
}

void promt() {
    printf("rotero@bloodmon:%s$ ", cwd);
}

void handler(void* ttyb, uint8_t event) {
    (void)ttyb;
    int pid = get_current_task()->pid;
    add_signal(pid, DBGSHELL_SIGNAL_TTY, ttyb, (uint64_t)event);

    //switch (event) {
    //    case 0x1: { //TTY_INB
    //        char cmd[1024] = {0};
    //        int read = device_read(devno, 1024, 0, (uint8_t*)cmd);
    //        if (read > 0) {
    //            //Convert string to array of words
    //            char* args[32] = {0};
    //            int argc = 0;
    //            char* tok = strtok(cmd, " ");
    //            while (tok != 0) {
    //                args[argc] = tok;
    //                argc++;
    //                tok = strtok(0, " ");
    //            }
//
    //            for (uint32_t i = 0; i < sizeof(cmdlist) / sizeof(struct command); i++) {
    //                if (strcmp(cmdlist[i].keyword, args[0]) == 0) {
    //                    cmdlist[i].handler(argc, args);
    //                    break;
    //                }
    //            }
    //        }
//
    //        promt();
    //    }
    //    default:
    //        break;
    //}
}

void tty_signal_handler(int signo, void * ttyb, uint64_t event) {
    switch (event) {
        case 0x1: { //TTY_INB
            char cmd[1024] = {0};
            //Make sure cmd is aligned
            if ((uint64_t)cmd % 16 != 0) {
                panic("cmd not aligned");
            }

            int read = device_read(devno, 1024, 0, (uint8_t*)cmd);
            if (read > 0) {
                //Convert string to array of words
                char* args[32] = {0};
                int argc = 0;
                char* tok = strtok(cmd, " ");
                while (tok != 0) {
                    args[argc] = tok;
                    argc++;
                    tok = strtok(0, " ");
                }

                for (uint32_t i = 0; i < sizeof(cmdlist) / sizeof(struct command); i++) {
                    if (strcmp(cmdlist[i].keyword, args[0]) == 0) {
                        cmdlist[i].handler(argc, args);
                        break;
                    }
                }
            }

            promt();
        }
        default:
            break;
    }
}

void print_prompt() {
    struct task * curr = get_current_task();
    int fd = curr->open_files[1];
    if (fd == -1) {
        return;
    }

    vfs_file_flush(fd);
    vfs_file_write(fd, "    ____  __    ____  ____  ____  __  _______  ____  _   __\n", 60);
    vfs_file_write(fd, "   / __ )/ /   / __ \\/ __ \\/ __ \\/  |/  / __ \\/ __ \\/ | / /\n", 60);
    vfs_file_write(fd, "  / __  / /   / / / / / / / / / / /|_/ / / / / / / /  |/ / \n", 60);
    vfs_file_write(fd, " / /_/ / /___/ /_/ / /_/ / /_/ / /  / / /_/ / /_/ / /|  /  \n", 60);
    vfs_file_write(fd, "/_____/_____/\\____/\\____/_____/_/  /_/\\____/\\____/_/ |_/   \n", 60);
    vfs_file_write(fd, "                                                           \n", 60);
    vfs_file_flush(fd);

    vfs_file_close(fd);
}

void init_dbgshell() {
    const char * tty = get_current_tty();
    if (tty == 0) {
        return;
    }

    if (device_search(tty) == 0) {
        printf("Could not find device %s\n", tty);
        return;
    }

    subscribe_signal(DBGSHELL_SIGNAL_MOUSE, mouse_signal_handler);
    subscribe_signal(DBGSHELL_SIGNAL_TTY, tty_signal_handler);

    strncpy(devno, tty, strlen(tty));
    device_ioctl(tty, 0x1, handler); //ADD SUBSCRIBER
    printf("\n");
    dbg_flush();
    strcpy(cwd, root);
    print_prompt();
    promt();

    while (1) {};
}

void kill_dbgshell() {
    device_ioctl(devno, 0x2, handler); //REMOVE SUBSCRIBER
    memset(devno, 0, 32);
}

void ex_dbgshell(const char * command) {
    char cmd[1024] = {0};
    strncpy(cmd, command, strlen(command));
    char* args[32] = {0};
    int argc = 0;
    char* tok = strtok(cmd, " ");
    while (tok != 0) {
        args[argc] = tok;
        argc++;
        tok = strtok(0, " ");
    }

    for (uint32_t i = 0; i < sizeof(cmdlist) / sizeof(struct command); i++) {
        if (strcmp(cmdlist[i].keyword, args[0]) == 0) {
            cmdlist[i].handler(argc, args);
            break;
        }
    }
}