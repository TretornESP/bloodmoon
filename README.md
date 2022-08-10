# bloodmoon

WIP Os for my twitch series: https://www.twitch.tv/damebanda

## State:

- [x] Basic bootstrap
- [x] Interface with limine
- [x] Basic print functionality
- [ ] Page frame allocator
- [ ] CPU Structures (GDT, IDT, TSS, LDT)
- [ ] Paging and mappings
- [ ] Memory sectoring and protection
- [ ] Advanced print functionality
- [ ] Heap
- [ ] I/O Basic interfaces
- [ ] Keyboard drivers
- [ ] Mouse drivers
- [ ] ACHI structures
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
- [ ] Graphical interface
- [ ] Advanced device drivers (USB)

## Requirements

- **binutils** and **gcc** default toolchain stuff

- **xorriso**

- **qemu** (optional)


- **Limine**. Inside the folder:

      git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1


## How to compile

Modify GNUmakefile paths to point to your toolchain.
pay special attention to QEMU var as this is hardcoded with
my config and probs wont work with yours.

to compile just run:

    make setup
    make

you can cleanup garbage with the classic:

    make clean

Note: **go.sh** is redundant with make, used only to automate from vscode.
