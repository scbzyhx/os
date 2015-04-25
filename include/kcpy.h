#ifndef __KCPY_H__
#define __KCPY_H__
#include "process.h"

void copy_from_kernel(PCB*,void*,void*,int);
void copy_to_kernel(PCB*,void*,void*,int);
void strcpy_to_kernel(PCB*,void*,void*);
void strcpy_from_kernel(PCB*,void*,void*);
#endif
