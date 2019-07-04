; GameBoy Advance system...

@o=2000000h ; origin = external RAM
org @o

; GBA ROM header: 192/C0h bytes...

bal !main ; 4 bytes

; official Nintendo logo: 156/9Ch bytes

db \
 024h,0FFh,0AEh,051h,069h,09Ah,0A2h,021h,\
 03Dh,084h,082h,00Ah,084h,0E4h,009h,0ADh,\
 011h,024h,08Bh,098h,0C0h,081h,07Fh,021h,\
 0A3h,052h,0BEh,019h,093h,009h,0CEh,020h,\
 010h,046h,04Ah,04Ah,0F8h,027h,031h,0ECh,\
 058h,0C7h,0E8h,033h,082h,0E3h,0CEh,0BFh,\
 085h,0F4h,0DFh,094h,0CEh,04Bh,009h,0C1h,\
 094h,056h,08Ah,0C0h,013h,072h,0A7h,0FCh,\
 09Fh,084h,04Dh,073h,0A3h,0CAh,09Ah,061h,\
 058h,097h,0A3h,027h,0FCh,003h,098h,076h,\
 023h,01Dh,0C7h,061h,003h,004h,0AEh,056h,\
 0BFh,038h,084h,000h,040h,0A7h,00Eh,0FDh,\
 0FFh,052h,0FEh,003h,06Fh,095h,030h,0F1h,\
 097h,0FBh,0C0h,085h,060h,0D6h,080h,025h,\
 0A9h,063h,0BEh,003h,001h,04Eh,038h,0E2h,\
 0F9h,0A2h,034h,0FFh,0BBh,03Eh,003h,044h,\
 078h,000h,090h,0CBh,088h,011h,03Ah,094h,\
 065h,0C0h,07Ch,063h,087h,0F0h,03Ch,0AFh,\
 0D6h,025h,0E4h,08Bh,038h,00Ah,0ACh,072h,\
 021h,0D4h,0F8h,007h

macro ZEROS n {
 REPEAT n
  db 0
 END REPEAT
}

ZEROS 18 ; game title+version
db 96h   ; id
ZEROS 10
db 0     ; CRC checksum. unknown algorithm
db 0, 0  ; END of header

IF $<>(@o+192)
 'Error: Invalid Header Size'
END IF

_start_:

;;;;;;;;;;;;;;;;;;;; ADDRESSES ;;;;;;;;;;;;;;;;;;;

BIOS=0000000h  ; 0000000h-0003FFFh, 16k
EXRAM=2000000h ; 2000000h-203FFFFh, 256k
INRAM=3000000h ; 3000000h-3007FFFh, 32k
IORAM=4000000h ; 4000000h-40003FFh, 1k
PRAM=5000000h  ; 5000000h-50003FFh, 1k
VRAM=6000000h  ; 6000000h-6017FFFh, 96k
ORAM=7000000h  ; 7000000h-70003FFh, 1k
GPROM=8000000h ; 8000000h-?
CRAM=0E000000h ; E000000h-? (64k?)

; * BIOS: system ROM. only executable,
;   not readable or writeable
; * EXRAM: standard "external/work" RAM.
;   port size=16BIT. no limitations:
;   readable, writeable, executable
; * INRAM: fast 32BIT internal RAM.
; * IORAM: memory-mapped circuit registers
; * PRAM: palette, 15BIT BGRs
; * VRAM: primary video memory
; * ORAM: object/sprite memory
; * GPROM: game cartridge ROM. see note
; * CRAM: for save data

; note: GPROM (GamePak ROM) is where
; execution begins at 8000000h+, but it's not
; writeable, only readable and executable.
; at startup, program will be copied from
; GPROM to EXRAM

; stack in fast internal RAM

STACK.SIZE=2*1024
STACK.A=INRAM

output.t=STACK.A+STACK.SIZE
buffer.t=output.t+1024

;;;;;;;;;;;;;;;; BIOS INTERRUPTS ;;;;;;;;;;;;;;;;;

numeric BIOS.*,\
 RESET, RAM.RESET, HALT, STOP, INT.WAIT,\
 VR.WAIT, DIV, DIV.ARM, SQRT, ARC.TAN,\
 ARC.TAN2, CPU.SET, CPU.FAST.SET, CHECK.SUM,\
 BG.AFFINE.SET, OBJ.AFFINE.SET

;;;;;;;;;;;;;;;;;;;;; SCREEN ;;;;;;;;;;;;;;;;;;;;;

; #  DIMENSIONS  BPP   SIZE    PAGE-FLIP?
; ----------------------------------------
; 3   240x160    15   1*12C00h  NO
; 4   240x160    8    2*9600h   YES
; 5   160x128    15   2*A000h   YES

SET.VGA=400h
VGA.MODE=3

; screen size and depth

IF VGA.MODE<>5
 SCREEN.W=240
 SCREEN.H=160
ELSE
 SCREEN.W=160
 SCREEN.H=128
END IF

SCREEN.N=(SCREEN.W*SCREEN.H)

IF VGA.MODE=4
 SCREEN.BPP=8     ; 8BIT palette
 COLOR fix ubyte
 PIXEL fix ubyte
ELSE
 SCREEN.BPP=15    ; 15BBP 1.5.5.5 (X.B.G.R)
 COLOR fix ushort
 PIXEL fix ushort
END IF

IF SCREEN.BPP=8
 SCREEN.PITCH=SCREEN.W
ELSE
 SCREEN.PITCH=(SCREEN.W*2)
END IF

IF VGA.MODE<>3   ; 2 pages?
 PAGE.FLIP=1
ELSE
 PAGE.FLIP=0
END IF

; set graphics mode

function set.vga
  . r0=IORAM, r1=SET.VGA
  . r1+VGA.MODE, (u32) *r0=r1
endf

include 'input.inc'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

align 4

!main:
 . sp=STACK.A       ; setup stack
 . a1=EXRAM         ; copy program
 . a2=GPROM         ; to external RAM
 ldi a3,\           ; from GP ROM
  ((_end_-\
   _start_)/4)
 loop a3, (u32) \
  *a1++=*a2++
 endl

 callf set.screen   ; initialize VGA
 callf set.vga
 callf on.create    ; event create

.draw:
 callf on.draw      ; event draw

.input:
 callf update.keys  ; update input
 if true
  callf on.key      ; event input
  bal .draw
 end
; callf update.mouse
; if true
;   callf on.mouse
;   bal .draw
; end
 bal .input