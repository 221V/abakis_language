; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; ************** STAR^2 SOFTWARE *****************
; ????????????????????????????????????????????????
;	   __	       __   _	  
;   ____ _/ /_	____ _/ /__(_)____
;  / __ `/ __ \/ __ `/ //_/ / ___/
; / /_/ / /_/ / /_/ / ,< / (__	) 
; \__,_/_.___/\__,_/_/|_/_/____/	
;		     __      __  _	     
;   ___ _   ______  / /_  __/ /_(_)___	____ 
;  / _ \ | / / __ \/ / / / / __/ / __ \/ __ \
; /  __/ |/ / /_/ / / /_/ / /_/ / /_/ / / / /
; \___/|___/\____/_/\__,_/\__/_/\____/_/ /_/ 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; true portability

; define cpu intel windows
; define cpu intel kolibri
; define cpu arm angel
; define cpu arm mobile
; define cpu arm gba

; SLASH fix '/' ; direction

; current directory. optional '/usbhd0/1'
; prefix to assemble+run from SD in kolibri

; CD fix ''
; CD fix '../..'
; CD fix '/usbhd0/1'
; CD fix '\Storage Card'

MAIN.ICON fix ''

RET   equ ,0Dh,0Ah,
RET.C equ 0A0Dh

if.intel.cpu fix match =intel x, cpu
if.arm.cpu fix match =arm x, cpu

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

match x =windows, cpu {
 format PE GUI 4.0
 entry !main!
 stack 8*KB
 heap 16*KB
}

match x =kolibri, cpu {
 format binary as ""
 use32
 org 0
 db 'MENUET01'
 dd 1, !main!, end.code,\
  4*MB, 32*KB, 0, 0
 postpone \{ end.code: \}
}

match x =mobile, cpu {
 format PE GUI
 entry !main;!
 stack 8*KB
 heap 16*KB
 WINDOW.W=800
 WINDOW.H=480
}

match x =gba, cpu {
 format binary as 'GBA'
 postpone \{ _end_: \}
}

; include CD#'/include/x.inc'

if.intel.cpu {
 include 'intel/language.inc'
 include 'intel/fpu.inc'
}

if.arm.cpu {
 include 'arm/cpu.inc'
 include 'arm/language.inc'
}

match x =gba, cpu
 { include 'arm/gba/system.inc' }

include 'memory.inc'
include 'text.inc'

match x =windows, cpu {
 include 'draw.inc'
 include 'intel/windows/system.inc'
 include 'image.inc'
 include 'font.inc'
}

match x =kolibri, cpu {
 include 'draw.inc'
 include 'intel/kolibri/system.inc'
 include 'image.inc'
 include 'font.inc'
}

match x =angel, cpu
 { include 'arm/angel/system.inc' }

match x =mobile, cpu
 { include 'arm/mobile/system.inc' }

match x =gba, cpu {
 include 'arm/gba/draw.inc'
 include 'arm/gba/image.inc'
}

define ?z 0
match x =windows, cpu { define ?z 1 }
match x =angel, cpu { define ?z 1 }
match x =mobile, cpu { define ?z 1 }
match =1, ?z { include 'file.inc' }

macro use [f] {
 forward include \
  CD#'/include/'#`f#'.inc'
}

match =1, SEXY { include 'sexy.inc' }