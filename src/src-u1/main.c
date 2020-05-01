#define _DEFAULT_SOURCE

#include "../shared/communication.h"
#include "parsing.h"
#include "../shared/logging.h"

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
    if (write(publicFD, &request, sizeof(Message)) < 0) {
        perror("write");
    } 
    pthread_mutex_unlock(&messageInitLock);

    logOperation(&request, CLIENT_INITIAL_REQUEST);

    char privateFifoName[128];
    int privateFD;

    // TODO: Function return validation

    sprintf(privateFifoName, "/tmp/%d.%lu", request.pid, request.tid);
    mkfifo(privateFifoName, 0660);
    privateFD = open(privateFifoName, O_RDONLY);

    ssize_t readSize = read(privateFD, &response, sizeof(Message));
    if (readSize <= 0) {
        logOperation(&request, CLIENT_CANNOT_GET_RESPONSE);
    }
    
    if ((response.dur == -1) && (response.pl == -1)) {
        logOperation(&response, CLIENT_RECEIVED_INFO_BATHROOM_CLOSED);
    }
    else {
        logOperation(&response, CLIENT_USING_BATHROOM);
    }

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

    srand(time(NULL));

    // Open FIFO for requests to the server
    do {
        publicFD = open(args.fifoname, O_WRONLY);
        if (publicFD < 0) usleep(100 * MILLI_TO_MICRO);
    } while (publicFD < 0);

    alarm(args.nSecs);

    pthread_t threadId;
    while (!timeout) {
        pthread_create(&threadId, NULL, threadFunc, NULL);
        usleep(5 * MILLI_TO_MICRO);
    }

    pthread_exit(NULL);
}
