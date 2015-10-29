#include "kernel.h"
#include "hal.h"

#define NR_FILE 64
#define FILE_SIZE (128*1024)

#define FS_READ 1
#define FS_WRITE 2

pid_t FS;

static void fs_thread();

void init_fs() {
    PCB *p = create_kthread(fs_thread);

    FS = p->pid;
    hal_register("fs",FS, 0);
    wakeup(p);
}


static void fs_thread(void) {
	static Msg m;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
		    //do nothting
		} else if (m.type == FS_READ) {

            size_t sz =  dev_read("ram", current->pid, m.buf, m.offset, m.len);
			m.ret = sz;
			m.dest = m.src;
			m.src = FS;
			send(m.dest, &m);

		} else if (m.type == FS_WRITE) {
		    assert(0);
        }
		else {
			assert(0);
		}
	}
}

size_t do_read(int file_name, uint8_t *buf, off_t offset, size_t len) {

    if(file_name >= NR_FILE) {
        /*I should do sth here
         */
        return -1;
    }

    len = offset+len >= FILE_SIZE ? FILE_SIZE - offset: len;

	off_t oft = file_name * FILE_SIZE + offset;

    size_t sz = dev_read("fs",current->pid, buf, oft, len);
	return sz;
}
