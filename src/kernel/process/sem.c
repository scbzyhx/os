#include "kernel.h"
void P(Sem *s) {
    lock();
    s->token --;
    if(s->token >=0 ) {
        //with token

    }else {
        //no token
    }

    unlock();

}
void V(Sem *s) {
    lock();

    unlock();
}

//disable intterupt
void lock() {
    asm volatile("cli");
    
    // to protect recursive lock (cli)
    ++cli_lock_times;
}

//enable intterupt
void unlock() {
    //intterrupt is disabled
    --cli_lock_times;
    if(cli_lock_times == 0)
        asm volatile("sti");
    //else do nothing

}
