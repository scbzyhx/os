#include "kernel.h"
/*
   In fact, it shoud not be defined here
 */
void init_sem(Sem *s,int count) {
    s->token = count;
    list_init(&(s->block));
}
void P(Sem *s) {
    lock();
    s->token --;
    if(s->token <0 ) {
        //without token
        list_add_after(s->block.prev,&current->sem_list);
        sleep();
        /*
           sleep cause process switch, and eflags changed acorrdingly, so it almost probably 
           cli the IF in eflags, so interrupt may be enabled.
         */
    } 
    unlock();

}
void V(Sem *s) {
    lock();
    //wakeup()
    s->token++;
    if( ! list_empty(&s->block)) {
        PCB *tmp = list_entry(s->block.next, struct PCB,sem_list); 
        wakeup(tmp);
        list_del(&(tmp->sem_list));
    }
    
    unlock();
}

//disable intterupt
void lock() {
    if(current->intr_counter == 0) {
        uint32_t eflag;
        asm volatile("pushf\n\t" "pop %0\n\t":"=r"(eflag));
#define IF_FLAG 0x200
        current->enabled = (eflag & IF_FLAG ) >> 9;
    }
    asm volatile("cli");
    // to protect recursive lock (cli)
    current->intr_counter++;
}

//enable intterupt
void unlock() {
    //intterrupt should be disabled
    if(--(current->intr_counter) == 0 && current->enabled != 0) 
        asm volatile("sti");
    //else do nothing

}
