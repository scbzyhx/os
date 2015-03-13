#ifndef __PROCESS_H__
#define __PROCESS_H__
#include "adt/list.h"
#define KSTACK 4096
#define PCB_NUM 64

enum STATE {
    TASK_EMPTY = 0,  //show an empty slot
    TASK_RUNNING=1,
    TASK_STOPED,
    TASK_INTERRUPTIBLE
};

typedef struct PCB {
    struct ListHead head; //form a list for all process
    enum STATE state;     //process state
    pid_t pid;            //process id
    pid_t parent;         //parent process id
    struct PCB *ppcb;     //parent process pointer
	void *tf;
	char kstack[KSTACK];
} PCB;

extern PCB *current;

extern PCB pcbPool[PCB_NUM];
extern ListHead ready;
extern ListHead block;
extern ListHead free;

PCB* create_kthread(void *fun);

#endif
