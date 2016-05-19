#ifndef _KLIENT_H
#define _KLIENT_H

int connectToServer(int pager_id, char * address, int port);
void * messageReceiver(void * socketfd);

#endif

