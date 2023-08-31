#ifndef _NETSTACK_H
#define _NETSTACK_H

#define BIG_ENDIAN 0
#define LITTLE_ENDIAN 1

#define BYTE_ORDER LITTLE_ENDIAN

#define AF_INET 0x2
#define AF_INET6 0x1C

#include <stdint.h>

#define NET_RX_QUEUE 0
#define NET_TX_QUEUE 1

struct packet {
    uint16_t len;
    uint8_t * data;
    struct packet * next;
};

struct arp_cache {
    uint8_t hlen;
    uint8_t plen;
    uint8_t *ip;
    uint8_t *mac;
    struct arp_cache *next;
};

struct nic {
    char name[32];
    char device_name[32];
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t subnet[4];
    uint8_t gateway[4];
    uint8_t dns[4];
    uint8_t dhcp;

    struct arp_cache * arp_cache;

    uint8_t status; 
    
    struct packet * rx_queue;
    struct packet * tx_queue;
};

void dump_nics();
void dump_packets(struct nic * n);

struct nic* get_nic(const char* name);
struct nic* create_nic(const char* name, const char * devname);
void change_nic_status(struct nic * n, uint8_t status);
void dump_arp_cache(struct nic * n);
void get_mac_for_ip(struct nic * n, uint8_t * ip, uint8_t * mac);
void clear_arp_cache(struct nic * n);
struct packet* create_rx_packet(struct nic * n, uint16_t len);
struct packet* create_tx_packet(struct nic * n, uint16_t len);
struct packet* peek_rx(struct nic * n);
struct packet* get_packet_head(struct nic * n, uint8_t queue);
void flush_tx(struct nic * n);
void tx_n(struct nic * n, uint16_t num);

//htons and ntohs
#if BYTE_ORDER == BIG_ENDIAN

#define HTONS(n) (n)
#define NTOHS(n) (n)
#define HTONL(n) (n)
#define NTOHL(n) (n)

#else

#define HTONS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
#define NTOHS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))

#define HTONL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
                  ((((unsigned long)(n) & 0xFF00)) << 8) | \
                  ((((unsigned long)(n) & 0xFF0000)) >> 8) | \
                  ((((unsigned long)(n) & 0xFF000000)) >> 24))

#define NTOHL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
                  ((((unsigned long)(n) & 0xFF00)) << 8) | \
                  ((((unsigned long)(n) & 0xFF0000)) >> 8) | \
                  ((((unsigned long)(n) & 0xFF000000)) >> 24))
#endif

#define htons(n) HTONS(n)
#define ntohs(n) NTOHS(n)

#define htonl(n) HTONL(n)
#define ntohl(n) NTOHL(n)

//inet_pton
int inet_pton(int af, const char *src, void *dst);
void dump_arp_cache(struct nic * n);
void get_mac_for_ip(struct nic * n, uint8_t * ip, uint8_t * mac);
void clear_arp_cache(struct nic * n);

void force_network_worker();
#endif // _NETSTACK_H