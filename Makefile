CC = clang-19
CFLAGS = -std=c23 -Wall -Wextra -pedantic
.PHONY: all clean

all : clean alloc
	./alloc

alloc : alloc.o
	$(CC) $(CFLAGS) $^ -o $@

alloc.o : alloc.c alloc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o alloc