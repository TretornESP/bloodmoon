#include "netstack.h"

#include "../../util/printf.h"
#include "../../util/string.h"
#include "../../memory/heap.h"
#include "../../util/ctype.h"
#include "../../util/dbgprinter.h"

#include "../../net/eth.h"
#include "../../net/arp.h"
#include "../../net/crc.h"
#include "../devices.h"

struct nic * nics[16] = {0};

void dump_arp_cache(struct nic* nic) {
    struct arp_cache * c = nic->arp_cache;
    while (c != 0) {
        printf("  %d.%d.%d.%d -> %02x:%02x:%02x:%02x:%02x:%02x\n", c->ip[0], c->ip[1], c->ip[2], c->ip[3], c->mac[0], c->mac[1], c->mac[2], c->mac[3], c->mac[4], c->mac[5]);
        c = c->next;
    }
}

void print_nic(struct nic* nic) {
    printf("NIC %s:\n DEV: %s", nic->name, nic->device_name);
    printf("  MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", nic->mac[0], nic->mac[1], nic->mac[2], nic->mac[3], nic->mac[4], nic->mac[5]);
    printf("  Status: %d\n", nic->status);
    printf("  RX Queue: %p\n", nic->rx_queue);
    printf("  TX Queue: %p\n", nic->tx_queue);
    printf("Config:\n");
    printf("  IP: %d.%d.%d.%d\n", nic->ip[0], nic->ip[1], nic->ip[2], nic->ip[3]);
    printf("  Subnet: %d.%d.%d.%d\n", nic->subnet[0], nic->subnet[1], nic->subnet[2], nic->subnet[3]);
    printf("  Gateway: %d.%d.%d.%d\n", nic->gateway[0], nic->gateway[1], nic->gateway[2], nic->gateway[3]);
    printf("  DNS: %d.%d.%d.%d\n", nic->dns[0], nic->dns[1], nic->dns[2], nic->dns[3]);
    printf("  DHCP: %d\n", nic->dhcp);
    printf("Arp Cache:\n");
    dump_arp_cache(nic);
}

void add_arp_cache_entry(struct nic* nic, uint8_t * ip, uint8_t * mac) {
    struct arp_cache * c = nic->arp_cache;
    while (c != 0) {
        if (memcmp(c->ip, ip, 4) == 0) {
            memcpy(c->mac, mac, 6);
            return;
        }
        c = c->next;
    }
    struct arp_cache * new = (struct arp_cache*)malloc(sizeof(struct arp_cache));
    memcpy(new->ip, ip, 4);
    memcpy(new->mac, mac, 6);
    new->next = nic->arp_cache;
    nic->arp_cache = new;
}

void network_worker() {
    for (int i = 0; i < 16; i++) {
        struct nic* c = nics[i];

        if (c != 0) {
            struct packet * p = get_packet_head(c, NET_RX_QUEUE);
            if (p != 0 && p->len >= 14) {
                struct eth eth;
                if (!parse_eth(&eth, p->data))
                    continue;
                dump_eth(&eth);

                int data_size = eth.length[0] * 256 + eth.length[1];
                switch (get_eth_version(&eth)) {
                    case ETH_802_3: {
                        printf("802.3 packet not supported\n");
                        break;
                    }
                    case ETH_II: {
                        void * eth_raw_data = malloc(ETH_MAX_FRAME_SIZE);
                        get_eth_data(&eth, eth_raw_data, ETH_MAX_FRAME_SIZE);

                        switch(data_size) {
                            case ETHERT_ARP: {
                                data_size = get_arp_payload_size(eth_raw_data);
                                if (data_size == -1) {
                                    printf("Invalid ARP packet\n");
                                    break;
                                }

                                void * eth_data = malloc(data_size);
                                get_eth_data(&eth, eth_data, data_size);

                                struct arp arp;
                                parse_arp(&arp, eth_data, data_size);
                                dump_arp(&arp);

                                if (arp.oper == htons(ARP_REQUEST)) {
                                    printf("ARP request detected!\n");
                                    if (memcmp(arp.tpa, c->ip, 4) == 0) {
                                        printf("ARP request for me!\n");
                                        struct packet * reply = create_tx_packet(c, 14+data_size);
                                        struct eth * reply_eth = (struct eth*)reply->data;
                                        memcpy(reply_eth->da, arp.sha, 6);
                                        memcpy(reply_eth->sa, c->mac, 6);
                                        reply_eth->length[0] = data_size & 0xFF;
                                        reply_eth->length[1] = (data_size >> 8) & 0xFF;
                                        memcpy(reply_eth->data, eth_data, data_size);
                                        reply_eth->crc = 0;
                                        reply_eth->crc = crc32_byte(reply->data, 14+data_size);
                                        tx_n(c, 1);
                                    }
                                } else if (arp.oper == htons(ARP_REPLY)) {
                                    printf("ARP reply detected!\n");
                                    if (memcmp(arp.tpa, c->ip, 4) == 0) {
                                        printf("ARP reply for me!\n");
                                        add_arp_cache_entry(c, arp.spa, arp.sha);
                                    }
                                } else {
                                    printf("Unknown ARP operation %d\n", arp.oper);
                                }

                                
                                break;
                            }
                            case ETHERT_IPV4: {
                                printf("IPv4 frame detected!\n");
                                break;
                            }
                        }

                        free(eth_raw_data);
                        break;
                    }
                    case ETH_INVALID: {
                        printf("Invalid packet\n");
                        break;
                    }
                    default: {
                        printf("Unknown packet arrived!\n");
                        break;
                    }
                    
                }

                struct packet * next = p->next;
                free(p->data);
                free(p);
                c->rx_queue = next;
            }
            flush_tx(c);
        }
    }
}

