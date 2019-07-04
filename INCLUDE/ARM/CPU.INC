; $$$$$$$$$$$$$$$$ ABA-KIS 4 ARM $$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ??????????????????? CPU.INC ????????????????????

; UNFINISHED...

is.r fix in <r0,r1,r2,r3,r4,r5,r6,r7,\
  r8,r9,r10,r11,r12,r13,r14,r15>

macro nop { mov r0, r0 }
macro nops n { times n nop }

; load/store multiple to/from memory.
; registers are processed sequentially
; regardless of order. example:

; push r0-r3, r5, r6, r7, v7-v8, lr
; pop r0-r3, r5, r6, r7, v7-v8, pc

macro push [r] { common stmfd sp!, \{ r \} }
macro pop [r] { common ldmfd sp!, \{ r \} }

; pushx/popx all general purpose registers
; or pusha/popa all argument registers

macro pushx { push r0-r12 }
macro popx { pop r0-r12 }

macro pusha { push a1-a4 }
macro popa { pop a1-a4 }

; load/construct 8-32BIT immediate/address...

ldi  fix movi2
movi fix movi2

macro movi1 a, b {
 local n
 IF (b)=-1 | (b)=0FFFFFFFFh ; -1/FFFFFFFFh
  mvn a, 0                  ; mvn a, 0
 ELSE IF (b)>=0 & (b)<=255  ; 8BIT
  mov a, b
 ELSE
  n=(b) and 0FFh
  mov a, n                  ; mov a, b&FFh
  n=((b) and 0FF00h)
  IF n<>0                   ; if 16+BIT...
   orr a, n                 ; orr a, a, b&FF00h
  END IF
  n=((b) and 0FF0000h)
  IF n<>0
   orr a, n                 ; orr a, a, b&FF0000h
  END IF
  n=((b) and 0FF000000h)
  IF n<>0
   orr a, n                 ; orr a, a, b&FF000000h
  END IF
 END IF
}

; with constant rotation

macro movi2 a, b {
 local n
 IF (b)=-1 | (b)=0FFFFFFFFh  ; -1/FFFFFFFFh
  mvn a, 0                   ; mvn a, 0
 ELSE IF (b)>=0 & (b)<=255   ; 8BIT
  mov a, b
 ELSE IF \                   ; even bytes...
  ((b) and 0FFFF00FFh)=0     ; FF00h
  n=((b) shr 8) and 0FFh
  mov a, n, 32-8
 ELSE IF \
  ((b) and 0FF00FFFFh)=0     ; FF0000h
  n=((b) shr 16) and 0FFh
  mov a, n, 32-16
 ELSE IF \
  ((b) and 00FFFFFFh)=0      ; FF000000h
  n=((b) shr 24) and 0FFh
  mov a, n, 32-24
 ELSE IF \
  ((b) and 0FFFFF00Fh)=0     ; odd bytes...
  n=((b) shr 4) and 0FFh
  mov a, n, 32-4             ; FF0h
 ELSE IF \
  ((b) and 0FFF00FFFh)=0     ; FF000h
  n=((b) shr 12) and 0FFh
  mov a, n, 32-12
 ELSE IF \
  ((b) and 0F00FFFFFh)=0     ; 0FF00000h
  n=((b) shr 20) and 0FFh
  mov a, n, 32-20
 ELSE                        ; build value...
  n=(b) and 0FFh
  mov a, n                   ; mov a, b&FFh
  n=((b) and 0FF00h)
  IF n<>0                    ; if 16+BIT...
   orr a, n                  ; orr a, a, b&FF00h
  END IF
  n=((b) and 0FF0000h)
  IF n<>0
   orr a, n                  ; orr a, a, b&FF0000h
  END IF
  n=((b) and 0FF000000h)
  IF n<>0
   orr a, n                  ; orr a, a, b&FF000000h
  END IF
 END IF
}

macro test.movi {
 movi r3, -1         ; mvn
 movi r3, 0ABh       ; all 1 mov...
 movi r3, 07F0h
 movi r3, 07F00h
 movi r3, 07F000h
 movi r3, 07F0000h
 movi r3, 07F00000h
 movi r3, 0FF000000h
 movi r3, 080000000h
 movi r3, 1234h      ; mov+orr or movw/t
}

; load/store memory

macro ldm r, m {
  mov r, m
  ;match x, [m] \{
  ;
  ; ldi r12, x
  ;\}
  ;ldr r, [r12]
}

macro stm r, m {
  mov m, r
  ;match x, m \{
  ; ldi r12, x
  ;\}
  ;ldi r12, m
  ;str r, [r12]
}

;;;;;;;;;;;;;;;;;;; TRANSFER ;;;;;;;;;;;;;;;;;;;;;

macro jmp l { bal l } ; never use b
macro go l  { bal l }

