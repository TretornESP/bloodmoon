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
    uint8_t type[2];
    uint16_t datalen;
    uint8_t *data;
    //uint32_t crc;
};

//uint8_t eth_check_crc(struct eth* eth);

void eth_create(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *type, uint16_t length);
uint16_t eth_to_packet(struct eth* eth, uint8_t * buffer, uint16_t max_size);
uint8_t eth_from_packet(struct eth *eth, uint8_t* data, uint64_t size);

uint64_t eth_get_packet_size(struct eth *eth);
uint64_t eth_get_data_size(struct eth *eth);
uint8_t eth_get_version(uint8_t* data, uint64_t size);
uint16_t eth_get_type(struct eth *eth);
uint8_t* eth_get_data(struct eth *eth, uint16_t *size);
void eth_dump(struct eth *eth);

#endif