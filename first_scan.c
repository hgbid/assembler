#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE 80 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first_scan.h"

void findCaseAssembler(char* line, int* ans, int* error, int lineCounter);
int operationToBinar(char* line, FILE* objFile, List* labelList, int* error, int lineCounter);
int generalDataToBinar(char* line, FILE* tmpFile, int instCounter, int* error);

int data_after_label(char* line);
int digits_to_binar(char* line, int i, FILE* tmpFile, int instCounter, int* error);
int string_to_binar(char* line, int i, FILE* tmpFile, int instCounter, int* error);
int struct_to_binar(char* line, int i, FILE* tmpFile, int instCounter, int* error);
int digits_to_binar_struct(char* line, int i, FILE* tmpFile, int lineCounter);
int check_label_is_correct(char* word);
/*
Function that implements the first run on assembler file
and transfers to binary representation operands and data(.data, .string, .struct)

INPUT: char* fileName -  pointer to the file

OUTPUT: no output(void type)
*/
void asemblerFirst(char* fileName) {
    char* line, * word, * amFileName, * adressString;
    int error[1];
    int option[3], adress = 100;
    int isLabel = 0, i, k, lineCounter = 0, * counters, lineAdd; /*counter[0]-IC, counter[1]-DC*/
    FILE* tmpFile, * amFile;
    List* entList, * labelList;

    error[0] = 0;
    labelList = newList("", "");
    entList = newList("", "");
    counters = (int*)malloc(sizeof(int) * (2));
    counters[0] = 0; counters[1] = 0;

    /*open files:*/
    amFileName = (char*)malloc(sizeof(char) * (strlen(fileName) + 4));
    strcpy(amFileName, fileName);
    strcat(amFileName, ".am");
    amFile = fopen(amFileName, "r");
    if (amFile == NULL) {
        printf("Error in opening new file"); exit(1);
    }
    free(amFileName);

    tmpFile = fopen("tmp.txt", "w");
    if (tmpFile == NULL) {
        printf("Error in opening new file\n"); exit(1);
    }


    line = (char*)malloc(sizeof(char) * MAX_LINE);
    word = (char*)malloc(sizeof(char) * MAX_LINE);
    adressString = (char*)malloc(sizeof(char) * 4);
    while (fgets(line, MAX_LINE, amFile) != NULL) {
        fprintf(tmpFile, "*\n"); lineCounter++;
        findCaseAssembler(line, option, error, lineCounter);

        switch (option[0]) {

        case 0:
            break;

        case 1:     /*label - save in a list*/
            strncpy(word, line + option[1], option[2]);
            word[option[2]] = '\0';
            if (!check_label_is_correct(word)) {
                printf("Error, Invalid label name  (line %d)\n", lineCounter);
                error[0] = 1;
            }
            sprintf(adressString, "%d", adress);
            if (isInList(word, labelList)) {
                printf("Error, Defining a label with the name of an existing label (line %d)\n", lineCounter);
                error[0] = 1;
            }
            else
                add(adressString, word, &labelList);
            lineAdd = operationToBinar(line + option[1]+option[2]+1, tmpFile, labelList, error, lineCounter);
            if (lineAdd != -1) {
                adress += lineAdd;
                counters[0] += lineAdd;
            }
            else {/* check if there is data after the label*/
                if (data_after_label(line + option[1] + option[2]+1)) {
                    lineAdd = generalDataToBinar(line + option[1] + option[2]+1, tmpFile, lineCounter, error);
                    adress += lineAdd;
                    counters[1] += lineAdd;
                }
                else {
                    printf("Error, unreconized command (line %d)\n", lineCounter);
                    error[0] = 1;
                }
            }
            break;

        case 2: /*general data - print to tmp file*/
            lineAdd = generalDataToBinar(line + option[2], tmpFile, lineCounter, error);
            adress += lineAdd;
            counters[1] += lineAdd;
            break;

        case 3: /*entry - save to ent list*/
            i = option[1] + 1; k = 0;
            while (line[i] == ' ' || line[i] == '\t') i++;
            while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r')) {
                word[k] = line[i];
                i++; k++;
            } word[k] = '\0';

            if (!check_label_is_correct(word)) {
                printf("Error, Invalid label name  (line %d)\n ", lineCounter);
                error[0] = 1;
            }

            if (isInList(word, entList)) {
                printf("Error, Defining a label with the name of an existing label (line %d)\n ", lineCounter);
                error[0] = 1;
            }
            else
                add("", word, &entList);
            break;

        case 4: /*extern - add to labels' list*/
            i = option[1] + 1; k = 0;
            while (line[i] == ' ' || line[i] == '\t') i++;
            while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r')) {
                word[k] = line[i];
                i++; k++;
            } word[k] = '\0';

            if (!check_label_is_correct(word)) {
                printf("Error, Invalid label name  (line %d)\n ", lineCounter);
                error[0] = 1;
            }
            if (isLabel == 0)   isLabel = 1;
            if (isInList(word, labelList)) {
                printf("Error, Defining a label with the name of an existing label (line %d)\n ", lineCounter);
                error[0] = 1;
            }
            else 
                add("EXTERN", word, &labelList);

            break;

        default:
            lineAdd = operationToBinar(line, tmpFile, labelList, error, lineCounter);
            counters[0] += lineAdd;
            if (lineAdd == -1 && !isLineEmpty(line)) {
                printf("Error, unreconized operation (line %d)\n", lineCounter);
                error[0] = 1;
            }
            adress += lineAdd;
            break;

        }
    }
    free(line); free(word); free(adressString);
    fclose(tmpFile);
    asemblerSecond(fileName, entList, labelList, error, counters);
    deleteList((entList)->firstNode);
    free(entList);
    deleteList((labelList)->firstNode);
    free(labelList);
}


