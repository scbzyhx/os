CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I../include -O2 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer
ASFLAGS = -m32 -MD -I./include
LDFLAGS = -melf_i386

CFILES  = $(shell find . -name "*.c")
SFILES  = $(shell find . -name "*.S")
OBJS    = $(CFILES:.c=.o) $(SFILES:.S=.o)

run: $(OBJS)


clean:
	rm -f *.d *.o
