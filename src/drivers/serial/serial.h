//https://wiki.osdev.org/Serial_Ports
#ifndef _SERIAL_H
#define _SERIAL_H

#define SERIAL_BUFFER_SIZE 1024
#define MAX_COM_DEVICES 2
#define IRQ_COM1 0x24
#define IRQ_COM2 0x23
#define IRQ_COM3 0x24
#define IRQ_COM4 0x23
#define DEFAULT_COM1_PORT 0x3f8          // COM1
#define DEFAULT_COM2_PORT 0x2f8          // COM2
#define DEFAULT_COM3_PORT 0x3e8          // COM3
#define DEFAULT_COM4_PORT 0x2e8          // COM4

struct serial_device {
    int valid;
    int port;
    int irq;

    void (*handler)(char c);

    char * inb;
    int inb_size;
    int inb_index;
    char * outb;
    int outb_size;
    int outb_index;
};

void init_serial(int inbs, int outbs);

struct serial_device* get_serial(int port);
void enable_serial_com(int port);
void disable_serial_com(int port);

int serial_received(int port);
char read_serial(int port);
int is_transmit_empty(int port);
void write_serial(int port, char a);

#endif