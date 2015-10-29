#include "kernel.h"
#include "hal.h"
#include "mm.h"
#include "string.h"
#include "memory.h"

#define NR_PAGE (PHY_MEM / PAGE_SIZE)
#define NR_KERNEL_PAGE (KMEM /PAGE_SIZE) //2^12
#define USER_PAGE_OFFSET NR_KERNEL_PAGE
#define NR_USER_PAGE (NR_PAGE - NR_KERNEL_PAGE)

uint8_t mm_bits[(NR_USER_PAGE >>3)];// bitmap to manage pages, no need to add one.

pid_t MM_PID;

static void mm_thread();
static uint32_t find_free(uint32_t *n) ;
static inline void* pa_page(uint32_t n);
static inline void* va_page(uint32_t n);
static void set(uint32_t);
static inline  uint32_t va_to_page(uint32_t va) {
    return (va>>12);
}

void init_mm(void) {

    PCB *p = create_kthread(mm_thread);
    MM_PID = p->pid;
    uint32_t i;
    memset(mm_bits,'\0',sizeof(mm_bits));
    //init first 16MB
    for(i = 0; i < (NR_USER_PAGE >> 3); ++i)
        mm_bits[i] = 0x0; //set it as 


    hal_register("mm",MM_PID,0);
    wakeup(p);


}
/*I ignore error in middle, In fact, I should releae page when some error occured. But I didn't*/
static int _new_page(PCB *pcb, uint32_t va) {
    uint32_t page_n; //= find_free();
    uint32_t idx;
    void* ptr;
    PDE *pdr;
    PTE *ptable;
    if(find_free(&page_n) != 0)
        return -1;
    printk("pid=%d,cr3=%d\n",pcb->pid,pcb->cr3.val);
    if(pcb->cr3.val == 0) {
        //virtual address
        pdr = (PDE*) va_page(page_n); 
        printk("page_n=%d, va_in_kernel=%x\n",page_n,(uint32_t)pdr);

        //initialization
        for(idx = 0;idx < NR_PDE; idx++)
            make_invalid_pde(&pdr[idx]);

        //set kernel pte, in fact just 4 pdr entries
        ptable = (PTE*)(va_to_pa(get_kptable()));
        printk("ptable_physical_addr=%x\n",(uint32_t)ptable);

        for(idx = 0; idx < PHY_MEM / (PAGE_SIZE*1024); idx++) {
            make_pde(&pdr[idx + KOFFSET / (PAGE_SIZE*1024)],ptable);
            ptable = ptable + NR_PTE;
        }


        /*prepare cr3 for request thread, physical address here*/
        pdr =  (PDE*) pa_page(page_n);
        pcb->cr3.val = 0;
        pcb->cr3.page_directory_base = (uint32_t)(pdr) >> 12;
        printk("CR3=%x\n",pdr);
        set(page_n);

        if(find_free(&page_n) != 0)
            return -1;
    }


    pdr = (PDE*)pa_to_va((void*)((uint32_t)(pcb->cr3.page_directory_base)<<12));
    pdr = &pdr[va>>22];
    printk("PDE = %x\n",pdr);

    if(pdr->val == 0) {
        //virtual address
        ptable = (PTE*)va_page(page_n);

        for(idx=0; idx<NR_PTE; ++idx)
            make_invalid_pte(&ptable[idx]);

        /*physical address here*/
        ptable = (PTE*) pa_page(page_n);
        printk("PHY OF PTE TABLE = %x\n",ptable);
        make_pde(pdr, ptable);
        
        set(page_n);
        
        if(find_free(&page_n) != 0)
            return -1;
    }

    ptable = (PTE*)pa_to_va((void*)((uint32_t)(pdr->page_frame)<<12));
    ptable = &ptable[(va & 0x3ff000 )>>12]; // 0000 0000 0011 1111 1111 0000 0000 0000
    
    printk("BEFORE SET PAGE PTE = %x\n",ptable);

    if(ptable->val != 0) 
        return 0; // or -1 

    ptr = pa_page(page_n);

    printk("PHY PTR = %x\n",ptr);

    make_pte(ptable,ptr);

    set(page_n);

    return 0;


}
static int new_page(PCB* pcb, uint32_t va_start, uint32_t memsz) {
    uint32_t va = va_start & 0xfffffc00; // 11111111 1111111 1111100 00000000
    uint32_t end = va_start + memsz; 
    end = (end & 0xfffffc00) + (0x1 << 12); 

    for(; va < end; va += PAGE_SIZE) { 
        if(_new_page(pcb,va) != 0) 
            assert(0); // allcoate error, 
    }
    return 0;

}

