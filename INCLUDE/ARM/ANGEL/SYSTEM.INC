; $$$$$$$$$$$$$$$$$$ MAGIC-ARM $$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ????????????????? SYSTEM.INC ???????????????????

; QEMU+ARM Versatile PB interface with
; Primecell PL110 Color LCD Controller

; memory management

@stack   = 00001000h
@vga     = 00200000h
@memory  = 00400000h
@offset  = 0
@maximum = 8*MB
@omega   = @memory+@maximum

; associate name with address and size

macro @reserve [p] {
  match a(n), p \{
    if @offset>=@maximum
      'No memory remaining'
    end if
    a=@memory+@offset
    a\#.size=n
    @offset=@offset+n
  \}
}

macro @align.o n
 { @offset=@offset+(@offset and (n-1)) }

macro @available {
 display 'Remain:'
 display.h @omega-(@memory+@offset)
 display 'Start:'
 display.h @memory+@offset
 display 'End:'
 display.h @omega
}

@reserve @source(1*MB), @destiny(1*MB)
@reserve @buffer(1*MB), @console(1*MB)
@reserve @token(4*KB), @error(4*KB)
@reserve @path(4*KB), @name(4*KB)
@reserve @global(8*KB)

@input=@source
@output=@destiny

; "Remain:003FA000h Start:00806000h End:00C00000h"

; @available ; memory

; @global parameters...

virtual at @global
 integer g.p, g.a, g.b,\
  g.x, g.y, g.w, g.h,\
  g.left, g.top, g.right, g.bottom,\
  g.bpp, g.c1, g.c2, g.c3, g.style
 void g.text, g.palette, g.image, g.font
 void g.sprite, g.theme, g.control, g.window
 void g.buffer ; 7k+
end virtual

;;;;;;;;;;;;;;;;;;;;; SCREEN ;;;;;;;;;;;;;;;;;;;;;

SCREEN.W=640
SCREEN.H=480
SCREEN.BPP=16

SCREEN.N=SCREEN.W*SCREEN.H
SCREEN.SIZE=SCREEN.N*(SCREEN.BPP/8)
SCREEN.PITCH=SCREEN.W*(SCREEN.BPP/8)
SWD2=SCREEN.W/2
SHD2=SCREEN.H/2

LCD.BASE=0C0000000h ; LCD registers

LCD.TIMING.0=0
LCD.TIMING.1=4
LCD.TIMING.2=8
LCD.TIMING.3=0Ch
LCD.UP.BASE=10h
LCD.CONTROL=1Ch

; LCD register 0:

; [31-24] [23-16] [15-8] 765432.10
; HBPXXXX.HFPXXXX.HSWXXX.PPLXXX.00
; |
; Horizontal back porch
;         | Horizontal front porch
;                 | Horizontal pulse width
;                        |
;                        Pixels per line

; LCD register 1:

; [31-24] [23-16] [15-8] 765432.10
; VBPXXXX.VFPXXXX.VSWXXX.LPPXXX.00
; |
; Vertical back porch
;         | Vertical front porch
;                 | Vertical pulse width
;                        |
;                        Lines per pixel

; LCD control register BITs:

; * #31-#17: zeros?
; * #13-#12: generate interrupt at start of:
;   0=vertical synchronization, 1=back porch,
;   2=active video, 3=front porch
; * #11: power enable?
; * #10: endian pixel order? 0=low, 1=high
; * #9: endian byte order? 0=low, 1=high
; * #8: color format: 0=RGB, 1=BGR
; * #7: dual panel? 0=no, single. 1=dual
; * #6: 0, not monochrome
; * #5: lcd type: 1=TFT, 0=STN/grayscale
; * #4: monochrome? 0=no, color. 1=yes
; * #3-#1: 0=1BPP, 1=2BPP, 2=4BPP, 3=8BPP,
;   4=16BPP, 5=24BPP (TFT only), 6-7=reserved
; * #0: lcd enable? 1/0=yes/no

macro set.vga {
  movi a1, LCD.BASE
  movi a2, 3F0F4F9Ch
  str a2, [a1, LCD.TIMING.0]
  movi a2, 0D030DDFh
  str a2, [a1, LCD.TIMING.1]
  movi a2, 67F3800h
  str a2, [a1, LCD.TIMING.2]
  movi a2, @vga
  str a2, [a1, LCD.UP.BASE]
  ;        12-9876543210
  movi a2, 1100100101001b
  str a2, [a1, LCD.CONTROL]
}

macro @swi a, b, c { ; angel interrupt
  . r0=a, r1=b
  if ~c eq
    . r2=c
  end if
  swi 123456h
}

; heap.descriptor

align 4

void heap.base, heap.limit
void stack.base, stack.limit

void elapsed.ticks

; untested...

macro get.heap { @swi 16h, heap.base }

macro get.frequency n { @swi 31h, n }

macro elapsed { @swi 30h, elapsed.ticks }

; # seconds since 00:00 January 1, 1970

macro get.time { @swi 11h, 0 }