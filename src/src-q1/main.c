#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int publicFD;
    char fifoName[] = "fifo"; // TODO: get fifoname from command line argument parsing

    if (mkfifo(fifoName, 0660) < 0) {
        perror("mkfifo");
        return 1;
    }

    publicFD = open(fifoName, O_RDONLY);

    if (publicFD < 0) {
        perror("open");
        unlink(fifoName);
        return 1;
    }

    if (close(publicFD) < 0) {
        perror("close");
        unlink(fifoName);
        return 1;
    }

    if (unlink(fifoName) < 0) {
        perror("unlink");
        return 1;
    }

    return 0;
}
