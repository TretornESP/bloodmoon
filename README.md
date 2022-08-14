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
- [ ] Clocks and signal lines
- [ ] Proces structures
- [ ] Task switching
- [ ] Basic concurrency
- [ ] Basic FS
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

## Acknoledgements and credits

- Marco Paland (info@paland.com)
- Lukás Chmela
- Absurd Poncho
- The Limine project