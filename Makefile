CC=gcc
CFLAGS=-g -Wall

all: memsim

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -c linkedlist.c -o linkedlist.o

memsim: memsim.c linkedlist.o
	$(CC) $(CFLAGS) -o memsim memsim.c linkedlist.c
clean:
	del *.o memsim