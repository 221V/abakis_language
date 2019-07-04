; XECUTOR - minimal shortcut program.
; 2-DO: executable icon image resource

format PE GUI 4.0
entry code

kernel_name: db 'KERNEL32.DLL', 0
_ExitProcess: db 0, 0, 'ExitProcess', 0
kernel_table:
ExitProcess: dd RVA _ExitProcess
dd 0

shell_name: db 'SHELL32.DLL', 0
_ShellExecuteA: db 0, 0, 'ShellExecuteA', 0
shell_table:
ShellExecuteA: dd RVA _ShellExecuteA
dd 0

user_name:    db 'USER32.DLL', 0
_MessageBoxA: db 0, 0, 'MessageBoxA', 0
_wsprintfA:   db 0, 0, 'wsprintfA', 0
user_table:
MessageBoxA: dd RVA _MessageBoxA
wsprintfA:   dd RVA _wsprintfA
dd 0

data import
dd 0,0,0, RVA kernel_name, RVA kernel_table
dd 0,0,0, RVA shell_name, RVA shell_table
dd 0,0,0, RVA user_name, RVA user_table
dd 0,0,0,0,0
end data

X.VBA equ 'BINARY\EMULATOR\VBA\VISUALBOYADVANCE.EXE'
X.FASMW equ 'BINARY\ASSEMBLER\FASMW.EXE'
X.FASMWARM equ 'BINARY\ASSEMBLER\FASMWARM.EXE'

X equ X.FASMW ; < file to execute

error.t db 'Error', 0
no.exist.t db 'File does not exist: '
db 0Dh, 0Ah, 0Dh, 0Ah
name db X, 0
times (128-($-name)-1) db 0

code:
push 3 0 0 name 0 0
call dword [ShellExecuteA]
cmp eax, 2
jne .exit

.error:
push 0 error.t no.exist.t 0
call dword [MessageBoxA]
jmp .exit

.exit:
push 0
call dword [ExitProcess]

include '..\..\include\intel\windows\resource.inc'

RT_ICON=3
RT_GROUP_ICON=14
LANG_NEUTRAL=0
IDI_MAIN=200h

section '.rsrc' resource data readable

  directory RT_ICON,icons,\
   RT_GROUP_ICON,group_icons

  resource icons,\
	   1,LANG_NEUTRAL,main_icon_data
  resource group_icons,\
	   IDI_MAIN,LANG_NEUTRAL,main_icon

  icon main_icon,main_icon_data,'main.ico'