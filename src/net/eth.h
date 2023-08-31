#ifndef _ETH_H_
#define _ETH_H_

#include <stdint.h>

#define ETHERT_IPV4    0x0800
#define ETHERT_ARP     0x0806
#define ETHERT_WOL     0x0842
#define ETHERT_RARP    0x8035
#define ETHERT_VLAN    0x8100
#define ETHERT_IPX     0x8137
#define ETHERT_IPV6    0x86DD
#define ETHERT_TEST    0x9000

#define ETH_INVALID 0x1
#define ETH_II      0x2
#define ETH_802_3   0x3

#define ETH_MAX_FRAME_SIZE 1518
//We dont support jumbo frames right now

//#define INCLUDE_PREAMBLE //Only if your nic is weird
#ifdef INCLUDE_PREAMBLE
#define ETH_PAYLOAD_SIZE_OFFSET 20
#else
#define ETH_PAYLOAD_SIZE_OFFSET 12
#endif


struct eth {
#ifdef INCLUDE_PREAMBLE
    uint8_t preamble[7];
    uint8_t sfd;
#endif
    uint8_t da[6];
    uint8_t sa[6];
    uint8_t length[2];
    uint8_t *data;
    uint32_t crc;
} __attribute__((packed));

uint8_t parse_eth(struct eth *eth, uint8_t* data);
uint8_t get_eth_version(struct eth *eth);
void get_eth_data(struct eth *eth, uint8_t *data, uint16_t size);
void init_eth(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *type, uint8_t *length);
void destroy_eth(struct eth *eth);
uint64_t size_eth(struct eth *eth);
void dump_eth(struct eth *eth);

#endif