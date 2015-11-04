#include "kernel.h"
#include "hal.h"
#include "elf.h"
#include "mm.h"
#include "fs.h"
#include "string.h"
#include "pm.h"


#define FILE_SIZE (128*1024)

pid_t PM;
static void  pm_thread();
static pid_t _do_fork(PCB*pcb);

static uint8_t buf[FILE_SIZE];

PCB* create_process(uint8_t *buf);
void init_pm() {
    PCB *p = create_kthread(pm_thread);

    PM = p->pid;
    hal_register("pm",PM,0);
    wakeup(p);

}

void pm_thread() {
    static Msg m;
    PCB* pcb;
	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
		    //do nothting
		} else if (m.type == NEW_PROCESS) {

            //TODO: only first file here, I should change it 
            uint32_t *file_name_buf = (uint32_t*)m.buf;

            size_t sz =  do_read(file_name_buf[0], buf, 0, FILE_SIZE);
            assert(sz == FILE_SIZE);
            pcb = create_process(buf);
			m.ret = pcb->pid;
			m.dest = m.src;
			m.src = PM;
			send(m.dest, &m);

		} else if (m.type == FORK) {
		    PCB *old = fetch_pcb(m.req_pid);
		    pid_t new_pid = _do_fork(old);
		    m.ret = new_pid;
		    m.dest = m.src;
		    m.src = PM;
		    send(m.dest,&m);
		    //for child process
		    m.ret = 0;
		    m.dest = new_pid; //send to new_pid
		    m.src = PM;
		    send(m.dest,&m);
        }
		else {
			assert(0);
		}
	}
}

static inline uint32_t to_next_page(uint32_t addr) {
    return ((addr & 0xfffff000) + 0x00001000);

}
static int alloc_user_stack(PCB*pcb) {
    return alloc_pages(pcb,USER_STACK_BEGIN,USER_STACK_LEN);
}
//#define ENTRY 0X8048074
PCB* create_process(uint8_t *buf) {
    struct ELFHeader *elf = (struct ELFHeader*) buf;
    struct ProgramHeader *ph, *eph;
    uint32_t va, eva, pa, len, sublen, tmpva, offset;
    uint8_t *kst;
    int ret;
    PCB* pcb = create_kthread((void*)NULL);
    ph = (struct ProgramHeader*)((char*)elf + elf->phoff);
    eph = ph + elf->phnum;
    assert(pcb != NULL);
    printk("in create process pid=%d\n",pcb->pid);

    for(; ph < eph; ph++) {
        if(ph->type != PT_LOAD)
            continue;

        va = ph->vaddr; //in boot/main.c is paddr, it may be a bug
        len = ph->memsz;
        eva = va + len;
        printk("segment start = %x\n",va);
        printk("segment end = %x\n",eva);

        //request new page here
        //start= ph->vaddr, len=ph->memsz
        //
        printk("before alloc pages...va=%x\n",va);
        ret = alloc_pages(pcb,va,len);
        printk("after alloc pages\n");
        assert(ret == 0);  // must be successful
        printk("after alloc pages and assertion\n");
        printk("before alloc stack\n");
        ret = alloc_user_stack(pcb);
        assert(ret==0);
        printk("after alloc user stack\n");
        
        //attention: virtual adrress to physical address
        offset = ph->off;
        for(; va < eva;) {
            pa = pcb_va_to_pa(pcb,va);
            printk("physical address = %x\n",pa);
            kst = (uint8_t*)pa_to_va(pa); //virtual address
            printk("virtual  address = %x\n",kst);
            tmpva = va; va = to_next_page(va) < eva ? to_next_page(va) : eva; //when va == ea this loop end

            sublen = va - tmpva;
            memcpy(kst, (char*)elf + offset,sublen);
            offset += sublen;

        }

    }
    struct TrapFrame* tf = ((struct TrapFrame*)(pcb->tf));
    tf->eip = elf->entry;//ENTRY;
    //set user stack here
    tf->cs = SELECTOR_USER(SEG_USER_CODE);
    tf->ds = SELECTOR_USER(SEG_USER_DATA);

    tf->irq = 1000;

    tf->ss = SELECTOR_USER(SEG_USER_DATA);
    tf->esp = USER_STACK_END;
    //should I set tf->ebp?
   
    
    return pcb;

}


static pid_t _do_fork(PCB* pcb) {
    PCB *new_pcb = create_kthread(NULL);
    //Sem *old_sem,*new_sem;
    //Msg *old_msg,*new_msg;
    //struct ListHead *ptr;
    
    //head, no
    /*the guide said that all semaphore is empty and msg queue is also empty for a user process.
      So we do nothing here
     */
    //sem_list, copy
    
    //:msg_list copy

    //:msg_free, copy
    //state, block OK,

    new_pcb->intr_counter = pcb->intr_counter;
    //pid
    new_pcb->parent = pcb->pid;
    //TODO:CR3
    if(copy_vm_space(new_pcb,pcb) != 0 )
        return -1; //error

    new_pcb->counter = pcb->counter;

    new_pcb->ppcb = pcb;
    //msg_pool, no nead
    //kstack copy
    memcpy(new_pcb->kstack,pcb->kstack,sizeof(pcb->kstack)); //complete copying context
    new_pcb->tf = new_pcb->kstack + ((char*)(pcb->tf) - pcb->kstack); //relative offset

    //set ebp of each trapframe
    struct TrapFrame *otf = (struct TrapFrame*) pcb->tf;
    struct TrapFrame *ntf = (struct TrapFrame*) new_pcb->tf;
    uint32_t *nebp, *oebp, offset;

    oebp = &otf->ebp;
    nebp = &ntf->ebp;

    while(*oebp > KOFFSET /*Condition: util  to user space stack*/) {
        offset =  *oebp - (uint32_t)pcb->kstack;
        *nebp = (uint32_t)(new_pcb->kstack) + offset;

        oebp = (uint32_t*)*oebp;
        nebp = (uint32_t*)*nebp;
    }


    return new_pcb->pid;

}
