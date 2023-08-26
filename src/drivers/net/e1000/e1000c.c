/*	ChickenOS Intel Pro/1000 Network Card Driver
* Copyright (c) 2011-2016 Bryce Lanham.  All rights reserved.
* 
* Developed by: Bryce Lanham
*               Snell-Hitchcock Instute of Technology
*               http://www.brycelanham.com/
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal with the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimers.
*   2. Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimers in the
*      documentation and/or other materials provided with the distribution.
*   3. Neither the names of Snell-Hitchcock Institute of Technology, nor the
*      names of its contributors may be used to endorse or promote products
*      derived from this Software without specific prior written permission.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* WITH THE SOFTWARE.
 */

#include <stdint.h>
#include "e1000c.h"
#include "../../../io/io.h"
#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"
#include "../../../util/string.h"
#include "../../../dev/pci/pci.h"
#include "../../../memory/heap.h"
#include "../../../memory/paging.h"

#define GET_PAGE_SIZE(x) (((x) % 4096) ? (((x) >>12) + 1) : ((x) >> 12))

struct e1000 *e1000_global;
#define E1000_DEV 0x100E

#define REG_CTRL 		0x0000
#define REG_EEPROM 		0x0014
#define REG_IMASK 		0x00D0
#define REG_RCTRL 		0x0100
#define REG_RXDESCLO  	0x2800
#define REG_RXDESCHI  	0x2804
#define REG_RXDESCLEN 	0x2808
#define REG_RXDESCHEAD 	0x2810
#define REG_RXDESCTAIL 	0x2818

#define REG_TCTRL 		0x0400
#define REG_TXDESCLO  	0x3800
#define REG_TXDESCHI  	0x3804
#define REG_TXDESCLEN 	0x3808
#define REG_TXDESCHEAD 	0x3810
#define REG_TXDESCTAIL 	0x3818

#define RCTRL_EN 	0x00000002
#define RCTRL_SBP 	0x00000004
#define RCTRL_UPE 	0x00000008
#define RCTRL_MPE 	0x00000010
#define RCTRL_8192 	0x00030000

#define ECTRL_FD   	0x01//FULL DUPLEX
#define ECTRL_ASDE 	0x20//auto speed enable
#define ECTRL_SLU  	0x40//set link up

static inline void mmio_write32(void* address, uint32_t value) {
    *(volatile uint32_t*) address = value;
}

static inline uint32_t mmio_read32(void* address) {
    return *(volatile uint32_t*) address;
}

void WriteRegister(struct e1000* e, uint16_t p_address, uint32_t p_value);
uint32_t ReadRegister(struct e1000* e, uint16_t p_address);

void WriteRegister(struct e1000* e, uint16_t Reg, uint32_t Value) {
    if(e->bar_type == PCI_BAR_TYPE_IO) {
        outl(e->io_base, Reg);
        outl(e->io_base + sizeof(uint32_t), Value);
    } else {
        mmio_write32((void*) ((uint64_t)e->mem_base + Reg), Value);
    }
}

uint32_t ReadRegister(struct e1000* e, uint16_t Reg) {
    if(e->bar_type == PCI_BAR_TYPE_IO) {
        outl(e->io_base, Reg);
        return inl(e->io_base + sizeof(uint32_t));
    } else {
        return mmio_read32((void*) ((uint64_t)e->mem_base + Reg));
    }
}
/*
void e1000_outl(struct e1000 *e, uint16_t addr, uint32_t val)
{
	outl(e->io_base, addr);
	outl(e->io_base + 4, val);
}
void e1000_outb(struct e1000 *e, uint16_t addr, uint32_t val)
{
	outl(e->io_base, addr);
	outb(e->io_base + 4, val);
}
uint32_t e1000_inl(struct e1000 *e, uint16_t addr)
{
	outl(e->io_base, addr);
	return inl(e->io_base + 4);
}*/
uint32_t e1000_eeprom_read(struct e1000 *e, uint8_t addr)
{
	uint32_t val = 0;
	uint32_t test;
	if(e->is_e == 0)
		test = addr << 8;
	else
		test = addr << 2;

	WriteRegister(e, REG_EEPROM, test | 0x1);
	if(e->is_e == 0)
		while(!((val = ReadRegister(e, REG_EEPROM)) & (1<<4)))
		;//	printf("is %i val %x\n",e->is_e,val);
	else
		while(!((val = ReadRegister(e, REG_EEPROM)) & (1<<1)))
		;//	printf("is %i val %x\n",e->is_e,val);
	val >>= 16;
	return val;
}

uint8_t * getMacAddress(void *e) {
    return (uint8_t *)((struct e1000*)(e))->mac;
}

