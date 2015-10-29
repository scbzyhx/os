#ifndef __MM_H_
#define __MM_H_
#include "hal.h"

//to simplfy the code
#define NEW_PAGE  DEV_READ
#define FREE_PAGE DEV_WRITE

int alloc_pages(PCB *pcb, uint32_t va, uint32_t len);
int free_pages(PCB *pcb, uint32_t va, uint32_t len);

#endif
