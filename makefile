assembler: 
	gcc -g -pedantic -Wall -ansi main.c pre_assm.c first_scan.c second_scan.c -o assembler
Clean:
	rm *.o assembler
