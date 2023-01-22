file ps.as 

.entry LOOP 
.entry LENGTH 
.extern L3 
.extern W 
MAIN: mov S1.1, W 
 add r2,STR 
LOOP: jmp W 
 prn #-5 
 sub r1, r4 
 inc K 
 
mov S1.2
 bne L3 
END: hlt 
STR: .string "tuj"k 
LENGTH: .data 6,-9,15 
K: .data 22 

