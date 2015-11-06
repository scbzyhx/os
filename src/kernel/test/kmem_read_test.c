#include "kernel.h"
#include "hal.h"
#include "fs.h"
#include "mm.h"

pid_t READ_KMEM;
#define VA 0x08048000
//size_t dev_read(const char *dev_name, pid_t reqst_pid, void *buf ,off_t offset, size_t len);
void kmem_read_test_thread() {
    //do something here
   /* uint8_t buf[129*1024];
    size_t size = do_read(0,buf,0,749);
    uint32_t i;
    printk("size = %d\n",size);
    uint32_t count = 0;
    uint32_t tmp;
    for(i=0;i<size;++i)  {
        if(count == 0) {
            printk("\n"); 
            count =( count + 1) %10;
        }
        tmp = buf[i];
        printk("%x ",tmp);
    }*/
    /*now test memory allocation*/
    //printk("before alloc: pid=%d,state=%d\n",current->pid,current->state);
    int tmp,tmp2;
    printk("before create_thread\n");
    uint32_t file[] = {6,0};
    tmp = create_thread(file);
    printk("tmp=%d\n",tmp);
    //wakeup(fetch_pcb(tmp));
    file[0] = 5;
    tmp2 = create_thread(file); 
    printk("second user proces:%d\n",tmp2);
    wakeup(fetch_pcb(tmp));
    wakeup(fetch_pcb(tmp2));
    
}

void init_kmem_read_test(void) {
    printk("initing test thread\n");
    PCB *p = create_kthread(kmem_read_test_thread);
    printk("init_kmem: pid=%d,state=%d\n",p->pid,p->state);
    READ_KMEM = p->pid;
    wakeup(p);

}
