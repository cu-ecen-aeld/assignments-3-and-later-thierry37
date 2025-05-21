# define compiler
ifeq ($(CROSS_COMPILE),aarch64-none-linux-gnu-)
	CC := aarch64-none-linux-gnu-gcc
else
	CC := gcc
endif

all: writer

writer: writer.c 
	$(CC) writer.c -o writer 

clean: 
	rm -f writer *.o
