#ifndef _PCI_H
#define _PCI_H

#define DEV_ENABLE "/sys/bus/pci/devices/0000:03:00.0/enable"
#define DEV_ADDRESS 0xfe800000
//DEV_ADDRESS is result of: lspci -nn -v -d 1172:1f32 (pøes /proc/bus/pci)
// It is necessary to check by your program

unsigned char * base;

void nanowait(long time_seconds, long time_nanoseconds);
void uploadMessage(int sender, int message);
int areThreadsRunning();
void noLED();

#endif
