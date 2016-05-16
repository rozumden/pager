/*******************************************************************
  Components for embedded applications builded for
  laboratory and medical instruments firmware  
 
  chmod_lcd.h - control codes for two or four line character
                LCD modules
 
  Copyright (C) 2001 by Pavel Pisa pisa@cmp.felk.cvut.cz
            (C) 2002 by PiKRON Ltd. http://www.pikron.com

 *******************************************************************/

#ifndef _CHMOD_LCD_H_
#define _CHMOD_LCD_H_

/* Status flags in LCD_STAT */
#define CHMOD_LCD_BF   0x80  /* busy flag */

/* Control codes for LCD_INST */
#define CHMOD_LCD_POS  0x80  /* set cursor position */
#define CHMOD_LCD_CLR  0x01  /* clear display */
#define CHMOD_LCD_HOME 0x02  /* cursor home */
#define CHMOD_LCD_MOD  0x38  /* mode dual line, 8 bits */
#define CHMOD_LCD_NROL 0x10  /* not scrolling display */
#define CHMOD_LCD_DON  0x0C  /* display on */
#define CHMOD_LCD_CON  0x0A  /* cursor on */
#define CHMOD_LCD_BON  0x09  /* blink on */
#define CHMOD_LCD_NSH  0x04  /* motion */

#endif /*  _CHMOD_LCD_H_ */
