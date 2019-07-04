format ELF executable
entry start

segment readable writable executable

start:
  mov r0, 0
  add r1, pc, t-$-8
  mov r2, n
  swi 900004h
  mov r0, 6
  swi 900001h

t: db 'Example', 0Ah
n=$-t