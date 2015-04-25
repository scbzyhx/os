#ifndef __COMMON_H__
#define __COMMON_H__

#include "types.h"
#include "const.h"
#include "assert.h"
//#include "process.h"
void printk(const char *ctl, ...);
//void int2Str(char*,int32_t,int32_t);
void lock();
void unlock();
//void copy_from_kernel(struct PCB*,void*,void*,int);
//void copy_to_kernel(struct PCB*,void*,void*,int);

//void strcpy_to_kernel(struct PCB*,char*,char*);
//void strcpy_from_kernel(struct PCB*,char*,char*);


#endif
