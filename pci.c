#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>    //clock_nanosleep
#include <unistd.h>  //usleep
#include <string.h>
#include <pthread.h>
#include "klient.h"
#include "pci.h"
#include "pci_lcd.h"
#include "kbd_hw.h"

int inDefaultMode = 1;
int isRunning = 1;
char * lcdText;
int lenOfText;
struct timespec stopwatch, current;
pthread_mutex_t messageLock;

struct message {
    int messageReceived;
    int senderID;
    int isWaiting;
};
volatile int senderID;
volatile int messageReceived;
volatile int isWaiting = 0;

volatile int threadsRunning;

void toDefaultMode() {
	puts("Sending message. Switching to default mode.");
	inDefaultMode = 1;
	lcdText[lenOfText] = '\0';
	printf("Input sequence: %s\n", lcdText);
	lenOfText = 0;
	initLCD();
	clock_gettime(CLOCK_REALTIME, &stopwatch);
}

void toMessageMode() {
	puts("Switching from default mode.");
	inDefaultMode = 0;
	lenOfText = 0;
	clock_gettime(CLOCK_REALTIME, &stopwatch);
}

void initPager() {
    int soubor = open("/dev/mem", O_RDWR | O_SYNC);
    if (soubor == -1) {
        printf("Failure to open dev/mem\n");
        exit(1);
    }

    // TODO: you must find  DEV_ADDRESS constant and the length 0x10000 by searchung list of PCI devices
    base = mmap(NULL, 0x10000, PROT_WRITE | PROT_READ, MAP_SHARED, soubor, DEV_ADDRESS);
    if (base == MAP_FAILED) {
	   printf("Failure to map device\n");
	   exit(2);
    }

//-- Initialization of our device ------------------
    *(base+PCI_CTRL) = 0x80;    // power up
    usleep(10);                 // wait for power up
    writeBus(3, 0x7F);          // switch off beeper

//----------- Our program --------------------------

    printf("Device started.\n");
    //beep(1000);

	lcdText = (char *)malloc(sizeof(char) * 33);
	writeIntoLCD("**** PAGER ****", 15);
}

void uploadMessage(int sender, int message) {

    pthread_mutex_lock(&messageLock);

    senderID = sender;
    messageReceived = message;
    isWaiting = 1;

    pthread_mutex_unlock(&messageLock);
}

struct message getMessage() {
    struct message m;
    pthread_mutex_lock(&messageLock);

    m.senderID = senderID;
    m.messageReceived = messageReceived;
    m.isWaiting = isWaiting;

    pthread_mutex_unlock(&messageLock);

    return m;
}

void * keyboardListener() {
    int run = 1;
    char * window = (char *)malloc(sizeof(char) * 3);

    window[0] = getchar();
    window[1] = getchar();

    while (run && threadsRunning) {
        window[2] = getchar();

        if (window[0] == 'e' && window[1] == 'n' && window[2] == 'd') {
            isRunning = threadsRunning = run = 0;
        }

        window[0] = window[1];
        window[1] = window[2];
    }
    printf("Closing keyboard listener.\n");
}

int areThreadsRunning() {
    return threadsRunning;
}

int main(int argc, char ** argv) {
	int ID;
	char * IPaddr;
	if (argc == 3) {
		ID = atoi(argv[1]);
		IPaddr = (char *)malloc(sizeof(char) * 16);
		strcpy(IPaddr, argv[2]);
	} else {
		ID = 10;
		IPaddr = "127.0.0.1";
	}

    //initPager();

	int socket = connectToServer(IPaddr, 55556);
	if (socket == -1) {
		perror("Cannot connect to server.");
	}

	threadsRunning = 1;
	pthread_t clientThread;
	pthread_create(&clientThread, NULL, messageReceiver, &socket);
	pthread_t keyboardThread;
	pthread_create(&keyboardThread, NULL, keyboardListener, NULL);

    //usleep(1000000 * 600); // wait 10 minutes
    usleep(1000000 * 10);
    usleep(300000);

	while (isRunning) {
		if (inDefaultMode) {
			// Default mode
			if (isDigit(readKeyboard())) {
				toMessageMode();
			}
			// Check whether message was received
            struct message m = getMessage();
            if (m.isWaiting) {
                // Display message
                printf("Obtained message from server: %d %d\n", m.senderID, m.messageReceived);
                if (m.messageReceived < 1000000 && m.messageReceived > -1
                        && m.senderID < 1000 && m.senderID > -1) {
                    char * text = (char *)malloc(sizeof(char) * 16);
                    sprintf(text, "%d %d", m.senderID, m.messageReceived);
                    writeIntoLCD(text, strlen(text));
                    printf("NA LCD: %s\n", text);
                }
            }
            // Send server querry every 5 seconds
            clock_gettime(CLOCK_REALTIME, &current);
            if ((current.tv_sec - stopwatch.tv_sec) > 4) {
                clock_gettime(CLOCK_REALTIME, &stopwatch);
                char * querry = (char *)malloc(sizeof(char) * 64);
                sprintf(querry, "querrymessage %d\n", ID);
                write(socket, querry, strlen(querry));
            }
		} else {
			// Message mode
			int read = readKeyboard();
			if (isValid(read)) {
				if (read == -4) {
					toDefaultMode();
					// Send recipient and message to server
					int rec, mes;
                    int loaded = sscanf(lcdText, "%d %d", &rec, &mes);
                    if (loaded == 2 && rec > -1 && rec < 1000 && mes > -1 && mes < 1000000) {
                        char * text = (char *)malloc(sizeof(char) * 64);
                        sprintf(text, "sendmessage %d %d %d\n", ID, rec, mes);
                        write(socket, text, strlen(text));
                    } else {
                        printf("Given message has invalid format.");
                        char * text = (char *)malloc(sizeof(char) * 16);
                        sprintf(text, "Invalid");
                        writeIntoLCD(text, strlen(text));
                        beep(500);
                    }
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
						writeIntoLCD(lcdText, lenOfText);
					}
				}
				clock_gettime(CLOCK_REALTIME, &stopwatch);
				usleep(200000); //beep(200000);
			}
			clock_gettime(CLOCK_REALTIME, &current);
			if ((current.tv_sec - stopwatch.tv_sec) > 29) {
				toDefaultMode();
			}
		}
		usleep(10000);
	}

	// join thread
	printf("Closing application...\n");
    threadsRunning = 0;
    close(socket);
    close(0);
    pthread_join(keyboardThread, NULL);
    pthread_cancel(clientThread);

    // switch off device
    //*(base+PCI_CTRL) = 0x00;
    puts("\nDone\n");
    return EXIT_SUCCESS;
}

void nanowait(long time_seconds, long time_nanoseconds) {
    const long int ONESECOND = 1000000000;
	struct timespec wait_time  = {
        .tv_sec = time_seconds,                     /* seconds */
        .tv_nsec = time_nanoseconds % ONESECOND     /* nanoseconds [0 .. 999999999] */
    };
	clock_nanosleep(CLOCK_MONOTONIC, 0, &wait_time, NULL);
}
