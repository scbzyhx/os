#include "kernel.h"
#include "x86/x86.h"
#include "hal.h"
#include "time.h"
#include "string.h"
#include "timer.h"

#define PORT_TIME 0x40
#define PORT_RTC  0x70
#define FREQ_8253 1193182

#define MAX_TIMER 64

typedef struct timer{
    pid_t pid;
    int time_out; //when reach 0,return message
    Msg msg; //to store the reply msg
    ListHead list;
} timer;

pid_t TIMER;
static long jiffy = 0;
static Time rt;

//static ListHead timer_free, timing;
//static timer timers[MAX_TIMER];

static void update_jiffy(void);
static void init_i8253(void);
static void init_rt(void);
static void timer_driver_thread(void);
//static void init_timers(void);
//static void send_to_timer(void);
/*
void init_timers(void) {
    int i = 0;
    list_init(&timer_free);
    list_init(&timing);

    for(;i<MAX_TIMER;++i) {
        timers[i].pid = 0;
        timers[i].time_out = 0;
        list_add_after(timer_free.prev,&timers[i].list);
    }

}
*/
void init_timer(void) {
	init_i8253();
	init_rt();
	add_irq_handle(0, update_jiffy);
	PCB *p = create_kthread(timer_driver_thread);
	TIMER = p->pid;

    //init_timers();

	wakeup(p);
	hal_register("timer", TIMER, 0);
}

static void
timer_driver_thread(void) {
	static Msg m;
	//struct timer *ptimer;
	//struct ListHead *l;
	
	while (true) {
	//	receive(ANY, &m);
		
		switch (m.type) {
 /*           case NEW_TIMER:
                
                if(list_empty(&timer_free)) {
                    printk("no timer is empty\n");
                    assert(0);
                    return;
                }
                
                l = timer_free.next;
                assert(l != NULL);
                ptimer = list_entry(l,struct timer,list);
                ptimer->pid = m.src;
                ptimer->time_out = m.i[0];
                list_add_after(timing.prev,&(ptimer->list));

                break;
            case TIME_OUT:
                //not here
                break;
            case ONE_SECOND:
                list_foreach(l,&timing) {
                    ptimer = list_entry(l,struct timer,list);
                    ptimer->time_out -= 1;
                    if(ptimer->time_out < 0) {

                    }
                }
                break;*/
			//default: assert(0);
		}
	}
}

long
get_jiffy() {
	return jiffy;
}

static int
md(int year, int month) {
	bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
	static int tab[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	return tab[month] + (leap && month == 2);
}

static void
update_jiffy(void) {
	jiffy ++;
	if (jiffy % HZ == 0) {
		rt.second ++;
		//
		//notify that a second passed
		//send_to_timer();
		//
		if (rt.second >= 60) { rt.second = 0; rt.minute ++; }
		if (rt.minute >= 60) { rt.minute = 0; rt.hour ++; }
		if (rt.hour >= 24)   { rt.hour = 0;   rt.day ++;}
		if (rt.day >= md(rt.year, rt.month)) { rt.day = 1; rt.month ++; } 
		if (rt.month >= 13)  { rt.month = 1;  rt.year ++; }
	}
}

static void
init_i8253(void) {
	int count = FREQ_8253 / HZ;
	assert(count < 65536);
	out_byte(PORT_TIME + 3, 0x34);
	out_byte(PORT_TIME, count & 0xff);
	out_byte(PORT_TIME, count >> 8);	
}

static void
init_rt(void) {
	memset(&rt, 0, sizeof(Time));
	/* Optional: Insert code here to initialize current time correctly */

}

/*
   This method may result timer loss,
   but without memory managment, so just temporialy be so
 */
/*static void send_to_timer(void) {
    static Msg msg;
    msg.src = -2; //how to indicat that it is an interupt
    msg.dest = TIMER;
    msg.type = ONE_SECOND;

    send(TIMER,&msg);
    
    
}
*/
void 
get_time(Time *tm) {
	memcpy(tm, &rt, sizeof(Time));
}
