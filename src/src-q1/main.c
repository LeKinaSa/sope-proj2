#define _DEFAULT_SOURCE     // Allows usage of some GNU/Linux standard functions and structures

#include "../shared/communication.h"
#include "parsing.h"
#include "../shared/logging.h"
#include "stack.h"

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
#include <semaphore.h>

static bool timeout = false;
static CmdArgs args;

static sem_t nThreads;

static Stack bathroomNumberStack;
static pthread_mutex_t mutex             = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  bathroomStackCond = PTHREAD_COND_INITIALIZER;

void* threadFunc(void* arg) {
    Message* requestPtr = (Message*) arg;

    logOperation(requestPtr, SERVER_RECEIVED_REQUEST);

    char privateFifoName[128];
    sprintf(privateFifoName, "/tmp/%d.%lu", requestPtr->pid, requestPtr->tid);

    int privateFD;
    static const ushort MAX_ATTEMPTS = 8;
    ushort numAttempts = 0;

    do {
        privateFD = open(privateFifoName, O_WRONLY);
        if (privateFD < 0) usleep(MILLI_TO_MICRO);
        ++numAttempts;
    } while (privateFD < 0 && numAttempts < MAX_ATTEMPTS);

    if (privateFD < 0) {
        logOperation(requestPtr, SERVER_CANNOT_SEND_RESPONSE);
        free(arg);
        return NULL;
    }

    Message response;

    bool localTimeout = timeout;
    
    response.i = requestPtr->i;
    response.pid = getpid();
    response.tid = pthread_self();
    response.dur = localTimeout ? -1 : requestPtr->dur;

    if (localTimeout) {
        response.pl = -1;
    } else {
        // Critical section
        pthread_mutex_lock(&mutex);
        while (bathroomNumberStack.size == 0) {
            pthread_cond_wait(&bathroomStackCond, &mutex);
        }
        response.pl = pop(&bathroomNumberStack);
        pthread_mutex_unlock(&mutex);
    }

    if (write(privateFD, &response, sizeof(Message)) < 0) {
        logOperation(requestPtr, SERVER_CANNOT_SEND_RESPONSE);
    }
    
    if (close(privateFD) < 0) {
        perror("close");
    }

    if (!localTimeout) {
        logOperation(&response, SERVER_ACCEPTED_REQUEST);
        usleep(requestPtr->dur * MILLI_TO_MICRO);
        logOperation(&response, SERVER_REQUEST_TIME_UP);
    }
    else {
        logOperation(&response, SERVER_REJECTED_REQUEST_BATHROOM_CLOSED);
    }

    // Memory for the message is dynamically allocated; we must free it
    free(arg);

    if (response.pl != -1) {
        // Lets return the bathroom number
        pthread_mutex_lock(&mutex);
        push(&bathroomNumberStack, response.pl);
        pthread_cond_broadcast(&bathroomStackCond);
        pthread_mutex_unlock(&mutex);
    }

    sem_post(&nThreads); // This thread has "ended", lets unlock the semaphore

    // Note: There is a small gap (right here!) where the semaphore is released but the thread is still alive (just barely)
    // There aren't any concurrency issues here, given that the thread does literally nothing else other than dying

    return NULL;
}

void sigHandler(int signo) {
    timeout = true;

    if (unlink(args.fifoname) < 0) {
        perror("unlink");
    }
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

    if(sem_init(&nThreads, 0, args.nThreads)) {
        perror("sem_init");
        return 1;
    }

    bathroomNumberStack = initStack(args.nPlaces);
    for (unsigned int i = 0; i < args.nPlaces; i++) {
        push(&bathroomNumberStack, i);
    }

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
    while (!(timeout && bytesRead == 0)) {
        bytesRead = read(publicFD, &message, sizeof(Message));

        if (bytesRead > 0) {

            sem_wait(&nThreads);

            Message* requestPtr = malloc(sizeof(Message));
            memcpy(requestPtr, &message, sizeof(Message));

            if (pthread_create(&threadId, NULL, threadFunc, requestPtr) != 0) {
                fprintf(stderr, "pthread_create failed in server\n");
            }
            else {
                pthread_detach(threadId);
            }
        }
    }

    if (close(publicFD) < 0) {
        perror("close");
    }

    pthread_exit(NULL);
}
