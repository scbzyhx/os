#include "kernel.h"

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
    PCB *p = create_kthread(fs_thread);
    RAM = p->pid;
    hal_register("ram",FS,0);
    wakeup(p);
}

static void ram_thread(void) {
	static Msg m;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
			if (m.type == IDE_WRITEBACK) {
				cache_writeback();
			} else {
				panic("IDE interrupt is leaking");
			}
		} else if (m.type == DEV_READ) {
			uint32_t i;
			uint8_t data;
			for (i = 0; i < m.len; i ++) {
				data = read_byte(m.offset + i);
				copy_from_kernel(fetch_pcb(m.req_pid), m.buf + i, &data, 1);
			}
			m.ret = i;
			m.dest = m.src;
			m.src = IDE;
			send(m.dest, &m);
		} else if (m.type == DEV_WRITE) {
			uint32_t i;
			uint8_t data;
			for (i = 0; i < m.len; i ++) {
				copy_to_kernel(fetch_pcb(m.req_pid), &data, m.buf + i, 1);
				write_byte(m.offset + i, data);
			}
			m.ret = i;
			m.dest = m.src;
			m.src = IDE;
			send(m.dest, &m);
		}
		else {
			assert(0);
		}
	}
}