/*
Function checks what was found in a row - label, .data, .string, .struct,
.entry , .extern

INPUT: char* line - pointer to the line in the file
       int* ans   - pointer to the array(will store answers according to what was found for use in asemblerFirst function.

OUTPUT: no output(void type)
*/
void findCaseAssembler(char* line, int* ans, int* error, int lineCounter) {

    char word[MAX_LINE];
    int i = 0, k = 0;
    ans[0] = -1; ans[1] = 0;
    while (line[i] == ' ' || line[i] == '\t') i++;
    while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r' || line[i] == ':')) {
        word[k] = line[i];
        i++;  k++;
    } word[k] = '\0';
    if (word[0] == ';' || k == 0) {   /*note*/
        ans[0] = 0;
    }
    if ((line[i] == ':') && (isAlpha(word[0]) != 0)) {    /*label*/
        ans[0] = 1;
        ans[1] = i - k;       /*start index*/
        ans[2] = k;       /*word length*/
        if (ans[2] > 30) {
            printf("Error, Label length cannot be more then 30 (line %d)\n", lineCounter);
            error[0] = 1;
        }
    }
    if (strcmp(word, ".data") == 0 || strcmp(word, ".struct") == 0 || strcmp(word, ".string") == 0) {
        ans[0] = 2;
        ans[1] = i - k;  /*start*/
        ans[2] = 1;
    }
    if (strcmp(word, ".entry") == 0) {
        ans[0] = 3;
        ans[1] = i;       /*end index*/

    }
    if (strcmp(word, ".extern") == 0) {
        ans[0] = 4;
        ans[1] = i;  /*end index*/
    }
}

