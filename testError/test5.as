;file ps.as 
.extern L3 
.extern W 
MAIN: move S1.1, W 
 add r2,STR 

macro m1
	END: hlt 
endmacro

 prn #-5 
 sub r1, r4 
 rts K
 bne L3 
LOOP: jmp W, #6 
STR: .string "tuyj" 
m1
m1
LENGTH: .data 6,-9,1000 
K: .data 22 

