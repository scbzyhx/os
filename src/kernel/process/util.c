#include "kernel.h"
typedef void(*FUN)(void);
PCB* getFreePCB() {

    PCB* retp = NULL;
    if(free.next != &free)
    {
        retp = list_entry(free.next,PCB,head);
        list_del(&retp->head);
    }
    return retp;
}

void threadWrapper(void *fun) {
    ((FUN)fun)();
    current->state = TASK_DEAD;
    while(1);
}

PCB*
create_kthread(void *fun) {
    PCB *fr = getFreePCB();
    TrapFrame *tf;
    void ** floor;
    if (fr == NULL)
        return NULL;
    floor = (void**)((uint32_t)(fr->kstack) + KSTACK);

    floor = floor - sizeof(TrapFrame)/sizeof(void**);// + 2*sizeof(void**);
    fr->tf = (void*)floor;
    tf = (TrapFrame*)floor;
    tf->eip = (uint32_t)threadWrapper;
    tf->ebp = (uint32_t)(fr->kstack + KSTACK);
    tf->eflags = 0x202;
    tf->irq = 1000;
    tf->cs = SELECTOR_KERNEL(SEG_KERNEL_CODE); //kernel code segment
    tf->ds = SELECTOR_KERNEL(SEG_KERNEL_DATA); //kernel data segment
    /*because of no stack switch, 
      so ss is thre parameter of function threadwrapper
       and esp is the return addr of function threadwrapper
    */
    tf->ss = (uint32_t)fun;
    tf->esp = (uint32_t)tf;   
    
    fr->intr_counter = 0;
    
    //put it into the tail of ready
    //list_add_after(ready.prev,&fr->head);
    
	return fr;
}
void ffun() {
    //while(1)
    //while(1)
    printk("in fun\n");
    printk("in fun\n");
    printk("in fun\n");
}
extern void test_proc();
void
init_proc() {
    //idle use the kernel stack of os
    //init free,ready,block
    list_init(&free);
    list_init(&block);
    list_init(&ready);
    //printk("%x\n",ready.next);
    //printk("%x\n",&ready);
    //ready.prev = ready.next = &ready;
    //block.prev = block.next = &block;
    //free is more
    ListHead *p;
    int i = 0;
    for(;i<PCB_NUM;i++)
    {
        p = &pcbPool[i].head; 
        //pcbPool[i].state = TASK_EMPTY;
        list_add_after(free.prev,p); // append to the last
    }

    //init idle
    current->pid = 0;
    current->parent = 0;
    current->head.prev = current->head.next = NULL;
    current->state = TASK_RUNNING;
    
    //init
 //   set_kthread_state(create_kthread(ffun),TASK_RUNNING);
    test_proc();
    
}

void sleep(void) {
    current->state = TASK_BLOCKED;
    asm volatile("int $0x80");
}
void wakeup(PCB *p) {
    p->state = TASK_RUNNING;
    lock();
    list_del(&p->head);
    list_add_after(ready.prev,&p->head);
    unlock();
}
void set_kthread_state(PCB *p, enum STATE state) {
    if (state == TASK_RUNNING) {
        //list_del(&p->head);
        list_add_after(ready.prev,&p->head);
    }else if(state == TASK_BLOCKED) {
        //list_del(&p->head);
        list_add_after(block.prev,&p->head);
    } else {
        assert(0);
    }
}

