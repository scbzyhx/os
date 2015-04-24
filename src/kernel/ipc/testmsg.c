#include "kernel.h"
pid_t pidE,pidA,pidB,pidC,pidD;
void A () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	while(1) {
		if(x % 10000000 == 0) {
			printk("a"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void B () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(pidE, &m2);
	while(1) {
		if(x % 10000000 == 0) {
			printk("b"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void C () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(pidE, &m2);
	while(1) {
		if(x % 10000000 == 0) {
			printk("c"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void D () { 
	Msg m1, m2;
	m1.src = current->pid;
	receive(pidE, &m2);
	int x = 0;
	while(1) {
		if(x % 10000000 == 0) {
			printk("d"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
 
void E () {
	Msg m1, m2;
	m2.src = current->pid;
	char c;
	while(1) {
		receive(ANY, &m1);
		if(m1.src == pidA) {c = '|'; m2.dest = pidB; }
		else if(m1.src == pidB) {c = '/'; m2.dest = pidC;}
		else if(m1.src == pidC) {c = '-'; m2.dest = pidD;}
		else if(m1.src == pidD) {c = '\\';m2.dest = pidA;}
		else assert(0);
 
		printk("\033[s\033[1000;1000H%c\033[u", c);
		send(m2.dest, &m2);
	}
 
}
void test_msg() {
    struct PCB *pcba = create_kthread(A);
    struct PCB *pcbb = create_kthread(B);
    struct PCB *pcbc = create_kthread(C);
    struct PCB *pcbd = create_kthread(D);
    struct PCB *pcbe = create_kthread(E);
    pidA = pcba->pid;
    pidB = pcbb->pid;
    pidC = pcbc->pid;
    pidD = pcbd->pid;
    pidE = pcbe->pid;
    printk("init test_msg,pidA=%d,pidB=%d,pidC=%d,pidD=%d,pidE=%d\n",pidA,pidB,pidC,pidD,pidE);

    wakeup(pcba);
    wakeup(pcbb);
    wakeup(pcbc);
    wakeup(pcbd);
    wakeup(pcbe);



}
