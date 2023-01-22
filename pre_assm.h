#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#ifndef PRE_ASSM_H
#define PRE_ASSM_H
/*node - Composed from data (string of NUM chars and 0)
and a pointer to the next node*/
typedef struct node {
    char* name;
    char* data;
    struct node* next;
} Node;


/*list - list of nodes.
Composed from pointer to first node and pointer to the last node*/
typedef struct list {
    Node* firstNode;
    Node* lastNode;
} List;

#endif

void findCasePreAssembler(char* line, List* macro, int* ans);
void add(char* n, char* Data, List** pL);
List* newList(char* n, char* Data);
void deleteList(Node* pN);


