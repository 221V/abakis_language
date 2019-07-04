;;;;;;;;;;;;;;;;;;;;;; FPU ;;;;;;;;;;;;;;;;;;;;;;;

; unfinished...

; minimal FPU arithmetic. memory
; operands only

align 2

ushort fcw, fsw ; control and status word

align 4

; constants

float f0.1=0.1, fn1=-1.0, f180=180.0

align 8

double result.d=0.0

; integer i
; float f, a=1.234567, b=6.543210

; convert float to/from integer:

; % i=f2i(f), f=i2f(i)

; arithmetic:

; % f=a+b, f=a*b, a=b/f

; % f=abs(x)  ; absolute
; % f=sqrt(x) ; square root

; trigonometry: sine, cosine, tangent:

; % f=sin(x), f=cos(x), f=tan(x)

macro % [p] {
 common
  match =,, p \{      ; %, ...
   finit
   define ?s 1
  \}
 forward
  define ?s 0
  match , p \{	      ; %
   finit
   define ?s 1
  \}
  match =0 f==\
   =pi(), ?s p \{     ; f=pi()
   fldpi
   fstp f
   define ?s 1
  \}
  match =0 i==\       ; i=f2i(f)
   =f2i(f), ?s p \{
   fld f
   fistp i
   define ?s 1
  \}
  match =0 f==\       ; f=i2f(i)
   =i2f(\i), ?s p \{
   fild i
   fstp f
   define ?s 1
  \}
  match =0 f==\       ; f=d2r(x)
   =d2r(x), ?s p \{
   fld x	      ; x*(pi/180)
   fldpi
   fmulp
   fld f180
   fdivp
   fstp f
   define ?s 1
  \}
  match =0 f==\       ; f=r2d(x)
   =r2d(x), ?s p \{
   fld f180	      ; (180/pi)*x
   fldpi
   fdivp
   fld x
   fmulp
   fstp f
   define ?s 1
  \}
  match =0 f==\       ; f=abs(x)
   =abs(x), ?s p \{
   fld x
   fabs
   fstp f
   define ?s 1
  \}
  match =0 f==\       ; f=sqrt(x)
   =sqrt(x), ?s p \{
   fld x
   fsqrt
   fstp f
   define ?s 1
  \}
  match =0 f==\       ; f=sin(x)
   =sin(x), ?s p \{
   fld x
   fsin
   fstp f
   define ?s 1
  \}
  match =0 f==\       ; f=cos(x)
   =cos(x), ?s p \{
   fld x
   fcos
   fstp f
   define ?s 1
  \}
  match =0 f==\       ; f=tan(x)
   =tan(x), ?s p \{
   fld x
   fptan
   fstp f
   define ?s 1
  \}
  ; ...
  match =0 \
   f==x, ?s p \{
   match =0 \
    =0.0, ?s x \\{
    fldz
    define ?s 1
   \\}
   match =0 \
    =1.0, ?s x \\{
    fld1
    define ?s 1
   \\}
   match =0 \
    a+b, ?s x \\{
    fld a
    fld b
    faddp
    define ?s 1
   \\}
   match =0 \
    a-b, ?s x \\{
    fld a
    fld b
    fsubp
    define ?s 1
   \\}
   match =0 \
    a*b, ?s x \\{
    fld a
    fld b
    fmulp
    define ?s 1
   \\}
   match =0 \
    a/b, ?s x \\{
    fld a
    fld b
    fdivp
    define ?s 1
   \\}
   match =0 , ?s \\{  ; a=b
    fld x
    define ?s 1
   \\}
   fstp f
  \}
  verify %
}

; convert, display...

text float.t='%f'

macro f2t v, t {
 % result.d=v
 . r0=&result.d
 sprintf t, float.t, [r0], [r0+4]
}

macro say.f f {
 f2t f, t
 say t
}

; save float value/s to file

macro log.fp [f] {
 forward
  f2t f, t
  log t
}