void force_network_worker() {
    network_worker();
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
    printf("Packet arrived to %02x:%02x:%02x:%02x:%02x:%02x len: %d\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len);
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

void nic_configure(struct nic* nic, uint8_t * ip, uint8_t * subnet, uint8_t * gw, uint8_t * dns, uint8_t dhcp) {
    memcpy(nic->ip, ip, 4);
    memcpy(nic->subnet, subnet, 4);
    memcpy(nic->gateway, gw, 4);
    memcpy(nic->dns, dns, 4);
    nic->dhcp = dhcp;
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
            nics[i]->ip[0] = 0;
            nics[i]->ip[1] = 0;
            nics[i]->ip[2] = 0;
            nics[i]->ip[3] = 0;
            nics[i]->subnet[0] = 0;
            nics[i]->subnet[1] = 0;
            nics[i]->subnet[2] = 0;
            nics[i]->subnet[3] = 0;
            nics[i]->gateway[0] = 0;
            nics[i]->gateway[1] = 0;
            nics[i]->gateway[2] = 0;
            nics[i]->gateway[3] = 0;
            nics[i]->dns[0] = 0;
            nics[i]->dns[1] = 0;
            nics[i]->dns[2] = 0;
            nics[i]->dns[3] = 0;
            nics[i]->dhcp = 0;
            nics[i]->arp_cache = 0x0;
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

static int hexval(unsigned c)
{
	if (c-'0'<10) return c-'0';
	c |= 32;
	if (c-'a'<6) return c-'a'+10;
	return -1;
}

int inet_pton(int af, const char *restrict s, void *restrict a0)
{
	uint16_t ip[8];
	unsigned char *a = a0;
	int i, j, v, d, brk=-1, need_v4=0;

	if (af==AF_INET) {
		for (i=0; i<4; i++) {
			for (v=j=0; j<3 && isdigit(s[j]); j++)
				v = 10*v + s[j]-'0';
			if (j==0 || (j>1 && s[0]=='0') || v>255) return 0;
			a[i] = v;
			if (s[j]==0 && i==3) return 1;
			if (s[j]!='.') return 0;
			s += j+1;
		}
		return 0;
	} else if (af!=AF_INET6) {
		return -1;
	}

	if (*s==':' && *++s!=':') return 0;

	for (i=0; ; i++) {
		if (s[0]==':' && brk<0) {
			brk=i;
			ip[i&7]=0;
			if (!*++s) break;
			if (i==7) return 0;
			continue;
		}
		for (v=j=0; j<4 && (d=hexval(s[j]))>=0; j++)
			v=16*v+d;
		if (j==0) return 0;
		ip[i&7] = v;
		if (!s[j] && (brk>=0 || i==7)) break;
		if (i==7) return 0;
		if (s[j]!=':') {
			if (s[j]!='.' || (i<6 && brk<0)) return 0;
			need_v4=1;
			i++;
			ip[i&7]=0;
			break;
		}
		s += j+1;
	}
	if (brk>=0) {
		memmove(ip+brk+7-i, ip+brk, 2*(i+1-brk));
		for (j=0; j<7-i; j++) ip[brk+j] = 0;
	}
	for (j=0; j<8; j++) {
		*a++ = ip[j]>>8;
		*a++ = ip[j];
	}
	if (need_v4 && inet_pton(AF_INET, (void *)s, a-4) <= 0) return 0;
	return 1;
}

void get_mac_for_ip(struct nic * n, uint8_t * ip, uint8_t * mac) {
    struct arp_cache * c = n->arp_cache;
    while (c != 0) {
        if (memcmp(c->ip, ip, 4) == 0) {
            memcpy(mac, c->mac, 6);
            return;
        }
        c = c->next;
    }
    //printf("IP %d.%d.%d.%d not found in ARP cache\n", ip[0], ip[1], ip[2], ip[3]);
    mac[0] = 0;
    mac[1] = 0;
    mac[2] = 0;
    mac[3] = 0;
    mac[4] = 0;
    mac[5] = 0;

}
void clear_arp_cache(struct nic * n) {
    struct arp_cache * c = n->arp_cache;
    while (c != 0) {
        struct arp_cache * next = c->next;
        free(c);
        c = next;
    }
    n->arp_cache = 0;
}