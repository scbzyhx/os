#ifndef __COMMON_H__
#define __COMMON_H__

#include "types.h"
#include "const.h"

void printk(const char *ctl, ...);
//void int2Str(char*,int32_t,int32_t);
void lock();
void unlock();
#include "assert.h"


#endif
