#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>

/**
 * @brief All the cmd arguments in one place
 */
typedef struct CmdArgs {
    char *fifoname;     /** The FIFO path */
    unsigned int nSecs; /** The lifetime of the program*/
} CmdArgs;

/**
 * @brief Parses all the arguments into a neat struct
 *
 * This function WILL CRASH if invalid options are parsed
 *
 * @param argc
 * @param argv
 * @return struct CmdArgs
 */
CmdArgs parseArgs(int argc, char* argv[]);

/**
 * @brief Prints all the values of the struct
 *
 * @param args
 */
void printArgs(const CmdArgs *args);


#endif // PARSING_H
