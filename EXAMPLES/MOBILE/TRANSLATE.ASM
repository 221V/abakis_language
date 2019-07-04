; TEST TRANSLATION FROM INTEL-LIKE
; ASSEMBLER TO ARM

include '../../include/arm/cpu.inc'

lea r1, [r2]
lea r1, [r2+r3]
lea r1, [r2+10000000h]
lea r1, [r2-20000000h]
lea r1, [r2*3]
lea r1, [r2*4]
lea r1, [r2*5]
lea r1, [r2*10]
lea r1, [r2+r3*4]
lea r1, [r4+r7*2+30000000h]
lea r2, 40000000h
lea r3, [50000000h]
lea r4, [60000000h+r7]
lea r5, [70000000h+r7*8]

mov r1, r2
mov r1, 10000000h
mov r1, [r2]
mov [r1], r2
mov r1, [40000000h]
mov r1, [r2+r3]
mov [r1+r2*4], r3
mov r1, [r2+80000000h]

add r1, r2
sub r1, 10000000h
and r1, [r2]
or [r1], r2
cmp [r1+r2*4], r3
xor r1, [r2+80000000h]