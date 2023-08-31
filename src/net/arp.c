#include "arp.h"

#include "../util/printf.h"
#include "../util/string.h"
#include "../memory/heap.h"
#include "../dev/net/netstack.h"

void init_arp(struct arp* arp, uint8_t* mac, const char* sips, const char* tips) {

    uint8_t tip[4];
    inet_pton(AF_INET, tips, tip);

    uint8_t sip[4];
    inet_pton(AF_INET, sips, sip);

    uint8_t hlen = 6;
    uint8_t plen = 4;
    
    arp->htype = htons(1);
    arp->ptype = htons(0x0800);
    arp->hlen = hlen;
    arp->plen = plen;
    arp->oper = htons(ARP_REQUEST);
    arp->sha = (uint8_t*)malloc(hlen);
    arp->spa = (uint8_t*)malloc(plen);
    arp->tha = (uint8_t*)malloc(hlen);
    arp->tpa = (uint8_t*)malloc(plen);

    memcpy(arp->sha, mac, hlen);
    memcpy(arp->spa, sip, plen);
    memcpy(arp->tha, (uint8_t*)"\x00\x00\x00\x00\x00\x00", hlen);
    memcpy(arp->tpa, tip, plen);
}

void parse_arp(struct arp* arp, uint8_t* packet, uint64_t size) {
        
        arp->htype = ntohs(*(uint16_t*)(packet + 0));
        arp->ptype = ntohs(*(uint16_t*)(packet + 2));
        arp->hlen = *(packet + 4);
        arp->plen = *(packet + 5);
        arp->oper = ntohs(*(uint16_t*)(packet + 6));
        arp->sha = (uint8_t*)(packet + 8);
        arp->spa = (uint8_t*)(packet + 8 + arp->hlen);
        arp->tha = (uint8_t*)(packet + 8 + arp->hlen + arp->plen);
        arp->tpa = (uint8_t*)(packet + 8 + arp->hlen + arp->plen + arp->hlen);
}

void get_reply_ip(struct arp* arp, uint8_t* ip) {
    if (arp->oper == htons(ARP_REPLY))
        memcpy(ip, arp->spa, 4);
}

void destroy_arp(struct arp* arp) {
    //free(arp->sha); //XQ NO FUNCIONAS
    free(arp->spa);
    free(arp->tha);
    free(arp->tpa);
}

void size_arp(struct arp* arp, uint8_t* size) {
    uint16_t len = 8+2*(arp->hlen+arp->plen);
    size[0] = len;
    size[1] = len >> 8;
}

void ethertype_arp(uint8_t *type) {
    type[0] = 0x08;
    type[1] = 0x06;
}

uint8_t get_arp_payload_size(void * buffer) {
    uint8_t * packet = (uint8_t *) buffer;
    uint8_t hlen = packet[4];
    uint8_t plen = packet[5];
    if ((hlen + plen) > 120) {
        return -1;
    }
    return 8 + 2*(hlen+plen);
}

void data_arp(struct arp* arp, uint8_t* data) {
    memcpy(data, &arp->htype, 2);
    memcpy(data+2, &arp->ptype, 2);
    memcpy(data+4, &arp->hlen, 1);
    memcpy(data+5, &arp->plen, 1);
    memcpy(data+6, &arp->oper, 2);
    memcpy(data+8, arp->sha, arp->hlen);
    memcpy(data+8+arp->hlen, arp->spa, arp->plen);
    memcpy(data+8+arp->hlen+arp->plen, arp->tha, arp->hlen);
    memcpy(data+8+arp->hlen+arp->plen+arp->hlen, arp->tpa, arp->plen);
    printf("Copied %d bytes\n", 8+arp->hlen+arp->plen+arp->hlen+arp->plen);
}

void dump_arp(struct arp* arp) {
    printf("ARP:\n");
    printf("  HTYPE: %x\n", arp->htype);
    printf("  PTYPE: %x\n", arp->ptype);
    printf("  HLEN: %x\n", arp->hlen);
    printf("  PLEN: %x\n", arp->plen);
    printf("  OPER: %x\n", arp->oper);
    printf("  SHA: %x:%x:%x:%x:%x:%x\n", arp->sha[0], arp->sha[1], arp->sha[2], arp->sha[3], arp->sha[4], arp->sha[5]);
    printf("  SPA: %d.%d.%d.%d\n", arp->spa[0], arp->spa[1], arp->spa[2], arp->spa[3]);
    printf("  THA: %x:%x:%x:%x:%x:%x\n", arp->tha[0], arp->tha[1], arp->tha[2], arp->tha[3], arp->tha[4], arp->tha[5]);
    printf("  TPA: %d.%d.%d.%d\n", arp->tpa[0], arp->tpa[1], arp->tpa[2], arp->tpa[3]);
}