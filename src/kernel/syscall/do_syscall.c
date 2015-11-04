#include "kernel.h"
#include "do_syscall.h"
#include "fs.h"
#include "msg.h"
// asm volatile("int $0x80":"=a"(ret): "a"(args[0]),"b"(args[1]),"c"(args[2]),"d"(args[3]));
/*
for all:
AX is ID;


SYS_READ: BX is file name, CX address of buffer, DX  is len
SYS_CALL: no other parameter, return the pid of new thread
SYS_EXEC: 
SYS_PUTS: bx: address of string
 */
void for_test(){}
pid_t SYS_fork();
int SYS_exec(char*file, int argc, char* argv[]);
void SYS_exit(int exit_code);
int SYS_getpid();
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
            {
                //copy_from_user_to_kernel
                tf->eax = SYS_exec(NULL,0,NULL);
            }
            break;
        case SYS_SCHEDULE:
            //do nothing here
            //printk("In SYS_SCHEDULE\n");
            break; 
        case SYS_GETPID:
            {
                tf->eax = SYS_getpid();
                
            }
            break;
        case SYS_EXIT:
            {
                int exit_code = tf->ebx;
                SYS_exit(exit_code);
            }
            break;
        case SYS_PUTS:
            {
                char buf[256];
                char *p = (char*)tf->ebx;
                int i = 0;
                buf[255] = '\0';
                while(*p != '\0') {
                    
                    if(i == 255) {
                        printk(buf);
                        i = 0;
                    }

                    buf[i++] = *p++;
                }
                buf[i] = '\0';
                printk(buf);
                
                tf->eax = 0;
            }
            break;
        default:
            printk("unkown system call\n");
    }
}

pid_t SYS_fork() {
    //send to process manager
    Msg m;
    m.src = current->pid;
    printk("%x\n",&m);
   
    return 0;

}

int SYS_exec(char*file, int argc, char* argv[]) {
    return 0;
}
void SYS_exit(int exit_code) {
    
}

int SYS_getpid() {

    return current->pid;
}




