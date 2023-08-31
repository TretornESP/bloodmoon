#include "crc.h"
#include "eth.h"

#include "../util/printf.h"
#include "../memory/heap.h"
#include "../util/string.h"

void init_eth(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *type, uint8_t *length) {
    uint16_t llength = *length;
    eth->data = malloc(llength);
    printf("%p\n", eth->data);
    if (llength < 46) {
        llength = 46;
        free(eth->data);
        eth->data = malloc(llength);
        length[0] = llength;
        length[1] = llength >> 8;
    }
    if (llength > 1500) {
        llength = 1500;
        free(eth->data);
        eth->data = malloc(llength);
        printf("WARNING: fragmentation isn't supported yet\n");
        length[0] = llength;
        length[1] = llength >> 8;
    }
    memcpy(eth->data, data, llength);
    #ifdef INCLUDE_PREAMBLE

    for (int i = 0; i < 7; i++) {
        eth->preamble[i] = 0x55;
    }
    eth->sfd = 0x7e;
    #endif


    memcpy(eth->length, type, 2);
    memcpy(eth->sa, sa, 6);
    memcpy(eth->da, da, 6);

    uint8_t* packet = malloc(llength+26);

    #ifdef INCLUDE_PREAMBLE

    memcpy(packet, &eth->preamble, 7);
    memcpy(packet+7, &eth->sfd, 1);

    #endif
    memcpy(packet, &eth->da, 6);
    memcpy(packet+6, &eth->sa, 6);
    memcpy(packet+12, &eth->length, 2);
    memcpy(packet+14, eth->data, llength);
    free(eth->data);
    eth->crc = crc32_byte(packet, llength+14);
    memcpy(packet+llength+14, &eth->crc, 4);

    printf("SENT CRC: %x\n", eth->crc);
    eth->data = packet; 
    //Esto es una mierda, deberia seguir la interfaz de data_arp que te saca el paquete del header
    //Pero como entonces habria que esperar para calcular el crc me da profunda pereza y lo hago asi
 
}

uint8_t get_eth_version(struct eth *eth) {
    uint16_t ethsize = eth->length[0] + eth->length[1]*256;
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

void dump_eth(struct eth *eth) {
    printf("ETH:\n");
    printf("  DA: %x:%x:%x:%x:%x:%x\n", eth->da[0], eth->da[1], eth->da[2], eth->da[3], eth->da[4], eth->da[5]);
    printf("  SA: %x:%x:%x:%x:%x:%x\n", eth->sa[0], eth->sa[1], eth->sa[2], eth->sa[3], eth->sa[4], eth->sa[5]);
    printf("  LENGTH: %x\n", eth->length[0] + eth->length[1]*256);
    printf("  CRC: %x\n", eth->crc);
    printf("  DATA: ");
    for (int i = 0; i < eth->length[0] + eth->length[1]*256; i++) {
        printf("%x ", eth->data[i]);
    }
    printf("\n");
}

uint8_t parse_eth(struct eth *eth, uint8_t* data) {

    #ifdef INCLUDE_PREAMBLE
    memcpy(eth, data, 22);
    #else
    memcpy(eth, data, 14);
    #endif
    
    if (eth->length[0] + eth->length[1]*256 < 46) {
        printf("Invalid length\n");
        return 0;
    }
    eth->data = malloc(eth->length[0] + eth->length[1]*256);
    if (eth->length[0] + eth->length[1]*256 > 1500) {
        printf("WARNING: fragmentation isn't supported yet\n");
        printf("Size: %d\n", eth->length[0] + eth->length[1]*256);
        return 0;
    }
    if (eth->data == 0) {
        printf("Malloc failed\n");
        return 0;
    }
    #ifdef INCLUDE_PREAMBLE
    memcpy(eth->data, data+22, eth->length[0] + eth->length[1]*256);
    eth->crc = *(uint32_t*)(data+eth->length[0] + eth->length[1]*256 + 22);
    #else
    memcpy(eth->data, data+14, eth->length[0] + eth->length[1]*256);
    eth->crc = *(uint32_t*)(data+eth->length[0] + eth->length[1]*256 + 14);
    #endif

    return 1;
}

//Checks if ethernet crc is valid
int check_crc(struct eth *eth) {
    if (eth->crc == 0) {
        return 1;
    }
    uint8_t *packet = malloc(eth->length[0] + eth->length[1]*256 + 26);
    memcpy(packet, &eth->da, 6);
    memcpy(packet+6, &eth->sa, 6);
    memcpy(packet+12, &eth->length, 2);
    memcpy(packet+14, eth->data, eth->length[0] + eth->length[1]*256);
    uint32_t crc = crc32_byte(packet, eth->length[0] + eth->length[1]*256);
    free(packet);
    printf("CRC: %x\n", crc);
    printf("STORED: %x\n", eth->crc);
    return crc == eth->crc;
}

void get_eth_data(struct eth *eth, uint8_t *data, uint16_t size) {
    if (check_crc(eth)) {
        printf("Length: %d\n", size);
        memcpy(data, eth->data, size);
    } else {
        printf("CRC error\n");
    }
}

void destroy_eth(struct eth *eth) {
    free(eth->data);
}

uint64_t size_eth(struct eth *eth) {
    return 18;
}