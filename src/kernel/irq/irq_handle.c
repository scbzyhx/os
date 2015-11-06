#include "kernel.h"

#define NR_IRQ_HANDLE 32

/* In Nanos, there are no more than 16(actually, 3) hardward interrupts. */
#define NR_HARD_INTR 16

/* Structures below is a linked list of function pointers indicating the
   work to be done for each interrupt. Routines will be called one by
   another when interrupt comes.
   For example, the timer interrupts are used for alarm clocks, so
   device driver of timer will use add_irq_handle to attach a simple
   alarm checker. Also, the IDE driver needs to write back dirty cache
   slots periodically. So the IDE driver also calls add_irq_handle
   to register a handler. */

struct IRQ_t {
	void (*routine)(void);
	struct IRQ_t *next;
};

static struct IRQ_t handle_pool[NR_IRQ_HANDLE];
static struct IRQ_t *handles[NR_HARD_INTR];
static int handle_count = 0;

void
add_irq_handle(int irq, void (*func)(void) ) {
	struct IRQ_t *ptr;
	assert(irq < NR_HARD_INTR);
	if (handle_count > NR_IRQ_HANDLE) {
		panic("Too many irq registrations!");
	}
	ptr = &handle_pool[handle_count ++]; /* get a free handler */
	ptr->routine = func;
	ptr->next = handles[irq]; /* insert into the linked list */
	handles[irq] = ptr;
}
void schedule();
void do_syscall(TrapFrame *tf);
uint32_t pcb_va_to_pa(PCB *pcb, uint32_t va);
void irq_handle(TrapFrame *tf) {
	int irq = tf->irq;
	current->tf = tf;

	if (irq < 0) {
		panic("Unhandled exception!");
	}

	if (irq < 1000) {
		extern uint8_t logo[];
	    //printk("IRQ = %d \n ", irq);
	    int cr2;
		switch(irq) {
            case 0x80:
                do_syscall(tf);
                break;
            case 14:
                asm volatile("movl %%cr2, %0":"=r"(cr2));
                //printk("%x\n",pcb_va_to_pa(current,cr2));
                printk("page fault address = %x\n",cr2);
                printk("current->pid=%d\n",current->pid);
		        printk("irq = 14,eip = %x\n",tf->eip);
		        printk("irq=14, current->cr3 = %x\n",current->cr3.val);
		        panic("Unexpected exception #%d\n\33[1;31mHint: The machine is always right! For more details about exception #%d, see\n%s\n\33[0m", irq, irq, logo);
		        break;

            default:
		        panic("Unexpected exception #%d\n\33[1;31mHint: The machine is always right! For more details about exception #%d, see\n%s\n\33[0m", irq, irq, logo);
                break;
        }
		//printk("%x\n",irq);
		//if(irq != 0x80)
		//panic("Unexpected exception #%d\n\33[1;31mHint: The machine is always right! For more details about exception #%d, see\n%s\n\33[0m", irq, irq, logo);
	} else if (irq >= 1000) {
		/* The following code is to handle external interrupts.
		 * You will use this code in Lab2.  */
		int irq_id = irq - 1000;
		assert(irq_id < NR_HARD_INTR);
		struct IRQ_t *f = handles[irq_id];

		while (f != NULL) { /* call handlers one by one */
			f->routine(); 
			f = f->next;
		}
	}

	//current->tf = tf;
	schedule();
}

