#ifndef _E1000_H_
#define _E1000_H_

#include <stdint.h>
#include "../../../dev/pci/pci.h"

//https://wiki.osdev.org/Intel_Ethernet_i217

#define E1000_FAILURE 0x0
#define E1000_SUCCESS 0x1

// control
#define REG_CTRL                0x0000
#define CTRL_RESET_MASK         (1 << 26)
#define CTRL_ASDE_MASK          (1 << 5) // Auto-speed detection enable
#define CTRL_SLU_MASK           (1 << 6) // Set link up

// status
#define REG_STATUS              0x0008
#define STATUS_SPEED_MASK       (1 << 6) 

#define TX_STATUS_DD    (1 << 0) // Descriptor done
#define TX_STATUS_EC    (1 << 1) // Excess Collisions
#define TX_STATUS_LC    (1 << 2) // Late Collision

// packet
#define PACKET_SIZE     2048

// Receive base address
#define RCV_BA_LOW      0x5400 
#define RCV_BA_HIGH     0x5404
#define RBAH_AV_MASK    (1 << 31) // bit "Address valid" sur le registre RAH (Receive address high)

// Receive descriptor (RCVD)
#define RCVD_ADDR_LOW       0x2800
#define RCVD_ADDR_HIGH      0x2804
#define RCVD_LENGTH         0x2808
#define RCVD_HEAD           0x2810
#define RCVD_TAIL           0x2818

// Receive control
#define REG_RCV_CTRL        0x0100
#define RCV_EN_MASK         (1 << 1)
#define RCV_BAM_MASK        (1 << 15)
#define RCV_BSIZE_MASK      (1 << 16)


#define INTEL_VEND     0x8086  // Vendor ID for Intel 
#define E1000_DEV      0x100E  // Device ID for the e1000 Qemu, Bochs, and VirtualBox emmulated NICs
#define E1000_I217     0x153A  // Device ID for Intel I217
#define E1000_82577LM  0x10EA  // Device ID for Intel 82577LM
 
 
// I have gathered those from different Hobby online operating systems instead of getting them one by one from the manual
 
#define REG_CTRL        0x0000
#define REG_STATUS      0x0008
#define REG_EEPROM      0x0014
#define REG_CTRL_EXT    0x0018
#define REG_IMASK       0x00D0
#define REG_RCTRL       0x0100
#define REG_RXDESCLO    0x2800
#define REG_RXDESCHI    0x2804
#define REG_RXDESCLEN   0x2808
#define REG_RXDESCHEAD  0x2810
#define REG_RXDESCTAIL  0x2818
 
#define REG_TCTRL       0x0400
#define REG_TXDESCLO    0x3800
#define REG_TXDESCHI    0x3804
#define REG_TXDESCLEN   0x3808
#define REG_TXDESCHEAD  0x3810
#define REG_TXDESCTAIL  0x3818
 
 
#define REG_RDTR         0x2820 // RX Delay Timer Register
#define REG_RXDCTL       0x2828 // RX Descriptor Control
#define REG_RADV         0x282C // RX Int. Absolute Delay Timer
#define REG_RSRPD        0x2C00 // RX Small Packet Detect Interrupt
 
 
 
#define REG_TIPG         0x0410      // Transmit Inter Packet Gap
#define ECTRL_SLU        0x40        //set link up
 
 
#define RCTL_EN                         (1 << 1)    // Receiver Enable
#define RCTL_SBP                        (1 << 2)    // Store Bad Packets
#define RCTL_UPE                        (1 << 3)    // Unicast Promiscuous Enabled
#define RCTL_MPE                        (1 << 4)    // Multicast Promiscuous Enabled
#define RCTL_LPE                        (1 << 5)    // Long Packet Reception Enable
#define RCTL_LBM_NONE                   (0 << 6)    // No Loopback
#define RCTL_LBM_PHY                    (3 << 6)    // PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF                 (0 << 8)    // Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER              (1 << 8)    // Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH               (2 << 8)    // Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36                      (0 << 12)   // Multicast Offset - bits 47:36
#define RCTL_MO_35                      (1 << 12)   // Multicast Offset - bits 46:35
#define RCTL_MO_34                      (2 << 12)   // Multicast Offset - bits 45:34
#define RCTL_MO_32                      (3 << 12)   // Multicast Offset - bits 43:32
#define RCTL_BAM                        (1 << 15)   // Broadcast Accept Mode
#define RCTL_VFE                        (1 << 18)   // VLAN Filter Enable
#define RCTL_CFIEN                      (1 << 19)   // Canonical Form Indicator Enable
#define RCTL_CFI                        (1 << 20)   // Canonical Form Indicator Bit Value
#define RCTL_DPF                        (1 << 22)   // Discard Pause Frames
#define RCTL_PMCF                       (1 << 23)   // Pass MAC Control Frames
#define RCTL_SECRC                      (1 << 26)   // Strip Ethernet CRC
 
