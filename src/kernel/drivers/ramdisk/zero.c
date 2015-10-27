#include "kernel.h"
#include "hal.h"
#include "kcpy.h"


pid_t ZERO;

static void zero_thread(void);

void init_zero(void) {
    //create thread
    printk("init_zero\n");
    PCB *p = create_kthread(zero_thread);
    ZERO = p->pid;
    hal_register("zero",ZERO,0);
    wakeup(p);
}

static void zero_thread(void) {
	static Msg m;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
		    //do nothting
		} else if (m.type == DEV_READ) {
		    uint32_t i;
		    uint8_t *p = (uint8_t*)m.buf;
		    for(i=0;i<m.len;++i) {
		        p[i] = '\0';
            }
			m.ret = m.len;
			m.dest = m.src;
			m.src = ZERO;
			send(m.dest, &m);
		} else if (m.type == DEV_WRITE) {
			m.ret = 0;
			m.dest = m.src;
			m.src = ZERO;
			send(m.dest, &m);
		}
		else {
			assert(0);
		}
	}
}
