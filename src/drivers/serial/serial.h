//https://wiki.osdev.org/Serial_Ports
#ifndef _SERIAL_H
#define _SERIAL_H

#include "../../io/interrupts.h"

#define SERIAL_BUFFER_SIZE 1024
#define MAX_COM_DEVICES 2
#define SERIAL_OF_IRQ 0x91
#define IRQ_COM1 0x24
#define IRQ_COM2 0x23
#define IRQ_COM3 0x24
#define IRQ_COM4 0x23
#define DEFAULT_COM1_PORT 0x3f8          // COM1
#define DEFAULT_COM2_PORT 0x2f8          // COM2
#define DEFAULT_COM3_PORT 0x3e8          // COM3
#define DEFAULT_COM4_PORT 0x2e8          // COM4

//Important: dont be dislexic
//Inb: Buffer that hold what the serial port is sending to this computer
//Outb: Buffer that hold what this computer is sending to the serial port

struct serial_device {
    int valid;
    int port;
    int irq;

    void (*handler)(struct interrupt_frame *frame);
    void (*write_handler)(char c, int port);
    void (*read_handler)(char c, int port);

    char * inb;
    int inb_size;
    int inb_write;
    int inb_read;

    char * outb;
    int outb_size;
    int outb_write;
    int outb_read;
};

void init_serial(int inbs, int outbs);

struct serial_device* get_serial(int port);
volatile struct serial_device* get_last_interrupted_serial();

void serial_read_event_add(int port, void (*handler)(char c, int port));
void serial_read_event_remove(int port);

void serial_write_event_add(int port, void (*handler)(char c, int port));
void serial_write_event_remove(int port);

void serial_flush(int port);
void serial_write(int port, char c);
char serial_read(int port);
#endif