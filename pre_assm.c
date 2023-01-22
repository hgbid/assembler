#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE 80 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assm.h"

/*
Function that searchs for macro in the file, checks if the macro is found
and puts the content of macro to the file
INPUT: FILE*          pointer to the file
       char* fileName name of the file
OUTPUT: no output(void type)
*/
void preAssembler(FILE* fp, char* fileName) {
    char c, * line, * tempMacro, * lastWord, * macroName, * amFileName;
    List* macro;
    Node* index;
    int option[2];
    int isMacro = 0, i, j;
    FILE* amFile;

    amFileName = (char*)malloc(sizeof(char) * (strlen(fileName) + 4));
    strcpy(amFileName, fileName);
    strcat(amFileName, ".am");
    amFile = fopen(amFileName, "w");
    if (amFile == NULL) {
        printf("Error!");
        exit(1);
    }
    free(amFileName);
    line = (char*)malloc(sizeof(char) * MAX_LINE);
    lastWord = (char*)malloc(sizeof(char) * MAX_LINE);
    macroName = (char*)malloc(sizeof(char) * MAX_LINE);
    tempMacro = (char*)malloc(sizeof(char) * MAX_LINE);

    macro = NULL;
    while (fgets(line, MAX_LINE, fp) != NULL) {
        findCasePreAssembler(line, macro, option);
        switch (option[0]) {
        case 1:         /*macro*/
            j = 0; i = option[1];
            while (line[i] == ' ' || line[i] == '\t') i++;
            while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r')) {
                macroName[j] = line[i];
                i++; j++;
            } macroName[j] = '\0';

            macroName[j] = '\0';
            tempMacro[0] = '\0';
            while (1) {
                j = 0;
                lastWord[j] = '\0';
                while ((c = fgetc(fp)) != ' ' && c != '\t' && c != '\n' && c!='\r') {
                    lastWord[j] = c;
                    j++;
                }
                lastWord[j] = '\0';
                if (strcmp(lastWord, "endmacro") == 0)
                    break;
                if (strlen(tempMacro) + strlen(lastWord) + 1 > MAX_LINE)  /*grow the macro space*/
                    tempMacro = (char*)realloc(tempMacro, sizeof(char) * ((strlen(tempMacro) + MAX_LINE)));

                strncat(tempMacro, lastWord, j);
                strncat(tempMacro, &c, 1);
            }
            tempMacro[strlen(tempMacro)] = '\0';
            if (isMacro == 0) {
                macro = newList(macroName, tempMacro);
                isMacro = 1;
            }
            else
                add(macroName, tempMacro, (&macro));
            break;

        case 2:     /*use in macro*/
            index = macro->firstNode;
            for (i = 0; i < option[1]; i++)
                index = index->next;
            fprintf(amFile, "%s", index->data);
            break;

        default:
            fprintf(amFile, "%s", line);
        }
    }
    free(lastWord); free(line);
    free(macroName); free(tempMacro);
    if(macro!=NULL){
      deleteList((macro)->firstNode);
}
    free(macro);
    fclose(amFile);
}


void findCasePreAssembler(char* line, List* macro, int* ans) {
    Node* index;
    char* word;
    int i, k, j;

    i = 0; k = 0; j = 0;
    word = (char*)malloc(sizeof(char) * MAX_LINE);
    index = (Node*)malloc(sizeof(Node));

    ans[0] = 0, ans[1] = 0;
    while (line[i] == ' ' || line[i] == '\t') i++;
    while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n'|| line[i] == '\r')) {
        word[k] = line[i];
        i++; k++;
    } word[k] = '\0';

    if (strcmp(word, "macro") == 0) {   /*macro*/
        ans[0] = 1;
        ans[1] = i;  /*start index in line*/
    }
    else {
        if (macro) {
            index = macro->firstNode;
            while (index != NULL) {
                if (strcmp(word, index->name) == 0) {   /*use in macro*/
                    ans[0] = 2;
                    ans[1] = j;  /*macro index in list*/
                }
                j++;
                index = index->next;
            }
        }
    }
    free(word);
}

/*newList - make a new list from data.
Input: the data in the first node
Output: pointer the new list*/
List* newList(char* n, char* Data) {
    List* Newlist;
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->name = malloc(sizeof(char) * (strlen(n) + 1));
    strcpy(newNode->name, n);
    newNode->data = malloc(sizeof(char) * (strlen(Data) + 1));
    strcpy(newNode->data, Data);
    newNode->next = NULL;
    Newlist = (List*)malloc(sizeof(List));
    Newlist->firstNode = newNode;
    Newlist->lastNode = newNode;
    return Newlist;
}

/*add - add a node to the exist list.
Input: the data in the new node, and the pointer to the list*/
void add(char* n, char* Data, List** pL) {
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    newNode->name = malloc(sizeof(char) * (strlen(n) + 1));
    strcpy(newNode->name, n);
    newNode->data = malloc(sizeof(char) * (strlen(Data) + 1));
    strcpy(newNode->data, Data);
    newNode->next = NULL;
    (*pL)->lastNode->next = newNode;
    (*pL)->lastNode = newNode;
}

/*deleteList - delete the list.
Input: pointer to the list*/
void deleteList(Node* pN) {
    if (pN->next != NULL)
        deleteList(pN->next);
    free(pN->data);
    free(pN->name);
    free(pN);
}

