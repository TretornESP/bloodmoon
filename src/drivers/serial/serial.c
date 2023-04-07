#include "serial.h"

#include "../../io/io.h"
#include "../../io/interrupts.h"
#include "../../memory/heap.h"

#include "../../util/printf.h"

#include <stdint.h>

__attribute__((interrupt)) void COM1_HANDLER(struct interrupt_frame * frame);
__attribute__((interrupt)) void COM2_HANDLER(struct interrupt_frame * frame);
__attribute__((interrupt)) void COM3_HANDLER(struct interrupt_frame * frame);
__attribute__((interrupt)) void COM4_HANDLER(struct interrupt_frame * frame);

//TODO: This are not the correct handlers!
struct serial_device serial_devices[] = {
   {
      .port = DEFAULT_COM1_PORT,
      .irq = IRQ_COM1,
      .handler = COM1_HANDLER,
      .valid = 0
   },
   {
      .port = DEFAULT_COM2_PORT,
      .irq = IRQ_COM2,
      .handler = COM2_HANDLER,
      .valid = 0
   },
   {
      .port = DEFAULT_COM3_PORT,
      .irq = IRQ_COM3,
      .handler = COM3_HANDLER,
      .valid = 0
   },
   {
      .port = DEFAULT_COM4_PORT,
      .irq = IRQ_COM4,
      .handler = COM4_HANDLER,
      .valid = 0
   }
};

uint8_t initialized = 0;

__attribute__((interrupt)) void COM1_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = serial_devices[0];
   pic_eoi(device->irq);
   if (!device->valid) return;
   char c = read_serial(device->port);
   if (device->handler) device->handler(c);
}

__attribute__((interrupt)) void COM2_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = serial_devices[1];
   pic_eoi(device->irq);
   if (!device->valid) return;
   char c = read_serial(device->port);
   if (device->handler) device->handler(c);
}

__attribute__((interrupt)) void COM3_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = serial_devices[2];
   pic_eoi(device->irq);
   if (!device->valid) return;
   char c = read_serial(device->port);
   if (device->handler) device->handler(c);
}

__attribute__((interrupt)) void COM4_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = serial_devices[3];
   pic_eoi(device->irq);
   if (!device->valid) return;
   char c = read_serial(device->port);
   if (device->handler) device->handler(c);
}

struct serial_device* get_serial(int port) {
   if (!initialized) return;

   for (int i = 0; i < MAX_COM_DEVICES; i++) {
      if (serial_devices[i].comm && serial_devices[i].port == port) {
         return &(serial_devices[i]);
      }
   }
   return 0;
}

int init_serial_comm(int port) {

   outb(port + 1, 0x00);    // Disable all interrupts
   outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(port + 1, 0x00);    //                  (hi byte)
   outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(port + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(inb(port + 0) != 0xAE) {
      return 0;
   }
 
   // If serial is not faulty set it in normal operation mode
   outb(port + 4, 0x0F);

   return 1;
}

void init_serial(int inbs, int outbs) {
   if (initialized) return;
   for (int i = 0; i < MAX_COM_DEVICES; i++) {
      struct serial_device* device = &(serial_devices[i]);
      if (init_serial_comm(device->port)) {
         device->inb_size = inbs;
         device->outb_size = outbs;
         device->inb = (char*)calloc(1, inbs);
         device->outb = (char*)calloc(1, outbs);
         device->inb_index = 0;
         device->outb_index = 0;

         if (device->inb && device->outb) {
            device->valid = 1;
            printf("Serial port %d initialized successfully\n");
         }
      }
   }
   initialized = 1;
}

void enable_serial_com(int port) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   outb(port + 1, 0x01);    // Enable all interrupts
   if (device) {
      device->valid = 1;
   }
}
 
void disable_serial_com(int port) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   outb(port + 1, 0x00);    // Disable all interrupts
   if (device) {
      device->valid = 0;
   }
}

int serial_received(int port) {
   return inb(port + 5) & 1;
}
 
char read_serial(int port) {
   while (serial_received(port) == 0);
 
   return inb(port);
}

int is_transmit_empty(int port) {
   return inb(port + 5) & 0x20;
}
 
void write_serial(int port, char a) {
   while (is_transmit_empty(port) == 0);
 
   outb(port,a);
}