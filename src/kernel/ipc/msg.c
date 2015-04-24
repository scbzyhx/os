#include "kernel.h"
#include "string.h"
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
        sleep();
    }
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
