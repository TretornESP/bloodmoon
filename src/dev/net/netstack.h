#ifndef _NETSTACK_H
#define _NETSTACK_H

#include <stdint.h>

#define NET_RX_QUEUE 0
#define NET_TX_QUEUE 1

struct packet {
    uint16_t len;
    uint8_t * data;
    struct packet * next;
};

struct nic {
    char name[32];
    char device_name[32];
    uint8_t mac[6];
    uint8_t status; 
    struct packet * rx_queue;
    struct packet * tx_queue;
};

void dump_nics();
void dump_packets(struct nic * n);

struct nic* get_nic(const char* name);
struct nic* create_nic(const char* name, const char * devname);
void change_nic_status(struct nic * n, uint8_t status);
struct packet* create_rx_packet(struct nic * n, uint16_t len);
struct packet* create_tx_packet(struct nic * n, uint16_t len);
struct packet* peek_rx(struct nic * n);
struct packet* get_packet_head(struct nic * n, uint8_t queue);
void flush_tx(struct nic * n);
void tx_n(struct nic * n, uint16_t num);

#endif // _NETSTACK_H