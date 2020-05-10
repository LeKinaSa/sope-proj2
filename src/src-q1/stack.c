#include "stack.h"
#include <stdlib.h>

Stack initStack(unsigned int maxSize) {
    Stack res;
    res.size = 0;
    res.maxSize = maxSize;
    res.values = (unsigned int*) malloc(maxSize * sizeof(unsigned int));
    return res;
}

bool empty(const Stack *s) {
    return s->size == 0;
}

void push(Stack *s, unsigned int i) {
    if (s->size < s->maxSize) {
        s->values[s->size++] = i;
    }
}

unsigned int pop(Stack *s) {
    if (!empty(s)) {
        return s->values[s->size--];
    } else {
        return -1;
    }
}
