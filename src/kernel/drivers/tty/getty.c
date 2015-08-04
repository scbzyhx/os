#include "kernel.h"
#include "tty.h"
#include "hal.h"

static int tty_idx = 1;

static void
getty(void) {
	char name[] = "tty0", buf[256];
	lock();
	name[3] += (tty_idx ++);
	unlock();
	//buf[0] = 't';
	pid_t req_id = 0;
	off_t offset = 0;
	size_t size = 0;

	while(1) {
		/* Insert code here to do these:
		 * 1. read key input from ttyd to buf (use dev_read())
		 * 2. convert all small letters in buf into capitcal letters
		 * 3. write the result on screen (use dev_write())
		 */
		size = dev_read(name,req_id,buf,offset,256);
		offset += size;

		dev_write(name,req_id,buf,0,size);
		req_id++;

		printk("just for use buf yhx%c\n",buf[0]);

	}
}

void
init_getty(void) {
	int i;
	for(i = 0; i < NR_TTY; i ++) {
		wakeup(create_kthread(getty));
	}
}