/*
Function checks the assembly operation and transfers the operation to binary representaion

INPUT: char* line      - pointer to the current line
       FILE* tmpFile   - pointer to temporary file
       List* labelList - pointer to the list that stores labels

OUTPUT: number of lines in binary representation
*/
int operationToBinar(char* line, FILE* tmpFile, List* labelList, int* error, int lineCounter) {

    char* word, binarOparation[11], binarTempAdd[12], * binarAdd;
    char* operations[16] = { "mov", "cmp","add","sub","not","clr", "lea", "inc",
    "dec","jmp","bne","get","prn","jsr","rts","hlt" };
    int oprCode = -1, i = 0, k = 0, registersFounded, p, foundOperand = 0, addLine = 0, j;
    Node* index;
    binarAdd = (char*)malloc(sizeof(char) * (MAX_LINE * 5));
    strcpy(binarAdd, "");
    word = (char*)malloc(sizeof(char) * (MAX_LINE));

    while (line[i] == ' ' || line[i] == '\t') i++;
    while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r')) {
        word[k] = line[i];
        i++; k++;
    } word[k] = '\0';

    for (j = 0; j < 16; j++) { /*find opration*/
        if (strcmp(word, operations[j]) == 0) {
            oprCode = j;
            addLine++;
            break;
        }
    } if (oprCode == -1) return oprCode;

    decToBin(oprCode, 4, binarOparation);

    if (oprCode == 14 || oprCode == 15) { /*non operands*/
        while (line[i] == ' ' || line[i] == '\t') i++;
        if (!(line[i] == '\n' || line[i] == '\r')) {
            error[0] = 1;
            printf("Error, too many arguments (line %d)\n", lineCounter);
        }
        for (j = 4; j < 8; j++)
            binarOparation[j] = '0';
    }

    else /*one or two operands*/
    {
        for (p = 0; p < 2; p++) {
            foundOperand = 0;
            while (line[i] == ' ' || line[i] == '\t') i++;
            if (p == 1) {
                if (line[i] != ',') {
                    printf("Error, no comma (line %d)\n", lineCounter);
                    error[0] = 1;
                }
                else i++;
            }
            while (line[i] == ' ' || line[i] == '\t') i++;

            /*find the operand type:*/
            /*register*/
            if (foundOperand == 0) {
                registersFounded = 0;
                registersFounded = register_to_binar(line, i, oprCode, binarAdd, p, error, lineCounter);
                if (registersFounded == 2) {
                    if (oprCode == 6) {
                        printf("Error! 'lea' canot get a register as source (line %d)\n", lineCounter);
                        error[0] = 1;
                    }
                    if (oprCode == 4 || oprCode == 5 || (oprCode >= 7 && oprCode <= 13)) {
                        printf("Error! too many argument (line %d)\n", lineCounter);
                        error[0] = 1;
                    }
                    else {
                        for (k = 4; k < 8; k++)  binarOparation[k] = '1';
                        i += 2;
                        while (line[i] == ' ' || line[i] == '\t' || line[i] == ' ' || line[i] == ',') i++;
                        i += 2;
                        foundOperand = 1;
                        p++;
                    }
                }
                if (registersFounded == 1) {
                    if (oprCode == 6 && p==0) {
                        printf("Error! 'lea' canot get a register as source (line %d)\n", lineCounter);
                        error[0] = 1;
                    }
                    i ++;
                    while (line[i] == ' ' || line[i] == '\t') i++;
                    if (p == 0 && line[i] != ',') i++;

                    binarOparation[4 + 2 * p] = '1';
                    binarOparation[5 + 2 * p] = '1';
                    foundOperand = 1;
                }
                if (registersFounded) addLine++;
            }

            while (line[i] == ' ' || line[i] == '\t') i++;
            k = 0;
            while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r' || line[i] == '.' || line[i] == ',' || line[i] == '#')) {
                word[k] = line[i];
                i++; k++;
            } word[k] = '\0';

            /*number*/
            if (line[i] == '#') {
                addLine++;
                if (oprCode == 0 || oprCode == 2 || oprCode == 3) {
                    if (p == 0) {
                        binarOparation[4] = '0'; binarOparation[5] = '0'; /*source address*/
                    }
                    else {
                        error[0] = 1;
                        printf("Error, Illegal use of instant number (line %d)\n", lineCounter);
                    }
                }
                else if (oprCode == 12) {
                             binarOparation[6] = '0'; binarOparation[7] = '0'; /*destination address*/
                 }
                 else if (oprCode == 1) {
                     if (p == 0) {
                         binarOparation[4] = '0'; binarOparation[5] = '0'; } /*source address*/
                     else {
                         binarOparation[6] = '0'; binarOparation[7] = '0'; } /*destination address*/
                 }
                 else {
                    error[0] = 1;
                    printf("Error, Illegal use of instant number (line %d)\n", lineCounter);
                }
                k = 0; i++;
                memset(word, 0, strlen(word));
                while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n'|| line[i] == ',')) {
                    word[k] = line[i];
                    i++; k++;
                } word[k] = '\0';
		if(atoi(word)>127 || atoi(word)<-128){
                    error[0] = 1;
                    printf("Error, the instant number's range must be in [-128,127] (line %d)\n", lineCounter);
		}
                decToBin(atoi(word), 8, binarTempAdd);
                binarTempAdd[8] = '0'; binarTempAdd[9] = '0'; binarTempAdd[10] = '\n'; binarTempAdd[11] = '\0';
                strcat(binarAdd, binarTempAdd);
                foundOperand = 1;
            }

            /*label*/
            if (foundOperand == 0) {
                index = labelList->firstNode->next;
                while (index != NULL) {
                    if (strcmp(word, index->data) == 0) {
                        if (!strcmp("EXTERN", index->name)) { /*extern*/
                            addLine++;
                            foundOperand = 2;
                            if (p == 0 && !(oprCode == 4 || oprCode == 5 || (oprCode >= 7 && oprCode <= 13))) {
                                binarOparation[4] = '0'; binarOparation[5] = '1';
                            } /*source address*/
                            else { binarOparation[6] = '0'; binarOparation[7] = '1'; } /*destination address*/
                            strcat(binarAdd, "? ");
                            strcat(binarAdd, index->data);
                            if (line[i] != '.') strcat(binarAdd, "\n");
                        }
                        break;
                    }
                    index = index->next;
                }
                if (foundOperand == 0) {  /* label (not extern)*/
                    if (oprCode == 4 || oprCode == 5 || (oprCode >= 7 && oprCode <= 13)) {
                        binarOparation[6] = '0'; binarOparation[7] = '1';
                    } /*source address*/
                    else { binarOparation[4 + 2 * p] = '0'; binarOparation[5 + 2 * p] = '1'; }
		    if (!check_label_is_correct(word)) {
		        printf("Error, Invalid label name  (line %d)\n ", lineCounter);
		        error[0] = 1;
		    }

                    strcat(binarAdd, "? ");
                    strcat(binarAdd, word);
                    if (line[i] != '.') strcat(binarAdd, "\n");
                    addLine++;
                }
                if (line[i] == '.') {
                    if (line[i + 1] != '1' && line[i + 1] != '2') {
                        printf("Error, Struct has only first and second fields (line %d)\n", lineCounter);
                        error[0] = 1;
                    }
                    addLine++;/*index*/
                    binarOparation[4 + p * 2] = '1'; binarOparation[5 + p * 2] = '0'; /*struct*/
                    k = 0; i++;
                    while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == ',' || line[i] == '\r')) {
                        word[k] = line[i];
                        i++; k++;
                    } word[k] = '\0';
                    decToBin(atoi(word), 8, binarTempAdd);
                    strcat(binarAdd, ".");
                    strcat(binarAdd, word);
                    strcat(binarAdd, "\n");
                    binarTempAdd[8] = '0'; binarTempAdd[9] = '0'; binarTempAdd[10] = '\n'; binarTempAdd[11] = '\0';
                    strcat(binarAdd, binarTempAdd);
                }
            }
            if (oprCode == 4 || oprCode == 5 || (oprCode >= 7 && oprCode <= 13)) { /*one operand*/
                binarOparation[4] = '0'; binarOparation[5] = '0'; /*source address = 00*/
                break;
            }

        }
    }
    while (line[i] == ' ' || line[i] == '\t') i++;
            if (!(line[i] == '\n' || line[i] == '\r')) {
                error[0] = 1;
                printf("Error, too many arguments (line %d)\n", lineCounter);
            }

    binarOparation[8] = '0'; binarOparation[9] = '0'; /* A.R.E. = 00*/
    binarOparation[10] = '\0';
    fprintf(tmpFile, "%s\n", binarOparation);

    if (strlen(binarAdd) > 1) {
        binarAdd[strlen(binarAdd) - 1] = '\0';
        fprintf(tmpFile, "%s\n", binarAdd);
    }
    free(word); free(binarAdd);
    return addLine;
}


