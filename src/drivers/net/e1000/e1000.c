#include "e1000.h"
#include "../../../io/io.h"
#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"
#include "../../../util/string.h"
#include "../../../dev/pci/pci.h"
#include "../../../memory/heap.h"
#include "../../../memory/paging.h"

struct e1000 nic = {0};

void WriteRegister( uint16_t p_address, uint32_t p_value);
uint32_t ReadRegister(uint16_t p_address);

//Given a size in bytes, return the number of pages needed to store it
//Use bit shifts instead of division
#define GET_PAGE_SIZE(x) (((x) % 4096) ? (((x) >>12) + 1) : ((x) >> 12))
 
 
//bool detectEEProm(); // Return true if EEProm exist, else it returns false and set the eerprom_existsdata member
//uint32_t eepromRead( uint8_t addr); // Read 4 bytes from a specific EEProm Address
//bool readMACAddress();       // Read MAC Address
//void startLink ();           // Start up the network
//void rxinit();               // Initialize receive descriptors an buffers
//void txinit();               // Initialize transmit descriptors an buffers
//void enableInterrupt();      // Enable Interrupts
//void handleReceive();        // Handle a packet reception.
// 
//void start ();                             // perform initialization tasks and starts the driver
//void fire (InterruptContext * p_interruptContext);  // This method should be called by the interrupt handler 
//uint8_t * getMacAddress ();                         // Returns the MAC address
//int sendPacket(const void * p_data, uint16_t p_len);  // Send a packet

//MMIO

static inline void mmio_write32(void* address, uint32_t value) {
    *(volatile uint32_t*) address = value;
}

static inline uint32_t mmio_read32(void* address) {
    return *(volatile uint32_t*) address;
}

void WriteRegister(uint16_t Reg, uint32_t Value) {
    if(nic.bar_type == PCI_BAR_TYPE_IO) {
        outl(nic.io_base, Reg);
        outl(nic.io_base + sizeof(uint32_t), Value);
    } else {
        mmio_write32((void*) ((uint64_t)nic.mem_base + Reg), Value);
    }
}

uint32_t ReadRegister(uint16_t Reg) {
    if(nic.bar_type == PCI_BAR_TYPE_IO) {
        outl(nic.io_base, Reg);
        return inl(nic.io_base + sizeof(uint32_t));
    } else {
        return mmio_read32((void*) ((uint64_t)nic.mem_base + Reg));
    }
}
   

uint8_t detectEEProm()
{
    uint32_t val = 0;
    WriteRegister(REG_EEPROM, 0x1); 
 
    for(int i = 0; i < 1000 && ! nic.eerprom_exists; i++)
    {
            val = ReadRegister( REG_EEPROM);
            if(val & 0x10)
                    nic.eerprom_exists = E1000_SUCCESS;
            else
                    nic.eerprom_exists = E1000_FAILURE;
    }
    return nic.eerprom_exists;
}
 
uint32_t eepromRead( uint8_t addr)
{
	uint16_t data = 0;
	uint32_t tmp = 0;
        if ( nic.eerprom_exists)
        {
            	WriteRegister( REG_EEPROM, (1) | ((uint32_t)(addr) << 8) );
        	while( !((tmp = ReadRegister(REG_EEPROM)) & (1 << 4)) );
        }
        else
        {
            WriteRegister( REG_EEPROM, (1) | ((uint32_t)(addr) << 2) );
            while( !((tmp = ReadRegister(REG_EEPROM)) & (1 << 1)) );
        }
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}

uint8_t readMACAddress()
{
    if (nic.eerprom_exists)
    {
        uint32_t temp;
        temp = eepromRead( 0);
        nic.mac[0] = temp &0xff;
        nic.mac[1] = temp >> 8;
        temp = eepromRead( 1);
        nic.mac[2] = temp &0xff;
        nic.mac[3] = temp >> 8;
        temp = eepromRead( 2);
        nic.mac[4] = temp &0xff;
        nic.mac[5] = temp >> 8;
    }
    else
    {
        uint8_t * mem_base_mac_8 = (uint8_t *) (nic.mem_base+0x5400);
        uint32_t * mem_base_mac_32 = (uint32_t *) (nic.mem_base+0x5400);
        if ( mem_base_mac_32[0] != 0 )
        {
            for(int i = 0; i < 6; i++)
            {
                nic.mac[i] = mem_base_mac_8[i];
            }
        }
        else return E1000_FAILURE;
    }
    return E1000_SUCCESS;
}

