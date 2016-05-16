/*******************************************************************
 
  kbd_hw.h - definitions of mmaped_8bit_bus_kbd hardware.
             The connected hardware is MA_KBD3 board connected
	     through emulated 8 bit paralel bus to EVB/Altera
	     DB4CGX15 PCI Express card through CON-DB4CGX15
	     adapter board. Original refernce design VID:PID
	     are used 1172:1f32 for mmaped_8bit_bus_kbd hardware
	     Quartus FPGA design.
 
  Copyright (C) 2011 by Pavel Pisa - originator 

  This code can be used and distributed under any license.
  THERE IS NO WARRANTY FOR THE SOFTWARE, TO THE EXTENT PERMITTED
  BY APPLICABLE LAW.

  The code is based on COLAMI code by Pavel Pisa and PiKRON company
  
  Components for embedded applications builded for
  laboratory and medical instruments firmware

 *******************************************************************/

#ifndef _KBD_HW_H_
#define _KBD_HW_H_

#include <stdint.h>

/* MA_KBD3 register mapping on 8-bit emulated bus */
#define BUS_LCD_INST_o  0x0000 /* WR LCD instruction */
#define BUS_LCD_STAT_o  0x0001 /* RD LCD status */
#define BUS_LCD_WDATA_o 0x0002 /* WR LCD data to display or CG */
#define BUS_LCD_RDATA_o 0x0003 /* RD LCD RAM data */
#define BUS_LED_WR_o    0x0001 /* WR data to show on LED diodes */
#define BUS_KBD_WR_o    0x0003 /* WR data to keyboard scan register */
#define BUS_KBD_RD_o    0x0000 /* RD data returned from keyboard matrix */

#endif /* _KBD_HW_H_ */