/*
Function checks the type of data and calls for corresponding function
INPUT:  char* line      - pointer to the current line
        FILE* tmpFile   - pointer to the temporary file
        int instCounter - row number of the file
OUTPUT: number of rows in binary representation
*/
int generalDataToBinar(char* line, FILE* tmpFile, int lineCounter, int* error) { /*genaral data*/
    char* word;
    int i = 0, k = 0;
    int line_count = 0;
    word = (char*)malloc(sizeof(char) * (MAX_LINE));
    while (line[i] == ' ' || line[i] == '\t') i++;
    while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r' || line[i] == ',')) {
        word[k] = line[i];
        i++;   k++;
    } word[k] = '\0';

    if (strcmp(word, ".data") == 0)
        line_count = digits_to_binar(line, i, tmpFile, lineCounter, error);
    if (strcmp(word, ".string") == 0)
        line_count = string_to_binar(line, i, tmpFile, lineCounter, error);
    if (strcmp(word, ".struct") == 0)
        line_count = struct_to_binar(line, i, tmpFile, lineCounter, error);
    if (strcmp(word, ".data") != 0 && strcmp(word, ".string") != 0 && strcmp(word, ".struct") != 0) {
        printf("Error, .data , . string or .struct was not found (line %d)\n", lineCounter);
        error[0] = 1;
    }
    free(word);
    return line_count;
}

