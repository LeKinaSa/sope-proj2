#include "logging.h"

#include <stdio.h>
#include <time.h>

static const char* OP_CODES[] = {
    "IWANT",
    "RECVD",
    "ENTER",
    "IAMIN",
    "TIMUP",
    "2LATE",
    "CLOSD",
    "FAILD",
    "GAVUP"
};

void logOperation(const Message* message, Operation op) {
    printf("%ld ; %lu ; %d ; %lu ; %d ; %d ; %s\n", time(NULL), message->i, message->pid, message->tid, 
            message->dur, message->pl, OP_CODES[op]);
}
