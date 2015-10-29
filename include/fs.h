#ifndef __FS_H_
#define __FS_H_
#include "kernel.h"
size_t do_read(int file_name, uint8_t *buf, off_t offset, size_t len);

#endif
