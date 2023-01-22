#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE 80 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assm.h"

#ifndef FIRST_SCAN_H
#define FIRST_SCAN_H

int powerTwo(int power) {
	if (power == 0) return 1;
	return 2 * powerTwo(power-1);
}

int isDigit(char c) {
	if ((int)c>47 && (int)c<58) return 1;
	return 0;
}

int isLineEmpty (char *line) {
	int i = 0;
	for (i = 0; i < strlen(line); i++)
		if (line[i]!=' ' && line[i] != '\t')
			return 0;
	return 1;
}

int isAlpha(char c) {
	if ((int)c > 64 && (int)c < 91) return 1;
	if ((int)c > 96 && (int)c < 123) return 1;
	return 0;
}
void binarToCode(char* binar, char *code){	/*gets string of 10 bits */
	int decimal, k, mult, j = 1, i;
	char numbers[] = "!@#$%^&*<>abcdefghijklmnopqrstuv";
	for (i = strlen(binar) - 1; i > 0; i -= 5) {
		decimal = 0; mult = 1; /*takes 5 bits and covert them to dec*/
		for (k = i; k > i - 5; k--) {
			decimal += ((int)(binar[k]) - '0') * mult;
			mult *= 2;
		} 
		code[j] = numbers[decimal];
		j--;
	}
	code[2] = '\0';
}

void decToBin(int num, int bitsNum, char* bin) { /* convert to bin base*/
	int i, temp;
	int flag = 0,counter = 0;
	char* nbit;
	nbit = (char*)malloc(sizeof(char) * (bitsNum + 1));
	if (num < 0) {
		num = num * -1;
		flag = 1;
	}
	for (i = 0; i < bitsNum; i++)
	{
		temp = num / powerTwo(bitsNum - 1 - i);
		bin[i] = temp + '0';
		if (temp != 0) num -= powerTwo(bitsNum - 1 - i);
	}
	bin[bitsNum] = '\0';
	if (flag) {
		for (i = strlen(bin); i >= 0; i--) {
			if (bin[i] == '0' && counter == 1) {
				nbit[i] = '1';
			}
			else if (bin[i] == '1' && counter == 1) {
				nbit[i] = '0';
			}
			else if (bin[i] == '0' && counter == 0) {
				nbit[i] = '0';
			}
			else if (bin[i] == '1' && counter == 0) {
				nbit[i] = '1';
				counter = 1;
			}
		}
		nbit[bitsNum] = '\0';
		strcpy(bin, nbit);
	}
	free(nbit);
}

int isInList(char* label, List* list) {
	Node* index;
	int i=0;
	index = list->firstNode;
	while (index != NULL) {
		if (strcmp(label, index->data) == 0) {
			return i;
		}
		index = index->next;
		i++;
	}
	return 0;
}
int check_label_is_correct(char* word) {
    char* save_words[27] = { "r0","r1","r2","r3","r4","r5","r6","r7","mov", "cmp","add","sub","not","clr", "lea", "inc",
       "dec","jmp","bne","get","prn","jsr","rts","hlt", "data", "string", "struct" };
    int i;
    if (!(isAlpha(word[0]))) {
        return 0;
    }

    for (i = 0; i < 24; i++) { /*save word*/
        if (strcmp(word, save_words[i]) == 0)
            return 0;
    }

    for (i = 0; i < strlen(word); i++) { /*invalid character*/
        if (!(isDigit(word[i]) || isAlpha(word[i]))) {
            return 0;
        }
    }
    return 1;
}

/*
Function checks if there is .data , .string or .struct after label
INPUT:  char* line - pointer to the current line
OUTPUT: 1 if data was found, 0 - if no */
int data_after_label(char* line) {
    int k = 0, i = 0, data_found = 0;
    char* word;
    word = (char*)malloc(sizeof(char) * (MAX_LINE));
    while (line[i] == ' ' || line[i] == '\t') i++;
    while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == ',' || line[i] == '\r')) {
        word[k] = line[i];
        i++;  k++;
    } word[k] = '\0';
    if (strcmp(word, ".data") == 0 || strcmp(word, ".string") == 0 || strcmp(word, ".struct") == 0)
        data_found = 1;

    free(word);
    return data_found;
}
#endif

void add(char* n, char* Data, List** pL);
List* newList(char* n, char* Data);
void asemblerSecond(char* fileName, List* entList, List* labelList, int * error, int* counters);
void writeEntFile(List* entList, List* labelList, char* fileName);
void deleteList(Node* pN);
int register_to_binar(char* line, int i, int oprCode, char* binarAdd, int p, int * error, int lineCounter);
