#include "kernel.h"
#include "hal.h"
#include "kcpy.h"

#define NR_MAX_FILE 64
#define NR_FILE_SIZE (128*1024)

static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE] = {
    {0x12,0x34,0x56,0x78}, //the first file '0'
    {"Hello world\n"},     //the first file '1'
};
static uint8_t *disk = (void*)file;

pid_t RAM;

static void ram_thread(void);

void init_ram(void) {
    //create thread
    printk("init_ram\n");
    PCB *p = create_kthread(ram_thread);
    RAM = p->pid;
    hal_register("ram",RAM,0);
    wakeup(p);
}

static void ram_thread(void) {
	static Msg m;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
		    //do nothting
		} else if (m.type == DEV_READ) {
			copy_to_kernel(fetch_pcb(m.req_pid),&m.buf,&disk[m.offset],m.len);
			m.ret = m.len;
			m.dest = m.src;
			m.src = RAM;
			send(m.dest, &m);
		} else if (m.type == DEV_WRITE) {
			copy_to_kernel(fetch_pcb(m.req_pid),&disk[m.offset],&m.buf,m.len);
			m.ret = m.len;
			m.dest = m.src;
			m.src = RAM;
			send(m.dest, &m);
		}
		else {
			assert(0);
		}
	}
}
