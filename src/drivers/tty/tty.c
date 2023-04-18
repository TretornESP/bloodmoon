//This is perhaps the world's worst TTY implementation
//Don't use it for anything serious, or even for fun

#include "tty.h"
#include "../../util/string.h"
#include "../../util/printf.h"
#include "../../dev/devices.h"
#include "../../memory/heap.h"

//TODO: This dependency is awful
#include "../serial/serial_dd.h"

struct tty ttys[32] = {0};

struct tty* get_tty(int index) {
    if (index < 0 || index >= 32) return 0;
    return &ttys[index];
}

void tty_run_subscribers(struct tty* tty, uint8_t event) {
    if (tty == 0 || !tty->valid) return;
    if (tty->subscribers == 0) return;

    struct tty_subscriber * subscriber = tty->subscribers;

    while(subscriber->next) {
        subscriber = subscriber->next;
        subscriber->handler((void*)tty, event);
    }
}

void _tty_flush(struct tty* device) {
    if (device == 0) return;
    if (device->outb == 0) return;

    if (device->outb_read == device->outb_write) return;

    //This avoids using read_outb, not sure if it's a good idea
    int len = device->outb_write - device->outb_read;
    if (len < 0) {
        len = device->outb_size - device->outb_read;
    }

    device_write(device->device, len, 0, (uint8_t*)(device->outb + device->outb_read));
    device->outb_read += len;
    if (device->outb_read >= device->outb_size) {
        device->outb_read = 0;
    }
    device_ioctl(device->device, SERIAL_FLUSH_TX, 0);
}

void tty_write_inb(struct tty* device, char c) {
    if (device == 0) return;
    if (device->inb == 0) return;

    device->inb[device->inb_write] = c;
    device->inb_write++;
    if (device->inb_write >= device->inb_size) {
        device->inb_write = 0;
    }

    if (device->inb_write == device->inb_read) {
        device->inb_read++;
        if (device->inb_read >= device->inb_size) {
            device->inb_read = 0;
        }
    }
}

void tty_write_outb(struct tty* device, char c) {
    if (device == 0) return;
    if (device->outb == 0) return;

    device->outb[device->outb_write] = c;
    device->outb_write++;
    if (device->outb_write >= device->outb_size) {
        device->outb_write = 0;
    }

    if (device->outb_write == device->outb_read) {
        device->outb_read++;
        if (device->outb_read >= device->outb_size) {
            device->outb_read = 0;
        }
    }

    if (c == TTY_FLUSH_CHAR) {
        _tty_flush(device);
        tty_run_subscribers(device, TTY_EVENT_OUTB);
    }
}

void tty_read_inb(struct tty* device, char* c) {
    if (device == 0) return;
    if (device->inb == 0) return;

    if (device->inb_read == device->inb_write) {
        *c = 0;
        return;
    }

    *c = device->inb[device->inb_read];
    device->inb_read++;
    if (device->inb_read >= device->inb_size) {
        device->inb_read = 0;
    }
}

void tty_read_outb(struct tty* device, char* c) {
    if (device == 0) return;
    if (device->outb == 0) return;

    if (device->outb_read == device->outb_write) {
        *c = 0;
        return;
    }

    *c = device->outb[device->outb_read];
    device->outb_read++;
    if (device->outb_read >= device->outb_size) {
        device->outb_read = 0;
    }
}

void tty_read_cb(void* ttyb, char c, int port) {
    if (ttyb == 0) return;
    struct tty* tty = (struct tty*)ttyb;
    if (!is_valid_tty(tty)) return;
    line_discipline_read(tty->line_discipline, c);
}

//TODO: Maybe it is best for the line discipline to send itself and contain a field with the tty! idk...
void flush_cb(void* ttyb, char* buffer, int size) {
    struct tty* tty = (struct tty*)ttyb;
    for (int i = 0; i < size; i++) {
        tty_write_inb(tty, buffer[i]);
    }
    tty_run_subscribers(tty, TTY_EVENT_INB);
}

void echo_cb(void* ttyb, char c) {
    struct tty* tty = (struct tty*)ttyb;
    if (!tty->echo) return;
    device_write(tty->device, 1, 0, (uint8_t*)&c);
    device_ioctl(tty->device, SERIAL_FLUSH_TX, 0);
}

void _tty_add_subscriber(struct tty* tty, void (*handler)(void*, uint8_t)) {
    if (tty == 0 || !tty->valid) return;
    if (tty->subscribers == 0) return;

    struct tty_subscriber * subscriber = tty->subscribers;

    while(subscriber->next) {
        subscriber = subscriber->next;
    }

    subscriber->next = malloc(sizeof(struct tty_subscriber));
    subscriber->next->next = 0;
    subscriber->next->handler = handler;
}

