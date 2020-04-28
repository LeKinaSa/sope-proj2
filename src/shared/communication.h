#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdlib.h>

/**
 * @brief Structure that holds information for a message between the client and the server.
 * Can be used for requests or responses.
 */
typedef struct Message {
    size_t i;   /** Request number */
    pid_t pid;  /** Process ID of the client (for requests) or the server (for response) */
    pid_t tid;  /** Thread ID */
    int dur;    /** Duration of the access in milliseconds, -1 if access is denied */
    int pl;     /** Place number, -1 for requests or for responses if access is denied */
} Message;

void printMessage(const Message* message);

#endif // COMMUNICATION_H