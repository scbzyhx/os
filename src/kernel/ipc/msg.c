#include "kernel.h"
#include "string.h"
#define MAX_MSG 10240


void print_msg(Msg*);
void memcpy(void *, const void *, size_t );
/*invoked locked
 */
void _send(struct PCB *pcb,Msg *msg) {
    //lock();
    Msg *pmsg = NULL;
    ListHead *plist = NULL;

    /*pick one struct Msg*/
    if(list_empty(&(pcb->msg_free) )==true) {
       // unlock();
        return;
    }
    plist = pcb->msg_free.next;
    list_del(plist);
    pmsg = list_entry(plist, Msg,list);

    //copy
    memcpy((void*)pmsg,(const void*)msg,sizeof(Msg));
//    printk("pcb pid = %x, msg_list addr = %x, prev=%x\n",pcb->pid,&pcb->msg_list,pcb->msg_list.prev);
    list_add_after(pcb->msg_list.prev,&(pmsg->list));
//    printk("addr of msg=%x\n",&(msg->list));
//    printk("pcb pid = %x, msg_list addr = %x, next=%x\n",pcb->pid,&pcb->msg_list,pcb->msg_list.next->next);
    //unlock();
    return;
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
    //lock();
    if(list_empty(&(current->msg_list))) {
        //printk("empty\n");
        sleep();
    }
    //if(current->msg_list.next == NULL) {
        

    {
//        printk("receive\n");
//        printk("current->pid=%x,next = %x,addr of msg_list= %x\n",current->pid,current->msg_list.next,current->msg_list);
//        printk("judge empty=%x\n",list_empty(&(current->msg_list)));
    }

    //printk("not empty next = %x, msg_list = %x\n,",current->msg_list.next,current->msg_list);
    while(1) {
        list_foreach(ptr,&(current->msg_list)) {
            pmsg = list_entry(ptr,Msg,list);
            
            //printk("pmsg->src = %d and src = %d\n",pmsg->src,src);
            //if ANY, get the first one
            if (pmsg->src == src || src == ANY) {
                memcpy((void*)msg,(void*)pmsg,sizeof(Msg));
               // printk("true condition\n");
                //but how to free msgs
//                printk("in receve the addr of msg=0x%x\n",pmsg);
                //print_msg(pmsg);
//                printk("pid = %x, msg_list add = %x, next=%x\n",current->pid,&current->msg_list,current->msg_list.next);
                list_del(ptr);
                //move to free
                list_add_after(&(current->msg_free),ptr);
                //unlock(); //before return ,remember to unlock
                return;
            }
        }
        sleep();
    }
    //unlock();
    

}

void receive(pid_t src, Msg *msg) {
    lock();
    _receive(src,msg);    
    unlock();
}
void print_msg(Msg *msg) {
//    printk("src=%d,dest=%d\n",msg->src,msg->dest);
//    printk("type or ret = %d\n",msg->type);
//    printk("union array i is %x, %x, %x, %x, %x\n",msg->i[0],msg->i[1],msg->i[2],msg->i[3],msg->i[4]);
}
Msg* get_free_msg(struct ListHead* lh) {
    struct ListHead *ptr;
    if(lh->next == lh) return NULL; //no more free msg
    ptr = lh->next;
    list_del(ptr);
    return list_entry(ptr,Msg,list);
}
void insert_msg(struct ListHead *lh, Msg *msg) {
    list_add_after(lh->prev,&msg->list);
    return;

}
