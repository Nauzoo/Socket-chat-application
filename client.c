#include "netwCommons.h"


typedef struct client { //struct containing important information about the client
    int clientSocketFD;
    SA_in addr;

} CLIENT;


CLIENT * newClient(int domain, int type, int protocol, long port) { // creates a client and returns a pointer to it
    CLIENT * nClient = malloc(sizeof(CLIENT));

    nClient->clientSocketFD = socket(domain, type, protocol); // Socket uses -> IPV4, stream data, default TCP
    nClient->addr.sin_family = domain;
    nClient->addr.sin_port = htons(port); // Converts the u_int16_t hostshort from host endianness to network endianness.

    if (nClient->clientSocketFD < 0)
        panic(ERR_SER_F_LOAD, "Socket failed to load, no connection can be started.");

    if (inet_pton(AF_INET, S_ADDRESS, &nClient->addr.sin_addr) <= 0)
       panic(ERR_IP_CONF_LOAD, "Unable to set an IP address for this client.");

    return nClient;
}

int makeRequest(CLIENT* myClient, char *mesBuffer) { // writes data in the client's socket, "sends data to server"

    int reqSize = strlen(mesBuffer);

    ssize_t sendOk = send(myClient->clientSocketFD, mesBuffer, reqSize, 0);

    if (sendOk != reqSize){
        panic(REQ_FAIL, "This connection failed to send the data.");
        return 1;
    }

    return 0;
}

void *begginRecivingResoponses(void * socketFD){ // (separate thread) start checking for data stream in the respective socket

    int castedFD = *(int *)socketFD;
    char buffer[MSG_LEN];

    while (true) {
        memset(buffer, 0, MSG_LEN-1);
        ssize_t resLen = recv(castedFD, buffer, MSG_LEN, 0);
        if (resLen > 0)
            printf("%s", buffer);

    }

    return NULL;
}
void begginCommunication(CLIENT *aClient) {

    pthread_t newThread; // starts a new thread to updtade responses
    pthread_create(&newThread, NULL, begginRecivingResoponses, (void *) &aClient->clientSocketFD);


    char* idBuffer =  NULL;
    size_t nameSize = 0;

    printf("Please, type your nickname to identify your message:\n");
    ssize_t nameLen = getline(&idBuffer, &nameSize, stdin); // saves the name of the client
    idBuffer[nameLen-1] = 0;

    printf("type /exit to quit the chat.\n");

    char finalMsg[MSG_LEN];

    sprintf(finalMsg, "==== %s joined the chat! ====\n", idBuffer);
    makeRequest(aClient, finalMsg);

    char *reqMsg = NULL;
    size_t msgSize = 0;

    while(true){
        ssize_t charCount = getline(&reqMsg, &msgSize, stdin); // reads client's typing

        if (charCount > 0){
            if (strcmp(reqMsg, "/exit\n")==0){
                sprintf(finalMsg, "==== %s left the chat! ====\n", idBuffer);
                makeRequest(aClient, finalMsg);

                break;
            }
        }

        memset(finalMsg, 0, MSG_LEN-1);
        sprintf(finalMsg, "%s : %s", idBuffer, reqMsg);

        int reqRes = makeRequest(aClient ,finalMsg); // sends the typed text
        if (reqRes != 0)
            break;
    }

    close(aClient->clientSocketFD); // closes the socket after utilizing it
}

int main(int argc, char *argv[])
{

    CLIENT * myClient = newClient(AF_INET, SOCK_STREAM, 0, PORT);

    if ((connect(myClient->clientSocketFD, (SA *) &myClient->addr, sizeof(myClient->addr)) < 0 ))
        panic(ERR_CON_CONF, "Failed to connect to the server address.");

    begginCommunication(myClient);

    shutdown(myClient->clientSocketFD, SHUT_RDWR);

    return 0;
}
