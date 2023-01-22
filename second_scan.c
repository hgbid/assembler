#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE 80 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_scan.h"

void writeEntFile(List* entList, List* labelList, char* fileName, int* error);
void printAdress(int adress, FILE* objFile);

/*
Function that implements a second run on an assembly file

INPUT: char* fileName  - pointer to the file name
       List* entList   - list of entries
       List* labelList - list of labels
       int error       - sign if the error was found

OUTPUT: no output(void type)
*/
void asemblerSecond(char* fileName, List* entList, List* labelList, int* error, int* counters) {
    FILE* extFile, * objFile, * tempFile;
    Node* index;
    char* tempFileName, * line, word[MAX_LINE], binNum[11], * code;
    int i, k, found, tempAdress = 100, lineCounter = 0, adress = 99, externExist=0;
    code = malloc(sizeof(char) * 3);

    writeEntFile(entList, labelList, fileName, error);

    tempFileName = (char*)malloc(sizeof(char) * (strlen(fileName) + 5));
    line = (char*)malloc(sizeof(char) * MAX_LINE);
    strcpy(tempFileName, fileName);
    strcat(tempFileName, ".ob");
    objFile = fopen(tempFileName, "w");
    strcpy(tempFileName, fileName);
    strcat(tempFileName, ".ext");
    extFile = fopen(tempFileName, "w");
    tempFile = fopen("tmp.txt", "r");

    for (i = 0; i < 2; i++) {
        decToBin(counters[i], 10, binNum);
        binarToCode(binNum, code);
        fprintf(objFile, "%s", code);
        if (i == 0) fprintf(objFile, " ");
        else fprintf(objFile, "\n");
    }

    while (fgets(line, MAX_LINE, tempFile) != NULL) {
        k = 0; i = 0; found = 0;
        while (line[i] == '*') {
            lineCounter++;
            if (fgets(line, MAX_LINE, tempFile) == NULL)   break;
        }
        if (line[i] == '*') break;
        while (!(line[i] == ' ' || line[i] == '\n' || line[i] == '\r')) {
            word[k] = line[i];
            i++; k++;
        } word[k] = '\0';
        index = labelList->firstNode;
        if (strcmp(word, "?") == 0) { /*label*/
            while (line[i] == ' ') i++; k = 0;
            while (!(line[i] == ' ' || line[i] == '\n' || line[i] == '.' || line[i] == '\r')) {
                word[k] = line[i];
                i++; k++;
            } word[k] = '\0';
            while (index != NULL) {
                if (strcmp(index->data, word) == 0) {
                    if (strcmp(index->name, "EXTERN") == 0) {
                        found = 1; externExist = 1;
                        decToBin(1, 10, binNum);
                        printAdress(++adress, objFile);
                        binarToCode(binNum, code);
                        fprintf(objFile, "%s\n", code);
                        decToBin(tempAdress, 10, binNum);
                        binarToCode(binNum, code);
                        fprintf(extFile, "%s %s\n", word, code);
                        break;
                    }
                    else {
                        printAdress(++adress, objFile);
                        decToBin(atoi(index->name), 8, binNum);
                        strcat(binNum, "10");
                        binarToCode(binNum, code);
                        fprintf(objFile, "%s\n", code);
                        found = 1;
                        break;
                    }
                }
                index = index->next;
            }
            if (!found) {
                printf("Error! the label %s not found (line %d)\n", word, lineCounter);
                error[0] = 1;
            }
        }
        else { /*binar code*/
            printAdress(++adress, objFile);
            strcpy(binNum, word);
            binarToCode(binNum, code);
            fprintf(objFile, "%s\n", code);
        }
        tempAdress++;
    }
    free(line); free(code);
    fclose(tempFile); fclose(objFile); fclose(extFile);

    if (!externExist) {
        strcpy(tempFileName, fileName); strcat(tempFileName, ".ext"); remove(tempFileName);
    }
    if (error[0] == 1) {
        strcpy(tempFileName, fileName); strcat(tempFileName, ".ob"); remove(tempFileName);
        strcpy(tempFileName, fileName); strcat(tempFileName, ".am"); remove(tempFileName);
        strcpy(tempFileName, fileName); strcat(tempFileName, ".ext"); remove(tempFileName);
        strcpy(tempFileName, fileName); strcat(tempFileName, ".ent"); remove(tempFileName);
    }
    else 
        printf("Succeeded\n");
    free(tempFileName);

    remove("tmp.txt");

}

/*

*/
void printAdress(int adress, FILE* objFile) {
    char* binNUm, * code;
    binNUm = (char*)malloc(sizeof(char) * 12);
    code = (char*)malloc(sizeof(char) * 3);
    decToBin(adress, 10, binNUm);
    binarToCode(binNUm, code);
    fprintf(objFile, "%s ", code);
    free(binNUm); free(code);
}

/*
Function writes data to the file
*/
void writeEntFile(List* entList, List* labelList, char* fileName, int* error) {
    FILE* entFile;
    Node* indexLabel, * indexEnt;
    char* tempFileName, binNum[11], * code;
    int isEntFile = 0;
    code = malloc(sizeof(char) * 3);
    tempFileName = (char*)malloc(sizeof(char) * (strlen(fileName) + 5));
    strcpy(tempFileName, fileName);
    strcat(tempFileName, ".ent");
    entFile = fopen(tempFileName, "w");
    if (entFile == NULL) {
        printf("Error\n"); exit(1);
    }
    indexEnt = entList->firstNode->next;
    while (indexEnt != NULL) {
        indexLabel = labelList->firstNode->next;
        while (indexLabel != NULL) {
            if (strcmp(indexEnt->data, indexLabel->data) == 0) {
                decToBin(atoi(indexLabel->name), 10, binNum);
                binarToCode(binNum, code);
                fprintf(entFile, "%s %s\n", indexLabel->data, code);
                isEntFile = 1;
                break;
            }
            indexLabel = indexLabel->next;
        }
        if (indexEnt == NULL){
            printf("Error! the entry label %s not found\n", indexEnt->data);
        error[0] = 1;
        }
        indexEnt = indexEnt->next;
    }
    fclose(entFile);
    if (isEntFile == 0)
        remove(tempFileName);
    free(tempFileName); free(code);
}