macro call l { bl l }

macro je l  { beq l }
macro jne l { bne l }
macro jz l  { beq l }
macro jnz l { bne l }

;;;;;;;;;;;;;;;;;; ARITHMETIC ;;;;;;;;;;;;;;;;;;;;

macro inc r { adds r, r, 1 }
macro dec r { subs r, r, 1 }

; abs n
; min.max n, min, max
; mul3
; mul5
; mul10
; udiv
; idiv
; udiv10 
; udivr10 
; udiv255 

; negate: n=0-n

macro neg x { rscs x, x, 0 }

; absolute value

macro abs x {
 teq x, 0
 rsbmi x, x, 0 ; if x<0, x=0-x
}

; ensure min-max

macro min.max n, min, max {
 IF n<min
   . n=min
 ELSE.IF n>max
   . n=max
 END
}

; fast multiply r0 by common constant

macro mul3  { adds r0, r0, r0, lsl 1 }
macro mul5  { adds r0, r0, r0, lsl 2 }

macro mul10 {
 mov r0, r0, lsl 1  ; r0=(r0<<1)+(r0<<2)
 adds r0, r0, lsl 2
}

; r0/r1. unsigned 32BIT division via
; repeated subtraction. alters r0-r1,
; r11-r12.

; long macros are designed to be expanded
; once inside of a function or they can
; be converted to functions

; r0=destiny/quotient
; r1=source/dividend/remainder

macro _DIV {
 local ..a, ..b
 mov r11, r0
 mov r12, r1
 cmp r12, r11, lsr 1    ; while divisor<(n*2)
 ..a:
  movls r12, r12, lsl 1 ; *2 if lower
  cmp r12, r11, lsr 1
 bls ..a
 mov r0, 0
 ..b:
  cmp r11, r12          ; subtract only
  subcs r11, r11, r12   ; if carry
  adc r0, r0, r0        ; *2
  mov r12, r12, lsr 1   ; /2
  cmp r12, r1           ; while > divisor
 bhs ..b
 mov r1, r11            ; remainder
}

; safer versions...

macro UDIV {
 push r11-r12
 _DIV
 pop r11-r12
}

; signed 32BIT division: r0/r1

macro IDIV {
 local ..a
 push r9-r12
 mov r9, r0
 mov r10, r1
 abs r0
 abs r1
 _DIV
 if r9<0      ; both negative?
   if r10<0   ; same sign
     abs r0   ; positive
     bal ..a
   end
 end
 if r9>=0     ; both positive?
   if r10>=0  ; same sign
     abs r0   ; positive
     bal ..a
   end
 end
 neg r0       ; signs differ. negative
 ..a:
 pop r9-r12
}

; fast unsigned division by 10. r0/10.
; r0/r1 is altered

macro udiv10 {
 movi r1, 1999999Ah     ; r1=((2^32)/10)+1
 sub r0, r0, r0, lsr 30 ; r0=r0-(r0>>>30)
 umull r2, r0, r1, r0   ; r0=r1*r0
}

; with remainder in r1

macro udivr10 {
 mov r3, r0        ; dividend
 udiv10
 mov r1, r0, lsl 1 ; multiply by 10:
 add r1, r1, lsl 2 ; r1=(r0<<1)+(r1<<2)
 sub r1, r3, r1    ; r1=r3-r1 ; remainder
}

; faster than div10

macro __udiv10 {
 sub r0, r0, r0, lsr 14 ; r0=r0-(r0>>>14)
 add r1, r0, r0, lsl 1  ; r1=r0+(r0<<1)
 add r0, r0, r1, lsl 2  ; r0=r0+(r1<<2)
 add r0, r0, r1, lsl 6  ; r0=r0+(r1<<6)
 add r0, r0, r1, lsl 10 ; r0=r0+(r1<<10)
 mov r0, r0, lsr 15     ; r0=(r0>>>15)
}

; divide by 255 (256-1)

macro udiv255 {
 mov r1, r0, lsr 8 ; n=((n>>8)+n+1)>>8
 add r0, r0, 1
 add r0, r1, lsr 8
}

;;;;;;;;;;;;;;;;;; SMART-MOVE ;;;;;;;;;;;;;;;;;;;;

; * MOVE IMMEDIATE: MOV/MVN, ROTATION, MOVW/MOVT
; * LITERAL TABLE, OPTIONAL INSIDE FUNCTIONS
; * PC RELATIVE LOAD
; * LDR =I/&I UPGRADE (FOR GAS/GCC
;   COMPATIBILITY)

; "Smart-Move" immediate with PC relative
; load from nearby literal table - ldr r,
; [pc, p-$-8] - or optional movw/movt
; for FASMARM. =i prefix for "move 32BIT
; immediate" (compatible with GAS/GCC).
; =& for PC relative load

