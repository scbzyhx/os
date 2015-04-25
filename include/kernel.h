#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "common.h"
#include "x86/x86.h"
#include "memory.h"
#include "process.h"
#include "sem.h"
#include "msg.h"
extern PCB *current;
extern PCB pcbPool[PCB_NUM];
extern ListHead ready;
extern ListHead block;
extern ListHead free;
extern pid_t global_pid;


void sleep();
void wakeup(PCB*);
struct PCB* fetch_pcb(pid_t); //get PCB struct through pid_t

#endif
