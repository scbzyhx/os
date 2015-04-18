#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include "kernel.h"
typedef struct Semaphore {
    int token;
    ListHead block;
} Sem;

void P(Sem *);
void V(Sem *);
void lock();
void unlock();


#endif