void e1000_getmac(struct e1000 *e, char *mac)
{
	uint32_t temp;
	temp = e1000_eeprom_read(e, 0);
	mac[0] = temp &0xff;
	mac[1] = temp >> 8;
	temp = e1000_eeprom_read(e, 1);
	mac[2] = temp &0xff;
	mac[3] = temp >> 8;
	temp = e1000_eeprom_read(e, 2);
	mac[4] = temp &0xff;
	mac[5] = temp >> 8;
}
void e1000_linkup(struct e1000 *e)
{
	uint32_t val;
	val = ReadRegister(e, REG_CTRL);
	WriteRegister(e, REG_CTRL, val | ECTRL_SLU);

	if (e->inject_status_change != 0) {
		e->inject_status_change(e->mac, e1000_get_status(e));
	}
}
void e1000_interrupt_enable(struct e1000 *e)
{
	WriteRegister(e, REG_IMASK ,0x1F6DC);
	WriteRegister(e, REG_IMASK ,0xff & ~4);
	ReadRegister(e, 0xc0);
}

size_t e1000_send(struct e1000 *e, uint8_t *_buf, size_t length)
{
    memcpy((void*)e->tx_descs[e->tx_cur]->addr, _buf, length);
	e->tx_descs[e->tx_cur]->length = length;
	e->tx_descs[e->tx_cur]->cmd = ((1 << 3) | 3);
	uint8_t old_cur = e->tx_cur;
	e->tx_cur = (e->tx_cur + 1) % NUM_TX_DESC;
	WriteRegister(e, REG_TXDESCTAIL, e->tx_cur);
	while(!(e->tx_descs[old_cur]->status & 0xff));
    memset((void*)e->tx_descs[old_cur]->addr, 0, 8192);
	return 0;
}

int sendPacket(void* addr, uint8_t * p_data, uint16_t p_len) {
    return e1000_send((struct e1000 *)addr, (uint8_t *)p_data, p_len);
}

void e1000_received(struct e1000 *e)
{
	uint16_t old_cur;
	while((e->rx_descs[e->rx_cur]->status & 0x1))
	{
		uint8_t *buf = (uint8_t*)e->rx_descs[e->rx_cur]->addr;
		uint16_t len = e->rx_descs[e->rx_cur]->length;

        if (e->inject_packet != 0) {
			e->inject_packet(e->mac, buf, len);
		}  

		e->rx_descs[e->rx_cur]->status = 0;
		old_cur = e->rx_cur;
		e->rx_cur = (e->rx_cur + 1) % NUM_RX_DESC;
		WriteRegister(e, REG_RXDESCTAIL, old_cur ) ;
	}
}

void e1000_set_injection_callback(struct e1000 *e, void (*inject_packet)(uint8_t*, uint8_t * p_data, uint16_t p_len)) {
	e->inject_packet = inject_packet;
}

void e1000_eeprom_gettype(struct e1000 *e)
{
	uint32_t val = 0;
	WriteRegister(e, REG_EEPROM, 0x1);

	for(int i = 0; i < 1000; i++)///while( val & 0x2 || val & 0x10)
	{
		val = ReadRegister(e, REG_EEPROM);
		if(val & 0x10)
			e->is_e = 0;
		else
			e->is_e = 1;
	}
}

void e1000_inject_status_change_callback(struct e1000 * nic, void (*data)(uint8_t*, uint8_t)) {
	nic->inject_status_change = data;
}

uint8_t e1000_get_status(void *nic) {
	return ReadRegister((struct e1000*)nic, 0x0008);
}

void handle_nic_int()
{
	struct e1000 *e = e1000_global;
	uint32_t status = ReadRegister(e, 0xc0);

	if(status & 0x04)
	{
		e1000_linkup(e);
	}

	if(status & 0x10)
	{
		printf("threshold good\n");
	}

	if(status & 0x80)
	{
		e1000_received(e);
	}

//	printf("e1000 IRQ %x\n",status);
//	e1000_inl(e,0xc0);
}

