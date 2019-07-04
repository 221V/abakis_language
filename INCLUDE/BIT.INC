;;;;;;;;;;;;;; CHARACTER BIT STREAM ;;;;;;;;;;;;;;

; "bit stream"; array of '1'/'0' bytes ending
; with 0. for variable compression schemes

align

integer bs.end ; end?
text bs.t(33)  ; temporary

macro reset.bs t { . r0=t, *r0=0, bs.end=0 }

; readable? return 0 if end. used by read

function verify.bs, bs
  . r0=bs
  if false, go .e, end, r0=*r0
  if false, .e:, bs.end=1, end
endf

; read/write 1-32BIT value to/from
; bit stream. read removes prefix and
; decreases size. write attaches suffix

; example: text bs='11110000'
; read.bs bs, 4 ; returns integer 1111b,
; BITs shifted out left, and after,
; bs = '0000', remaining BITs in stream

function read.bs, bs, size
  alias p=r0
  try verify.bs bs
  text.copy bs.t, bs
  . p=bs.t, p+size, *p=0
  text.shift.la bs, bs.t, size
  t2b bs.t
endf

; read as 'text' into t: '1111'

function read.bs.t, bs, t, size
  try read.bs bs, size
  b2t.a r0, t, size
endf

; example: write.bs bs, 11110011b, 8

function write.bs, bs, v, size
  b2t.a v, bs.t, size
  text.attach bs, bs.t
endf

; align last byte, attach zeros

function align.bs, bs
  alias p=r0, i=r1, c=r2
  text.n bs
  . c=p, c&7, i=8, i-c, p+bs
  if i<=0, return, end
  loop i, *p++='0', endl, *p=0
endf

; read/write individual BITs...

macro get.bs bs  { read.bs bs, 1 }
macro set.bs bs  { write.bs bs, 1, 1 }
macro zero.bs bs { write.bs bs, 0, 1 }

; conversions...

; convert binary data to 'bit stream'.
; b=address of binary bytes. example:

; b: db 11111111b, 00000000b,\
; 11110000b, 10100001b

; b2bs b, t, 27
; result: t='111111110000000011110000101'

function b2bs, b, bs, n
  locals i
  alias p=r7, x=r1, c=r2
  push p
  . p=b, i=n, i>>>3, c=n, c&7, n=c
  loop i, c=*p++
    write.bs bs, c, 8
  endl
  if n, c=*p, x=8, x-n, c>>>cl
    b2t.a c, bs.t, n
    text.attach bs, bs.t
  end
  pop p
endf

; convert 'bit stream' to binary data.
; b=address of binary bytes. example:

; text t='11111111001111001111000011111111'
        ; 111111110011110011110000111 (27)

; bs2b t, b, 27
; result: b = db 11111111b, 00111100b,\
; 11110000b, 11100000b (last byte)
;               XXXXX  (alignment)

; writes the last byte if n is not
; divisible by 8

function bs2b, bs, b, n
  locals i
  alias p=r7, c=r0, x=r1
  push p
  . p=b, i=n, i>>>3, c=n, c&7, n=c
  loop i
    read.bs bs, 8
    . *p++=c
  endl
  if n
    read.bs bs, 8
    . x=8, x-n, c>>>cl, c<<cl
    . x=*p, x|c, *p=x
  end
  pop p
endf

; with same parameters: bs, b

macro load.bs bs, b, n { b2bs b, bs, n }
macro save.bs bs, b, n { bs2b bs, b, n }