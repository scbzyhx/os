#include "do_syscall.h"

int __attribute__((__noinline__))
syscall(int id, ...) {
    int ret;
    int  *args = &id;
    asm volatile("int $0x80": "=a"(ret): "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]));

    return ret;
}

void puts(char *s) {
    syscall(SYS_PUTS,s);
}

int getpid(){
    return syscall(SYS_GETPID);
}
