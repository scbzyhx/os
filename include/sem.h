#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include "kernel.h"

typedef struct Semaphore {
    volatile int token;
    ListHead block;
} Sem;

void P(Sem *);
void V(Sem *);
void lock();
void unlock();
void init_sem(Sem *,int);


#endif
