#include "hal.h"

void init_hal();
void init_timer();
void init_tty();
void init_ide();
void init_ram();
void init_zero();
void init_kmem();
void init_mm();
void init_fs();
void init_pm();

/*all these functions would enable interupts*/
void init_driver() {
	init_hal();
	init_timer();
	init_tty();
	init_ide();

	init_zero();
	init_ram();
	init_kmem();
	init_mm();
	init_fs();
	init_pm();

	hal_list();
}
