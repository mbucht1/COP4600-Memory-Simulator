#include "linkedlist.h"



Node *createNode(const int VPN, int dirty, const char rw, const int accessTime){
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->virtualPageNumber = VPN;
    newNode->dirty = dirty;
    newNode->rw = rw;
    newNode->accessTime = accessTime;
    newNode->next = NULL;
    return newNode;
}

List *initList(){
    List *list = (List *)malloc(sizeof(List));
    list->head = list->tail = NULL;
    list->size = 0;
    return list;
}



void insertNodeFront(List *list, const int VPN, int dirty, const char rw, const int accessTime){
    Node *newNode = createNode(VPN, dirty, rw, accessTime);
    if(list->head == NULL && list->tail == NULL){
        list->head = list->tail = newNode;
    }
    else{
        newNode->next = list->head;
        list->head = newNode;
    }
    list->size++;
}

void insertNodeEnd(List *list, Node *entry, const int VPN, int dirty, const char rw, const int accessTime){
    if(list->head == NULL && list->tail == NULL){
        list->head = list->tail = entry;
    }
    else{
        list->tail->next = entry;
        list->tail = list->tail->next;
    }
    list->size++;
}

void removeNode(List *list, Node* entry, int nframes){
    Node *prev, *cur;
    for(prev = NULL, cur = list->head; cur->virtualPageNumber != entry->virtualPageNumber; prev = cur, cur = cur->next);
    if(prev == NULL){
        if(list->head->next != NULL) list->head = list->head->next;
        else{
            list->head = NULL;
            list->tail = NULL;
        }
    }
    else prev->next = cur->next;
    list->size--;
    free(cur);
}


bool isInMemory(List *list, Node *entry){
    Node *temp = list->head;
    while(temp != NULL){
        if(temp->virtualPageNumber == entry->virtualPageNumber) return true;
        temp = temp->next;
    }
    return false;
}

Node *nodeInMemory(List *list, Node *entry){
    Node *temp = list->head;
    while(temp != NULL){
        if(temp->virtualPageNumber == entry->virtualPageNumber) return temp;
        temp = temp->next;
    }
    return NULL;
}

Node *front(List *list){
    return list->head;
}

void popFront(List* list){
    // if(list->head == NULL) printf("List is empty");
    // else{
    //     // printf("popping %d", list->head->virtualPageNumber);
    //     Node *temp = list->head;
    //     list->head = list->head->next;
    //     free(temp);
    // }
    // list->size--;
    Node *temp = list->head;
    if(list->head->next != NULL) list->head = list->head->next;
    else{
        list->head = NULL;
        list->tail = NULL;
    }
    free(temp);
    list->size--;
}

void printList(List *list, List *list2){
    Node *temp = list->head;
    printf("list1: size %d",list->size);
    printf("primary buffer: ");
    while(temp != NULL){
        printf("%d ", temp->virtualPageNumber);
        temp = temp->next;
    }
    printf("\n");
    temp = list2->head;
    printf("list2: size %d",list2->size);
    printf("secondary buffer: ");
    while(temp != NULL){
        printf("%d ", temp->virtualPageNumber);
        temp = temp->next;
    }
    printf("\n");
}


bool isDirty(Node *node){
    return node->dirty == 1;
}
