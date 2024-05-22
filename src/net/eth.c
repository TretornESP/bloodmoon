//#include "crc.h"
#include "eth.h"

#include "../util/printf.h"
#include "../memory/heap.h"
#include "../util/string.h"

//uint8_t eth_check_crc(struct eth* eth) {
//    if (eth == 0) return 0;
//    uint32_t crc = crc32_byte(eth->data, eth->datalen);
//    if (eth->crc != crc) {
//        printf("CRC error --- stored: %x calculated: %x\n", eth->crc, crc);
//        return 0;
//    }
//    return 1;
//}

uint16_t eth_get_type(struct eth *eth) {
    if (eth == 0) return 0;
    return (eth->type[0] << 8) | eth->type[1];
}

void eth_create(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *type, uint16_t length) {
    eth->data = kmalloc(length);
    memcpy(eth->data, data, length);
    
    if (length < 46) {
        length = 46;
    }
    if (length > 1500) {
        length = 1500;
        printf("WARNING: fragmentation isn't supported yet\n");
    }
    eth->datalen = length;
    memcpy(eth->sa, sa, 6);
    memcpy(eth->da, da, 6);
    memcpy(eth->type, type, 2);
    //eth->crc = crc32_byte(data, length);

    #ifdef INCLUDE_PREAMBLE
    for (int i = 0; i < 7; i++) {
        eth->preamble[i] = 0x55;
    }

    eth->sfd = 0x7e;

    #endif
}

uint16_t eth_to_packet(struct eth* eth, uint8_t * buffer, uint16_t max_size) {
    if (eth == 0) return 0;
    //if (eth_check_crc(eth) == 0) return 0;
    memset(buffer, 0, max_size);
    
    #ifdef INCLUDE_PREAMBLE
    if (max_size < eth->datalen + 22) {
        printf("WARNING: buffer is too small (%d) for the packet (%d)\n", max_size, eth->datalen + 22);
        return 0;
    }
    memcpy(buffer, &eth->preamble, 7);
    memcpy(buffer+7, &eth->sfd, 1);
    memcpy(buffer+8, &eth->da, 6);
    memcpy(buffer+14, &eth->sa, 6);
    memcpy(buffer+20, &eth->type, 2);
    memcpy(buffer+22, eth->data, eth->datalen);
    //memcpy(buffer+eth->datalen+22, &eth->crc, 4);
    return eth->datalen + 22;
    #else
    if (max_size < eth->datalen + 14) {
        printf("WARNING: buffer is too small (%d) for the packet (%d)\n", max_size, eth->datalen + 14);
        return 0;
    }
    memcpy(buffer, &eth->da, 6);
    memcpy(buffer+6, &eth->sa, 6);
    memcpy(buffer+12, &eth->type, 2);
    memcpy(buffer+14, eth->data, eth->datalen);
    //memcpy(buffer+eth->datalen+14, &eth->crc, 4);
    return eth->datalen + 14;
    #endif
}

uint8_t eth_from_packet(struct eth *eth, uint8_t* data, uint64_t size) {
    if (eth == 0 || data == 0) return 0;

    #ifdef INCLUDE_PREAMBLE
    if (size < 22) return 0;
    memcpy(&eth->preamble, data, 7);
    memcpy(&eth->sfd, data+7, 1);
    memcpy(&eth->da, data+8, 6);
    memcpy(&eth->sa, data+14, 6);
    memcpy(&eth->type, data+20, 2);

    eth->datalen = size - 22;
    eth->data = kmalloc(eth->datalen);
    memcpy(eth->data, data+22, eth->datalen);
    
    #else
    if (size < 14) return 0;
    memcpy(&eth->da, data, 6);
    memcpy(&eth->sa, data+6, 6);
    memcpy(&eth->type, data+12, 2);

    eth->datalen = size - 14;
    eth->data = kmalloc(eth->datalen);
    memcpy(eth->data, data+14, eth->datalen);
    #endif
    
    return 1;
}

void eth_destroy(struct eth *eth) {
    if (eth == 0) return;
    if (eth->data) kfree(eth->data);
}

uint64_t eth_get_data_size(struct eth *eth) {
    if (eth == 0) return 0;
    return eth->datalen;
}

uint8_t eth_get_version(uint8_t* data, uint64_t size) {
    struct eth eth;
    if (eth_from_packet(&eth, data, size)) {
        uint16_t ethsize = eth_get_type(&eth);
        if (ethsize < 46) {
            return ETH_INVALID;
        }
        if (ethsize > 1500 && ethsize < 1536) {
            return ETH_INVALID;
        }
        if (ethsize > 1536) {
            return ETH_II;
        }
        return ETH_802_3;
    }
    return ETH_INVALID;
}

uint8_t* eth_get_data(struct eth *eth, uint16_t *size) {
    if (eth == 0) return 0;
    *size = eth->datalen;
    return eth->data;
}

uint64_t eth_get_packet_size(struct eth *eth) {
    if (eth == 0) return 0;
    #ifdef INCLUDE_PREAMBLE
    return eth->datalen + 22;
    #endif
    return eth->datalen + 14;
}

void eth_dump(struct eth *eth) {
    printf("ETH version: ");
    switch (eth_get_version(eth->data, eth->datalen)) {
        case ETH_802_3:
            printf("802.3\n");
            break;
        case ETH_II:
            printf("II\n");
            break;
        default:
            printf("INVALID\n");
            break;
    }
    printf("  DA: %x:%x:%x:%x:%x:%x\n", eth->da[0], eth->da[1], eth->da[2], eth->da[3], eth->da[4], eth->da[5]);
    printf("  SA: %x:%x:%x:%x:%x:%x\n", eth->sa[0], eth->sa[1], eth->sa[2], eth->sa[3], eth->sa[4], eth->sa[5]);
    printf("  DATALENGTH: %x\n", eth->datalen);
    printf("  TYPE: %x\n", (eth->type[0] << 8) | eth->type[1]);
//    printf("  CRC: %x\n", eth->crc);
    printf("  DATA: ");
    for (int i = 0; i < eth->datalen; i++) {
        printf("%02x ", eth->data[i]);
        if (i % 16 == 0 && i != 0) {
            printf("\n        ");
        }
    }
    printf("\n");
}