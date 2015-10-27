#include "kernel.h"
#include "hal.h"

pid_t READ_KMEM;

//size_t dev_read(const char *dev_name, pid_t reqst_pid, void *buf ,off_t offset, size_t len);
void kmem_read_test_thread() {
    //do something here
    uint8_t buf[1024];
    size_t size = dev_read("kmem",READ_KMEM,buf,0x7c00,512);
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
    }
    
}

void init_kmem_read_test(void) {
    PCB *p = create_kthread(kmem_read_test_thread);
    READ_KMEM = p->pid;
    wakeup(p);

}
