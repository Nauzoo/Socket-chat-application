#include "netwCommons.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>

void panic(int errType ,char* errMsg) {
    printf("!%d : %s\n", errType, errMsg);

    exit(1);
}

