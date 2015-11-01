#include "kernel.h"
#include "hal.h"
#include "elf.h"
#include "mm.h"
#include "fs.h"
#include "string.h"

#define FILE_SIZE (128*1024)
#define NEW_PROCESS DEV_WRITE

pid_t PM;
static void  pm_thread();

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

		} else if (m.type == 0) {
		    assert(0);
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
        printk("va=%x\n",va);
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

pid_t create_thread(void* file_name_buf) {
    //TODO: buf store filename,
    return dev_write("pm",current->pid,file_name_buf,0,0);
}
