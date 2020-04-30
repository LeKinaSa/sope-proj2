#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>

#include "parsing.h"

//https://stackoverflow.com/questions/4770985/how-to-check-if-a-string-starts-with-another-string-in-c
bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

struct CmdArgs parseArgs(int argc, char* argv[]) {
    struct CmdArgs res;
    res.fifoname = NULL;
    res.nSecs    = 0;

    if (argc != 4) {
        printf("%s <-t nsecs> fifoname\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc;){

        if (!startsWith("-", argv[i])) {
            if (res.fifoname == NULL) {
                int strSize = strlen(argv[i]) + 1;
                res.fifoname = (char *) malloc(strSize);
                strcpy(res.fifoname, argv[i]);
            } else {
                fprintf(stderr, "Error: More than one possible path\n");
                exit(1);
            }
            i++;
            continue;
        }

        if (strcmp("-t", argv[i]) == 0) {
            if (i + 1 == argc) {
                fprintf(stderr, "Error: \"-t\" option requires a numerical argument\n");
                exit(1);
            } else {
                bool success = sscanf(argv[i + 1], "%u", &res.nSecs) == 1;
                if (!success) {
                    fprintf(stderr, "Error: \"-t\" option requires a numerical argument\n");
                    exit(1);
                }
            }
            i += 2;
            continue;
        }

        fprintf(stderr, "Error: option \"%s\" not recognized\n", argv[i]);
        exit(1);
    }

    if (res.fifoname == NULL) {
        fprintf(stderr, "Error: fifoname not provided\n");
        exit(1);
    }

    if (res.nSecs == 0) {
        fprintf(stderr, "Error: option \"-t\" not provided\n");
        exit(1);
    }

    return res;
}

void printArgs(const struct CmdArgs *args) {
    printf("Args debug:       \n");
    printf("    fifoname:     %s\n", args->fifoname);
    printf("    nSecs:        %u\n", args->nSecs);
}
