#define _XOPEN_SOURCE 700   // Allows usage of some GNU/Linux standard functions and structures

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "communication.h"

void* threadFunc(void* arg) {
    Message* messagePtr = (Message*) arg;
    char privateFifoName[128];
    sprintf(privateFifoName, "/tmp/%d.%lu", messagePtr->pid, messagePtr->tid);

    // TODO: Validation
    int privateFD = open(privateFifoName, O_WRONLY);
    

    return NULL;
}

int main(int argc, char* argv[]) {
    int publicFD;
    char fifoName[] = "fifo"; // TODO: get fifoname from command line argument parsing

    if (mkfifo(fifoName, 0660) < 0) {
        perror("mkfifo");
        return 1;
    }

    publicFD = open(fifoName, O_RDONLY);

    if (publicFD < 0) {
        perror("open");
        unlink(fifoName);
        return 1;
    }

    Message* message = malloc(sizeof(Message));

    while (read(publicFD, message, sizeof(Message)) > 0) {
        printMessage(message);
        printf("\n");
    }

    if (close(publicFD) < 0) {
        perror("close");
        unlink(fifoName);
        return 1;
    }

    if (unlink(fifoName) < 0) {
        perror("unlink");
        return 1;
    }

    return 0;
}