literals equ  ; literal table
literals.i=-1 ; offset

; define literal 32BIT number. attach line/data
; definition: literals=itself+dw x

macro .literal x {
 literals equ literals, dw x
 literals.i=literals.i+4
}

; store literal table or initialize

macro .literals {
 local ..literals
 ?LITERALS \          ; address
  equ ..literals
 match j, literals \{ ; expand into a,b,c
  irp i, j \\{        ; expand each line
   i                  ; dw 1, dw 2, dw 3
  \\}
  restore ?LITERALS
 \}
 match , literals \{  ; initialize
  literals equ \      ; first lines...
  align 4,\
  ?LITERALS:,\        ; begin
  literals.i=0        ; offset
 \}
}

;;;;;;;;;;;;;;;;; MOVE IMMEDIATE ;;;;;;;;;;;;;;;;;

use.mov? fix \
 (i>=0 & i<=255)\         ; use mov?
 | (i=-1 | i=$FFFFFFFF)\  ; use mvn?
 | (i and $FFFFF00F)=0\   ; use mov+ror?
 | (i and $FFFF00FF)=0\   ; FF00h
 | (i and $FFF00FFF)=0\   ; FF000h
 | (i and $FF00FFFF)=0\   ; FF0000h
 | (i and $F00FFFFF)=0\   ; FF00000h
 | (i and $00FFFFFF)=0    ; FF000000h

use.ror? fix \
 (i>=0 & i<=255)\         ; use mov?
 | (i and $FFFFF00F)=0\   ; use mov+ror?
 | (i and $FFFF00FF)=0\   ; FF00h
 | (i and $FFF00FFF)=0\   ; FF000h
 | (i and $FF00FFFF)=0\   ; FF0000h
 | (i and $F00FFFFF)=0\   ; FF00000h
 | (i and $00FFFFFF)=0    ; FF000000h

; move wide/top 32BIT

; use.movwt? equ 0   ; 1 IF CPU>=ARM.v6T2

; generic move 32BIT immediate (worst
; case scenerio)

macro movi r, i {
 local n
 n=(i and 0FFh)
 mov r, n             ; mov r, b&FFh
 n=(i and 0FF00h)
 IF n<>0              ; IF 16+BIT...
  orr r, n            ; orr r, r, b&FF00h
 END IF
 n=(i and 0FF0000h)
 IF n<>0
  orr r, n            ; orr r, r, b&FF0000h
 END IF
 n=(i and 0FF000000h)
 IF n<>0
  orr r, n            ; orr r, r, i&FF000000h
 END IF
}

;;;;;;;;;;;;;;; LDR =I/&I UPGRADE ;;;;;;;;;;;;;;;;

; upgrade ldr instruction to move immediate.
; =i for "move 32BIT immediate"; mov/mvn,
; constant rotation or movw/movt (if supported,
; CPU>=ARM.v6T2). &i for explicit PC relative
; load from literal table:

; ldr r, [pc, p-$-8+4] ; ldr r, &77777777h
; .literal 77777777h

macro ldr [p] {
 common
  local i, x
  define ?s 0
  match r=, &n, p \{  ; ldr r, &i
   x=(?LITERALS+\     ; PC relative
    literals.i)-$-8+4 ; address
   ldr r, [pc, x]     ; load
   .literal n         ; store i
   define ?s 1        ; matched
  \}
  match =0 \          ; else
   r=,==n, ?s p \{    ; ldr r, =i
   i=(n)
   IF use.mov?        ; use mov?
    mov r, i
;   ELSE IF use.movwt? ; use movw/movt?
;    movwt r, i        ; CPU>=ARM.v6T2
   ELSE               ; worst case
    movi r, i         ; scenerio: mov+orr
   END IF
   define ?s 1        ; matched
  \}
  IF ?s eq 0          ; else, use
   ldr p              ; original ldr
  END IF              ; instruction
}

;;;;;;;;;;;;;;;;;;; INTEL.INC ;;;;;;;;;;;;;;;;;;;;

; TRANSLATION FROM INTEL ASSEMBLER TO ARM.
; UNFINISHED...

; * MOV+DATA [M]/I/[A+B*C] UPGRADE
;   (ADD/SUB/CMP/ETC)
; * LEA+[S*I+B] ADDRESSING MODES
; * MOVXZ/SZ, EXCHANGE (XCHG)
; * & MORE

@r fix r12  ; general
@sr fix r11 ; source
@dr fix r10 ; destiny
@sp fix r4  ; esp
@bp fix r5  ; ebp

is.i? fix eqtype 0  ; is immediate?

is.r? fix in \      ; is register?
 <r0,r1,r2,r3,\
 r4,r5,r6,r7,\
 r8,r9,r10,r11,\
 r12,r13,r14,r15>

