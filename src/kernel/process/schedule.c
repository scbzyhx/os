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
void just_for_test(){};
void
schedule(void) {
	/* implement process/thread schedule here */
	//PCB *last = current;
	//PCB *next = NULL;
	//printk("schedule\n");
	/*
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
    */
    //new scheduling here
//    printk("new scheduler, pid = %d\n",current->pid);
    //printk("new scheduler, state=%d\n",current->state);
    if(current->state == TASK_DEAD) {
        list_add_after(free.prev,&current->head);
        printk("task_dead, pid = %d\n",current->pid);
        current->state = TASK_EMPTY;
        current = NULL;
    }else if(current->state == TASK_BLOCKED) {
	    list_add_after(block.prev,&current->head);
        current = NULL;
    }else if(current == &idle) {
        current->state = TASK_READY;
        current = NULL;
    }else {
        //current->state == TASK_RUNNING
       // printk("state = %d\n",current->state);
        assert(current->state == TASK_RUNNING);
        current->counter -= 1;
        if(current->counter == 0) {
            current->state = TASK_READY;
            current->counter = MAX_TIME_SLOT;
            list_add_after(ready.prev,&current->head);
            current = NULL;
        }else 
            //no scheduling, just return
            return ;
    }

    if(ready.next == &ready)
        current = &idle;
    else {
        current = list_entry(ready.next,PCB,head);
        list_del(&current->head);
        
    }
    current->state = TASK_RUNNING;
    if(current->cr3.val == 0) {
        write_cr3(get_kcr3());
    }else {
        write_cr3(&(current->cr3));
    }
    if(current->pid == 19){
        just_for_test();
    }

  //  printk("after new scheduler, pid = %d\n",current->pid);
   // printk("after new scheduler, state=%d\n",current->state);


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
