#include "kernel.h"
#include "hal.h"
#include "kcpy.h"


pid_t KMEM_PID;

static void kmem_thread(void);

void init_kmem(void) {
    //create thread
    printk("init_kmem\n");
    PCB *p = create_kthread(kmem_thread);
    KMEM_PID = p->pid;
    hal_register("kmem",KMEM_PID,0);
    wakeup(p);
}

static void kmem_thread(void) {
	static Msg m;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
		    //do nothting
		} else if (m.type == DEV_READ) {
		    uint8_t *pchar = (uint8_t*)(KOFFSET + m.offset);

			copy_to_kernel(fetch_pcb(m.req_pid),m.buf,pchar,m.len);
		    
			m.ret = m.len;
			m.dest = m.src;
			m.src = KMEM_PID;
			send(m.dest, &m);
		} else if (m.type == DEV_WRITE) {
			m.ret = 0;
			m.dest = m.src;
			m.src = KMEM_PID;
			send(m.dest, &m);
		}
		else {
			assert(0);
		}
	}
}
