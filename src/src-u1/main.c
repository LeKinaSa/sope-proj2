#define _DEFAULT_SOURCE

#include "communication.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define MAX_DURATION    200

static size_t requestNum = 0;
static int publicFD;
static pthread_mutex_t messageInitLock = PTHREAD_MUTEX_INITIALIZER;

void* threadFunc(void* arg) {
    Message request;
    Message response;
    
    request.pid = getpid();
    request.tid = pthread_self();
    request.pl = -1;

    // Critical section
    pthread_mutex_lock(&messageInitLock);
    request.i = requestNum++;
    request.dur = rand() % MAX_DURATION;
    pthread_mutex_unlock(&messageInitLock);

    write(publicFD, &request, sizeof(Message));

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

int main(int argc, char* argv[]) {
    pthread_t threadIds[32];
    char fifoName[] = "fifo"; // TODO: get fifoname from command line argument parsing

    srand(time(NULL));

    // Open FIFO for requests to the server
    do {
        publicFD = open(fifoName, O_WRONLY);
        if (publicFD < 0) sleep(1);
    } while (publicFD < 0);

    for (size_t i = 0; i < 32; ++i) {
        pthread_create(&threadIds[i], NULL, threadFunc, NULL);
        usleep(5 * MILLI_TO_MICRO);
    }

    for (size_t i = 0; i < 32; ++i) {
        pthread_join(threadIds[i], NULL);
    }

    close(publicFD);

    return 0;
}
