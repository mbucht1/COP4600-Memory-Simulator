#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int diskread = 0;
int diskwrite = 0;
int pagefault = 0;
int pagehit = 0;
int accessTime = 0;

void fifo(List* pageTable, Node *entry, int nframes, char *displayMode){
    if(!isInMemory(pageTable, entry)){
        pagefault++;
        diskread++;
        insertNodeEnd(pageTable, entry, entry->virtualPageNumber, entry->dirty, entry->rw, entry->accessTime); 
        if(pageTable->size > nframes){
            if(pageTable->head->dirty == 1) diskwrite++;
            popFront(pageTable);
        }
    }
    else{ 
        pagehit++;
        if(entry->dirty == 1){
            Node *temp = nodeInMemory(pageTable, entry);
            temp->dirty = 1;
        }
    }
}


void lru(List* pageTable, Node *entry, int nframes, char *displayMode){  
    if(!isInMemory(pageTable, entry)){
        pagefault++;
        diskread++;
        insertNodeEnd(pageTable, entry, entry->virtualPageNumber, entry->dirty, entry->rw, entry->accessTime); 
        if(pageTable->size > nframes){
            int min = -1;
            Node *leastUsed = pageTable->head;
            for(Node *temp = pageTable->head; temp != NULL; temp = temp->next){
                if(temp->accessTime > min){
                    leastUsed = temp;
                    min = temp->accessTime; 
                }
            }
            if(leastUsed->dirty == 1) diskwrite++;
            removeNode(pageTable, leastUsed, nframes);
            for(Node *t = pageTable->head; t != NULL; t = t->next){
                t->accessTime++;
            }
        }
    }
    else{
        pagehit++;
        Node *temp = nodeInMemory(pageTable, entry);
        if(entry->dirty == 1) temp->dirty = 1;
        for(Node *t = pageTable->head; t != NULL; t = t->next){
            if(t->virtualPageNumber == temp->virtualPageNumber) t->accessTime = 0;
            else t->accessTime++;
        }
    }
}

/**
 * As pages are faulted into memory, they are placed into beginning of primary buffer which is fifo
 * if fault occurs when primary is full, the oldest page is moved from the bottom of the primary to the top of secondary
 * if the second is full when a page is added its oldest page is removed from memory
 * when a reference is made to a page in the secondary buffer, that page is removed and placed to the top of the primary buffer

**/
void vms(List *pageTable, List *secondBuffer, Node *entry, int nframes, int percentage, char *displayMode){
    int secondaryFrames = nframes * (percentage * 0.01);
    int primaryFrames = nframes - secondaryFrames;

    if(primaryFrames == 0) lru(pageTable, entry, nframes, displayMode);
    else if(secondaryFrames== 0) fifo(pageTable, entry, nframes, displayMode);
    


    if(!isInMemory(pageTable, entry) && !isInMemory(secondBuffer, entry)){
        pagefault++;
        diskread++;
        if(pageTable->size == primaryFrames){
            if(secondBuffer->size == secondaryFrames){
                int min = -1;
                Node *leastUsed = NULL;
                for(Node *temp = secondBuffer->head; temp != NULL; temp = temp->next){
                    if(temp->accessTime > min){
                        leastUsed = temp;
                        min = temp->accessTime;
                    }
                }
                if(leastUsed->dirty == 1) diskwrite++;
                removeNode(secondBuffer, leastUsed, nframes);
                for(Node *t = secondBuffer->head; t != NULL; t = t->next){
                    t->accessTime = 0;
                }
                Node *oldNode = createNode(pageTable->head->virtualPageNumber, pageTable->head->dirty, pageTable->head->rw, 0);
                insertNodeEnd(secondBuffer, oldNode, oldNode->virtualPageNumber, oldNode->dirty, oldNode->rw, oldNode->accessTime);
                insertNodeEnd(pageTable, entry, entry->virtualPageNumber, entry->dirty, entry->rw, entry->accessTime);
                popFront(pageTable);
            }
            else{
                insertNodeEnd(pageTable, entry, entry->virtualPageNumber, entry->dirty, entry->rw, entry->accessTime);
                for(Node *t = secondBuffer->head; t != NULL; t = t->next){
                    t->accessTime = 0;
                }
                Node *oldNode = createNode(pageTable->head->virtualPageNumber, pageTable->head->dirty, pageTable->head->rw, 0);
                insertNodeEnd(secondBuffer, oldNode, oldNode->virtualPageNumber, oldNode->dirty, oldNode->rw, oldNode->accessTime);
                popFront(pageTable);
            }
        }
        else{
            insertNodeEnd(pageTable, entry, entry->virtualPageNumber, entry->dirty, entry->rw, entry->accessTime);
        }
    }
    else if(isInMemory(pageTable, entry)){
        pagehit++;
        Node *temp = nodeInMemory(pageTable, entry);
        if(entry->dirty == 1) temp->dirty = 1;
    }
    else if(isInMemory(secondBuffer, entry)){
        pagehit++;
        Node *temp = nodeInMemory(secondBuffer, entry);
        if(temp->dirty == 1) entry->dirty = 1;
        insertNodeEnd(pageTable, entry, entry->virtualPageNumber, entry->dirty, entry->rw, entry->accessTime);
        removeNode(secondBuffer, temp, nframes);
        for(Node *t = secondBuffer->head; t != NULL; t = t->next){
            t->accessTime = 0;
        }
        Node *oldNode = createNode(pageTable->head->virtualPageNumber, pageTable->head->dirty, pageTable->head->rw, 0);
        insertNodeEnd(secondBuffer, oldNode, oldNode->virtualPageNumber, oldNode->dirty, oldNode->rw, oldNode->accessTime);
        popFront(pageTable);
    }
}




