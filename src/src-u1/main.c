#define _DEFAULT_SOURCE

#include "communication.h"
#include "parsing.h"
#include "logging.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#define MAX_DURATION    200
#define MAX_THREADS     512

static bool timeout = false;
static size_t requestNum = 1;
static int publicFD;
static pthread_mutex_t messageInitLock = PTHREAD_MUTEX_INITIALIZER;


void* threadFunc(void* arg) {
    Message request, response;
    
    request.pid = getpid();
    request.tid = pthread_self();
    request.pl = -1;

    // Critical section
    pthread_mutex_lock(&messageInitLock);
    request.i = requestNum++;
    request.dur = rand() % MAX_DURATION;
    write(publicFD, &request, sizeof(Message));
    pthread_mutex_unlock(&messageInitLock);

    char privateFifoName[128];
    int privateFD;

    // TODO: Function return validation

    sprintf(privateFifoName, "/tmp/%d.%lu", request.pid, request.tid);
    mkfifo(privateFifoName, 0660);
    privateFD = open(privateFifoName, O_RDONLY);

    read(privateFD, &response, sizeof(Message));

    close(privateFD);
    unlink(privateFifoName);

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
    CmdArgs args = parseArgs(argc, argv);

    registerHandler();

    pthread_t threadIds[MAX_THREADS];

    srand(time(NULL));

    // Open FIFO for requests to the server
    do {
        publicFD = open(args.fifoname, O_WRONLY);
        if (publicFD < 0) usleep(100 * MILLI_TO_MICRO);
    } while (publicFD < 0);

    alarm(args.nSecs);

    size_t numThreads;
    for (numThreads = 0; numThreads < MAX_THREADS && !timeout; ++numThreads) {
        pthread_create(&threadIds[numThreads], NULL, threadFunc, NULL);
        usleep(5 * MILLI_TO_MICRO);
    }

    for (size_t i = 0; i < numThreads; ++i) {
        pthread_join(threadIds[i], NULL);
    }

    close(publicFD);

    return 0;
}
