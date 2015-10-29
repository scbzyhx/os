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
            size_t sz =  do_read(0, buf, 0, FILE_SIZE);
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
#define ENTRY 0X8048000
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
        printk("before alloc pages,and pid=%d\n",pcb->pid);
        ret = alloc_pages(pcb,va,len);
        printk("after alloc pages\n");
        assert(ret == 0);  // must be successful
        printk("after alloc pages and assertion\n");
        
        //attention: virtual adrress to physical address
        offset = ph->off;
        for(; va < eva;) {
            pa = pcb_va_to_pa(pcb,va);
            kst = (uint8_t*)pa_to_va(pa); //virtual address
            tmpva = va; va = to_next_page(va) < eva ? to_next_page(va) : eva; //when va == ea this loop end

            sublen = va - tmpva;
            memcpy(kst, (char*)elf + offset,sublen);
            offset += sublen;

        }

    }
    ((struct TrapFrame*)(pcb->tf))->eip = ENTRY;
    return pcb;

}

pid_t create_thread() {
    //TODO: buf store filename, 
    return dev_write("pm",0,NULL,0,0);
}
