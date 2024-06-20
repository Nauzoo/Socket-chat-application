#include "netwCommons.h"


#define BACK_LOG  5

utilSocket socketQueue[BACK_LOG];
int socketQueueLen = 0;

typedef struct server { // struct containing important information about the server
    int serverSocketFD;
    SA_in addr;
} SERVER;

SERVER * newServer (int domain, int type, int protocol, long port) {
    SERVER * nServer = malloc(sizeof(SERVER));

    nServer->serverSocketFD =  socket(domain, type, protocol);
    nServer->addr.sin_family = domain;
    nServer->addr.sin_port = htons(port);

    if (nServer->serverSocketFD <= 0)
        panic(ERR_SER_F_LOAD, "Socket failed to load, server can't be started.");

    if (inet_pton(AF_INET, S_ADDRESS, &nServer->addr.sin_addr) <= 0)
        panic(ERR_IP_CONF_LOAD, "Unable to set an IP address for this server.");

    if (bind(nServer->serverSocketFD, (SA *) &nServer->addr, sizeof(nServer->addr)) < 0)
        panic(ERR_CON_CONF, "Failed to bind the server socket.");

    return nServer;
}

utilSocket * acceptConnection(int serverSocketFD) { // when it's called, wait for a connection and buids a utilSocket struct.

    SA_in clientAddress; //  SA_in = struct sockaddr_in
    socklen_t clientAddressSize = sizeof(SA_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *) &clientAddress, &clientAddressSize); // stay waiting for a connection in the server's socket

    utilSocket * thisUtilSocket = malloc(sizeof(utilSocket)); // builds the utilSocket

    thisUtilSocket->address = clientAddress;
    thisUtilSocket->SocketFileDescriptor = clientSocketFD;
    thisUtilSocket->failedConnection = (clientSocketFD < 0);

    return thisUtilSocket;
}

void notifyAllClients(char* message, int senderSocketFD) {
    // iterates in all client's sockets file descriptor list and send the last updtades excluding the sender client
    for(int i = 0; i < socketQueueLen; i++){
        if (socketQueue[i].SocketFileDescriptor != senderSocketFD) {
            send(socketQueue[i].SocketFileDescriptor, message, strlen(message), 0);
        }
    }

}

void *begginCommunication(void *socketFD) { //(separate thread) stay waiting for client "requests" (messages) and delivers it to all other clients".
    char buffer[MSG_LEN];

    int castedFD = *(int *)socketFD;

    while (true) {

        memset(buffer, 0, MSG_LEN-1);
        ssize_t reqLen = recv(castedFD, buffer, MSG_LEN, 0);
        if (reqLen > 0) {
            notifyAllClients(buffer, castedFD);
        }
        else
            break;
    }

    close(castedFD);

    return NULL;
}

void begginSreamThread(utilSocket *aUtilSocket) { // opens begginCommunication in a separate thread.
    pthread_t newThread;
    pthread_create(&newThread, NULL, begginCommunication,  &aUtilSocket->SocketFileDescriptor);

}


void begginAcceptingConnections(int socketFD) {
    // stays waiting for connection in acceptConnection, add the new socket to the client's list and opens the deliver thread for that client
    while (true) {

        utilSocket *newUtilSocket = acceptConnection(socketFD);

        printf("connection stabilishied with %d\n", newUtilSocket->address.sin_addr.s_addr);

        socketQueue[socketQueueLen] = *newUtilSocket;
        socketQueueLen++;

        begginSreamThread(newUtilSocket);
    }
}

int main(int argc, char *argv[])
{
    SERVER * myServer = newServer(AF_INET, SOCK_STREAM, 0, PORT);

    // sets the server in listening mode.
    if ((listen(myServer->serverSocketFD, BACK_LOG) < 0))
        panic(ERR_CON_CONF, "Failed to listen to connections.");

    begginAcceptingConnections(myServer->serverSocketFD);

    shutdown(myServer->serverSocketFD, SHUT_RDWR);

    return 0;
}