void _tty_remove_subscriber(struct tty* tty, void (*handler)(void*, uint8_t)) {
    if (tty == 0 || !tty->valid) return;
    if (tty->subscribers == 0) return;

    struct tty_subscriber * subscriber = tty->subscribers;

    while(subscriber->next) {
        if (subscriber->handler == handler) {
            struct tty_subscriber * to_remove = subscriber->next;
            subscriber->next = subscriber->next->next;
            free(to_remove);
            return;
        }
        subscriber = subscriber->next;
    }
}

void tty_destroy(struct tty* tty) {
    if (tty == 0 || !tty->valid) return;
    if (tty->inb) free(tty->inb);
    if (tty->outb) free(tty->outb);
    if (tty->subscribers) free(tty->subscribers);
    if (tty->line_discipline) free(tty->line_discipline);
    tty->valid = 0;
}

int tty_init(char* device, int mode, int inbs, int outbs) {
    printf("tty_init(%s, %d, %d, %d)\n", device, mode, inbs, outbs);
    struct tty * tty = 0;   
    int index = 0;
    for (index = 0; index < 32; index++) {
        if (ttys[index].valid == 0) {
            tty = &ttys[index];
            break;
        }
    }
    if (tty == 0) return -1;
    if (mode == TTY_MODE_SERIAL || mode == TTY_MODE_PTY) tty->mode = mode;
    else return -1;
    
    tty->valid = 0;
    strncpy(tty->device, device, 32);
    tty->signal = 0;
    tty->inb = (char*)calloc(1, inbs);
    if (tty->inb == 0) return -1;
    tty->inb_size = inbs;
    tty->inb_write = 0;
    tty->inb_read = 0;
    tty->outb = (char*)calloc(1, outbs);
    if (tty->outb == 0) {
        free(tty->inb);
        return -1;
    }
    tty->outb_size = outbs;
    tty->outb_write = 0;
    tty->outb_read = 0;

    tty->subscribers = malloc(sizeof(struct tty_subscriber));
    if (tty->subscribers == 0) {
        free(tty->inb);
        free(tty->outb);
        return -1;
    }
    tty->subscribers->next = 0;
    tty->subscribers->handler = 0;

    tty->line_discipline = line_discipline_create(
        LINE_DISCIPLINE_MODE_CANONICAL,
        LINE_DISCIPLINE_MODE_ECHO_ON,
        LD_DEFAULT_TABLE,
        1024,
        tty,
        flush_cb,
        echo_cb
    );
    tty->israw = 0;
    tty->echo = 1;
    if (tty->line_discipline == 0) {
        free(tty->inb);
        free(tty->outb);
        free(tty->subscribers);
        return -1;
    }
    
    //Wow this is utterly stupid
    //All of this to avoid a single #include

    switch (mode) {
        case TTY_MODE_SERIAL: {
            struct serial_ioctl_subscriptor sync_data = {
                .parent = tty,
                .handler = tty_read_cb
            };
            device_ioctl(tty->device, SERIAL_SUBSCRIBE_READ, (void*)&sync_data);
            break;
        }
        case TTY_MODE_PTY: {
            printf("Not implemented yet!\n");
            return -1;
        }
        default:
            return -1;
    }

    tty->valid = 1;
    return index;
}

void _tty_write(struct tty* tty, char* buffer, int size) {
    if (tty == 0 || !tty->valid) return;
    if (tty->line_discipline == 0) return;

    for (int i = 0; i < size; i++) {
        char tbuffer[4];
        int translation = line_discipline_translate(tty->line_discipline, buffer[i], tbuffer);
        for (int j = 0; j < translation; j++) {
            tty_write_outb(tty, tbuffer[j]);
        }
    }
}

int is_valid_tty(struct tty* tty) {
    if (tty == 0) return 0;
    if (tty->valid == 0) return 0;
    return 1;
}

int _tty_get_size(struct tty* tty) {
    if (tty == 0 || !tty->valid) return 0;
    return tty->inb_write - tty->inb_read;
}

void _tty_modes(struct tty* tty, int mode, int value) {
    if (tty == 0 || !tty->valid) return;
    if (tty->line_discipline == 0) return;

    switch (mode) {
        case _TTY_MODE_RAW: {
            tty->israw = value;
            if (value) line_discipline_set_mode(tty->line_discipline, LINE_DISCIPLINE_MODE_RAW);
            else line_discipline_set_mode(tty->line_discipline, LINE_DISCIPLINE_MODE_CANONICAL);
            break;
        }
        case _TTY_MODE_ECHO:
            tty->echo = value;
            break;
        default:
            return;
    }
}

void _tty_read(struct tty* tty, char* buffer, int size) {
    if (tty == 0 || !tty->valid) return;
    if (tty->line_discipline == 0) return;

    for (int i = 0; i < size; i++) {
        tty_read_inb(tty, &buffer[i]);
        if (buffer[i] == 0) return;
    }
}