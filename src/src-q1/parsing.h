#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>

/**
 * @brief All the cmd and envp arguments in one place
 *
 * If a bool is true, it means that option was set (e.g. countLinks is always true)
 */
struct CmdArgs {
    char *fifoname; /** The FIFO path */
    unsigned int nSecs; /** The lifetime of the program*/
    unsigned int nPlaces; /** The number of places */
    unsigned int nThreads; /** maximum number of threads */
};

/**
 * @brief Parses all the arguments into a neat struct
 *
 * This function WILL CRASH if invalid options are parsed
 *
 * @param argc
 * @param argv
 * @return struct CmdArgs
 */
struct CmdArgs parseArgs(int argc, char* argv[]);

/**
 * @brief Prints all the values of the struct
 *
 * @param args
 */
void printArgs(const struct CmdArgs *args);


#endif // PARSING_H
