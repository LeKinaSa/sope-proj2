#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "parsing.h"

int main(int argc, char* argv[]) {
    struct CmdArgs args = parseArgs(argc, argv);

    int publicFD;
    char fifoName[] = "fifo"; // TODO: get fifoname from command line argument parsing

    // Open FIFO for requests to the server
    do {
        publicFD = open(fifoName, O_WRONLY);
        if (publicFD < 0) sleep(1);
    } while (publicFD < 0);

    return 0;
}
