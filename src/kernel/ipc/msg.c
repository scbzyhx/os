#include "kernel.h"
#include "string.h"

void print_msg(Msg*);
/*invoked locked
 */
void _send(struct PCB *pcb,Msg *msg) {
    lock();
    list_add_after(pcb->msg_list.prev,&(msg->list));
    unlock();
}

void send(pid_t dest, Msg *msg) {
    struct PCB *pcb = fetch_pcb(dest);
    
    if(msg == NULL) {
        printk("msg is NULL\n");
        return ;
    }
    msg->dest = dest;

    /*
      pcb is NULL ,should do something
     */
    if (!pcb )
        return;
    lock();
    if (pcb->state == TASK_EMPTY) {
        //do something ,maybe set wrong or other
        //but now do nothing
        printk("destination error\n");
        unlock();
        return ;
    }
    _send(pcb,msg);
    wakeup(pcb);
    unlock();

    


}
void _receive(pid_t src,Msg *msg) {
    
    Msg *pmsg;
    struct ListHead *ptr;
    if(list_empty(&(current->msg_list))) {
        //printk("empty\n");
        sleep();
    }
    //printk("not empty\n");
    while(1) {
        list_foreach(ptr,&(current->msg_list)) {
            pmsg = list_entry(ptr,Msg,list);
            
            //printk("pmsg->src = %d and src = %d\n",pmsg->src,src);
            //if ANY, get the first one
            if (pmsg->src == src || src == ANY) {
                memcpy((void*)msg,(void*)pmsg,sizeof(Msg));
               // printk("true condition\n");
                //but how to free msgs
                list_del(ptr);
                return;
            }
        }
        sleep();
    }
    

}

void receive(pid_t src, Msg *msg) {
    lock();
    _receive(src,msg);    
    unlock();
}
void print_msg(Msg *msg) {
    printk("src=%d,dest=%d\n",msg->src,msg->dest);
    printk("type or ret = %d\n",msg->type);
    printk("union array i is %x, %x, %x, %x, %x\n",msg->i[0],msg->i[1],msg->i[2],msg->i[3],msg->i[4]);
}
