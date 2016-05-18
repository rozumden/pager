#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>    //clock_nanosleep
#include <unistd.h>  //usleep
#include "pci.h"
#include "pci_lcd.h"
#include "kbd_hw.h"

int inDefaultMode = 1;
int isRunning = 1;
char * lcdText;
int lenOfText;
struct timespec stopwatch, current;

void toDefaultMode() {
	puts("Sending message. Switching to default mode.");
	inDefaultMode = 1;
	lcdText[lenOfText] = '\0';
	printf("Input sequence: %s\n", lcdText);
	lenOfText = 0;
	initLCD();
}
void toMessageMode() {
	puts("Switching from default mode.");
	inDefaultMode = 0;
	lenOfText = 0;
	clock_gettime(CLOCK_REALTIME, &stopwatch);
}

int main(int agrc, char ** argv) {
	if (argc == 3) {
		int ID = atoi(argv[1]);
		char * IPaddr = (char *)malloc(sizeof(char) * 16;
		strcpy(IPaddr, argv[2]);
	} else {
		int ID = 10;
		char * IPaddr = "127.0.0.1";
	}
	
    int soubor = open("/dev/mem", O_RDWR | O_SYNC);
    if(soubor == -1) {
        printf("Failure to open dev/mem\n");
        return 1;
    }

    // TODO: you must find  DEV_ADDRESS constant and the length 0x10000 by searchung list of PCI devices
    base = mmap(NULL, 0x10000, PROT_WRITE | PROT_READ, MAP_SHARED, soubor, DEV_ADDRESS);
    if(base == MAP_FAILED) {
	   printf("Failure to map device\n");
	   return 2;
    }

//-- Initialization of our device ------------------
    *(base+PCI_CTRL) = 0x80;    // power up
    usleep(10);                 // wait for power up
    writeBus(3, 0x7F);          // switch off beeper

//----------- Our program ------------------------------

    printf("Device started.");
    //beep(1000);

	lcdText = (char *)malloc(sizeof(char) * 33);
	initLCD();
	writeIntoLCD("**** PAGER ****", 15);


	while (isRunning) {
		if (inDefaultMode) {
			// Default mode
			if (isDigit(readKeyboard())) {
				toMessageMode();
			}
		} else {
			// Message mode
			int read = readKeyboard();
			if (isValid(read)) {
				if (read == -4) {
					toDefaultMode();
				} else {
					if (lenOfText > 31) {
						if (read == -3) lenOfText--;
					} else {
						switch (read) {
							case -1:
							case -2:
								lcdText[lenOfText] = 95;			// '_'
								break;
							case -3:
								if (lenOfText >= 0) lenOfText -= 2; // erase last character
								break;
							default:
								lcdText[lenOfText] = read + 48;		// 0-9
						}
						lenOfText++;
						initLCD();
						writeIntoLCD(lcdText, lenOfText);
					}
				}
				clock_gettime(CLOCK_REALTIME, &stopwatch);
				usleep(200000); // beep(200000);
			}
			clock_gettime(CLOCK_REALTIME, &current);
			if ((current.tv_sec - stopwatch.tv_sec) > 29) {
				toDefaultMode();
			}
		}
		usleep(150000);
	}

    *(base+PCI_CTRL) = 0x00;  // switch off device
    printf("\nDone\n");
    return 0;
}

void nanowait(long time_seconds, long time_nanoseconds) {
    const long int ONESECOND = 1000000000;
	struct timespec wait_time  = {
        .tv_sec = time_seconds,                     /* seconds */
        .tv_nsec = time_nanoseconds % ONESECOND     /* nanoseconds [0 .. 999999999] */
    };
	clock_nanosleep(CLOCK_MONOTONIC, 0, &wait_time, NULL);
}
