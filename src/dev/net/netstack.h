#ifndef _NETSTACK_H
#define _NETSTACK_H

struct packet {
    uint16_t len;
    uint8_t * data;
    struct packet * next;
}

struct nic {
    char name[32];
    uint8_t mac[6];
    void * real_device;
    uint8_t status;
    struct packet * rx_queue;
    struct packet * tx_queue;
};

void dump_nics();
void dump_packets(struct nic * n);

void create_nic();
void change_nic_status(struct nic * n, uint8_t status);
struct packet* create_rx_packet(struct nic * n, uint16_t len);
struct packet* create_tx_packet(struct nic * n, uint16_t len);

void flush_tx(struct nic * n);

void init_netstack();

#endif // _NETSTACK_H