is.r32? fix in \    ; intel names
 <eax,ecx,edx,ebx,\
 esp,ebp,esi,edi>
is.r16? fix in <ax,cx,dx,bx>
is.r8? fix in \
 <al,ah,cl,ch,dl,dh,bl,bh>
is.r8l? fix in <al,cl,dl,bl>
is.r8h? fix in <ah,ch,dh,bh>

is.r32? fix in \    ; any 32BIT
 <r0,r1,r2,r3,\
 r4,r5,r6,r7,\
 r8,r9,r10,r11,\
 r12,r13,r14,r15,\
 eax,ecx,edx,ebx,\
 esp,ebp,esi,edi>

is.ra? fix in \     ; any
 <r0,r1,r2,r3,\
 r4,r5,r6,r7,\
 r8,r9,r10,r11,\
 r12,r13,r14,r15,\
 eax,ecx,edx,ebx,\
 esp,ebp,esi,edi,\
 ax,cx,dx,bx,\
 al,ah,cl,ch,dl,dh,bl,bh>

macro nop { mov r0, r0 }
macro nops n { times n nop }

;;;;;;;;;;;;;;;;; STATUS, FLAGS ;;;;;;;;;;;;;;;;;;

; conditions: c.eq, c.ne, etc

n.c=0
irp x,\
 eq, ne, hs, lo, mi, pl, vs, vc,\
 hi, ls, ge, lt, gt, le, al, nv {
 c.#x=n.c
 n.c=n.c+1
}

c.cs=c.hs
c.cc=c.lo

; ARM status/flags, PSR leftmost 4BITs: NZCV

; N: Negative. Z: Zero. C: Carry. V: Overflow

; * EQ, NE: Z=1/0. Equality
; * CS/HS: C=1. Higher or same. Unsigned >=
; * CC/LO: C=0. Lower. Unsigned <
; * MI, PL: N=1/0. Negative, positive or zero
; * VS, VC: V=1/0. Overflow
; * HI: C=1 and Z=0. Higher. Unsigned >
; * LS: C=0 or Z=1. Lower or same. Unsigned <=
; * GE: N and V same. Signed >=
; * LT: N and V different. Signed <
; * GT: Z=0, and N and V same. Signed >
; * LE: Z=1, or N and V different. Signed <=

PSR.N=1 shl 31
PSR.Z=1 shl 30
PSR.C=1 shl 29
PSR.V=1 shl 28
PSR.I=1 shl 7
PSR.T=1 shl 5

PSR.D=1 shl 8  ; <edit>
PSR.P=1 shl 9  ; use globals
PSR.A=1 shl 10

; set/zero/assign flags

; set.flags 1010b
; set.flag N
; zero.flag Z
; assign.flag C, 1

macro set.flags c { msr CPSR, c shl 28 }

macro set.flag f {
 mrs r10, CPSR
 orr r10, PSR.#f
 msr CPSR, r10
}

macro zero.flag f {
 mrs r10, CPSR
 bic r10, PSR.#f
 msr CPSR, r10
}

macro assign.flag f, v {
 IF v is.r32?       ; register?
  mrs r10, CPSR     ; get status
  bic r10, PSR.#f   ; zero
  cmp v, 1          ; r=1?
  orreq r10, PSR.#f ; f=1
  msr CPSR, r10     ; set status
 ELSE               ; immediate
  IF v
   set.flag f       ; set
  ELSE
   zero.flag f      ; zero
  END IF
 END IF
}

macro stc { set.flag C }
macro clc { zero.flag C }
macro sti { set.flag I }
macro cli { zero.flag I }
macro std { set.flag D }
macro cld { zero.flag D }

macro cmc {
 mrs r12, CPSR
 tst r12, PSR.C   ; cf?
 orreq r12, PSR.C ; if 0, set
 bicne r12, PSR.C ; else, zero
 msr CPSR, r12
}

; 2-DO: ensure all instructions alter
; the correct flags. some instructions do
; not effect flags:

; mov, lea, bswap, xchg, set/x, loop/x,
; rep lods/stos/movs (except cmps/scas),
; jmp/jxx/call, loop/x, ret

;;;;;;;;;;;;;; USE ROTATION FOR A/B? ;;;;;;;;;;;;;

use.ror.a? fix \
 (a>=0 & a<=255)\         ; use mov?
 | (a and $FFFFF00F)=0\   ; use mov+ror?
 | (a and $FFFF00FF)=0\   ; FF00h
 | (a and $FFF00FFF)=0\   ; FF000h
 | (a and $FF00FFFF)=0\   ; FF0000h
 | (a and $F00FFFFF)=0\   ; FF00000h
 | (a and $00FFFFFF)=0    ; FF000000h

