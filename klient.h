#ifndef _KLIENT_H
#define _KLIENT_H

int connectToServer(char * address, int port);
void * messageReceiver(void * socketfd);

#endif

