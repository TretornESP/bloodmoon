#include "serial.h"

#include "../../io/io.h"
#include "../../memory/heap.h"

#include "../../util/printf.h"
#include "../../util/string.h"

#include <stdint.h>

__attribute__((interrupt)) void COM1_HANDLER(struct interrupt_frame * frame);
__attribute__((interrupt)) void COM2_HANDLER(struct interrupt_frame * frame);
__attribute__((interrupt)) void COM3_HANDLER(struct interrupt_frame * frame);
__attribute__((interrupt)) void COM4_HANDLER(struct interrupt_frame * frame);

volatile struct serial_device* interrupted_serial_device = 0;

__attribute__((interrupt)) void SERIAL_OF_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   if (interrupted_serial_device) {
      printf("PORT %d: Overflow detected!\n", interrupted_serial_device->port);
      printf("Ignoring as this is not implemented...\n");
   }
}

//TODO: This are not the correct handlers!
struct serial_device serial_devices[] = {
   {
      .port = DEFAULT_COM1_PORT,
      .irq = IRQ_COM1,
      .handler = COM1_HANDLER,
      .read_subscribers = 0,
      .write_subscribers = 0,
      .valid = 0
   },
   {
      .port = DEFAULT_COM2_PORT,
      .irq = IRQ_COM2,
      .handler = COM2_HANDLER,
      .read_subscribers = 0,
      .write_subscribers = 0,
      .valid = 0
   },
   {
      .port = DEFAULT_COM3_PORT,
      .irq = IRQ_COM3,
      .handler = COM3_HANDLER,
      .read_subscribers = 0,
      .write_subscribers = 0,
      .valid = 0
   },
   {
      .port = DEFAULT_COM4_PORT,
      .irq = IRQ_COM4,
      .handler = COM4_HANDLER,
      .read_subscribers = 0,
      .write_subscribers = 0,
      .valid = 0
   }
};

uint8_t initialized = 0;

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

void add_subscriber(struct serial_device* device, uint8_t type, void* handler) {
   struct serial_subscriber* subscriber;
   if (type == SERIAL_READ) {
      subscriber = device->read_subscribers;
   } else if (type == SERIAL_WRITE) {
      subscriber = device->write_subscribers;
   } else {
      return;
   }

   while (subscriber->next) {
      subscriber = subscriber->next;
   }

   subscriber->next = malloc(sizeof(struct serial_subscriber));
   subscriber->next->handler = handler;
   subscriber->next->next = 0;
}

void remove_subscriber(struct serial_device* device, uint8_t type, void* handler) {
   struct serial_subscriber* subscriber;
   if (type == SERIAL_READ) {
      subscriber = device->read_subscribers;
   } else if (type == SERIAL_WRITE) {
      subscriber = device->write_subscribers;
   } else {
      return;
   }

   while (subscriber->next) {
      if (subscriber->next->handler == handler) {
         struct serial_subscriber* to_remove = subscriber->next;
         subscriber->next = subscriber->next->next;
         free(to_remove);
         return;
      }
      subscriber = subscriber->next;
   }
}

void run_subscribers(struct serial_device* device, uint8_t type, char c) {
   struct serial_subscriber* subscriber;
   if (type == SERIAL_READ) {
      subscriber = device->read_subscribers;
   } else if (type == SERIAL_WRITE) {
      subscriber = device->write_subscribers;
   } else {
      return;
   }

   while (subscriber->next) {
      subscriber = subscriber->next;
      subscriber->handler(c, device->port);
   }
}

void write_inb(struct serial_device* device, char c) {
   device->inb[device->inb_write++] = c;
   if (device->inb_write >= device->inb_size) device->inb_write = 0;
   if (device->inb_write == device->inb_read) {
      raise_interrupt(SERIAL_OF_IRQ);
   }
   run_subscribers(device, SERIAL_WRITE, c);
}

char read_inb(struct serial_device* device) {
   
   if (device->inb_read == device->inb_write) return 0; //No data available
   char c = device->inb[device->inb_read++];
   if (device->inb_read >= device->inb_size) device->inb_read = 0;
   run_subscribers(device, SERIAL_READ, c);
   return c;
}

void write_outb(struct serial_device* device, char c) {
   device->outb[device->outb_write++] = c;
   if (device->outb_write >= device->outb_size) device->outb_write = 0;
   if (device->outb_write == device->outb_read) {
      raise_interrupt(SERIAL_OF_IRQ);
   }
   run_subscribers(device, SERIAL_WRITE, c);
}

