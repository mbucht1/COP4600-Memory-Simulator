#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct node{
    int virtualPageNumber;
    int valid;
    int dirty;
    char rw;
    int accessTime;
    struct node *next;
}Node;

typedef struct list{
    int size;
    Node *head;
    Node *tail;
}List;


void insertNodeFront(List *list, const int VPN, int dirty, const char rw, const int accessTime);
void insertNodeEnd(List *list, Node *entry, const int VPN, int dirty, const char rw, const int accessTime);
void printList(List *list, List *list2);
// void printBuffers(List *primaryBuffer, List *secondBuffer);
void popFront(List *list);
List *initList();
bool isInMemory(List *list, Node *entry);
Node *front(List *list);
Node *createNode(const int VPN, int dirty, const char rw, const int accessTime);
bool isDirty(Node *node);
Node *nodeInMemory(List *list, Node *entry);
void removeNode(List *list, Node* entry, int nframes);

#endif