/*
Function receives a line from file and transfers all numbers after .data
to binary representation

INPUT: current file line
       index of current position in line
       FILE * pointer to the temp file
       number of row in file

OUTPUT: number of rows after transforming all numbers after .data
*/
int digits_to_binar(char* line, int i, FILE* tmpFile, int lineCounter, int* error) {
    int count_numbers = 0;
    int number_found = 0;
    int comma_after_number = 0;
    int num, k = 0, negative = 0;
    char* word;
    char* binLetter;
    word = (char*)malloc(sizeof(char) * (MAX_LINE));
    binLetter = (char*)malloc(sizeof(char) * (11));
    while (line[i] == ' ' || line[i] == '\t') i++;
    if (!(isDigit(line[i])) && (line[i] != '\n' && line[i] == '\r')) {
        printf("Error. Symbol found before number (line %d)\n", lineCounter);
        error[0] = 1;
    }
    if (line[i] == '\n' || line[i] == '\r') {
        printf("Error, No number was found after .data (line %d)\n", lineCounter);
        error[0] = 1;
    }
    while (line[i] != '\n' && line[i] != '\r') {
        if (!isDigit(line[i]) && (line[i]!='-' && line[i] != '+')) {
            printf("Error, Wrong symbol after .data (line %d)\n", lineCounter);
            error[0] = 1; break;
        }
        if ((number_found == 1 && comma_after_number == 0) && line[i] == ',') {
            comma_after_number = 1;
        }
        if ((number_found == 1 && comma_after_number == 1) && line[i] == ',') {
            printf("Error, Found additional comma after number (line %d)\n", lineCounter);
            error[0] = 1; break;
        }
        if ((comma_after_number = 1 && number_found == 1) && line[i + 1] == '\n') {
            printf("Error, Comma found after last number (line %d)\n", lineCounter);
            error[0] = 1; break;
        }

        if (line[i] == '\n' || line[i] == '\r') break;
        if (isDigit(line[i])) {

            number_found = 1;
            k = 0;
            if (line[i - 1] == '-') negative = 1;
            while (isDigit(line[i])) {
                word[k] = line[i];
                i++; k++;
            }
            word[k] = '\0';
            num = atoi(word);
		if(num>511 || num<-512){
                    error[0] = 1;
                    printf("Error, the data number's range must be in [-512,511] (line %d)\n", lineCounter);
		}
            count_numbers += 1;
            if (negative == 1)  num = num * (-1);
            decToBin(num, 10, binLetter);
            fprintf(tmpFile, "%s\n", binLetter);
            while (line[i] == ' ' || line[i] == '\t') i++;
        if (line[i] == '\n' || line[i] == '\r') break;
            negative = 0;
        }
        comma_after_number = 0;
        if (line[i] == ',' ||line[i] == '-' || line[i] == '+')  i ++;

        while (line[i] == ' ' || line[i] == '\t') i++;
    }
    if (number_found == 0) {
        printf("Error. No data after .data (line %d)\n", lineCounter);
        error[0] = 1;
    }
    free(word);
    free(binLetter);
    return count_numbers;
}

