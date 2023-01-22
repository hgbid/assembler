
.entry LOOP 
.entry LENGTH 
.extern L3 
.extern W 
MAIN: mov S1.3, W 
 add r2,STR 
LOOP: jmp /W 
 prn #-5 
 sub r1 r4 
 inc K 
 lea r3, r4	
mov S1.2, r3 
not END, r1
END: hlt 
STR: .string 
LENGTH: .data 6,-9,15 
K: .data 22 
S1: .struct 8, "ab