void e1000_rxinit(struct e1000 *e)
{
	uint8_t* ptr;
	struct e1000_rx_desc *descs;
    ptr = (uint8_t*)request_current_pages_identity(GET_PAGE_SIZE(sizeof(struct e1000_rx_desc)*NUM_RX_DESC + 16));
    memset(ptr, 0, sizeof(struct e1000_rx_desc)*NUM_RX_DESC);
	e->rx_free = (uint8_t *)ptr;
	if((uint64_t)ptr % 16 != 0)
		ptr = (ptr + 16) - ((uint64_t)ptr % 16);
	descs = (struct e1000_rx_desc *)ptr;
	for(int i = 0; i < NUM_RX_DESC; i++)
	{
		e->rx_descs[i] = (struct e1000_rx_desc *)((uint8_t*)descs + i*16);
		//XXX: Don't use kmalloc here
		e->rx_descs[i]->addr = (uint64_t)(uint8_t*)(request_current_pages_identity(GET_PAGE_SIZE(8192 + 16)));
        memset((void*)e->rx_descs[i]->addr, 0, 8192);
		e->rx_descs[i]->status = 0;
	}

	//give the card the pointer to the descriptors
    WriteRegister(e, REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(e, REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
    WriteRegister(e, REG_RXDESCLO, (uint64_t)ptr);
    WriteRegister(e, REG_RXDESCHI, 0);

	//now setup total length of descriptors
	WriteRegister(e, REG_RXDESCLEN, NUM_RX_DESC * 16);

	//setup numbers
	WriteRegister(e, REG_RXDESCHEAD, 0);
	WriteRegister(e, REG_RXDESCTAIL, NUM_RX_DESC);
	e->rx_cur = 0;

	//enable receiving
	//uint32_t flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (1 << 4) | (1 << 3) | ( 1 << 2);
uint32_t flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (0 << 4) | (0 << 3) | ( 1 << 2);
//	uint32_t flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (1 << 4) | ( 1 << 2);
//	WriteRegister(REG_RCTRL, RCTRL_8192 | RCTRL_MPE);
	WriteRegister(e, REG_RCTRL, flags);//RCTRL_8192 | RCTRL_MPE | RCTRL_UPE |RCTRL_EN);
}
void e1000_txinit(struct e1000 *e)
{
	uint8_t* ptr;
	struct e1000_tx_desc *descs;
    ptr = (uint8_t *)(request_current_pages_identity(GET_PAGE_SIZE(sizeof(struct e1000_tx_desc)*NUM_TX_DESC + 16)));
    memset(ptr, 0, sizeof(struct e1000_tx_desc)*NUM_TX_DESC);
    e->tx_free = (uint8_t *)ptr;
	if((uint64_t)ptr % 16 != 0)
		ptr = (ptr + 16) - ((uint64_t)ptr % 16);
	descs = (struct e1000_tx_desc *)ptr;
	for(int i = 0; i < NUM_TX_DESC; i++)
	{
		e->tx_descs[i] = (struct e1000_tx_desc *)((uint8_t*)descs + i*16);
		e->tx_descs[i]->addr = (uint64_t)(uint8_t *)(request_current_pages_identity(GET_PAGE_SIZE(8192 + 16)));
        memset((void*)e->tx_descs[i]->addr, 0, 8192);
		e->tx_descs[i]->cmd = 0;
	}

	//give the card the pointer to the descriptors
    WriteRegister(e, REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(e, REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

	//now setup total length of descriptors
	WriteRegister(e, REG_TXDESCLEN, NUM_TX_DESC * 16);

	//setup numbers
	WriteRegister(e, REG_TXDESCHEAD, 0);
	WriteRegister(e, REG_TXDESCTAIL, NUM_TX_DESC);
	e->tx_cur = 0;

	WriteRegister(e, REG_TCTRL, (1 << 1) | (1 << 3));
}

void e1000_start(struct e1000 *e)
{
	//set link up
	e1000_linkup(e);
	//have to clear out the multicast filter, otherwise shit breaks
	for(int i = 0; i < 0x80; i++)
		WriteRegister(e, 0x5200 + i*4, 0);

	e1000_interrupt_enable(e);

	e1000_rxinit(e);
	e1000_txinit(e);
}

void print_mac(char *mac)
{

	printf("MAC ADDR: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",
		mac[0]&0xff,mac[1]&0xff,mac[2]&0xff,
		mac[3]&0xff,mac[4]&0xff,mac[5]&0xff);

}

struct e1000 *e1000_init(struct pci_device_header_0 * _pciConfigHeader, uint32_t base_address)
{
	struct e1000 *e = (struct e1000 *)malloc(sizeof(*e));
	e1000_global = e;
	e->inject_packet = 0x0;
	e->inject_status_change = 0x0;

    uint32_t bar_address = (uint32_t)(uint64_t)get_bar_address(_pciConfigHeader, 0x0);
    e->bar_type = get_bar_type(bar_address);

    printf("[NIC] Bar0 Type: %x interrupt line: %x\n", e->bar_type, _pciConfigHeader->interrupt_line);

    if (e->bar_type == PCI_BAR_TYPE_IO) {
        printf("[NIC] IO Base: %x\n", bar_address);
        e->io_base = bar_address;
    } else {
        printf("[NIC] Memory Base: %x\n", bar_address);
        uint64_t bar_size = get_bar_size((void*)(uint64_t)bar_address, base_address);
        map_current_memory_size((void*)(uint64_t)bar_address, (void*)(uint64_t)bar_address, bar_size);
        e->mem_base = (uint8_t*)(uint64_t) bar_address;
    }

	e1000_eeprom_gettype(e);
	e1000_getmac(e, (char *)e->mac);
	print_mac((char *)&e->mac);

	e1000_start(e);
	uint32_t flags = ReadRegister(e, REG_RCTRL);
	WriteRegister(e, REG_RCTRL, flags | RCTRL_EN);//RCTRL_8192 | RCTRL_MPE | RCTRL_UPE |RCTRL_EN);
    
	return e;
}