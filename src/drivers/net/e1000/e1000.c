#include "e1000.h"
#include "../../../util/printf.h"
#include "../../../dev/pci/pci.h"
#include "../../../memory/heap.h"

struct e1000 nic = {0};

void writeCommand( uint16_t p_address, uint32_t p_value);
uint32_t readCommand(uint16_t p_address);
 
 
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
void write32 (uint64_t p_address,uint32_t p_value)
{
    (*((volatile uint32_t*)(p_address)))=(p_value);
 
}

uint32_t read32 (uint64_t p_address)
{
    return *((volatile uint32_t*)(p_address));
 
}

//Ports
void outportl (uint16_t p_port,uint32_t p_data)
{
    __asm__ volatile ("outl %1, %0" : : "dN" (p_port), "a" (p_data));
}
uint32_t inportl( uint16_t p_port)
{
    uint32_t l_ret;
    __asm__ volatile ("inl %1, %0" : "=a" (l_ret) : "dN" (p_port));
    return l_ret;
}

void writeCommand( uint16_t p_address, uint32_t p_value)
{
    if ( nic.bar_type == 0 )
    {
        write32(nic.mem_base+p_address,p_value);
    }
    else
    {
        outportl(nic.io_base, p_address);
        outportl(nic.io_base + 4, p_value);
    }
}

uint32_t readCommand( uint16_t p_address)
{
    if ( nic.bar_type == 0 )
    {
        return read32(nic.mem_base+p_address);
    }
    else
    {
        outportl(nic.io_base, p_address);
        return inportl(nic.io_base + 4);
    }
}
   

uint8_t detectEEProm()
{
    uint32_t val = 0;
    writeCommand(REG_EEPROM, 0x1); 
 
    for(int i = 0; i < 1000 && ! nic.eerprom_exists; i++)
    {
            val = readCommand( REG_EEPROM);
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
            	writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 8) );
        	while( !((tmp = readCommand(REG_EEPROM)) & (1 << 4)) );
        }
        else
        {
            writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 2) );
            while( !((tmp = readCommand(REG_EEPROM)) & (1 << 1)) );
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
 
    ptr = (uint8_t *)malloc(sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16);
 
    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        nic.rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i*16);
        nic.rx_descs[i]->addr = (uint64_t)(uint8_t *)(malloc(8192 + 16));
        nic.rx_descs[i]->status = 0;
    }
 
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
    writeCommand(REG_RXDESCLO, (uint64_t)ptr);
    writeCommand(REG_RXDESCHI, 0);
 
    writeCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
 
    writeCommand(REG_RXDESCHEAD, 0);
    writeCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
    nic.rx_cur = 0;
    writeCommand(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
 
}
 
 
void txinit()
{    
    uint8_t *  ptr;
    struct e1000_tx_desc *descs;
    // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
    // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
    ptr = (uint8_t *)(malloc(sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16));
 
    descs = (struct e1000_tx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        nic.tx_descs[i] = (struct e1000_tx_desc *)((uint8_t*)descs + i*16);
        nic.tx_descs[i]->addr = 0;
        nic.tx_descs[i]->cmd = 0;
        nic.tx_descs[i]->status = TSTA_DD;
    }
 
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
 
    //now setup total length of descriptors
    writeCommand(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);
 
 
    //setup numbers
    writeCommand( REG_TXDESCHEAD, 0);
    writeCommand( REG_TXDESCTAIL, 0);
    nic.tx_cur = 0;
    writeCommand(REG_TCTRL,  TCTL_EN
        | TCTL_PSP
        | (15 << TCTL_CT_SHIFT)
        | (64 << TCTL_COLD_SHIFT)
        | TCTL_RTLC);
 
    // This line of code overrides the one before it but I left both to highlight that the previous one works with e1000 cards, but for the e1000e cards 
    // you should set the TCTRL register as follows. For detailed description of each bit, please refer to the Intel Manual.
    // In the case of I217 and 82577LM packets will not be sent if the TCTRL is not configured using the following bits.
    writeCommand(REG_TCTRL,  0b0110000000000111111000011111010);
    writeCommand(REG_TIPG,  0x0060200A);
 
}

void printMac() {
    printf("[NIC] MAC Address: %x:%x:%x:%x:%x:%x\n", nic.mac[0], nic.mac[1], nic.mac[2], nic.mac[3], nic.mac[4], nic.mac[5]);
}

int sendPacket(const void * p_data, uint16_t p_len)
{    
    nic.tx_descs[nic.tx_cur]->addr = (uint64_t)p_data;
    nic.tx_descs[nic.tx_cur]->length = p_len;
    nic.tx_descs[nic.tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
    nic.tx_descs[nic.tx_cur]->status = 0;
    uint8_t old_cur = nic.tx_cur;
    nic.tx_cur = (nic.tx_cur + 1) % E1000_NUM_TX_DESC;
    writeCommand(REG_TXDESCTAIL, nic.tx_cur);   
    while(!(nic.tx_descs[old_cur]->status & 0xff));    
    return 0;
}

uint8_t e1000_init(struct pci_device_header_0 * _pciConfigHeader) {
    printf("### NETWORK CARD INIT ###\n");
    printf("[NIC] Bar0 Address: %x\n", _pciConfigHeader->bar0);
    nic.bar_type = BAR_TYPE(_pciConfigHeader->bar0);

    printf("[NIC] Bar0 Type: %x\n", nic.bar_type);
    struct pci_bar_mem * mbar = (struct pci_bar_mem *) &_pciConfigHeader->bar0;
    struct pci_bar_io * iobar = (struct pci_bar_io *) &_pciConfigHeader->bar0;

    nic.io_base = (uint16_t ) iobar->base_address;
    nic.mem_base = (uint64_t) mbar->base_address;

    enable_bus_mastering(&(_pciConfigHeader->header));
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
        writeCommand(0x5200 + i*4, 0);
    rxinit();
    txinit();  
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
