#ifdef CUAK
//#ifndef NET_i825xx_H
#define NET_i825xx_H

#ifndef PCI_H
	#include "drivers/pci.h"
#endif

#define i825xx_REG_CTRL			(dev->mmio_address + 0x0000)
#define i825xx_REG_STATUS		(dev->mmio_address + 0x0008)
#define i825xx_REG_EECD			(dev->mmio_address + 0x0010)
#define i825xx_REG_EERD			(dev->mmio_address + 0x0014)

#define i825xx_REG_MDIC			(dev->mmio_address + 0x0020)

#define i825xx_REG_IMS			(dev->mmio_address + 0x00D0)
#define i825xx_REG_RCTL			(dev->mmio_address + 0x0100)
#define i825xx_REG_TCTL			(dev->mmio_address + 0x0400)

#define i825xx_REG_RDBAL		(dev->mmio_address + 0x2800)
#define i825xx_REG_RDBAH		(dev->mmio_address + 0x2804)
#define i825xx_REG_RDLEN		(dev->mmio_address + 0x2808)
#define i825xx_REG_RDH			(dev->mmio_address + 0x2810)
#define i825xx_REG_RDT			(dev->mmio_address + 0x2818)

#define i825xx_REG_TDBAL		(dev->mmio_address + 0x3800)
#define i825xx_REG_TDBAH		(dev->mmio_address + 0x3804)
#define i825xx_REG_TDLEN		(dev->mmio_address + 0x3808)
#define i825xx_REG_TDH			(dev->mmio_address + 0x3810)
#define i825xx_REG_TDT			(dev->mmio_address + 0x3818)

#define i825xx_REG_MTA			(dev->mmio_address + 0x5200)

#define i825xx_REG_RAL          (dev->mmio_address + 0x5400)
#define i825xx_REG_RAH          (dev->mmio_address + 0x5404)

// PHY REGISTERS (for use with the MDI/O Interface)
#define i825xx_PHYREG_PCTRL		(0)
#define i825xx_PHYREG_PSTATUS	(1)
#define i825xx_PHYREG_PSSTAT	(17)

int net_i825xx_init( pci_device_t * );

#endif

