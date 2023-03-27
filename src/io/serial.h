//https://wiki.osdev.org/Serial_Ports
#ifndef _SERIAL_H
#define _SERIAL_H
int init_serial();
int serial_received();
char read_serial();
int is_transmit_empty();
void write_serial(char a);

#endif