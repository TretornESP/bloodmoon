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
 
#ifndef C_OS_DEVICE_NET_E1000_H
#define C_OS_DEVICE_NET_E1000_H

#include <stdint.h>
#include "../../../devices/pci/pci.h"

#define NUM_RX_DESC 32
#define NUM_TX_DESC 8

struct e1000_rx_desc {
	uint64_t addr;
	uint16_t length;
	uint16_t checksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
} __attribute__((packed));

struct e1000_tx_desc {
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));

struct e1000 {
    uint8_t bar_type;
	uint16_t io_base;
	uint8_t *mem_base;
	uint8_t  mac[6];
	uint8_t *rx_free;
	uint8_t *tx_free;
	struct e1000_rx_desc *rx_descs[NUM_RX_DESC];
	struct e1000_tx_desc *tx_descs[NUM_TX_DESC];
	int is_e;	
	uint16_t rx_cur;
	uint16_t tx_cur;
	void (*inject_packet)(uint8_t *, uint8_t * p_data, uint16_t p_len);
	void (*inject_status_change)(uint8_t *, uint8_t);
	struct pci_device_header_0 *pciConfigHeader;
};

void handle_nic_int();
uint8_t e1000_get_status(void *e);
void e1000_inject_status_change_callback(struct e1000 *e, void (*)(uint8_t *, uint8_t));
void e1000_set_injection_callback(struct e1000 *e, void (*)(uint8_t *, uint8_t * p_data, uint16_t p_len));
struct e1000 *e1000_init(struct pci_device_header_0 * _pciConfigHeader, uint32_t base_address);
uint8_t * getMacAddress(void *e);                         // Returns the MAC address
int sendPacket(void*e, uint8_t* p_data, uint16_t p_len);
#endif