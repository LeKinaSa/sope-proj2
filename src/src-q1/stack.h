#ifndef STACK_H
#define STACK_H

#include <stdbool.h>


/**
 * @brief A very much half-assed stack implementation
 */
typedef struct Stack {
    unsigned int *values; /** The values of the stack */
    unsigned int maxSize; /** The maximum size of the stack */
    unsigned int size;    /** The current size of the stack */
} Stack;

Stack initStack(unsigned int maxSize);

bool         empty(const Stack *s);
void         push( Stack *s, unsigned int i);
unsigned int pop(  Stack *s);

#endif // STACK_H