/*
Function receives a line from file and transfers all letters and symbols in string after .string to binary representation


INPUT: current file line
       index of current position in line
       FILE * pointer to the temp file
       number of row in file

OUTPUT: number of rows after transforming all string numbers after .string

*/
int string_to_binar(char* line, int i, FILE* tmpFile, int lineCounter, int* error) {
    int count_string_length = 0;
    char* binLetter;
    binLetter = (char*)malloc(sizeof(char) * (11));
    while (line[i] == ' ' || line[i] == '\t') i++;
    if (line[i] == '\"') {        /*string exist */
        i++;
        while (line[i] != '\"') {
            if (line[i] == '\n' || line[i] == '\r') {
                printf("Error! no closing bracket (line %d)\n", lineCounter);
                error[0] = 1; free(binLetter);
                return -1;
            }
            if (line[i] >= ' ' && line[i] <= '~' && line[i] != '\n' && line[i] != '\r') {
                count_string_length += 1;
                decToBin((int)line[i], 10, binLetter);
                fprintf(tmpFile, "%s\n", binLetter);
                i++;
            }
            else {
                printf("Error! Invalid input (line %d)\n", lineCounter);
                error[0] = 1;
                free(binLetter);
                return -1;
            }
        }
    }
    else {
        printf("Error. There was no inverted commas after '.string' (line %d)\n", lineCounter);
        error[0] = 1;
        free(binLetter);
        return -1;
    }
    i++;
    while (line[i] == ' ' || line[i] == '\t') i++;
    if (line[i] != '\r' && line[i] != '\n' && line[i] != 0) {
        printf("Error. Symbol after the string (line %d)\n", lineCounter);
        error[0] = 1;        free(binLetter);
        return -1;
    }
    count_string_length++;
    decToBin(0, 10, binLetter);
    fprintf(tmpFile, "%s\n", binLetter);
    free(binLetter);
    return count_string_length;
}

/*
Function receives a line from file and transfers all struct fields(two) after .string to binary representation
INPUT: current file line
       index of current position in line
       FILE * pointer to the temp file
       number of row in file
OUTPUT: number of rows after transforming all struct fields after .struct
*/
int struct_to_binar(char* line, int i, FILE* tmpFile, int lineCounter, int* error) {
    int count_struct=0, count_data, count_string;
    int first_field_exist;
    int comma_after_number;
    first_field_exist = 0;
    comma_after_number = 0;
    while (first_field_exist != 1) {

        if (line[i] == ' ' || line[i] == '\t' || line[i] == '-' || line[i] == '+') {
            i += 1;
        }
        if (isDigit(line[i])) {
            count_data = digits_to_binar_struct(line, i, tmpFile, lineCounter);
            first_field_exist = 1;
        }
        if (line[i] == '\n' || line[i] == '\r') {
            printf("Error. Struct fields not found (line %d)\n", lineCounter);
            error[0] = 1;
            break;
        }
        if (line[i] != ' ' && line[i] != '\t' && first_field_exist == 0) {
            printf("Error. First field of struct must be data (line %d)\n", lineCounter);
            i += 1;
            error[0] = 1;
        }
    }
    if (first_field_exist == 1) {
        while (isDigit(line[i])) i++;
        while (line[i] != '\"') {
            if (line[i] == ',' && comma_after_number == 0) {
                comma_after_number = 1;
                i += 1;
            }
            if (line[i] == '\"') {
                break;
            }
            if (line[i] == ',' && comma_after_number == 1) {
                printf("Error, additional comma after data field of struct (line %d)\n", lineCounter);
                error[0] = 1;
            }
            if (!(line[i] == ',') && line[i] != ' ' && line[i] != '\t') {
                printf("Error, not comma after data field of struct (line %d)\n", lineCounter);
                error[0] = 1;
            }
            if (line[i] == '\n' || line[i] == '\r') {
                printf("Error. Second field of struct is not found (line %d)\n", lineCounter);
                error[0] = 1;
            }
            i += 1;
        }
        count_string = string_to_binar(line, i, tmpFile, lineCounter, error);
        count_struct = count_data + count_string;
    }
    return count_struct;
}

