# COP4600-Memory-Simulator


##### This project is a memory simulator which is meant to evaluate how real applications respond to a variety of page replacement algorithms. This program will read in memory trace files and simulate and evaluate the the memory performance of a variety of page replacement algorithms which are listed below.

```
    1. FIFO
    2. LRU
    3. Segmented-FIFO
```
# How to compile

```
    A make file is provided in order to compile the program, this is done by using the "make" command. Otherwise the program can also be compiled using gcc memsim.c linkedlist.c
```

# How to run

```
    To the run the FIFO and LRU algorithms the format is ./memsim <tracefile> <number of frames> <algorithm> <quiet|debug>

    Otherwise for the VMS algorithm the format is similar although with the addition of a percentage
    ./memsim <tracefile> <number of frames> <algorithm> <percentage> <quiet|debug>

    tracefile: name of tracefile
    number of frames: frames used in memory
    algorithm: page replacement algorithm to be used
    quiet: displays the number of reads/writes
    debug: number of faults and hits of pages

```