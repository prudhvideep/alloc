CC = clang-19
CFLAGS = -std=c23 -Wall -Wextra -g -no-pie

.PHONY: all clean

all : clean alloc

alloc : alloc.o heap.o
	$(CC) $(CFLAGS) $^ -o $@

alloc.o : alloc.c alloc.h
	$(CC) $(CFLAGS) -c $<

heap.o : heap.asm
	nasm -f elf64 $^

clean:
	rm -rf *.o alloc