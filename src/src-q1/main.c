#define _XOPEN_SOURCE 700   // Allows usage of some GNU/Linux standard functions and structures

#include "communication.h"
#include "parsing.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void* threadFunc(void* arg) {
    Message* messagePtr = (Message*) arg;
    char privateFifoName[128];
    sprintf(privateFifoName, "/tmp/%d.%lu", messagePtr->pid, messagePtr->tid);

    // TODO: Validation
    int privateFD = open(privateFifoName, O_WRONLY);
    

    return NULL;
}

int main(int argc, char* argv[]) {
    CmdArgs args = parseArgs(argc, argv);
    int publicFD;

    if (mkfifo(args.fifoname, 0660) < 0) {
        perror("mkfifo");
        return 1;
    }

    publicFD = open(args.fifoname, O_RDONLY);

    if (publicFD < 0) {
        perror("open");
        unlink(args.fifoname);
        return 1;
    }

    Message* message = malloc(sizeof(Message));

    while (read(publicFD, message, sizeof(Message)) > 0) {
        printMessage(message);
        printf("\n");
    }

    if (close(publicFD) < 0) {
        perror("close");
        unlink(args.fifoname);
        return 1;
    }

    if (unlink(args.fifoname) < 0) {
        perror("unlink");
        return 1;
    }

    return 0;
}
