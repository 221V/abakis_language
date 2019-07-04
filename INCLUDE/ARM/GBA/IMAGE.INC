; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ;;;;;;;;;;;;;;;;;;; IMAGE ;;;;;;;;;;;;;;;;;;;;;;

include 'bmp.inc'

; image class/object/structure

macro IMAGE a {
  a:
  void a#.p
  integer a#.x, a#.y, a#.w, a#.h
  integer a#.bpp=32, a#.key, a#.alpha
}

virtual at 0
  ?image.p dw 0
  ?image.x dw 0
  ?image.y dw 0
  ?image.w dw 0
  ?image.h dw 0
  ?image.bpp dw 32
  ?image.key dw 0
  ?image.alpha dw 0
END virtual

?image.box fix ?image.x

; create image file/s with header:
; 8 bytes:

; byte s='I'  ; signature
; byte v=0    ; version: AABBCC.VV
; int16 w, h  ; size: w:h
; byte bpp    ; bpp: 32/24/16/15/8
; byte n      ; # colors or 0=256+

; byte pixels[w*h*(bpp/8)] ; or *2 if 15

macro IMAGEX folder, bpp, [p] {
 forward
  local w, h
  w=0
  h=0
  define ?s 0
  match a==b, p \{
   \local ..q
   ..q:
   inject.image folder, b, bpp
   load w hword from ..q+2
   load h hword from ..q+4
   a:
   void a\#.p=..q+8h
   integer a\#.x, a\#.y, a\#.w=w, a\#.h=h
   integer a\#.bpp=bpp, a\#.key, a\#.alpha
   void a\#.palette=..q+8
   define ?s 1
  \}
  IF ?s eq 0
   IMAGE p
  END IF
}

macro IMAGEX8 folder, [p] {
 forward
  local w, h
  w=0
  h=0
  define ?s 0
  match a==b, p \{
   \local ..q
   ..q: inject.image.8 folder, b
   load w hword from ..q+2
   load h hword from ..q+4
   a:
   void a\#.p=..q+408h
   integer a\#.x, a\#.y, a\#.w=w, a\#.h=h
   integer a\#.bpp=8, a\#.key, a\#.alpha
   void a\#.palette=..q+8
   define ?s 1
  \}
  IF ?s eq 0
   'Error: 8BPP must specify file'
  END IF
}

macro FONT [p] { common IMAGEX8 'FONT', p }
macro ICON24 [p] { common IMAGEX 'ICON/24', 32, p }
macro ICON32 [p] { common IMAGEX 'ICON/32', 32, p }

macro IMAGE [p] { common IMAGEX 'IMAGE', 32, p }
macro IMAGE8 [p] { common IMAGEX8 'IMAGE', p }

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; unfinished, unorganized...

; 2-DO: convert to functions. create one
; good draw.scanline.x with bpp and type
; BIT flags:

; draw.scanline.x p, x, y, w, 32,\
;  G.KEY or G.ALPHA or G.GRAY or G.INVERT.X

macro move.image i, x, y { . i#.x=x, i#.y=y }

macro draw.image i, x, y {
 IF ~x eq
   move.image i, x, y
 END IF
 draw.bitmap i#.p, i#.x, i#.y, i#.w, i#.h
}

macro draw.image.t i, x, y {
 IF ~x eq
   move.image i, x, y
 END IF
 . r0=i#.p, r1=i#.x, r2=i#.y
 . r3=i#.w, r4=i#.h
 draw.bitmap.t r0, r1, r2, r3, r4
}

; draw with inverted x/y

macro draw.image.ix i, x, y {
 IF ~x eq
   move.image i, x, y
 END IF
 draw.bitmap.ix i#.p,\
  i#.x, i#.y, i#.w, i#.h
}

macro draw.image.iy i, x, y {
 IF ~x eq
   move.image i, x, y
 END IF
 draw.bitmap.iy i#.p, i#.x, i#.y, i#.w, i#.h
}

macro draw.image.ixy i, x, y, ix, iy {
 IF ~x eq
   move.image i, x, y
 END IF
 draw.bitmap.ixy i#.p, i#.x, i#.y, i#.w, i#.h
}

macro draw.image.v i, x, y, c {
 IF ~x eq
   move.image i, x, y
 END IF
 draw.bitmap.v i#.p, i#.x, i#.y, i#.w, i#.h, c
}

; draw rotated. warning: no clipping

function draw.scanline.rl, pixels, x, y, w
  alias p=r0, s=r1, n=r2
  vga.xy x, y
  . s=w, s--, s*2, s+pixels, n=screen.pitch
  loop w, (u32) *p=*s--, p+n, endl
endf 1

function draw.scanline.rr, pixels, x, y, w
  alias p=r0, s=r1, n=r2
  vga.xy x, y
  . s=pixels, n=screen.pitch
  loop w, (u32) *p=*s++, p+n, endl
endf 1

function draw.bitmap.rl, pixels, x, y, w, h
  locals i, p
  try visible x, y, w, h
  . p=pixels
  loop h
    draw.scanline.rl p, x, y, w
    . r0=w, r0*2, p+r0, x++
  endl
endf 1

function draw.bitmap.rr, pixels, x, y, w, h
  locals i, p
  try visible x, y, w, h
  . r0=w, r0--, x+r0, p=pixels
  loop h
    draw.scanline.rr p, x, y, w
    . r0=w, r0*2, p+r0, x--
  endl
endf 1

macro draw.image.rl i, x, y {
 IF ~x eq
   move.image i, x, y
 END IF
 draw.bitmap.rl i#.p, i#.x, i#.y, i#.w, i#.h
}

macro draw.image.rr i, x, y {
 IF ~x eq
   move.image i, x, y
 END IF
 draw.bitmap.rr i#.p, i#.x, i#.y, i#.w, i#.h
}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 8BPP with palette...

macro draw.image.8 i, x, y {
 IF ~x eq
   move.image i, x, y
 END IF
 . palette.p=i#.palette
 draw.bitmap.8 i#.p, i#.x, i#.y, i#.w, i#.h
}

macro draw.image.v.8 i, x, y, c {
 IF ~x eq
   move.image i, x, y
 END IF
 . palette.p=i#.palette
 draw.bitmap.v.8 i#.p, i#.x, i#.y, i#.w, i#.h, c
}

function draw.image.v8, im, x, y, co
  locals p, w, h
  . r0=im
  . (u32) r1=*(r0+?image.p), p=r1
  . (u32) r1=*(r0+?image.w), w=r1
  . (u32) r1=*(r0+?image.h), h=r1
  draw.bitmap.v.8 p, x, y, w, h, co
endf

function draw.bitmap.viy.8, pixels,\
 x, y, w, h, c
  locals i, p
  try visible x, y, w, h
  . r0=h, y+r0, p=pixels
  loop h
    draw.scanline.v.8 p, x, y, w, c
    . r0=w, p+r0, y--
  endl
endf 1

function draw.image.viy.8, im, x, y, co
  locals p, w, h
  . r0=im
  . (u32) r1=*(r0+?image.p), p=r1
  . (u32) r1=*(r0+?image.w), w=r1
  . (u32) r1=*(r0+?image.h), h=r1
  draw.bitmap.viy.8 p, x, y, w, h, co
endf