use.ror.b? fix \
 (b>=0 & b<=255)\         ; use mov?
 | (b and $FFFFF00F)=0\   ; use mov+ror?
 | (b and $FFFF00FF)=0\   ; FF00h
 | (b and $FFF00FFF)=0\   ; FF000h
 | (b and $FF00FFFF)=0\   ; FF0000h
 | (b and $F00FFFFF)=0\   ; FF00000h
 | (b and $00FFFFFF)=0    ; FF000000h

;;;;;;;;;;; LEA: LOAD EFFECTIVE ADDRESS ;;;;;;;;;;

; lea r1, [r2]
; lea r1, [r2+r3]
; lea r1, [r2+10000000h]
; lea r1, [r2-20000000h]
; lea r1, [r2*3]
; lea r1, [r2*4]
; lea r1, [r2*5]
; lea r1, [r2*10]
; lea r1, [r2+r3*4]
; lea r1, [r4+r7*2+30000000h]
; lea r2, 40000000h
; lea r3, [50000000h]
; lea r4, [60000000h+r7]
; lea r5, [70000000h+r7*8]

macro lea [p] {
 common
  define ?s 0
  match r=,[x], p \{         ; r,[?]
   IF x is.r32?              ; r,[r]
    mov r, x
   ELSE                      ; r,[?]
    match =0 \
     a+b*c, ?s x \\{
     match n+i, c \\\{       ; a+b*c+i
      addms r, a, b, n
      IF use.ror?
       add r, i
      ELSE
       ldr r12, =i
       add r, a, r12
      END IF
      define ?s 1
     \\\}
     IF ?s eq 0
      IF a is.r32? \         ; r+r*c
       & b is.r32?
       addms r, a, b, c
      ELSE IF b is.r32? \    ; i+r*c
       & a is.i?
       IF use.ror.a?
        mov r, a
        addms r, r, b, c
       ELSE
        ldr r12, =i
        add r, a, r12
       END IF
      ELSE
       'Error'
      END IF
     END IF
     define ?s 1
    \\}
    match =0 \
     a+b, ?s x \\{
     IF a is.r32? \          ; r+i
      & b is.i?
      IF use.ror.b?
       add r, a, b
      ELSE
       ldr r, =b
       add r, a, r
      END IF
     ELSE IF \
      b is.r32? & \          ; i+r
      a is.i?
      IF use.ror.a?
       add r, b, a
      ELSE
       ldr r, =a
       add r, b, r
      END IF
     ELSE                    ; assume
      add r, a, b            ; r=a+b
     END IF
     define ?s 1
    \\}
    match =0 \
     a-b, ?s x \\{           ; r-i
     IF a is.r32? \
      & b is.i?
      IF use.ror.b?
       sub r, a, b
      ELSE
       ldr r, =b
       sub r, a, r
      END IF
     ELSE                    ; ?,?
      'Error'
     END IF
     define ?s 1
    \\}
    match =0 \
     a*b, ?s x \\{           ; r=a*n
     IF b eq 4
      mov r, a, lsl 2        ; r=a*4
     ELSE IF b eq 2
      mov r, a, lsl 1        ; r=a*2
     ELSE IF b eq 3
      add r, a, a, lsl 1     ; r=a*3
     ELSE IF b eq 5
      add r, a, a, lsl 2     ; r=a*5
     ELSE IF b eq 10
      add r, a, a, lsl 2     ; r=a*10
      add r, r
     ELSE                    ; *?
      'Error'
     END IF
     define ?s 1
    \\}
    IF ?s eq 0               ; r=[i]
     ldr r, =x
    END IF
   END IF
   define ?s 1
  \}
  match =0 \                 ; no []
   a=,b, ?s p \{
   IF a is.r32?              ; r,?
    IF b is.r32?             ; r,r
     mov a, b
    ELSE IF b is.i?          ; r,i
     ldr a, =b
    ELSE                     ; r,?
     'Error'
    END IF
   ELSE                      ; ?,?
    'Error'
   END IF
   define ?s 1
  \}
  IF ?s eq 0
   'Error'
  END IF
}

;;;;;;;;;;;;;;;; GET OPERAND SIZE ;;;;;;;;;;;;;;;;

; get size from p = type name or register.
; return size in s constant or 0 if unknown

; local size
; size=0
; @getm.size dword [x], size
; display '0'+size, 0Dh, 0Ah
; @getm.size ch, size
; display '0'+size, 0Dh, 0Ah

macro @getm.size p, s {
 s=0
 define ?s 0
 match =0 \
  [m], ?s p \{       ; [m]
  ; ...              ; size=0
  define ?s 1
 \}
 match =0 \
  t [m], ?s p \{     ; type [m]
  IF t eq dword
   s=4
  ELSE IF t eq word
   s=2
  ELSE IF t eq byte
   s=1
  END IF
  define ?s 1
 \}
 IF ?s eq 0          ; else
  IF p is.r? \       ; is register?
   | p is.r32?
   s=4
  ELSE IF p is.r16?
   s=2
  ELSE IF p is.r8?
   s=1
  END IF
 END IF
}

