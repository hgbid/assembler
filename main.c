#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

int main(int argc, char* argv[]) {
    FILE* fp;
    int i;
    if (argc >= 1) {

        for (i = 1; i < argc; i++) {
            char* fileName;
            fileName = (char*)malloc(sizeof(char) * (strlen(argv[i]) + 3));
            strcpy(fileName, argv[i]);
            strcat(fileName, ".as");
		printf("File %s: ", fileName);
            if (!(fp = fopen(fileName, "r"))) {
               printf("Error, Can not find the file %s\n", fileName);
            }
            else {
                preAssembler(fp, argv[i]);
                asemblerFirst(argv[i]);
                fclose(fp);
            }
            free(fileName);
        }
    }
    else {
        printf("Error, You have to enter at least one file name\n");
    }           
    return 0;
}
