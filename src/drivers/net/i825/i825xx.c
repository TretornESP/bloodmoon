/*
 * Copyright (c) 2011 Joshua Cornutt <jcornutt@randomaccessit.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*

#include "stddef.h"
#include "stdint.h"
#include "stdarg.h"
#include "string.h"
#include "stdlib.h"
#include "portio.h"
#include "stdio.h"
#include "drivers/pic.h"
#include "drivers/pci.h"
#include "drivers/net/i825xx.h"
#include "drivers/net/global.h"

#define NUM_RX_DESCRIPTORS	768
#define NUM_TX_DESCRIPTORS	768

#define CTRL_FD				(1 << 0)
#define CTRL_ASDE			(1 << 5)
#define CTRL_SLU			(1 << 6)

// RX and TX descriptor structures
typedef struct __attribute__((packed)) i825xx_rx_desc_s 
{
	volatile uint64_t	address;
	
	volatile uint16_t	length;
	volatile uint16_t	checksum;
	volatile uint8_t	status;
	volatile uint8_t	errors;
	volatile uint16_t	special;
} i825xx_rx_desc_t;

typedef struct __attribute__((packed)) i825xx_tx_desc_s 
{
	volatile uint64_t	address;
	
	volatile uint16_t	length;
	volatile uint8_t	cso;
	volatile uint8_t	cmd;
	volatile uint8_t	sta;
	volatile uint8_t	css;
	volatile uint16_t	special;
} i825xx_tx_desc_t;

// Device-specific structure
typedef struct i825xx_device_s
{
	uintptr_t	mmio_address;
	uint32_t	io_address;
	
	volatile uint8_t		*rx_desc_base;
	volatile i825xx_rx_desc_t	*rx_desc[NUM_RX_DESCRIPTORS];	// receive descriptor buffer
	volatile uint16_t	rx_tail;
	
	volatile uint8_t		*tx_desc_base;
	volatile i825xx_tx_desc_t	*tx_desc[NUM_TX_DESCRIPTORS];	// transmit descriptor buffer
	volatile uint16_t	tx_tail;
	
	uint16_t	(* eeprom_read)( struct i825xx_device_s *, uint8_t );
} i825xx_device_t;

static uint16_t net_i825xx_eeprom_read( i825xx_device_t *dev, uint8_t ADDR )
{
	uint16_t DATA = 0;
	uint32_t tmp = 0;
	mmio_write32( i825xx_REG_EERD, (1) | ((uint32_t)(ADDR) << 8) );
	
	while( !((tmp = mmio_read32(i825xx_REG_EERD)) & (1 << 4)) )
		wait_microsecond(1);
		
	DATA = (uint16_t)((tmp >> 16) & 0xFFFF);
	return DATA;
}
*/

/****************************************************
 ** Management Data Input/Output (MDI/O) Interface **
 ** "This interface allows upper-layer devices to  **
 **  monitor and control the state of the PHY."    **
 ****************************************************/
