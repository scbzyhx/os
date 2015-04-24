#include "kernel.h"

PCB idle, *current = &idle;
PCB pcbPool[PCB_NUM];
ListHead ready ,block,free;
pid_t global_pid = 5; //idle.id = 0

/*#define switchTo(next) \
    asm( \
            "movl %0,%%esp\t\n" \
            "popal\t\n" \
            "popl %%gs\t\n" \
            "popl %%fs\t\n" \
            "popl %%es\t\n" \
            "popl %%ds\t\n" \
            "addl $8,%%esp\t\n" \
            "iret \t\n"\
        ::"r"(next->tf) );*/
/*
   schedule is invoked every IDT
 */
void
schedule(void) {
	/* implement process/thread schedule here */
	//PCB *last = current;
	//PCB *next = NULL;
	//printk("schedule\n");
	if(current->state == TASK_BLOCKED) {
	    list_add_after(block.prev,&current->head);
        current = NULL;
    }else if(current->state != TASK_DEAD && current != &idle) {
	    list_add_after(ready.prev,&current->head);
    } else if(current->state == TASK_DEAD) {
        list_add_after(free.prev,&current->head);
        current->state = TASK_EMPTY;
    }


    if(ready.next == &ready) 
        current = &idle;
    else {
        current = list_entry(ready.next,PCB,head);
        list_del(&current->head);
    }

	/*
	if(current->state == TASK_STOPED)
	{
	    //printk("stoped\n");
        list_add_after(free.prev,&current->head);
    }else if(last != &idle)
        list_add_after(ready.prev,&current->head);


    if(ready.next == &ready)
    {
        next = &idle;        
    }else
    {
        next = list_entry(ready.next,PCB,head);
        //current = list_entry(ready.next);
        list_del(&next->head);
    }
    if(next == NULL)
        next = &idle; //this should not happened
    //printk("hello world");
    current = next;
    */
 //switchTo(current);
    
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