;;;;;;;;;;;;;;;;;; XXX A,B,C<<S ;;;;;;;;;;;;;;;;;;

; data processing multiply with shift.
; example: dpms add, a, b, c, 4 ; produces:
; add a, b, c, lsl 2

macro dpms o, a, b, c, s {
 local n
 n=0
 IF s eq 2
  n=1
 ELSE IF s eq 4
  n=2
 ELSE IF s eq 8
  n=3
 ELSE
  'Error'
 END IF
 o a, b, c, lsl n
}

macro addms a, b, c, s
 { dpms add, a, b, c, s }
macro subms a, b, c, s
 { dpms sub, a, b, c, s }

; load/store multiply with shift.
; example: lsms ldr, a, b, c, 4 ; produces:
; ldr a, [b, c, lsl 2]

macro lsms o, a, b, c, s {
 IF s eq 2
  n=1
 ELSE IF s eq 4
  n=2
 ELSE IF s eq 8
  n=3
 ELSE
  'Error'
 END IF
 o a, [b, c, lsl n]
}

;;;;;;;;;;;;;;;;;;; @GET/@SET ;;;;;;;;;;;;;;;;;;;;

; low-level get/set helper macros. usage:

; @getm r1, r2           ; r=r/[m]/i
; @getm r1, 10000h
; @getm r1, [r2]
; @getm r1, [20000h]
; @getm r1, [r2+30000h]
; @getm r1, [30000h+r2]
; @getm r1, [r2+r3*4]

; @setm [r1], r2         ; [m]=r
; @setm [20000h], r1
; @setm [r1+30000h], r2
; @setm [40000h+r1], r2
; @setm [r1+r2*4], r3

macro @getm r, x {
 define ?s 0
 match \
  [a], x \{
  match \
   [b+i], x \\{
   match \
    I*S, i \\\{         ; [a+b*c]
    IF b is.r32?
     lsms \
      ldr, r, b, I, S
    ELSE                ; [i+r*c]
     ldr r12, =b
     lsms \
      ldr, r, r12, I, S
    END IF
    define ?s 1
   \\\}
   IF ?s eq 0           ; [a+b]
    IF b is.r? \        ; [r+r]
     & i is.r?
     ldr r, [b, i]
    ELSE IF b is.r? \   ; [r+i]
     & i is.i?
     ldr @r, =i
     ldr r, [b, @r]
    ELSE IF i is.r? \   ; [i+r]
     & b is.i?
     ldr @r, =b
     ldr r, [@r, i]
    ELSE
     'Error'
    END IF
   END IF
   define ?s 1
  \\}
  IF ?s eq 0
   IF a is.r?           ; [r]
    ldr r, [a]
   ELSE                 ; [m]
    ldr @r, =a
    ldr r, [@r]
   END IF
  END IF
  define ?s 1
 \}
 IF ?s eq 0
  IF x is.r?            ; r
   mov r, x
  ELSE                  ; i
   ldr r, =x
  END IF
 END IF
}

macro @setm x, r {
 define ?s 0
 match \
  [a], x \{
  match \
   [b+i], x \\{
   match \
    I*S, i \\\{         ; [a+b*c]
    lsms \
     str, r, b, I, S
    define ?s 1
   \\\}
   IF ?s eq 0           ; [a+b]
    IF b is.r? \        ; [r+r]
     & i is.r?
     str r, [b, i]
    ELSE IF b is.r? \   ; [r+i]
     & i is.i?
     ldr @r, =i
     str r, [b, @r]
    ELSE IF i is.r? \   ; [i+r]
     & b is.i?
     ldr @r, =b
     str r, [@r, i]
    ELSE
     'Error'
    END IF
   END IF
   define ?s 1
  \\}
  IF ?s eq 0
   IF a is.r32?         ; [r]
    str r, [a]
   ELSE                 ; [m]
    match [i j], x
     \\{ 'Error' \\}
    ldr @r, =a
    str r, [@r]
   END IF
  END IF
 \}
 IF ?s eq 0
  'Error'
 END IF
}

;;;;;;;;;;;;;;; MOV [M]/I UPGRADE ;;;;;;;;;;;;;;;;

; upgrade mov to support r/[r]/[m]/i. examples:

; mov r1, r2
; mov r1, 10000000h
; mov r1, [r2]
; mov [r1], r2
; mov r1, [r2+r3]
; mov [r1+r2*4], r3
; mov r1, [r2+80000000h]

