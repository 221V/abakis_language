; $$$$$$$$$$$$$$$ Z6502 ASSEMBLER $$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ??????????????????? C64.INC ????????????????????

;;;;;;;;;;;;;;;;;; COMMODORE 64 ;;;;;;;;;;;;;;;;;;

format binary as 'PRG'
org $1000-2

brk
db $10

; example test program. to run:
; open with C64 VICE then enter: sys 4096

; include 'c64.inc'
;
; @@:
; inc $d020
; jmp @b