int main(int argc, char *argv[]){
    
     char *filename = argv[1];
    int nframes = atoi(argv[2]);
    char *algorithm = argv[3];
    int p;
    char *mode;
    if(strcmp(algorithm, "vms") == 0){
        p = atoi(argv[4]);
        if(p < 0 || p > 100){
            printf("Please enter a percentage from 0-100. ");
            return -1;
        } 
        mode = argv[5];
    }
    else{
        mode = argv[4];
    }

    if(strcmp(algorithm, "vms") == 0 && argc != 6){
       printf("Please enter in the form: memsim <tracefile> <nframes> <lru|fifo|vms> <p> <debug|quiet> when selecting vms.");
       return - 1; 
    }
    else if(argc != 5 && strcmp(algorithm, "vms") != 0){
        printf("Please enter in the form: memsim <tracefile> <nframes> <lru|fifo|vms> <debug|quiet>.");
        return -1;
    }

    FILE *file = fopen(filename, "r");
    
    //if file cannot open send error
    if (file == NULL) {
        printf("Failed to open file.");
        return -1;
    }

    char *pageAlgo;
    char *displayMode;

    if(strcmp(algorithm, "lru") == 0){
        pageAlgo = "lru";
    }
    else if(strcmp(algorithm, "fifo") == 0){
        pageAlgo = "fifo";
        
    }
    else if(strcmp(algorithm, "vms") == 0){
        pageAlgo = "vms";
    }

    if(strcmp(mode, "debug") == 0){
        displayMode = "debug";
    }
    else if(strcmp(mode, "quiet") == 0){
        displayMode = "quiet";
    }
    

    unsigned addr;
    char rw;
    int events = 0;
    unsigned pageSize = 4096;
    int dirty = 0;

   
    List *pageTable = initList();
    List *secondBuffer = initList();
    while(fscanf(file, "%x %c", &addr, &rw) != EOF){
        ++events;
        ++accessTime;
        unsigned pageNumber = addr / pageSize;
        if(rw == 'W') dirty = 1;
        else dirty = 0;
        Node *entry = createNode(pageNumber, dirty, rw, 0);
        if(strcmp(pageAlgo,"fifo") == 0){
            fifo(pageTable, entry, nframes, displayMode);
        }
        else if(strcmp(pageAlgo,"lru") == 0){
            lru(pageTable, entry, nframes, displayMode);
        }
        else if(strcmp(pageAlgo, "vms") == 0){
            vms(pageTable, secondBuffer, entry, nframes, p, displayMode);
        }
    }

    if(strcmp(displayMode,"quiet") == 0){
        printf("total memory frames: %d\n", nframes);
        printf("events in trace: %d\n", events); 
        printf("total disk reads: %d\n", diskread); 
        printf("total disk writes: %d\n", diskwrite); 
    }
    else if(strcmp(displayMode, "debug") == 0){
        printf("fault count: %d\n", pagefault); 
        printf("hit count: %d\n", pagehit);
    }
    
    // printList(pageTable, secondBuffer);
   
    
    fclose(file);
    return 0;
}