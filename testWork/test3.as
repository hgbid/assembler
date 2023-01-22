


MAIN: mov S1.1, r1 
 add r2,STR 
LOOP: jmp r1 
 prn r2 
lea STR , r2
 sub r1, r4 
 inc K 
mov S1.2  ,  r3 
 	bne  r2  
	END:  hlt 
 STR: 	.string "abcdef" 
LENGTH:  .data   6, 	-9,15 
K: .data 22 
S1: .struct 8, "ab" 