macro mov [p] {
 common
  define ?s 0
  match r=,[m], p \{ ; r, [m]
   IF m is.r?
    ldr r, [m]
   ELSE
    @getm r, [m]
   END IF
   define ?s 1
  \}
  match =0 \         ; [m], r
   [m]=,r, ?s p \{
   IF m is.r?        ; [r], r
    str r, [m]
   ELSE              ; [m], x
    IF r is.r?       ; [m], r
     @setm [m], r
    ELSE             ; [m], i
     ldi r12, r
     @setm [m], r12
    END IF
   END IF
   define ?s 1
  \}
  IF ?s eq 0         ; else
   mov p             ; standard mov
  END IF
}

; note: 32BIT only. I have an advanced
; get/set/mov that supports types/sizes and
; 8/16BIT registers and memory but it's
; not prepared to be released

;;;;;;;;;;;;;;;;; SHIFT, ROTATE ;;;;;;;;;;;;;;;;;;

; shl r1, 23
; shr r1, r2
; sal r1, 16
; sar r1, r2
; ror r1, 24
; ror r1, r2
; rol r1, 24
; rol r1, r2
; rcr r1, 1

macro shl a, b { lsl a, b }
macro shr a, b { lsr a, b }
macro sal a, b { lsl a, b }
macro sar a, b { asr a, b }

; shift double precision

; mov r1, 000000011h
; mov r2, 022330000h
; shld r1, r2, 16    ; r1=112233h
; mov r1, 33000000h
; mov r2, 00001122h  ; r1=112233h
; shrd r1, r2, 24

macro shld a, b, c {
 lsl a, c
 orr a, b, rol c
}

macro shrd a, b, c {
 lsr a, c
 orr a, b, ror c
}

; rotation

; mov r1, 77h ; 00000077h
; ror r1, 4   ; 70000007h
; rol r1, 20  ; 00770000h

; ARM doesn't have a rotate left but
; it's just ror 32-n

macro rol a, b {
 IF b is.r32?
  rsb r12, b, 32
  ror a, r12
 ELSE
  ror a, (32-b)
 END IF
}

; algorithm to acheive same effect of
; rol/ror with shift. for CPUs that
; don't have rotate

; L: (a<<b)|(a>>(32-b))
; R: (a>>b)|(a<<(32-b))

macro @rol a, b {
 lsl r12, a, b
 rsb r11, b, 32 
 orr r12, r12, a, lsr r11 
}

macro @rol a, b {
 lsr r12, a, b
 rsb r11, b, 32 
 orr r12, r12, a, lsl r11 
}

; rotate right once through carry
; (FASM source only uses this once:
; rcr eax, 1)

macro rcr a, b {
 rrx a
 IF ~ b eq 1
  'Error'
 END IF
}

;;;;;;;;;;; MOVE W/ SIGN/ZERO EXTENSION ;;;;;;;;;;

; movzx, movsx

; movzx r1, byte [r2]
; movzx r1, word [r3]
; movsx r1, byte [r4]
; movsx r1, word [r2+r3*2]

macro @movxx s, [p] {
 common
  define ?s 0
  match \
   r=,t[m], p \{
   IF m is.r32?
    IF s             ; movsx
     IF t eq byte    ; r, byte [m]
      ldrsb r, [m]
     ELSE IF \
      t eq word      ; r, word [m]
      ldrsh r, [m]
     ELSE
      'Error'
     END IF
    ELSE             ; movzx
     IF t eq byte    ; r, byte [m]
      ldrb r, [m]
     ELSE IF \
      t eq word      ; r, word [m]
      ldrh r, [m]
     ELSE
      'Error'
     END IF
    END IF
   ELSE              ; [b+i*s], not [r]
    lea r12, [m]     ; get address
    IF s             ; movsx
     IF t eq byte    ; r, byte [m]
      ldrsb r, [r12]
     ELSE IF \
      t eq word      ; r, word [m]
      ldrsh r, [r12]
     ELSE
      'Error'
     END IF
    ELSE             ; movzx
     IF t eq byte    ; r, byte [m]
      ldrb r, [r12]
     ELSE IF \
      t eq word      ; r, word [m]
      ldrh r, [r12]
     ELSE
      'Error'
     END IF
    END IF
   END IF
   define ?s 1
  \}
  IF ?s eq 0
   'Error'
  END IF
}

macro movsx [p] { common @movxx 1, p }
macro movzx [p] { common @movxx 0, p }

;;;;;;;;;;;;;;; DATA [M]/I UPGRADE ;;;;;;;;;;;;;;;

; upgrade all data processing instructions to
; support r/[r]/[m]/i. examples:

; add r1, r2
; sub r1, 10000000h
; and r1, [r2]
; orr [r1], r2
; bic r1, [r2+r3]
; cmp [r1+r2*4], r3
; xor r1, [r2+80000000h]

