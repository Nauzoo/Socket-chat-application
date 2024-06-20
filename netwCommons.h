#ifndef NETW_COMMONS_H
#define NETW_COMMONS_H

//#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <poll.h>
#include <stdbool.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>


#define S_ADDRESS "your local ip here or 127.0.0.1"
#define PORT 8080
#define MSG_LEN 4096 //(4kb)
#define SA_in struct sockaddr_in
#define SA struct sockaddr

#define ERR_SER_F_LOAD    1
#define ERR_IP_CONF_LOAD  2
#define ERR_CON_CONF      3
#define REQ_FAIL          4
#define RES_FAIL          5
#define ADDR_NOT_PROV     6


void panic(int errType ,char* errMsg);


typedef struct utilSocket {
    int SocketFileDescriptor;
    SA_in address;
    bool failedConnection;
} utilSocket;

#endif
