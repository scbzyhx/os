#ifndef __PROCESS_H__
#define __PROCESS_H__
#include "adt/list.h"
#define KSTACK 4096
#define PCB_NUM 64

enum STATE {
    TASK_EMPTY = 0,  //show an empty slot, put in free4
    TASK_RUNNING=1, // put in ready list
    TASK_BLOCKED,   //in block list
    TASK_DEAD,      //process exit
    TASK_STOPED,
    TASK_INTERRUPTIBLE,
    TASK_UNINTERRUPTIBLE
};

typedef struct PCB {
    void *tf;
    struct ListHead head; //form a list for all process,
    struct ListHead sem_list; // wait on semaphore

    enum STATE state;     //process state
    uint32_t intr_counter; //record intr count for recursive sti
    pid_t pid;            //process id
    pid_t parent;         //parent process id

    struct PCB *ppcb;     //parent process pointer
	//void *tf;
	char kstack[KSTACK];
} PCB;

extern PCB *current;

extern PCB pcbPool[PCB_NUM];
extern ListHead ready;
extern ListHead block;
extern ListHead free;

PCB* create_kthread(void *fun);
void set_kthread_state(PCB*,enum STATE);
void sleep();
void wakeup(PCB*);

#endif
