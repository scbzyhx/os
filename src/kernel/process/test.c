#include "kernel.h"

void A();
void B();
void C();
void D();
PCB *PCB_of_thread_A;
PCB *PCB_of_thread_B;
PCB *PCB_of_thread_C;
PCB *PCB_of_thread_D;

void 
test_proc() {

    PCB_of_thread_A = create_kthread(A);
    PCB_of_thread_B = create_kthread(B);
    PCB_of_thread_C = create_kthread(C);
    PCB_of_thread_D = create_kthread(D);
    set_kthread_state(PCB_of_thread_A,TASK_RUNNING);
    set_kthread_state(PCB_of_thread_B,TASK_BLOCKED);
    set_kthread_state(PCB_of_thread_C,TASK_BLOCKED);
    set_kthread_state(PCB_of_thread_D,TASK_BLOCKED);
    
}


void A() {
    int x = 0;
    while(1) {
        if(x %10000 == 0) {
            printk("a");
            wakeup(PCB_of_thread_B);
            sleep();
        }
        x++;
    }
}


void B() {
    int x = 0;
    while(1) {
        if(x %10000 == 0) {
            printk("b");
            wakeup(PCB_of_thread_C);
            sleep();
        }
        x++;
    }
}

void C() {
    int x = 0;
    while(1) {
        if(x %10000 == 0) {
            printk("c");
            wakeup(PCB_of_thread_D);
            sleep();
        }
        x++;
    }
}

void D() {
    int x = 0;
    while(1) {
        if(x %10000 == 0) {
            printk("d");
            wakeup(PCB_of_thread_A);
            sleep();
        }
        x++;
    }
}

