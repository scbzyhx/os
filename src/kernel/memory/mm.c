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
    for(i = 0; i < (USER_PAGE_OFFSET >> 3); ++i)
        mm_bits[i] = 0xFF; //set it as 


    hal_register("mm",MM_PID,0);
    wakeup(p);


}
/*I ignore error in middle, In fact, I should releae page when some error occured. But I didn't*/
static int new_page(PCB *pcb, uint32_t va) {
    uint32_t page_n; //= find_free();
    uint32_t idx;
    void* ptr;
    PDE *pdr;
    PTE *ptable;
    if(find_free(&page_n) != 0)
        return -1;

    if(pcb->cr3.val == 0) {
        //virtual address
        pdr = (PDE*) va_page(page_n); 

        //initialization
        for(idx = 0;idx < NR_PDE; idx++)
            make_invalid_pde(&pdr[idx]);

        //set kernel pte, in fact just 4 pdr entries
        ptable = (PTE*)(va_to_pa(get_kptable()));

        for(idx = 0; idx < KMEM / (PAGE_SIZE*1024); idx++) {
            make_pde(&pdr[idx + KOFFSET / (PAGE_SIZE*1024)],ptable);
            ptable = ptable + NR_PTE;
        }


        /*prepare cr3 for request thread, physical address here*/
        pdr =  (PDE*) pa_page(page_n);
        pcb->cr3.val = 0;
        pcb->cr3.page_directory_base = (uint32_t)(pdr) >> 12;

        set(page_n);

        if(find_free(&page_n) != 0)
            return -1;
    }


    pdr = (PDE*)pa_to_va((void*)((uint32_t)(pcb->cr3.page_directory_base)<<12));
    pdr = &pdr[va>>22];

    if(pdr->val == 0) {
        //virtual address
        ptable = (PTE*)va_page(page_n);

        for(idx=0; idx<NR_PTE; ++idx)
            make_invalid_pte(&ptable[idx]);

        /*physical address here*/
        ptable = (PTE*) pa_page(page_n);
        make_pde(pdr, ptable);
        
        set(page_n);
        
        if(find_free(&page_n) != 0)
            return -1;
    }

    ptable = (PTE*)pa_to_va((void*)((uint32_t)(pdr->page_frame)<<12));
    ptable = &ptable[(va & 0x3ff000 )>>12]; // 0000 0000 0011 1111 1111 0000 0000 0000

    if(ptable->val != 0) 
        return 0; // or -1

    ptr = pa_page(page_n);

    make_pte(ptable,ptr);

    set(page_n);

    return 0;


}

static void mm_thread(void) {
	static Msg m;
	PCB *pcb = NULL;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
		    //do nothting
		} else if (m.type == NEW_PAGE) {
			pcb = fetch_pcb(m.req_pid); //req_pid store pid of thread, equal to source or not
			m.ret = new_page(pcb,m.offset);
			
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
    *bucket = n >> 8;
    *bit = n % 8;
    assert(*bit >=0 && *bit < 8);
    return;
}

//set page to be occupied
static void set(uint32_t n) {
    uint32_t bucket;// = n >> 8;
    uint32_t bit;// = n % 8;
    find_pos(&bucket,&bit,n);
    mm_bits[bucket] = mm_bits[bucket] & (0x1<<bit);
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