char read_outb(struct serial_device* device) {
   if (device->outb_read == device->outb_write) return 0; //No data available
   char c = device->outb[device->outb_read++];
   if (device->outb_read >= device->outb_size) device->outb_read = 0;
   run_subscribers(device, SERIAL_READ, c);
   return c;
}

__attribute__((interrupt)) void COM1_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = &(serial_devices[0]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_inb(device, c);
   pic_eoi(device->irq);
}

__attribute__((interrupt)) void COM2_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = &(serial_devices[1]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_inb(device, c);
   pic_eoi(device->irq);
}

__attribute__((interrupt)) void COM3_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = &(serial_devices[2]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_inb(device, c);
   pic_eoi(device->irq);
}

__attribute__((interrupt)) void COM4_HANDLER(struct interrupt_frame * frame) {
   (void)frame;
   struct serial_device* device = &(serial_devices[3]);
   if (!device->valid) return;
   interrupted_serial_device = device;
   char c = read_serial(device->port);
   write_inb(device, c);
   pic_eoi(device->irq);
}

struct serial_device* get_serial_by_comm(int comm) {
   if (!initialized) return 0;
   if (comm < 0 || comm >= MAX_COM_DEVICES) return 0;
   if (!serial_devices[comm].valid) return 0;
   return &(serial_devices[comm]);
}

struct serial_device* get_serial(int port) {
   if (!initialized) return 0;

   for (int i = 0; i < MAX_COM_DEVICES; i++) {
      if (serial_devices[i].valid && serial_devices[i].port == port) {
         return &(serial_devices[i]);
      }
   }
   return 0;
}

volatile struct serial_device* get_last_interrupted_serial() {
   return interrupted_serial_device;
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

void enable_serial_int(struct serial_device* device) {
   if (device) {
      hook_interrupt(device->irq, device->handler, 0);
      outb(device->port + 1, 0x01);    // Enable all interrupts
   }
}
 
void disable_serial_int(struct serial_device* device) {
   if (device) {
      unhook_interrupt(device->irq, 0);
      outb(device->port + 1, 0x00);    // Disable all interrupts
   }
}

void init_serial(int inbs, int outbs) {
   if (initialized) return;
   for (int i = 0; i < MAX_COM_DEVICES; i++) {
      struct serial_device* device = &(serial_devices[i]);
      if (init_serial_comm(device->port)) {
         device->inb_size = inbs;
         device->outb_size = outbs;
         device->inb = (char*)calloc(1, inbs);
         memset(device->inb, 0, inbs);
         device->outb = (char*)calloc(1, outbs);
         memset(device->outb, 0, outbs);
         device->inb_read = 0;
         device->outb_read = 0;
         device->inb_write = 0;
         device->outb_write = 0;
         device->read_subscribers = malloc(sizeof(struct serial_subscriber));
         device->write_subscribers = malloc(sizeof(struct serial_subscriber));
         device->read_subscribers->next = 0;
         device->write_subscribers->next = 0;
         device->read_subscribers->handler = 0;
         device->write_subscribers->handler = 0;

         if (device->inb && device->outb) {
            enable_serial_int(device);
            device->valid = 1;
            printf("Serial port %x initialized successfully\n", device->port);
         }
      }
   }
   hook_interrupt(SERIAL_OF_IRQ, SERIAL_OF_HANDLER, 1);
   initialized = 1;
}

void serial_read_event_add(int port, void (*handler)(char c, int port)) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   if (!device) return;

   add_subscriber(device, SERIAL_READ, handler);
}

void serial_read_event_remove(int port, void (*handler)(char c, int port)) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   if (!device) return;

   remove_subscriber(device, SERIAL_READ, handler);
}

void serial_write_event_add(int port, void (*handler)(char c, int port)) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   if (!device) return;

   add_subscriber(device, SERIAL_WRITE, handler);
}

void serial_write_event_remove(int port, void (*handler)(char c, int port)) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   if (!device) return;

   remove_subscriber(device, SERIAL_WRITE, handler);
}

void serial_flush(int port) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   if (!device) return;

   char c = 0;
   do {
      c = read_outb(device);
      write_serial(port, c);
   } while (c != 0);
}

void serial_write(int port, char c) {
   if (!initialized) return;

   struct serial_device* device = get_serial(port);
   if (!device) return;

   write_outb(device, c);
}

char serial_read(int port) {
   if (!initialized) return 0;

   struct serial_device* device = get_serial(port);
   if (!device) return 0;

   return read_inb(device);
}
