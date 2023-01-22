; file ps.as 

.entry ABC
.extern DD
 add r2,STR 
macro m1
 	prn #-5 
 	sub r1  	,r6
 	inc DD
endmacro
mov S1.2, r3 
.entry LOOP 
m1
END: hlt 
; hi there how are you
STR: .string "abcdef"
ACB: .data  +6  ,31	, -15
K: .data 22 
m1
S1: .struct 2, "hellow" 

