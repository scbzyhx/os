#ifndef __PROCESS_H__
#define __PROCESS_H__
#include "adt/list.h"
#include "msg.h"
#include "x86/x86.h"

#define KSTACK 4096
#define PCB_NUM 64
#define MSG_POOL_SIZE 10240
#define MAX_TIME_SLOT 1

enum STATE {
    TASK_EMPTY = 0,  //show an empty slot, put in free4
    TASK_RUNNING=1, // put in ready list
    TASK_READY,     //in ready queue
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
    struct ListHead msg_list; //msg queue
    struct ListHead msg_free;     //for free msg structure

    enum STATE state;     //process state
    uint32_t intr_counter; //record intr count for recursive sti
    pid_t pid;            //process id
    pid_t parent;         //parent process id
    CR3   cr3;
    uint32_t counter;       //for scheduling

    struct PCB *ppcb;     //parent process pointer
    struct Message msg_pool[MSG_POOL_SIZE];
	//void *tf;
	char kstack[KSTACK];
} PCB;
/*
extern PCB *current;

extern PCB pcbPool[PCB_NUM];
extern ListHead ready;
extern ListHead block;
extern ListHead free;
extern pid_t global_pid;
*/
PCB* create_kthread(void *fun);
void set_kthread_state(PCB*,enum STATE);
pid_t create_thread();
/*void sleep();
void wakeup(PCB*);
struct PCB* fetch_pcb(pid_t); //get PCB struct through pid_t
*/
#endif
