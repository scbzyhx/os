#ifndef __PM_H_
#define __PM_H_
#include "kernel.h"
#include "hal.h"
/*This is header file for process manger service
 */
#define NEW_PROCESS DEV_WRITE
#define FORK 5




pid_t do_fork() {
    return dev_rw("pm",FORK,current->pid,NULL,0,0);
}

pid_t create_thread(void* file_name_buf) {
    //TODO: buf store filename,
    return dev_rw("pm",NEW_PROCESS,current->pid,file_name_buf,0,0);
}




#endif
