#ifndef _PCI_LCD_H
#define _PCI_LCD_H

#define PCI_DATA_OUT 0x8020
#define PCI_DATA_IN 0x8040
#define PCI_ADDR 0x8060
#define PCI_CTRL 0x8080

#define BIT_POWER 0x80
#define BIT_CSNOT 0x40
#define BIT_RDNOT 0x1
#define BIT_WRNOT 0x2

void initLCD();
void writeIntoLCD(char * str, int chars);
int readKeyboard();
int isDigit(int c);
int isValid(int c);
int translateKey(int keyall);
void beep(int miliseconds);
void displayOnLED(int value);
int writeBus(unsigned char adr, unsigned char value);
unsigned char readBus(unsigned char adr);

#endif
