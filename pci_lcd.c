#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>    //clock_nanosleep
#include <unistd.h>  //usleep
#include "pci_lcd.h"
#include "chmod_lcd.h"
#include "kbd_hw.h"
#include "pci.h"

void initLCD() {
    writeBus(BUS_LCD_INST_o, CHMOD_LCD_MOD);
    usleep(10000);
    writeBus(BUS_LCD_INST_o, CHMOD_LCD_MOD);
    usleep(10000);
    writeBus(BUS_LCD_INST_o, CHMOD_LCD_CLR);
    usleep(10000);
    writeBus(BUS_LCD_INST_o, CHMOD_LCD_DON);
}

void writeIntoLCD(char * str, int chars) {
	int cursor;
	initLCD();

    for (int offset = 0; offset < chars; offset++) {
        // Set cursor to position (default + offset + line)
        cursor = CHMOD_LCD_POS + offset;
        if (offset > 15) {
			cursor += 0x30;
		}
		writeBus(BUS_LCD_INST_o, cursor);

        // Wait for the device
        while(CHMOD_LCD_BF == readBus(BUS_LCD_INST_o)) usleep(1000);

        // Write character onto display
        writeBus(BUS_LCD_WDATA_o, str[offset]);
    }
}

int readKeyboard() {
	writeBus(3, 0);
	unsigned char pressed1 = readBus(0);
	if ((pressed1 & 0x1F) == 0x1F) return -5;

	beep(50);

	writeBus(3, 0);
	unsigned char pressed2 = readBus(0);
	if ((pressed2 & 0x1F) == 0x1F) return -5;

	int result;
	if (pressed1 == pressed2) {
		result = translateKey(pressed1);
	} else {
		return -5;
	}
	displayOnLED(result);
	return result;
}

int isDigit(int c) {
	return c >= 0 && c <= 9;
}

int isValid(int c) {
	return c >= -4 && c <= 9;
}

void beep(int miliseconds) {
    writeBus(3, 0xFF);
    usleep(miliseconds * 1000);
    writeBus(3, 0x7F);
}

void displayOnLED(int value) {
    switch (value) {
        case -1:
        case -2:
            writeBus(BUS_LED_WR_o, 15);
            break;
        case -3:
            writeBus(BUS_LED_WR_o, 240);
            break;
        case -4:
            writeBus(BUS_LED_WR_o, 255);
            break;
        default:
            if (isDigit(value)) {
                writeBus(BUS_LED_WR_o, value);
            }
    }
}

void noLED() {
	writeBus(BUS_LED_WR_o, 0);
}

int translateKey(int keyall) {
    int column;

    // scan first column
    writeBus(BUS_KBD_WR_o, 0x4);
    usleep(1000);
    if ((readBus(0) & 0x1F) == 0x1F) column = 1;
	else {
		// scan second column
		writeBus(BUS_KBD_WR_o, 0x2);
		usleep(1000);
		if ((readBus(0) & 0x1F) == 0x1F) column = 2;
		else {
			// scan third column
			writeBus(BUS_KBD_WR_o, 0x1);
			usleep(1000);
			if ((readBus(0) & 0x1F) == 0x1F) column = 3;
		}
	}

	switch (keyall) {
		case 239:
			if (column == 2) return -3;
			else return -4;
			break;
		case 247:
			switch (column) {
				case 1:
					return -1;
					break;
				case 2:
					return 0;
					break;
				case 3:
					return -2;
					break;
				default:
					break;
			}
			break;
		case 251:
			return 6 + column;
			break;
		case 253:
			return 3 + column;
			break;
		case 254:
			return column;
			break;
		default:
			break;
	}
    return -1;
}

//Write date to bus
int writeBus(unsigned char adr, unsigned char value) {
	*(base+PCI_DATA_OUT)=value; //zapis dat
	*(base+PCI_ADDR)=adr; //adresa
	*(base+PCI_CTRL)=BIT_POWER | BIT_CSNOT | BIT_RDNOT; //PWR=1 + WR=0
	*(base+PCI_CTRL)=BIT_POWER | BIT_RDNOT ; //PWR=1 + CS=0 + WR=0
	usleep(10); //cekani
	*(base+PCI_CTRL)=BIT_POWER | BIT_CSNOT | BIT_RDNOT; //PWR=1 + WR=0
	*(base+PCI_CTRL)=BIT_POWER | BIT_CSNOT | BIT_RDNOT | BIT_WRNOT; //PWR=1
    return 1;
}

//Read data from bus
unsigned char readBus(unsigned char adr) {
	unsigned char value;
	*(base+PCI_CTRL)=BIT_POWER | BIT_CSNOT | BIT_WRNOT; //PWR=1 + RD=0
	*(base+PCI_ADDR)=adr; //adresa
	*(base+PCI_CTRL)=BIT_POWER | BIT_WRNOT; //PWR=1 + CS=0 + RD=0
	usleep(10); // 10us
	value = *(base+PCI_DATA_IN);
//	usleep(50); // 10us
	*(base+PCI_CTRL)=BIT_POWER | BIT_WRNOT; //PWR=1 + CS=0 + RD=0
	*(base+PCI_CTRL)=BIT_POWER | BIT_CSNOT | BIT_WRNOT | BIT_RDNOT; //PWR=1 + CS=0 + RD=0
    return value; //value
}
