#ifndef _ARP_H
#define _ARP_H

#define ARP_REQUEST 0x1
#define ARP_REPLY 0x2

#include <stdint.h>

struct arp {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t *sha;
    uint8_t *spa;
    uint8_t *tha;
    uint8_t *tpa;
} __attribute__((packed));

void init_arp(struct arp* arp, uint8_t* mac, const char* sips, const char* tips);
void parse_arp(struct arp* arp, uint8_t* packet, uint64_t size);
void destroy_arp(struct arp* arp);
uint16_t size_arp(struct arp* arp);
void ethertype_arp(uint8_t *type);
void data_arp(struct arp* arp, uint8_t* data);
int get_arp_payload_size(void * buffer);
void dump_arp(struct arp* arp);
#endif