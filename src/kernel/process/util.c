#include "kernel.h"
typedef void(*FUN)(void);

void init_msg_pool(struct PCB *pcb) { 
    int i = 0;
    list_init(&(pcb->msg_free));
    for(i=0;i<MSG_POOL_SIZE;++i) {
        list_add_after(&(pcb->msg_free),&(pcb->msg_pool[i].list));
    }
}


PCB* getFreePCB() {

    PCB* retp = NULL;
    lock();
    if(free.next != &free)
    {
        retp = list_entry(free.next,PCB,head);
        list_del(&retp->head);
    }
    unlock();
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
    fr->state = TASK_BLOCKED;
    fr->cr3.val = 0;

    list_init(&(fr->msg_list));
    list_init(&(fr->sem_list));
    list_init(&(fr->head));
    
    init_msg_pool(fr);

    //set the id of process
    lock();
    fr->pid = global_pid++;
    unlock();
    
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
extern void init_driver();

//test kthread here
extern void test_msg();
extern void init_kmem_read_test();
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
    current->pid = 1;
    current->parent = 0;
    current->head.prev = current->head.next = NULL;
    current->state = TASK_RUNNING;
    
    //init
 //   set_kthread_state(create_kthread(ffun),TASK_RUNNING);
    //test_proc();
   // test_msg();
    init_driver();
 //   init_kmem_read_test();
    
}

void sleep(void) {
    current->state = TASK_BLOCKED;
    asm volatile("int $0x80");
}
void wakeup(PCB *p) {
    //p->state = TASK_READ;
    lock();
    if(p->state == TASK_BLOCKED) {
        list_del(&p->head);
        list_add_after(ready.prev,&p->head);
        p->state =TASK_READY;


    }
    unlock();
}
void set_kthread_state(PCB *p, enum STATE state) {
    if (state == TASK_READY) {
        //list_del(&p->head);
        list_add_after(ready.prev,&p->head);
    }else if(state == TASK_BLOCKED) {
        //list_del(&p->head);
        list_add_after(block.prev,&p->head);
    } else {
        assert(0);
    }
}


//get PCB struct through pid_t
//invoke with lock
struct PCB* fetch_pcb(pid_t pid) {
    //there should be an global, but there doesnot exists
    //so find in block,ready and cuurent and idle accordingly
    struct ListHead *ptr;
    struct PCB *pcb;
    /*
       lock may be not good
     */
    
    if (current->pid == pid ) {
        //return current;
        assert(0);
        //avoid storm
    }

    list_foreach(ptr, &ready) {
        //list
        pcb = list_entry(ptr,struct PCB, head);
        if (pcb->pid == pid) {
            return pcb;
        }
    }
    list_foreach(ptr,&block) {
        pcb = list_entry(ptr,struct PCB,head);
        if (pcb->pid == pid) {
            return pcb;
        }
    }
    return NULL;
}
