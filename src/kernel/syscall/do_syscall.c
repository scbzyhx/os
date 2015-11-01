#include "do_syscall.h"
#include "fs.h"
// asm volatile("int $0x80":"=a"(ret): "a"(args[0]),"b"(args[1]),"c"(args[2]),"d"(args[3]));
/*
for all:
AX is ID;


SYS_READ: BX is file name, CX address of buffer, DX  is len
SYS_CALL: no other parameter, return the pid of new thread
 */

pid_t SYS_fork();
void do_syscall(TrapFrame *tf) {
    int id = tf->eax;
    switch(id) {
        case SYS_READ:
            {
                int fd = tf->ebx;
                uint8_t *buf = (uint8_t *)tf->ecx;
                uint32_t len = tf->edx;

                tf->eax = do_read(fd,buf,0,len);
            }
            break;
        case SYS_WRITE:
            printk("testing SYS_WRITE OK\n");
            tf->eax = 0;
            break;
        case SYS_FORK:
            tf->eax = SYS_fork();
            break;
        case SYS_EXEC:
            break;
        case SYS_SCHEDULE:
            //do nothing here
            break; 
    }
}

pid_t SYS_fork() {

    return 0;

}