macro @dp name, [p] {
 common
  define ?s 0
  match r=,[m], p \{
   mov @sr, [m]
   name r, @sr
   define ?s 1
  \}
  match =0 \
   [m]=,r, ?s p \{
   mov @dr, [m]
   name @dr, r
   mov [m], @dr
   define ?s 1
  \}
  IF ?s eq 0
   name p
  END IF
}

macro mvn [p] { common @dp mvns, p }
macro add [p] { common @dp adds, p }
macro sub [p] { common @dp subs, p }
macro adc [p] { common @dp adcs, p }
macro sbc [p] { common @dp sbcs, p }
macro rsb [p] { common @dp rsbs, p }
macro rsc [p] { common @dp rscs, p }
macro and [p] { common @dp ands, p }
macro or [p]  { common @dp orrs, p }
macro xor [p] { common @dp eors, p }
macro bic [p] { common @dp bics, p }
macro tst [p] { common @dp tst, p }
macro teq [p] { common @dp teq, p }
macro cmp [p] { common @dp cmp, p }
macro cmn [p] { common @dp cmn, p }

macro inc r { adds r, 1 }
macro dec r { subs r, 1 }

macro neg r { rsbs r, 0 }
macro not r { mvns r, r }

macro sbb [p] { common @dp sbcs, p }

; absolute value

macro abs x {
 teq x, 0
 rsbmi x, x, 0 ; IF x<0, x=0-x
}

;;;;;;;;;;;;;;;;;;; TRANSFER ;;;;;;;;;;;;;;;;;;;;;

macro jmp l { bal l } ; never use b
macro go l  { bal l }
macro je l  { beq l }
macro jne l { bne l }
macro jz l  { beq l }
macro jnz l { bne l }
macro jc l  { bhs l }
macro jnc l { blo l }
macro ja l  { bhi l }
macro jae l { bhs l }
macro jb l  { blo l }
macro jbe l { bls l }
macro jg l  { bgt l }
macro jge l { bge l }
macro jl l  { blt l }
macro jle l { ble l }
macro js l  { bvs l }
macro jns l { bvc l }

;;;;;;;;;;;;;;;;;; CONVERSIONS ;;;;;;;;;;;;;;;;;;;

; sign extension: cbw, cwd, cdq

macro cbw {
 tst r0, 80h
 biceq r0, 0FF00h
 orrne r0, 0FF00h
}

macro cwd {
 mov r12, 000FF0000h
 mov r12, 0FF000000h
 tst r0, 8000h
 biceq r0, r12
 orrne r0, r12
}

macro cdq {
 tst r0, 80000000h
 moveq r1, 0
 mvnne r1, 0
}

;;;;;;;;;;;;;;;;;;;; EXCHANGE ;;;;;;;;;;;;;;;;;;;;

; xchg r1, r2
; xchg r1, [r2+1000h]
; xchg r1, [r2+r3*4]
; xchg [r1+r2*8], r3

macro xchg a, b {
 define ?s 0
 match [m], a \{   ; [a], b
  mov r10, [m]
  mov [m], b
  mov b, r10
  define ?s 1
 \}
 match =0 \
  [m], ?s b \{     ; a, [b]
  mov r10, a
  mov a, [m]
  mov [m], r10
  define ?s 1
 \}
 IF ?s eq 0
  IF a is.r32? \   ; r, r
   & b is.r32?
   mov r10, a
   mov a, b
   mov b, r10
  ELSE
   'Error'
  END IF
 END IF
}

;;;;;;;;;;;;;;;;;; BITS & BYTES ;;;;;;;;;;;;;;;;;;

; bswap: reverse byte order: AABBCCDD=DDCCBBAA

macro bswap a {
 mov r12, 0
 and r11, a, 0FFh
 orr r12, r11, lsl 24
 and r11, a, 0FF00h
 orr r12, r11, lsl 8
 and r11, a, 0FF0000h
 orr r12, r11, lsr 8
 and r11, a, 0FF000000h
 orr r12, r11, lsr 24
 mov a, r12
}

;;;;;;;;;;;;;;;;;; CALL FUNCTION ;;;;;;;;;;;;;;;;;

; unfinished, untested, undecided...

; f:
;  nop
; ret

; function foo, a, b, c
;  nop
; endf

; call f
; call foo, r1, r2, r3
; call f, 77h, [r1+r2*4], [r3+r4], [r2], r1

macro callxxx f, [p] {
 common
  local n
  n=0
  IF ~p eq          ; parameters?
   forward
    n=n+1
   reverse
    IF p is.r32?    ; register
     push p
    ELSE            ; m/i
     @getm r12, p
     push r12
    END IF
   common
  END IF
  add lr, pc, 3*4-8 ; get return address,
  push lr           ; 3 instructions ahead.
  bal f             ; call/branch
  IF n              ; parameters?
   add @sp, n*4     ; caller cleanup?
  END IF
}