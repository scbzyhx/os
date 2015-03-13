#include "kernel.h"

PCB idle, *current = &idle;
PCB pcbPool[PCB_NUM];
ListHead ready ,block,free;

//esp = tf
//popal
//popl gs,fs,es,ds
//popl irq
//pop error
//iret
#define switchTo(next) \
    asm( \
            "movl %0,%%esp\t\n" \
            "popal\t\n" \
            "popl %%gs\t\n" \
            "popl %%fs\t\n" \
            "popl %%es\t\n" \
            "popl %%ds\t\n" \
            "addl $8,%%esp\t\n" \
            "iret \t\n"\
        ::"r"(next->tf) );

void
schedule(void) {
    //printk("%d\n",ready.next==&ready);
    //printk("%d\n",(uint32_t)ready.next);
   // return;
	/* implement process/thread schedule here */
	if(current->state == TASK_STOPED)
        list_add_after(free.prev,&current->head);
    if(ready.next == &ready)
        return;
    else
    {
        //current = list_entry(ready.next,PCB,head);
        current = (PCB*)ready.next;
        list_del(&current->head);
    }
    switchTo(current);
    
}

//esp = tf
//popal
//pop gf
//pop fs
//pop es
//pop ds
//pop irq
//pop erro_code
//iret
