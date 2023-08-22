#include "e1000.h"
#include "../../../io/io.h"
#include "../../../util/printf.h"
#include "../../../util/string.h"
#include "../../../dev/pci/pci.h"
#include "../../../memory/heap.h"
#include "../../../memory/paging.h"

struct e1000 nic = {0};

void WriteRegister( uint16_t p_address, uint32_t p_value);
uint32_t ReadRegister(uint16_t p_address);
 
 
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
 
    ptr = (uint8_t *)malloc(sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16);
 
    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        nic.rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i*16);
        nic.rx_descs[i]->addr = (uint64_t)(uint8_t *)(malloc(8192 + 16));
        nic.rx_descs[i]->status = 0;
    }
 
    WriteRegister(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
    WriteRegister(REG_RXDESCLO, (uint64_t)ptr);
    WriteRegister(REG_RXDESCHI, 0);
 
    WriteRegister(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
 
    WriteRegister(REG_RXDESCHEAD, 0);
    WriteRegister(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
    nic.rx_cur = 0;
    WriteRegister(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
 
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
 
    WriteRegister(REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
 
    //now setup total length of descriptors
    WriteRegister(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);
 
 
    //setup numbers
    WriteRegister( REG_TXDESCHEAD, 0);
    WriteRegister( REG_TXDESCTAIL, 0);
    nic.tx_cur = 0;
    WriteRegister(REG_TCTRL,  TCTL_EN
        | TCTL_PSP
        | (15 << TCTL_CT_SHIFT)
        | (64 << TCTL_COLD_SHIFT)
        | TCTL_RTLC);
 
    // This line of code overrides the one before it but I left both to highlight that the previous one works with e1000 cards, but for the e1000e cards 
    // you should set the TCTRL register as follows. For detailed description of each bit, please refer to the Intel Manual.
    // In the case of I217 and 82577LM packets will not be sent if the TCTRL is not configured using the following bits.
    WriteRegister(REG_TCTRL,  0b0110000000000111111000011111010);
    WriteRegister(REG_TIPG,  0x0060200A);
 
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
    WriteRegister(REG_TXDESCTAIL, nic.tx_cur);   
    while(!(nic.tx_descs[old_cur]->status & 0xff));    
    return 0;
}

uint8_t e1000_init(struct pci_device_header_0 * _pciConfigHeader, uint32_t base_address) {
    printf("### NETWORK CARD INIT ###\n");
    printf("[NIC] Bar0 Address: %x\n", _pciConfigHeader->bar0);

    uint32_t bar_address = (uint32_t)get_bar_address(_pciConfigHeader, 0x0);
    nic.bar_type = get_bar_type(bar_address);

    printf("[NIC] Bar0 Type: %x\n", nic.bar_type);

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
    
    uint8_t mac2[6] = {0,0,0,0,0,0};
    // get mac address
    /* LOW */
    mac2[0] = (uint8_t) ReadRegister(RCV_BA_LOW) & 0xFF;
    mac2[1] = (uint8_t) (ReadRegister(RCV_BA_LOW) >> 8) & 0xFF;
    mac2[2] = (uint8_t) (ReadRegister(RCV_BA_LOW) >> 16) & 0xFF;
    mac2[3] = (uint8_t) (ReadRegister(RCV_BA_LOW) >> 24) & 0xFF;
    /* HIGH */
    mac2[4] = (uint8_t) ReadRegister(RCV_BA_HIGH) & 0xFF;
    mac2[5] = (uint8_t) (ReadRegister(RCV_BA_HIGH) >> 8) & 0xFF;

    // set bit "Address valid"
    WriteRegister(RCV_BA_HIGH, ReadRegister(RCV_BA_HIGH) | RBAH_AV_MASK);
    
    printf("[KOT NET/E1000] MAC address: %x:%x:%x:%x:%x:%x", mac2[0], mac2[1], mac2[2], mac2[3], mac2[4], mac2[5]);

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
    //if ( interruptManager->registerInterrupt(IRQ0+pciConfigHeader->getIntLine(),this))
    //{
    //    enableInterrupt();
    //    rxinit();
    //    txinit();        
    //    video.putString("E1000 card started\n",COLOR_RED,COLOR_WHITE);
    //    return E1000_SUCCESS;
    //}
    //else return E1000_FAILURE;

    uint8_t bufferExample[PACKET_SIZE];
    memset(bufferExample, 0, PACKET_SIZE);

    uint32_t ipsrc = 0x54400001;
    uint32_t ipdest = 0x7F000001;

    uint8_t arp_buffer[42];

    // Ethernet header
    memcpy((void*)&arp_buffer[0], (void*)"\xff\xff\xff\xff\xff\xff", 6); // Destination MAC address (broadcast)
    memcpy((void*)&arp_buffer[6], (void*)mac2, 6); // Source MAC address
    memcpy((void*)&arp_buffer[12], (void*)"\x08\x06", 2); // Ethernet Type: ARP

    // ARP header
    memcpy((void*)&arp_buffer[14], (void*)"\x00\x01", 2); // Hardware Type: Ethernet
    memcpy((void*)&arp_buffer[16], (void*)"\x08\x00", 2); // Protocol Type: IPv4
    arp_buffer[18] = 6; // Hardware Address Length: 6 (Ethernet MAC address)
    arp_buffer[19] = 4; // Protocol Address Length: 4 (IPv4 address)
    memcpy((void*)&arp_buffer[20], (void*)"\x00\x01", 2); // Operation: ARP Request
    memcpy((void*)&arp_buffer[22], (void*)mac2, 6); // Sender Hardware Address: Source MAC address
    memcpy((void*)&arp_buffer[28], (void*)&ipsrc, 4); // Sender Protocol Address: Source IP address
    memcpy((void*)&arp_buffer[32], (void*)"\x00\x00\x00\x00\x00\x00", 6); // Target Hardware Address: zero (unknown)
    memcpy((void*)&arp_buffer[38], (void*)&ipdest, 4); // Target Protocol Address: Destination IP address

    memcpy(bufferExample, arp_buffer, 42);

    sendPacket(bufferExample, PACKET_SIZE);

    return E1000_SUCCESS;
}