// Buffer Sizes
#define RCTL_BSIZE_256                  (3 << 16)
#define RCTL_BSIZE_512                  (2 << 16)
#define RCTL_BSIZE_1024                 (1 << 16)
#define RCTL_BSIZE_2048                 (0 << 16)
#define RCTL_BSIZE_4096                 ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192                 ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384                ((1 << 16) | (1 << 25))
 
 
// Transmit Command
 
#define TX_CMD_EOP                         (1 << 0)    // End of Packet
#define TX_CMD_IFCS                        (1 << 1)    // Insert FCS
#define TX_CMD_IC                          (1 << 2)    // Insert Checksum
#define TX_CMD_RS                          (1 << 3)    // Report Status
#define TX_CMD_RPS                         (1 << 4)    // Report Packet Sent
#define TX_CMD_VLE                         (1 << 6)    // VLAN Packet Enable
#define TX_CMD_IDE                         (1 << 7)    // Interrupt Delay Enable
 
 
// TCTL Register
 
#define TCTL_EN                         (1 << 1)    // Transmit Enable
#define TCTL_PSP                        (1 << 3)    // Pad Short Packets
#define TCTL_CT_SHIFT                   4           // Collision Threshold
#define TCTL_COLD_SHIFT                 12          // Collision Distance
#define TCTL_SWXOFF                     (1 << 22)   // Software XOFF Transmission
#define TCTL_RTLC                       (1 << 24)   // Re-transmit on Late Collision
 
#define TSTA_DD                         (1 << 0)    // Descriptor Done
#define TSTA_EC                         (1 << 1)    // Excess Collisions
#define TSTA_LC                         (1 << 2)    // Late Collision
#define LSTA_TU                         (1 << 3)    // Transmit Underrun

// Transmit control
#define REG_TST_CTRL    0x0400
#define TST_EN_MASK     (1 << 1) // Transmit enable
#define TST_PSP_MASK    (1 << 3) // Pad Short Packets

#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 8
 
struct e1000_rx_desc {
        volatile uint64_t addr;
        volatile uint16_t length;
        volatile uint16_t checksum;
        volatile uint8_t status;
        volatile uint8_t errors;
        volatile uint16_t special;
} __attribute__((packed));
 
struct e1000_tx_desc {
        volatile uint64_t addr;
        volatile uint16_t length;
        volatile uint8_t cso;
        volatile uint8_t cmd;
        volatile uint8_t status;
        volatile uint8_t css;
        volatile uint16_t special;
} __attribute__((packed));

struct e1000 {
    uint8_t bar_type;
    uint16_t io_base;
    uint64_t mem_base;
    uint8_t eerprom_exists : 1;
    uint8_t mac[6];
    struct e1000_rx_desc *rx_descs[E1000_NUM_RX_DESC];
    struct e1000_tx_desc *tx_descs[E1000_NUM_TX_DESC];
    uint16_t rx_cur;
    uint16_t tx_cur;
};

uint8_t e1000_init(struct pci_device_header_0 * _pciConfigHeader, uint32_t base_address); // Constructor. takes as a parameter a pointer to an object that encapsulate all he PCI configuration data of the device
//void fire (InterruptContext * p_interruptContext);  // This method should be called by the interrupt handler 
uint8_t * getMacAddress();                         // Returns the MAC address
int sendPacket(const void * p_data, uint16_t p_len);  // Send a packet
void handle_nic_int();
#endif