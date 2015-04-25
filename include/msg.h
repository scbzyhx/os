#ifndef __MSG_H__
#define __MSG_H__
#include "kernel.h"

//for src 
#define ANY -1

typedef struct Message {
    pid_t src,dest;
    union {
        int type;
        int ret;
    };
    union {
        int i[5];
        struct {
            pid_t req_pid;
            int dev_id;
            void *buf;
            off_t offset;
            size_t len;
        };
    };
    ListHead list;
} Msg;

//asynchronized,allowed in intterupt context
void send(pid_t, Msg *);
//synchronized
void receive(pid_t,Msg *);
void printk_msg(Msg*);
#endif
