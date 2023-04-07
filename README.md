![Logo](https://github.com/TretornESP/bloodmoon/raw/main/logo.png)

![Completion](https://github.com/TretornESP/bloodmoon/raw/main/badge.svg)

# bloodmoon


WIP Os for my twitch series: https://www.twitch.tv/damebanda

Current memory usage:
```
6MB
```

## State:

- [x] Basic bootstrap
- [x] Interface with limine
- [x] Basic print functionality
- [x] Page frame allocator
- [x] CPU Structures (GDT, IDT, TSS, LDT) (needs improvements)
- [x] Paging and mappings
- [x] Memory sectoring and protection
- [x] Advanced print functionality
- [x] Heap
- [x] I/O Basic interfaces
- [x] Keyboard drivers
- [x] ACPI structures
- [x] Clocks and signal lines (signal lines moved forward)
- [x] Proces structures
- [ ] Task switching
- [ ] Basic concurrency
- [x] Basic FS (in process)
- [ ] Process loading
- [ ] Syscalls
- [ ] Userspace
- [ ] GLibc (sorta)
- [ ] Basic software utilities
- [ ] Network drivers
- [ ] Network stack
- [ ] Mouse drivers
- [ ] Graphical interface
- [ ] Advanced device drivers (USB)

## Requirements

- **binutils** and **gcc** default toolchain stuff

- **xorriso**

- **qemu** (optional)


- **Limine**. Inside the folder:

      git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1


## How to configure

Modify GNUmakefile paths to point to your toolchain.
Yo will have to modify:

- **QEMU** Path to a qemu binary
- **GDB** Path to a gdb binary (required only for debug target)
- **CMDNEWSCREEN** In WSL2 pops up a new terminal (required only for debug target)
- **WSLHOSTIP** In WSL2 gets the windows host ip (required only for debug target)

## Makefile targets

To create required files (only do this once):
    
    make setup

To compile (and run) just run:

    make

You can cleanup garbage with the classic:

    make clean

If setup correctly you can start a debugging session with:

    make debug

You can modify the file **debug.gdb** in order to change the starting symbol.
Remember that you need the port 1234 open in your host firewall.

Note: **go.sh** is redundant with make, used only to automate from vscode through
a build task (ctrl+shift+b).

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "make",
            "type": "shell",
            "command": "./go.sh",
            "problemMatcher": [],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```

## Driver support

For adding drivers simply create a file in the drivers folder with at least the following

```c
#include "../dev/devices.h"

//Add here your functions
uint64_t my_read(struct file *file, uint64_t offset, uint64_t size, uint8_t *buffer) {
    //True read operation
    return 0;
}

struct file_operations fops = {
    .read = my_read
};

void init_dummy_driver(void) {
    //8 is the major number for SCSI devices, use your own.
    register_block(8, "DRIVER NAME", &fops);
}

void exit_dummy_driver(void) {unregister_block(8);}
```

Then, at some time you must call the init function of your driver:

```c
init_dummy_driver();

```

Now you are ready to call generic functions like:

```c
    device_read("/dev/sda", 8, 0, buffer);
```

As long as **/dev/sda** is a valid device with major=8, the function will call your driver.

## Some debug help

### I cancelled the gpt build:

Now you probably have spurious mounted drives in your system, you will have to:

- Unmount everything under **/mnt/bloodmoon**
- Delete the **/mnt/bloodmoon** folder
- delete every loop device used by the build **(losetup -a)** and delete them **(losetup -d /dev/loopX)**
- delete everything with **sudo cleansetup**
- rebuild

## TODO

### VFS

Okay, so the idea is the following:

- Implement the ext2 format functions
- Implement the ext2 link functions
- Allocate a chunk of memory for the filesystem
- Format it with ext2
- Create folders for /dev /sys /proc /bin /etc /home /mnt /tmp /var /usr and whatever else i feel like
- Link the devices to the /dev folder
- ¿What does mount mean in this context?
- Chroot this virtual filesystem to /

## Acknoledgements and credits

- Keep-Konect [github](https://github.com/Keep-Konect)
- Marco Paland (info@paland.com)
- Lukás Chmela
- Absurd Poncho
- The Limine project
- strawberryhacker