static void mm_thread(void) {
	static Msg m;
	PCB *pcb = NULL;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
		    //do nothting
		} else if (m.type == NEW_PAGE) { //infact DEV_READ
		    printk("in mm_thread, m.req_pid=%d\n",m.req_pid);
			pcb = fetch_pcb(m.req_pid); //req_pid store pid of thread, equal to source or not
			printk("req_pid=%d\n",pcb->pid);
			m.ret = new_page(pcb,m.offset,m.len);
			printk("mm_thread id=%x\n",m.req_pid);
			
			m.dest = m.src;
			m.src = MM_PID;
			send(m.dest, &m);

		} else if (m.type == DEV_WRITE) {
			//copy_to_kernel(fetch_pcb(m.req_pid),&disk[m.offset],m.buf,m.len);
			m.ret = m.len;
			m.dest = m.src;
			m.src = MM_PID;
			send(m.dest, &m);
		}
		else {
			assert(0);
		}
	}
}

static inline void find_pos(uint32_t *bucket, uint32_t *bit, uint32_t n) {
    *bucket = n >> 3; // /8
    *bit = n & 0x7; //%8
    assert(*bit >=0 && *bit < 8);
    return;
}

//set page to be occupied
static void set(uint32_t n) {
    uint32_t bucket;// = n >> 8;
    uint32_t bit;// = n % 8;
    find_pos(&bucket,&bit,n);
    mm_bits[bucket] = mm_bits[bucket] | (0x1<<bit);
    return;

}
/*
//set page to be unoccupied
static void clear(uint32_t n) {
    uint32_t bucket;// = n >> 8;
    uint32_t bit;// = n % 8;
    find_pos(&bucket,&bit,n);
    uint32_t mask = (0x1<<bit);

    mm_bits[bucket] = mm_bits[bucket] & (~mask);
    return;
}
*/
//@return: 0 means free, 1 means occupied
static inline uint32_t is_free(uint32_t n) {
    uint32_t bucket;// = n >> 8;
    uint32_t bit;// = n % 8;
    find_pos(&bucket,&bit,n);

    uint32_t ret = (mm_bits[bucket] & (0x1<<bit));

    return (ret>>bit); //zero or one



}
//0: success, otherwise failed
static uint32_t find_free(uint32_t *n) {
    //find an empty page
    uint32_t ind = 0;
    for(;ind <  NR_KERNEL_PAGE; ++ind) {
        if(is_free(ind) == 0) {
            *n = ind;
            printk("page_n=%d\n",ind);
            return 0;
        }
    }
    return 1; 

}

static inline void* pa_page(uint32_t n) {
    return (void*)(KMEM + n*PAGE_SIZE);

}
static inline void* va_page(uint32_t n) {
    return (void*)(KMEM + n*PAGE_SIZE + KOFFSET);
}


int alloc_pages(PCB* pcb, uint32_t va, uint32_t len) {
     printk("in alloc_pages pid=%d\n",pcb->pid);
     size_t ret = dev_read("mm",pcb->pid,NULL,va, len);
     //must be successful
     assert(ret==0);

     return ret;
    
}










