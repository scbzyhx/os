#ifndef __DO_SYSCALL_H_
#define __DO_SYSCALL_H_
#include "kernel.h"

#define SYS_READ             1
#define SYS_WRITE            2
#define SYS_EXEC             3
#define SYS_FORK             4
#define SYS_SCHEDULE         5

#define SYS_EXIT             6
#define SYS_WAITPID          7

#define SYS_PUTS             8
#define SYS_GETPID           9

void do_syscall(TrapFrame*);


#endif