/*
#define MDIC_PHYADD			(1 << 21)
#define MDIC_OP_WRITE		(1 << 26)
#define MDIC_OP_READ		(2 << 26)
#define MDIC_R				(1 << 28)
#define MDIC_I				(1 << 29)
#define MDIC_E				(1 << 30)

static uint16_t net_i825xx_phy_read( i825xx_device_t *dev, int MDIC_REGADD )
{
	uint16_t MDIC_DATA = 0;

	mmio_write32( i825xx_REG_MDIC, (((MDIC_REGADD & 0x1F) << 16) | 
									MDIC_PHYADD | MDIC_I | MDIC_OP_READ) );
									
	while( !(mmio_read32(i825xx_REG_MDIC) & (MDIC_R | MDIC_E)) )
	{
		wait_microsecond(1);
	}
	
	if( mmio_read32(i825xx_REG_MDIC) & MDIC_E )
	{
		printf("i825xx: MDI READ ERROR\n");
		return -1;
	}
	
	MDIC_DATA = (uint16_t)(mmio_read32(i825xx_REG_MDIC) & 0xFFFF);
	return MDIC_DATA;
}

static void net_i825xx_phy_write( i825xx_device_t *dev, int MDIC_REGADD, uint16_t MDIC_DATA )
{
	mmio_write32( i825xx_REG_MDIC, ((MDIC_DATA & 0xFFFF) | ((MDIC_REGADD & 0x1F) << 16) | 
									MDIC_PHYADD | MDIC_I | MDIC_OP_WRITE) );
									
	while( !(mmio_read32(i825xx_REG_MDIC) & (MDIC_R | MDIC_E)) )
	{
		wait_microsecond(1);
	}
	
	if( mmio_read32(i825xx_REG_MDIC) & MDIC_E )
	{
		printf("i825xx: MDI WRITE ERROR\n");
		return;
	}
}

#define RCTL_EN				(1 << 1)
#define RCTL_SBP			(1 << 2)
#define RCTL_UPE			(1 << 3)
#define RCTL_MPE			(1 << 4)
#define RCTL_LPE			(1 << 5)
#define RDMTS_HALF			(0 << 8)
#define RDMTS_QUARTER		(1 << 8)
#define RDMTS_EIGHTH		(2 << 8)
#define RCTL_BAM			(1 << 15)
#define RCTL_BSIZE_256		(3 << 16)
#define RCTL_BSIZE_512		(2 << 16)
#define RCTL_BSIZE_1024		(1 << 16)
#define RCTL_BSIZE_2048		(0 << 16)
#define RCTL_BSIZE_4096		((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192		((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384	((1 << 16) | (1 << 25))
#define RCTL_SECRC			(1 << 26)

static void net_i825xx_rx_enable( net_device_t *netdev )
{
	i825xx_device_t *dev = (i825xx_device_t *)netdev->lldev;
	mmio_write32( i825xx_REG_RCTL, mmio_read32(i825xx_REG_RCTL) | (RCTL_EN) );
}

static int net_i825xx_rx_init( net_device_t *netdev )
{
	int i;
	i825xx_device_t *dev = (i825xx_device_t *)netdev->lldev;
	
	// unaligned base address
	uint64_t tmpbase = (uint64_t)malloc((sizeof(i825xx_rx_desc_t) * NUM_RX_DESCRIPTORS) + 16);
	// aligned base address
	dev->rx_desc_base = (tmpbase % 16) ? (uint8_t *)((tmpbase) + 16 - (tmpbase % 16)) : (uint8_t *)tmpbase;
	
	for( i = 0; i < NUM_RX_DESCRIPTORS; i++ )
	{
		dev->rx_desc[i] = (i825xx_rx_desc_t *)(dev->rx_desc_base + (i * 16));
		dev->rx_desc[i]->address = (uint64_t)malloc(8192+16); // packet buffer size (8K)
		dev->rx_desc[i]->status = 0;
	}
	
	// setup the receive descriptor ring buffer (TODO: >32-bits may be broken in this code)
	mmio_write32( i825xx_REG_RDBAH, (uint32_t)((uint64_t)dev->rx_desc_base >> 32) );
	mmio_write32( i825xx_REG_RDBAL, (uint32_t)((uint64_t)dev->rx_desc_base & 0xFFFFFFFF) );
	printf("i825xx[%u]: RDBAH/RDBAL = %p8x:%p8x\n", netdev->hwid, mmio_read32(i825xx_REG_RDBAH), mmio_read32(i825xx_REG_RDBAL));
	
	// receive buffer length; NUM_RX_DESCRIPTORS 16-byte descriptors
	mmio_write32( i825xx_REG_RDLEN, (uint32_t)(NUM_RX_DESCRIPTORS * 16) );
	
	// setup head and tail pointers
	mmio_write32( i825xx_REG_RDH, 0 );
	mmio_write32( i825xx_REG_RDT, NUM_RX_DESCRIPTORS );
	dev->rx_tail = 0;
	
	// set the receieve control register (promisc ON, 8K pkt size)
	mmio_write32( i825xx_REG_RCTL, (RCTL_SBP | RCTL_UPE | RCTL_MPE | RDMTS_HALF | RCTL_SECRC | 
									RCTL_LPE | RCTL_BAM | RCTL_BSIZE_8192) );
	return 0;
}

#define TCTL_EN				(1 << 1)
#define TCTL_PSP			(1 << 3)

static int net_i825xx_tx_init( net_device_t *netdev )
{
	int i;
	i825xx_device_t *dev = (i825xx_device_t *)netdev->lldev;
	
	uint64_t tmpbase = (uint64_t)malloc((sizeof(i825xx_tx_desc_t) * NUM_TX_DESCRIPTORS) + 16);
	dev->tx_desc_base = (tmpbase % 16) ? (uint8_t *)((tmpbase) + 16 - (tmpbase % 16)) : (uint8_t *)tmpbase;
	
	for( i = 0; i < NUM_TX_DESCRIPTORS; i++ )
	{
		dev->tx_desc[i] = (i825xx_tx_desc_t *)(dev->tx_desc_base + (i * 16));
		dev->tx_desc[i]->address = 0;
		dev->tx_desc[i]->cmd = 0;
	}
	
	// setup the transmit descriptor ring buffer
	mmio_write32( i825xx_REG_TDBAH, (uint32_t)((uint64_t)dev->tx_desc_base >> 32) );
	mmio_write32( i825xx_REG_TDBAL, (uint32_t)((uint64_t)dev->tx_desc_base & 0xFFFFFFFF) );
	printf("i825xx[%u]: TDBAH/TDBAL = %p8x:%p8x\n", netdev->hwid, mmio_read32(i825xx_REG_TDBAH), mmio_read32(i825xx_REG_TDBAL));
	
	// transmit buffer length; NUM_TX_DESCRIPTORS 16-byte descriptors
	mmio_write32( i825xx_REG_TDLEN, (uint32_t)(NUM_TX_DESCRIPTORS * 16) );
	
	// setup head and tail pointers
	mmio_write32( i825xx_REG_TDH, 0 );
	mmio_write32( i825xx_REG_TDT, NUM_TX_DESCRIPTORS );
	dev->tx_tail = 0;
	
	// set the transmit control register (padshortpackets)
	mmio_write32( i825xx_REG_TCTL, (TCTL_EN | TCTL_PSP) );
	return 0;
}

static void net_i825xx_tx_poll( net_device_t *netdev, void *pkt, uint16_t length )
{
	i825xx_device_t *dev = (i825xx_device_t *)netdev->lldev;
	//printf("i825xx[%u]: transmitting packet (%u bytes) [h=%u, t=%u]\n", netdev->hwid, length, mmio_read32(i825xx_REG_TDH), dev->tx_tail);
	
	dev->tx_desc[dev->tx_tail]->address = (uint64_t)pkt;
	dev->tx_desc[dev->tx_tail]->length = length;
	dev->tx_desc[dev->tx_tail]->cmd = ((1 << 3) | (3));
	
	// update the tail so the hardware knows it's ready
	int oldtail = dev->tx_tail;
	dev->tx_tail = (dev->tx_tail + 1) % NUM_TX_DESCRIPTORS;
	mmio_write32(i825xx_REG_TDT, dev->tx_tail);
	
	while( !(dev->tx_desc[oldtail]->sta & 0xF) )
	{
		wait_microsecond(1);
	}
	
	netdev->tx_count++;
	//printf("i825xx[%u]: transmit status = 0x%p1x\n", netdev->hwid, (dev->tx_desc[oldtail]->sta & 0xF));
	
}

// This can be used stand-alone or from an interrupt handler
static void net_i825xx_rx_poll( net_device_t *netdev )
{
	i825xx_device_t *dev = (i825xx_device_t *)netdev->lldev;

	while( (dev->rx_desc[dev->rx_tail]->status & (1 << 0)) )
	{
		// raw packet and packet length (excluding CRC)
		uint8_t *pkt = (void *)dev->rx_desc[dev->rx_tail]->address;
		uint16_t pktlen = dev->rx_desc[dev->rx_tail]->length;
		bool dropflag = false;

		if( pktlen < 60 )
		{
			printf("net[u]: short packet (%u bytes)\n", netdev->hwid, pktlen);
			dropflag = true;
		}

		// while not technically an error, there is no support in this driver
		if( !(dev->rx_desc[dev->rx_tail]->status & (1 << 1)) )
		{
			printf("net[%u]: no EOP set! (len=%u, 0x%x 0x%x 0x%x)\n", 
				netdev->hwid, pktlen, pkt[0], pkt[1], pkt[2]);
			dropflag = true;
		}
		
		if( dev->rx_desc[dev->rx_tail]->errors )
		{
			printf("net[%u]: rx errors (0x%x)\n", netdev->hwid, dev->rx_desc[dev->rx_tail]->errors);
			dropflag = true;
		}

		if( !dropflag )
		{
			// send the packet to higher layers for parsing
		}
		
		// update RX counts and the tail pointer
		netdev->rx_count++;
		dev->rx_desc[dev->rx_tail]->status = (uint16_t)(0);
			
		dev->rx_tail = (dev->rx_tail + 1) % NUM_RX_DESCRIPTORS;
		
		// write the tail to the device
		mmio_write32(i825xx_REG_RDT, dev->rx_tail);
	}
}

static void i825xx_interrupt_handler( CPUContext *ctx )
{
	net_device_t *netdev = find_net_device_by_irq( ctx->vector );
	i825xx_device_t *dev = (i825xx_device_t *)netdev->lldev;
	
	if( netdev == NULL )
	{
		printf("i825xx: UNKNOWN IRQ / INVALID DEVICE\n");
		return;
	}
	
	// read the pending interrupt status
	uint32_t icr = mmio_read32(dev->mmio_address + 0xC0);

	// tx success stuff
	icr &= ~(3);

	// LINK STATUS CHANGE
	if( icr & (1 << 2) )
	{
		icr &= ~(1 << 2);
		mmio_write32(i825xx_REG_CTRL, (mmio_read32(i825xx_REG_CTRL) | CTRL_SLU));		
		
		// debugging info (probably not necessary in most scenarios)
		printf("i825xx: Link Status Change, STATUS = 0x%p8x\n", mmio_read32(i825xx_REG_STATUS));
		printf("i825xx: PHY CONTROL = 0x%p4x\n", net_i825xx_phy_read(dev, i825xx_PHYREG_PCTRL));
		printf("i825xx: PHY STATUS = 0x%p4x\n", net_i825xx_phy_read(dev, i825xx_PHYREG_PSTATUS));
		printf("i825xx: PHY PSSTAT = 0x%p4x\n", net_i825xx_phy_read(dev, i825xx_PHYREG_PSSTAT));
		printf("i825xx: PHY ANA = 0x%p4x\n", net_i825xx_phy_read(dev, 4));
		printf("i825xx: PHY ANE = 0x%p4x\n", net_i825xx_phy_read(dev, 6));
		printf("i825xx: PHY GCON = 0x%p4x\n", net_i825xx_phy_read(dev, 9));
		printf("i825xx: PHY GSTATUS = 0x%p4x\n", net_i825xx_phy_read(dev, 10));
		printf("i825xx: PHY EPSTATUS = 0x%p4x\n", net_i825xx_phy_read(dev, 15));
	}
	
	// RX underrun / min threshold
	if( icr & (1 << 6) || icr & (1 << 4) )
	{
		icr &= ~((1 << 6) | (1 << 4));
		printf(" underrun (rx_head = %u, rx_tail = %u)\n", mmio_read32(i825xx_REG_RDH), dev->rx_tail);
		
		volatile int i;
		for(i = 0; i < NUM_RX_DESCRIPTORS; i++)
		{
			if( dev->rx_desc[i]->status )
				printf(" pending descriptor (i=%u, status=0x%p4x)\n", i, dev->rx_desc[i]->status );
		}
		
		netdev->rx_poll(netdev);
	}

	// packet is pending
	if( icr & (1 << 7) )
	{
		icr &= ~(1 << 7);
		netdev->rx_poll(netdev);
	}
	
	if( icr )
		printf("i825xx[%u]: unhandled interrupt #%u received! (0x%p8x)\n", netdev->hwid, ctx->vector, icr);
	
	// clearing the pending interrupts
	mmio_read32(dev->mmio_address + 0xC0);
}
*/
/***************************************************
 ** Intel 825xx-series Chipset Driver Entry Point **
 ***************************************************/
