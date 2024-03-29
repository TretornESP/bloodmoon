#include "dbgshell.h"
#include "../dev/devices.h"
#include "../util/printf.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "../scheduling/scheduler.h"
#include "../scheduling/pit.h"
#include "../vfs/vfs_interface.h"
#include "../memory/heap.h"
#include "../process/loader.h"
#include "../drivers/net/e1000/e1000c.h"
#include "../process/process.h"
#include "../process/raw.h"
#include "../dev/net/netstack.h"
#include "../net/arp.h"
#include "../net/eth.h"
#include "debug.h"

struct command {
    char keyword[32];
    void (*handler)(int argc, char* argv[]);
};

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

    uint8_t* buf = malloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    elf_readelf(buf, size);
    free(buf);
}

void loadelf(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Loads an ELF file into memory\n");
        printf("Usage: loadelf <file>\n");
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

    uint8_t* buf = malloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    elf_load_elf(buf, size, 0);
    free(buf);
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

    uint8_t* buf = malloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);


    uint64_t addr = atou64(argv[2]);

    load_and_execute(buf, addr, size);
    free(buf);
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
    if (argc < 2) {
        printf("Spawns a process\n");
        printf("Usage: spawn <addr of init>\n");
        return;
    }

    //convert string to uint64_t
    char* endptr;
    uint64_t addr = strtoull(argv[1], &endptr, 16);
    add_task(create_task((void*)addr, get_current_tty()));
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

void ptasks(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Prints the task list\n");
        printf("Usage: ptasks\n");
        return;
    }

    task_test();
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
    uint8_t* buf = malloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    printf("%s\n", buf);
    free(buf);
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
    uint8_t* buffer = malloc(bsize);
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
    free(buffer);
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

    uint8_t * arp_data = malloc(length); //No se si este cast falla para numeros mas grandes que 2^8 idk
    data_arp(&arp, arp_data);

    printf("ARP initialized\n");
    dump_arp(&arp);
    //Ethernet initialization
    struct eth eth;
    eth_create(&eth, nic->mac, (uint8_t*)"\xff\xff\xff\xff\xff\xff", arp_data, type, length);


    uint64_t size = eth_get_packet_size(&eth);
    uint8_t * buffer = malloc(size);
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
    free(arp_data);
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

void dwrite(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Writes to a device\n");
        printf("Usage: dwrite <dir>\n");
        return;
    }

    printf("NOT IMPLEMENTED YET -\\_(-.-)_/-\n");

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
        .keyword = "dmesg",
        .handler = dump
    },
    {
        .keyword = "readelf",
        .handler = readelf
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
        .keyword = "ls",
        .handler = ls
    },
    {
        .keyword = "nicw",
        .handler = nicw
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
        .keyword = "dwrite",
        .handler = dwrite
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
        .keyword = "ptasks",
        .handler = ptasks
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
    switch (event) {
        case 0x1: { //TTY_INB
            char cmd[1024] = {0};
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

void init_dbgshell(const char* tty) {
    if (tty == 0) {
        return;
    }

    if (device_search(tty) == 0) {
        printf("Could not find device %s\n", tty);
        return;
    }

    strncpy(devno, tty, strlen(tty));
    device_ioctl(tty, 0x1, handler); //ADD SUBSCRIBER
    printf("\n");
    dbg_flush();
    strcpy(cwd, root);
    print_prompt();
    promt();
}

void kill_dbgshell() {
    device_ioctl(devno, 0x2, handler); //REMOVE SUBSCRIBER
    memset(devno, 0, 32);
}