/*
Function receives a line from file and transfers the number in first struct files after .struct to binary representation

INPUT: current file line
       index of current position in line
       FILE * pointer to the temp file
       number of row in file

OUTPUT: number of rows after transforming number after .struct
*/
int digits_to_binar_struct(char* line, int i, FILE* tmpFile, int lineCounter) {
    int count_numbers = 0;
    int num, k = 0, negative = 0;
    char* word;
    char* binLetter;
    word = (char*)malloc(sizeof(char) * (MAX_LINE));
    binLetter = (char*)malloc(sizeof(char) * (11));

    if (isDigit(line[i])) {
        k = 0;
        if (line[i - 1] == '-') negative = 1;
        while (isDigit(line[i])) {
            word[k] = line[i];
            i++; k++;
            if (line[i] == '\n' || line[i] == '\r') break;
        }
        word[k] = '\0';
        num = atoi(word);
        count_numbers += 1;
        if (negative == 1)  num = num * (-1);
        decToBin(num, 10, binLetter);
        fprintf(tmpFile, "%s\n", binLetter);
        negative = 0;
    }
    free(word);
    free(binLetter);
    return count_numbers;
}

/*
Function checks the registeres and transfers the operation with registeres to the binary representation

INPUT: char* line     -  pointer to the current line
       int i          -  position in current line
       int oprCode    -  type of operation(from caller function)
       char* binarAdd - receivred from function caller
       int p          -  number of operands(from caller function)
OUTPUT: number of rows in binary represetnation
*/
int register_to_binar(char* line, int i, int oprCode, char* binarAdd, int p, int* error, int lineCounter) {
    char* registers[8] = { "r0","r1","r2","r3","r4","r5","r6","r7" };
    int j, k = 0;
    char* word, * binarTemp;
    word = (char*)malloc(sizeof(char) * (MAX_LINE));
    binarTemp = (char*)malloc(sizeof(char) * (12));

    while (!(line[i] == ' ' || line[i] == '\t' || line[i] == ',' || line[i] == '\n' || line[i] == '\r')) {
        word[k] = line[i];
        i++; k++;
    } word[k] = '\0';

    for (j = 0; j < 8; j++) {
        if (strcmp(word, registers[j]) == 0) {
            if (p == 1) { /*second opernd -> one register*/
                memset(binarTemp, '0', 4);
                decToBin(j, 4, binarTemp + 4);
                binarTemp[8] = '0'; binarTemp[9] = '0';
                binarTemp[10] = '\n'; binarTemp[11] = '\0';
                strcat(binarAdd, binarTemp);
                free(word);    free(binarTemp);
                return 1;
            }
            decToBin(j, 4, binarTemp);
            memset(binarTemp + 4, '0', 6);

            while (line[i] == ' ' || line[i] == '\t') i++;
            if (line[i] != '\n' && line[i] != '\r') {
                if (line[i] != ',') {
                    printf("Error! no comma (line %d)\n", lineCounter);
                    error[0] = 1;
                }
                else i++;
                while (line[i] == ' ' || line[i] == '\t') i++;
                k = 0;
                while (!(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r')) {
                    word[k] = line[i];
                    i++; k++;
                } word[k] = '\0';
                for (j = 0; j < 8; j++) {
                    if (strcmp(word, registers[j]) == 0) { /*found 2 registers*/
                        decToBin(j, 4, binarTemp + 4);
                        binarTemp[8] = '0'; binarTemp[9] = '0';
                        binarTemp[10] = '\n'; binarTemp[11] = '\0';
                        strcat(binarAdd, binarTemp);
                        free(word);    free(binarTemp);
                        return 2;
                    }
                }
            }
            binarTemp[10] = '\n'; binarTemp[11] = '\0';
            strcat(binarAdd, binarTemp);
            free(word); free(binarTemp);
            return 1;
            break;
        }
    }
    free(binarTemp); free(word);
    return 0;
}


