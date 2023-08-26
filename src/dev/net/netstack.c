#include "netstack.h"

#include "../../util/printf.h"
#include "../../util/string.h"
#include "../../memory/heap.h"
#include "../devices.h"

struct nic * nics[16] = {0};

void print_nic(struct nic* nic) {
    printf("NIC %s:\n DEV: %s", nic->name, nic->device_name);
    printf("  MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", nic->mac[0], nic->mac[1], nic->mac[2], nic->mac[3], nic->mac[4], nic->mac[5]);
    printf("  Status: %d\n", nic->status);
    printf("  RX Queue: %p\n", nic->rx_queue);
    printf("  TX Queue: %p\n", nic->tx_queue);
}

void dump_nics() {
    printf("NICs:\n");
    for (int i = 0; i < 16; i++) {
        if (nics[i] != 0) {
            print_nic(nics[i]);
        }
    }
}

void print_packet_queue(struct packet * head) {
    struct packet * p = head;
    while (p != 0) {
        printf("  Packet %p: len=%d, data=%p, next=%p\n", p, p->len, p->data, p->next);
        p = p->next;
    }
}

void dump_packets(struct nic * n) {
    printf("NIC %s:\n", n->name);
    printf("RX Queue:\n");
    print_packet_queue(n->rx_queue);
    printf("TX Queue:\n");
    print_packet_queue(n->tx_queue);
}

struct nic* get_nic(const char* name) {
    for (int i = 0; i < 16; i++) {
        if (nics[i] != 0) {
            if (strcmp(nics[i]->name, name) == 0) {
                return nics[i];
            }
        }
    }
    return 0;
}

void inject_nic_status_callback(uint8_t *mac, uint8_t status) {
    for (int i = 0; i < 16; i++) {
        if (nics[i] != 0) {
            if (memcmp(nics[i]->mac, mac, 6) == 0) {
                nics[i]->status = status;
                return;
            }
        }
    }
}

void inject_packet_callback(uint8_t * mac, uint8_t * buffer, uint16_t len) {
    //printf("Packet arrived to %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    for (int i = 0; i < 16; i++) {
        if (nics[i] != 0) {
            if (memcmp(nics[i]->mac, mac, 6) == 0) {
                struct packet * p = create_rx_packet(nics[i], len);
                memcpy(p->data, buffer, len);
                return;
            }
        }
    }
}

struct nic* create_nic(const char* name, const char * device_name) {
    for (int i = 0; i < 16; i++) {
        if (nics[i] == 0) {
            nics[i] = (struct nic*)malloc(sizeof(struct nic));
            strcpy(nics[i]->name, name);
            strcpy(nics[i]->device_name, device_name);
            uint8_t mac[6];
            device_ioctl(device_name, 0x1001, mac); //Get mac
            memcpy(nics[i]->mac, mac, 6);
            nics[i]->status = device_ioctl(device_name, 0x1004, 0x0);
            nics[i]->rx_queue = 0;
            nics[i]->tx_queue = 0;

            device_ioctl(device_name, 0x1002, inject_packet_callback); //Set injection callback
            device_ioctl(device_name, 0x1003, inject_nic_status_callback); //Set status change callback
            return nics[i];
        }
    }

    printf("Failed to create NIC %s\n", name);
    return 0;
}

void change_nic_status(struct nic * n, uint8_t status) {
    n->status = status;
}

struct packet* create_rx_packet(struct nic * n, uint16_t len) {
    struct packet * p = (struct packet*)malloc(sizeof(struct packet));
    p->len = len;
    p->data = (uint8_t*)malloc(len);
    p->next = 0;

    if (n->rx_queue == 0) {
        n->rx_queue = p;
    } else {
        struct packet * last = n->rx_queue;
        while (last->next != 0) {
            last = last->next;
        }
        last->next = p;
    }

    return p;
}

struct packet* create_tx_packet(struct nic * n, uint16_t len) {
    struct packet * p = (struct packet*)malloc(sizeof(struct packet));
    p->len = len;
    p->data = (uint8_t*)malloc(len);
    p->next = 0;

    if (n->tx_queue == 0) {
        n->tx_queue = p;
    } else {
        struct packet * last = n->tx_queue;
        while (last->next != 0) {
            last = last->next;
        }
        last->next = p;
    }

    return p;
}

struct packet* peek_rx(struct nic * n) {
    struct packet * p = n->rx_queue;
    if (p == 0) return 0;
    while (p->next != 0) {
        p = p->next;
    }
    return p;
}

struct packet* get_packet_head(struct nic * n, uint8_t queue) {
    if (queue == NET_RX_QUEUE) {
        return n->rx_queue;
    } else if (queue == NET_TX_QUEUE){
        return n->tx_queue;
    }
    return 0;
}

void flush_tx(struct nic * n) {
    struct packet * p = n->tx_queue;
    while (p != 0) {
        //Send packet
        device_write(n->device_name, p->len, 0x0, p->data);
        struct packet * next = p->next;
        free(p->data);
        free(p);
        p = next;
    }
    n->tx_queue = 0;
}

//Send and free num packets
void tx_n(struct nic * n, uint16_t num) {
    struct packet * p = n->tx_queue;
    for (int i = 0; i < num; i++) {
        //Send packet
        device_write(n->device_name, p->len, 0x0, p->data);
        struct packet * next = p->next;
        free(p->data);
        free(p);
        p = next;
    }
    n->tx_queue = p;
}