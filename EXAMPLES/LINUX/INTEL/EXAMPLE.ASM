format ELF executable
entry start

segment readable writable executable

start:
  mov eax, 4
  mov ebx, 1
  mov ecx, t
  mov edx, n
  int 80h
  mov eax, 1
  xor ebx, ebx
  int 80h

t db 'Example', 0Ah
n=$-t