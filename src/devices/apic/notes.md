PCI vs PCIe:
    PCI has a problem in which only supports 4 interrupts, 
    pcie devices use MSI (Message Signaled Interrupts) which
    allows the device to send a message to the cpu without the IOAPIC.

How does IOAPIC work:
    - It contains 24 - 64 bit registers, each one contains a hardwired source.
    IRQ's 0..15 => IOAPIC registers 0..15
    PCI A..D    => IOAPIC registers 16..19
    ¿idk?       => IOAPIC registers 20..23 

How to program the IOAPIC registers:
    Set IRQ (0,15) to
        edge triggered
        active high (both <13> and <15> zero).
    Set PIC (A,D) to
        level triggered
        active low (both <13> and <15> one).

    Now assign each one of the 24 registers to a vector.
    Skip 0x00..0x1F, they are reserved. Use 0x20..0xFF.

    The priority of the interrupt is determined by the vector number.

PCI Interrupts A..D:
    The bios is the only one that knows the mapping of PCI interrupts to
    IOAPIC registers. The OS must read the motherboard. This is hard af...

MSI and MSIX:
    If you can, always use MSI or MSIX. They are much better than IOAPIC.

About the ICMR: (Copilot says)
    The ICMR is a 32 bit register, each bit corresponds to a vector.
    If the bit is set, the interrupt is masked.
    The ICMR is located at ports 0x22 and 0x23.
How to enable IOAPIC:
    1. Disable 8259 by writing 0xFF to ports 0x21 and 0xA1.
    2. Program the IOAPIC
    3. Write the ICMR (0x22,0x23) ports to accept IOAPIC interrupts.

About the lapic logical destination:
    0000 0001 -> core 0
    0000 0010 -> core 1
    0000 0100 -> core 2
    0000 1000 -> core 3
    0001 0000 -> core 4
    0010 0000 -> core 5
    0100 0000 -> core 6
    1000 0000 -> core 7

    You can support more than 8 cpus, ofc you can xd
    But i dont know how to do it, so i will just support 8 cpus.

    Of all the cpus that are ready to receive the interrupt, the one with the
    lowest lapic id will receive the interrupt.

    when i invalidate a shared page entry, i only invalidate the cpus that are
    using that page, not all the cpus.

