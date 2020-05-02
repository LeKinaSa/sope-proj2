#define _DEFAULT_SOURCE     // Allows usage of some GNU/Linux standard functions and structures

#include "../shared/communication.h"
#include "parsing.h"
#include "../shared/logging.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

static bool timeout = false;
static CmdArgs args;

static int place = 1;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* threadFunc(void* arg) {
    Message* requestPtr = (Message*) arg;

    logOperation(requestPtr, SERVER_RECEIVED_REQUEST);

    char privateFifoName[128];
    sprintf(privateFifoName, "/tmp/%d.%lu", requestPtr->pid, requestPtr->tid);

    int privateFD;
    const int MAX_ITERATIONS = 5;
    int iterations = 0;
    do {
        privateFD = open(privateFifoName, O_WRONLY);
        if (privateFD < 0) usleep(MILLI_TO_MICRO);
        ++iterations;
        if (iterations == MAX_ITERATIONS) {
            logOperation(requestPtr, SERVER_CANNOT_SEND_RESPONSE);
            return NULL;
        }
    } while (privateFD < 0);

    Message response;
    
    response.i = requestPtr->i;
    response.pid = getpid();
    response.tid = pthread_self();
    response.dur = timeout ? -1 : requestPtr->dur;

    // Critical section
    pthread_mutex_lock(&mutex);
    response.pl = timeout ? -1 : place;
    ++place;
    pthread_mutex_unlock(&mutex);

    if (write(privateFD, &response, sizeof(Message)) < 0) {
        logOperation(requestPtr, SERVER_CANNOT_SEND_RESPONSE);
    }
    
    if (close(privateFD) < 0) {
        perror("close");
    }

    if (!timeout) {
        logOperation(&response, SERVER_ACCEPTED_REQUEST);
        usleep(requestPtr->dur * MILLI_TO_MICRO);
        logOperation(&response, SERVER_REQUEST_TIME_UP);
    }
    else {
        logOperation(&response, SERVER_REJECTED_REQUEST_BATHROOM_CLOSED);
    }

    // Memory for the message is dynamically allocated; we must free it
    free(arg);

    return NULL;
}

void sigHandler(int signo) {
    timeout = true;
}

void registerHandler() {
    struct sigaction action;

    action.sa_flags = 0;
    action.sa_handler = sigHandler;

    sigaction(SIGALRM, &action, NULL);
}

int main(int argc, char* argv[]) {
    args = parseArgs(argc, argv);
    registerHandler();

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

    alarm(args.nSecs);

    Message message;

    pthread_t threadId;
    ssize_t bytesRead;
    while (!timeout) {
        bytesRead = read(publicFD, &message, sizeof(Message));

        if (bytesRead > 0) {
            Message* requestPtr = malloc(sizeof(Message));
            memcpy(requestPtr, &message, sizeof(Message));

            pthread_create(&threadId, NULL, threadFunc, requestPtr);
            pthread_detach(threadId);
        }
    }

    if (close(publicFD) < 0) {
        perror("close");
    }

    if (unlink(args.fifoname) < 0) {
        perror("unlink");
    }

    pthread_exit(NULL);
}
