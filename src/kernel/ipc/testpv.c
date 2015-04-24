#include "kernel.h"
#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4

int buf[NBUF],f=0,r=0,g=1;
int last = 0;
Sem empty,full,mutex_r,mutex_w;

void test_producer(void) {
    //printk("in test_producer\n");
    while(1) {
        //printk("in test_producer\n");
        //printk("counter=%d\n",mutex.token);
        P(&mutex_w);
        P(&empty);
     //   printk("in test_producer\n");
        //P(&mutex);
        if(g % 10000 ==0) {
            printk(".");
        }
        buf[f++] = g++;
        f %= NBUF;
        V(&full);

 //       printk("in producer\n");
        V(&mutex_w);
        //printk("in producer=%d\n",mutex.token);
    }
}
void test_consumer(void) {
    int get;
    while(1) {
        //printk("in test_consumer=%d\n",mutex.token);
        P(&mutex_r);
        //printk("in test_consumer=%d\n",empty.token);
        P(&full);
        //printk("in test_consumer=%d\n",empty.token);
        get = buf[r++];
        assert(last == get -1 );
        last = get;
        r %= NBUF;
        V(&empty);
        V(&mutex_r);
    }
}
void test_setup(void) {
    init_sem(&full,0);
    init_sem(&empty,NBUF);
    init_sem(&mutex_r,1);
    init_sem(&mutex_w,1);
    int i;
    for(i=0;i<NR_PROD;i++) {
        wakeup(create_kthread(test_producer));
    }
    for(i=0;i<NR_CONS;i++) {
        wakeup(create_kthread(test_consumer));
    }
}