void rxinit()
{
    uint8_t * ptr;
    struct e1000_rx_desc *descs;
 
    // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
    // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
    
    ptr = (uint8_t*)request_current_pages_identity(GET_PAGE_SIZE(sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16));
 
    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        nic.rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i*16);
        nic.rx_descs[i]->addr = (uint64_t)(uint8_t *)(request_current_pages_identity(GET_PAGE_SIZE(8192 + 16)));
        nic.rx_descs[i]->status = 0;
    }
 
    WriteRegister(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
    WriteRegister(RCVD_TAIL, E1000_NUM_RX_DESC - 1);
    WriteRegister(RCVD_HEAD, 0);
 
    WriteRegister(REG_RCV_CTRL, ReadRegister(REG_RCV_CTRL) | RCV_EN_MASK | RCV_BAM_MASK); // BSIZE is not enabled because PACKET_SIZE <= 2048

}
 
 
void txinit()
{    
    uint8_t *  ptr;
    struct e1000_tx_desc *descs;
    // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
    // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
    ptr = (uint8_t *)(request_current_pages_identity(GET_PAGE_SIZE(sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16)));
 
    descs = (struct e1000_tx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        nic.tx_descs[i] = (struct e1000_tx_desc *)((uint8_t*)descs + i*16);
        nic.tx_descs[i]->addr = (uint64_t)(uint8_t *)(request_current_pages_identity(GET_PAGE_SIZE(8192 + 16)));
        nic.tx_descs[i]->cmd = TX_CMD_EOP | TX_CMD_IFCS /* insert FCS (Frame Check Sequence) */ | TX_CMD_RS; ;
        nic.tx_descs[i]->status = TX_STATUS_DD;
    }
 
    WriteRegister(REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
    //now setup total length of descriptors
    WriteRegister(REG_TXDESCLEN, sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16);
 
    //setup numbers
    WriteRegister( REG_TXDESCHEAD, 0);
    WriteRegister( REG_TXDESCTAIL, 0);
    nic.tx_cur = 0;
    WriteRegister(REG_TST_CTRL, ReadRegister(REG_TST_CTRL) | TST_EN_MASK | TST_PSP_MASK); 
}

void printMac() {
    printf("[NIC] MAC Address: %x:%x:%x:%x:%x:%x\n", nic.mac[0], nic.mac[1], nic.mac[2], nic.mac[3], nic.mac[4], nic.mac[5]);
}

int sendPacket(const void * p_data, uint16_t p_len)
{    
    if(p_len > PACKET_SIZE) {
        return;
    }

    uint8_t idx = (uint8_t) ReadRegister(REG_TXDESCTAIL);
    
    while(!(nic.tx_descs[idx]->status & TX_STATUS_DD)){
        __asm__ volatile("":::"memory");
    }

    // copy data to the correct buffer of the correct tx descriptor
    memcpy((void*)nic.tx_descs[idx]->addr, (void*) p_data, p_len);

    nic.tx_descs[idx]->length = p_len;
    nic.tx_descs[idx]->status &= ~TX_STATUS_DD; // undone -> status = 0

    WriteRegister(REG_TXDESCTAIL, (idx + 1) % E1000_NUM_TX_DESC);

    // wait for the packet to be sent
    while(!(nic.tx_descs[idx]->status & TX_STATUS_DD)){
        __asm__ volatile("":::"memory");
    }
}

uint8_t e1000_init(struct pci_device_header_0 * _pciConfigHeader, uint32_t base_address) {
    printf("### NETWORK CARD INIT ###\n");
    printf("[NIC] Bar0 Address: %x\n", _pciConfigHeader->bar0);

    uint32_t bar_address = (uint32_t)get_bar_address(_pciConfigHeader, 0x0);
    nic.bar_type = get_bar_type(bar_address);

    printf("[NIC] Bar0 Type: %x interrupt line: %x\n", nic.bar_type, _pciConfigHeader->interrupt_line);

    if (nic.bar_type == PCI_BAR_TYPE_IO) {
        printf("[NIC] IO Base: %x\n", bar_address);
        nic.io_base = bar_address;
    } else {
        printf("[NIC] Memory Base: %x\n", bar_address);
        uint64_t bar_size = get_bar_size((void*)bar_address, base_address);
        map_current_memory_size((void*)bar_address, (void*)bar_address, bar_size);
        nic.mem_base = (uint64_t) bar_address;
    }

    enable_bus_mastering(&(_pciConfigHeader->header));

    // reset the device
    WriteRegister(REG_CTRL, CTRL_RESET_MASK);

    while(ReadRegister(REG_CTRL) & CTRL_RESET_MASK){
        __asm__ volatile("":::"memory");
    }

    // reconfigure
    WriteRegister(REG_CTRL, CTRL_ASDE_MASK | CTRL_SLU_MASK);
    
    // get mac address
    /* LOW */
    nic.mac[0] = (uint8_t) ReadRegister(RCV_BA_LOW) & 0xFF;
    nic.mac[1] = (uint8_t) (ReadRegister(RCV_BA_LOW) >> 8) & 0xFF;
    nic.mac[2] = (uint8_t) (ReadRegister(RCV_BA_LOW) >> 16) & 0xFF;
    nic.mac[3] = (uint8_t) (ReadRegister(RCV_BA_LOW) >> 24) & 0xFF;
    /* HIGH */
    nic.mac[4] = (uint8_t) ReadRegister(RCV_BA_HIGH) & 0xFF;
    nic.mac[5] = (uint8_t) (ReadRegister(RCV_BA_HIGH) >> 8) & 0xFF;

    // set bit "Address valid"
    WriteRegister(RCV_BA_HIGH, ReadRegister(RCV_BA_HIGH) | RBAH_AV_MASK);
    
    printf("[KOT NET/E1000] MAC address: %x:%x:%x:%x:%x:%x", nic.mac[0], nic.mac[1], nic.mac[2], nic.mac[3], nic.mac[4], nic.mac[5]);

    nic.eerprom_exists = 0;
    if (detectEEProm ()) {
        printf("[NIC] EEProm detected\n");
    } else {
        printf("[NIC] EEProm not detected\n");
    }
    if (! readMACAddress()) return E1000_FAILURE;
    printMac();

    //startLink();
    for(int i = 0; i < 0x80; i++)
        WriteRegister(0x5200 + i*4, 0);
    rxinit();
    txinit();  
    
    WriteRegister(REG_IMASK ,0x1F6DC);
    WriteRegister(REG_IMASK ,0xff & ~4);
    ReadRegister(0xc0);

    //if ( interruptManager->registerInterrupt(IRQ0+pciConfigHeader->getIntLine(),this))
    //{
    //    enableInterrupt();
    //    rxinit();
    //    txinit();        
    //    video.putString("E1000 card started\n",COLOR_RED,COLOR_WHITE);
    //    return E1000_SUCCESS;
    //}
    //else return E1000_FAILURE;

    return E1000_SUCCESS;
}

uint8_t * getMacAddress() {
    return nic.mac;
}

static void rx_poll()
{

    //MOVE ME!!!!!
	uint8_t *pkt = (void *)nic.rx_descs[nic.rx_cur]->addr;
	uint16_t pktlen = nic.rx_descs[nic.rx_cur]->length;
	uint8_t dropflag = 0;
    printf("[NIC] Packet: \n");
    for (int i = 0; i < pktlen; i++) {
        printf("%x ", pkt[i]);
        if (i % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");

	while( (nic.rx_descs[nic.rx_cur]->status & (1 << 0)) )
	{
		// raw packet and packet length (excluding CRC)

        //move here
		if( pktlen < 60 )
		{
			printf("[NIC] short packet (%u bytes)\n", pktlen);
			dropflag = 1;
		}

		// while not technically an error, there is no support in this driver
		if( !(nic.rx_descs[nic.rx_cur]->status & (1 << 1)) )
		{
			printf("[NIC] no EOP set! (len=%u, 0x%x 0x%x 0x%x)\n", 
				pktlen, pkt[0], pkt[1], pkt[2]);
			dropflag = 1;
		}
		
		if( nic.rx_descs[nic.rx_cur]->errors )
		{
			printf("[NIC] rx errors (0x%x)\n", nic.rx_descs[nic.rx_cur]->errors);
			dropflag = 1;
		}

		if( !dropflag )
		{
			dbg_print("VaLID PACKET\n");
		}
		
		// update RX counts and the tail pointer
		nic.rx_descs[nic.rx_cur]->status = (uint16_t)(0);
			
		nic.rx_cur = (nic.rx_cur + 1) % E1000_NUM_RX_DESC;
		
		// write the tail to the device
		WriteRegister(REG_RDTR, nic.rx_cur);
	}
}

void handle_nic_int() {
    WriteRegister(REG_IMASK, 0x1);

    dbg_print("[NIC] Interrupt\n");
    uint32_t icr = ReadRegister(0xc0);
    printf("[NIC] Status: %x\n", icr);
    icr &= ~(3);

	// LINK STATUS CHANGE
    if (icr & (1 << 2) ) {
        icr &= ~(1 << 2);
        WriteRegister(REG_CTRL, (ReadRegister(REG_CTRL) | CTRL_SLU_MASK));
		printf("[NIC] e1000: Link Status Change, STATUS = 0x%p8x\n", ReadRegister(REG_STATUS));
    }

	// RX underrun / min threshold
	if( icr & (1 << 6) || icr & (1 << 4) )
	{
		icr &= ~((1 << 6) | (1 << 4));
		printf(" underrun (rx_head = %u, rx_cur = %u)\n", ReadRegister(REG_RXDESCHEAD), nic.rx_cur);
		
		volatile int i;
		for(i = 0; i < E1000_NUM_RX_DESC; i++)
		{
			if( nic.rx_descs[i]->status )
				printf(" pending descriptor (i=%u, status=0x%p4x)\n", i, nic.rx_descs[i]->status );
		}
		
		rx_poll();
	}

	// packet is pending
	if( icr & (1 << 7) )
	{
		icr &= ~(1 << 7);
		rx_poll();
	}
	
	if( icr )
		printf("[NIC]: unhandled interrupt received! (0x%p8x)\n", icr);
	
	// clearing the pending interrupts
	ReadRegister(0xC0);
}