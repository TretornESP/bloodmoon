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
#include "../process/process.h"
#include "../process/raw.h"
#include "debug.h"

struct command {
    char keyword[32];
    void (*handler)(int argc, char* argv[]);
};

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
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);


    uint64_t addr = atou64(argv[2]);

    load_and_execute(buf, addr, size);
    free(buf);
}

void ls(int argc, char*argv[]) {
    if (argc < 2) {
        printf("Lists the contents of a directory\n");
        printf("Usage: ls <directory>\n");
        return;
    }

    vfs_dir_list(argv[1]);
}

void sched(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Calls the scheduler\n");
        printf("Usage: sched\n");
        return;
    }

    schedule();
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

void ticks(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Prints the number of ticks since boot\n");
        printf("Usage: ticks\n");
        return;
    }

    printf("Ticks: %ld\n", get_ticks_since_boot());
}

void dc(int argc, char* argv[]);

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
        .keyword = "ls",
        .handler = ls
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
        .keyword = "sched",
        .handler = sched
    },
    {
        .keyword = "preempt",
        .handler = ptoggle
    },
    {
        .keyword = "ticks",
        .handler = ticks
    }
};

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
    printf("dbgshell> ");
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

void init_dbgshell() {
    char* tty = get_current_tty();
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
    promt();
}

void kill_dbgshell() {
    device_ioctl(devno, 0x2, handler); //REMOVE SUBSCRIBER
    memset(devno, 0, 32);
}