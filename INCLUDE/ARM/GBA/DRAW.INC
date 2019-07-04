; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? DRAW.INC ????????????????????

; UNFINISHED...

; fast portable graphics. BGR15

include 'color.inc'
include '../../box.inc'

;;;;;;;;;;;;;;;;;;;;; SCREEN ;;;;;;;;;;;;;;;;;;;;;

align

void vga.p

integer os.w, os.h, os.bpp,\
 screen.x, screen.y, screen.w, screen.h,\
 screen.n, screen.bpp, screen.size,\
 screen.pw, screen.pitch

void palette.p

BOX g.box

function set.screen
  alias n=r0, pw=r1
  . r0=VRAM, vga.p=r0
  . r0=SCREEN.W, screen.w=r0
  . r0=SCREEN.H, screen.h=r0
  . r0=SCREEN.BPP, screen.bpp=r0
  . r0=SCREEN.PITCH, screen.pitch=r0
  . r0=SCREEN.N, screen.n=r0
  . r0*2, screen.size=r0
  . r0=2, screen.pw=r0
endf

;;;;;;;;;;;;;;;;;;;; DRAWING ;;;;;;;;;;;;;;;;;;;;;

; erase screen with color

function clear.screen, x
  alias vga=r0, c=r1, n=r2
  . vga=VRAM, n=SCREEN.N, c=x
  loop n, (u16) *vga++=c, endl
endf

; calculate x/y offset: (y*screen.w+x)*2

macro get.xy x, y {
;  . r0=y, r0*screen.w, r0+x, r0*2
  mov r0, y
  ldi r4, SCREEN.W ; screen.w
  mul r0, r4
  mov r4, x
  add r0, r4
  shl r0, 1
}

; address &vga[(y*screen.w+x)*2]

function vga.xy, x, y
  . r0=x, r1=y
  get.xy r0, r1
  . r1=VRAM, r0+r1
endf

; draw pixel

function draw.pixel, x, y, c
  alias p=r0, z=r1
  . r0=x
  ldi r1, 240
  mul r0, r1
  . r1=y
  add r0, r1
  shl r0, 1
  ldi r1, VRAM
  add r0, r1
  . z=c, (u16) *p=z
endf 1

; draw horizontal line

function draw.line.h, x, y, n, c
  alias p=r0, z=r1, w=r2, i=v1
  . r0=x, r1=y
  vga.xy r0, r1
  . z=c, i=n
  loop i, (u16) *p++=z, endl
endf 1

; draw vertical line

function draw.line.v, x, y, n, c
  locals swb
  alias p=r0, z=r1
  vga.xy x, y
  . z=c, swb=screen.pitch
  loop n, (u16) *p=z, p+swb, endl
endf 1

; draw solid rectangle

function draw.box, x, y, w, h, c
;  locals i
;  try visible x, y, w, h
  alias i=v1,\
   xx=v2, yy=v3, ww=v4, hh=v5, cc=v6
  . xx=x, yy=y, ww=w, hh=h, cc=c
  . i=yy, i--
  loop hh, i++
    draw.line.h xx, i, ww, cc
  endl
endf 1

; draw rectangle outline

function draw.outline, x, y, w, h, c
  try visible x, y, w, h
  draw.line.h x, y, w, c    ; top
  . r0=y, r0+h, r0--        ; bottom
  draw.line.h x, r0, w, c
  . r0=y, r0++, r1=h, r1-2  ; left
  draw.line.v x, r0, r1, c
  . r0=x, r0+w, r0--
  . r2=y, r2++, r1=h, r1-2  ; right
  draw.line.v r0, r2, r1, c
endf 1

macro draw.box.s b, c
 { draw.box b#.x, b#.y, b#.w, b#.h, c }
macro draw.box.o b, c
 { draw.outline b#.x, b#.y, b#.w, b#.h, c }

macro draw.box a, b, c, d, e {
  IF ~e eq
    draw.box a, b, c, d, e
  ELSE IF ~d eq
    'Unsupported'
  ELSE IF ~c eq
    draw.box.s a, b
    draw.box.o a, c
  ELSE IF ~b eq
    draw.box.s a, b
  END IF
}

; draw scanline

function draw.scanline, pixels, x, y, w
  alias p=r0, s=r1
  vga.xy x, y
  . s=pixels
  loop w, (u16) *p++=*s++, endl
endf 1

; draw scanline with transparent key

function draw.scanline.t, pixels, x, y, w, k
  alias p=r0, s=r1, c=r2, n=v1, z=v2
  . r0=x, r1=y
  vga.xy r0, r1
  . s=pixels, n=w, z=k
  loop n, (u16) c=*s++
    if c<>z
      . (u16) *p=c
    end
    . p+2
  endl
endf 1

; draw bitmap

function draw.bitmap, pixels, x, y, w, h
  locals i, p
  . i=y, p=pixels
  loop h
    draw.scanline p, x, i, w
    . r0=w, r0*2, p+r0, i++
  endl
endf 1

; draw bitmap with transparency by
; upper left pixel color (X=0, Y=0)

function draw.bitmap.t, pixels, x, y, w, h
;  locals i, p, k
;  try visible x, y, w, h
;  . i=y, r0=pixels, p=r0
  alias i=v1, p=v2, k=v3, n=v4
  . r0=y, i=r0, r0=pixels, p=r0
  . (u16) r0=*r0, k=r0
  . n=h
  loop n
    . r0=p, r1=x, r2=y, r3=w, r4=h
    draw.scanline.t r0, r1, i, r3, k
    . r0=w, r0*2, p+r0, i++
  endl
endf 1