/*
int net_i825xx_init( pci_device_t *pcidev )
{
	net_device_t *netdev = (net_device_t *)malloc(sizeof(net_device_t));
	i825xx_device_t *dev = (i825xx_device_t *)malloc(sizeof(i825xx_device_t));
	
	netdev->pci = pcidev;			// pci structure
	netdev->lldev = (void *)dev;	// i825xx-specific structure
	netdev->rx_count = netdev->tx_count = 0;
	netdev->packets_dropped = 0;
	
	// read the PCI BAR for the device's MMIO space
	pci_bar_t *bar = pci_resolve_bar(pcidev, 0);
	
	// is this BAR valid?
	if( bar == NULL )
		return -1;
	
	// the spec says BAR0 is MMIO
	if( bar->type != PCI_BAR_MMIO )
		return -1;
	
	// MMIO should not have this limitation for this chipset
	if( bar->locate_below_1meg )
		return -1;
	
	// TODO (REMOVE) : DIAGNOSTIC INFO
	printf("BAR0:\n");
	printf(" type: %s\n", bar->type ? "I/O" : "Memory Mapped I/O");
	printf(" prefetchable: %s\n", bar->prefetchable ? "true" : "false");
	printf(" is64bit: %s\n", bar->is64bit ? "true" : "false");
	printf(" locate below 1MB: %s\n", bar->locate_below_1meg ? "true" : "false");
	printf(" address: 0x%p16x\n", bar->address);
	
	// register the MMIO address
	dev->mmio_address = (uintptr_t)bar->address;

	// setup the function pointers
	netdev->rx_init = net_i825xx_rx_init;
	netdev->tx_init = net_i825xx_tx_init;
	dev->eeprom_read = net_i825xx_eeprom_read;
	netdev->rx_enable = net_i825xx_rx_enable;
	netdev->rx_poll = net_i825xx_rx_poll;
	netdev->tx_poll = net_i825xx_tx_poll;
	
	// register the interrupt handler
	netdev->interrupt_line = netdev->pci->configspace->interrupt_line;
	AddInterruptServiceRoutine(netdev->interrupt_line, i825xx_interrupt_handler);
		
	// globally register the devices
	register_global_network_device( netdev );
	printf("i825xx[%u]: interrupt line = %u (isr%u)\n", netdev->hwid, netdev->interrupt_line, 32+netdev->interrupt_line);
	
	// get the MAC address
	uint16_t mac16 = dev->eeprom_read(dev, 0);
	netdev->mac_address[0] = (mac16 & 0xFF);
	netdev->mac_address[1] = (mac16 >> 8) & 0xFF;
	mac16 = dev->eeprom_read(dev, 1);
	netdev->mac_address[2] = (mac16 & 0xFF);
	netdev->mac_address[3] = (mac16 >> 8) & 0xFF;
	mac16 = dev->eeprom_read(dev, 2);
	netdev->mac_address[4] = (mac16 & 0xFF);
	netdev->mac_address[5] = (mac16 >> 8) & 0xFF;
	
	// set the LINK UP
	mmio_write32(i825xx_REG_CTRL, (mmio_read32(i825xx_REG_CTRL) | CTRL_SLU));		
	
	// Initialize the Multicase Table Array
	printf("i825xx[%u]: Initializing the Multicast Table Array...\n", netdev->hwid);
	int i;
	for( i = 0; i < 128; i++ )
		mmio_write32(i825xx_REG_MTA + (i * 4), 0);
		
	// enable all interrupts (and clear existing pending ones)
	mmio_write32( i825xx_REG_IMS, 0x1F6DC );
	mmio_read32(dev->mmio_address + 0xC0);
	
	// start the RX/TX processes
	netdev->rx_init(netdev);
	netdev->tx_init(netdev);
	
	printf("i825xx[%u]: configuration complete\n", netdev->hwid);
	
	return 0;
}
*/