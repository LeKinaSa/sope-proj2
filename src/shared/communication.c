#include "communication.h"

#include <stdio.h>

void printMessage(const Message* message) {
    printf("i: %lu\npid: %d\ntid: %lu\ndur: %d\npl: %d\n", message->i, message->pid, 
            message->tid, message->dur, message->pl);
}