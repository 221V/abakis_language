; $$$$$$$$$$$$$$$$$$ MAGIC-ARM $$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ??????????????????? ARM.INC ????????????????????
;                               _
;            ?__. .__ __ ____ _(_) __?
;            /  |/  / @ `/ $ `/ / __/
;           /_/|_/_/\_,_/\_, /_/\__/
;                       /___/

;        Magic-ARM Assembler+Compiler *Beta*

;;;;;;;;;;;;;;;;;;;;;; CPU ;;;;;;;;;;;;;;;;;;;;;;;

; QEMU: ARM.v7M  ; Angel + PrimeCell LCD
; RPI:  ARM.v6Z  ; Raspberry PI: ARM1176JZF-S
; MINI: ARM.v5TE ; Minimal. Windows Mobile?
; GBA:  ARM.v4T  ; GameBoy Advance: ARM7TDMI

numeric ARM.*,\
 v4, v4T, v5, v5T, v5TE, v5TEJ, v6, v6T2,\
 v6Z, v6K, v6M, v7M, v7EM, v7R, v7A, v8A

@CPU=ARM.v5TE

if.cpu	   fix if @CPU>=
if.not.cpu fix if @CPU<

macro verify.cpu x {
 if.not.cpu x
  'Unsupported by selected CPU'
 end if
}

@a=0 ; base address
@$=0 ; current offset (10000h in QEMU)

@a$ fix (@a+@$)

; write instruction then advance @$

macro @arm i {
 if.not.align @$, 4
  'Address not aligned'
 end if
 dd i
 @$=@$+4
}

macro @align n {
 while @$ mod n<>0
  db 0
  @$=@$+1
 end while
}

align fix @align

macro @nop { @arm 0E1A00000h } ; mov r0, r0

; 31-28 27-20 19-16 15-12     98 7654 3210
; [CCCC/OOOO/PPPP/AAAA/BBBB/CCCC/DDDD/EEEE]

macro @arm1 C, o, p, a, b, c, d, e {
 @arm (C.#C shl 28) or ((o) shl 24) or \
  ((p) shl 20) or ((a) shl 16) or \
  ((b) shl 12) or ((c) shl 8) or \
  ((d) shl 4) or (e)
}

;;;;;;;;;;;;;;;;;;; REGISTERS ;;;;;;;;;;;;;;;;;;;;

;numeric \
; r0, r1, r2, r3, r4, r5, r6, r7,\
; @8, @9, @10, @11, @12, @13, @14, @15

a1 fix r0   ; a/rgument registers: a1-a4
a2 fix r1
a3 fix r2
a4 fix r3

v1 fix r4   ; v/ariable/scratch
v2 fix r5   ; registers: v1-v8
v3 fix r6
v4 fix r7
v5 fix @8
v6 fix @9
v7 fix @10
v8 fix @11

@sp fix @13 ; stack
@lr fix @14 ; link
@pc fix @15 ; program counter

;;;;;;;;;;;;;;;;;;; CONDITIONS ;;;;;;;;;;;;;;;;;;;

numeric C.*,\
 eq, ne, hs, lo, mi, pl, vs, vc,\
 hi, ls, ge, lt, gt, le, al, nv

;;;;;;;;;;;;;;;;; BARREL SHIFTER ;;;;;;;;;;;;;;;;;

; 31-28   25 24-21 20 19-16 15-12 987654/3210
; [CCCC/00/I/OPCODE/S/RNXX/RDXX/OPERAND2/XXXX]

;         /I=0: SHIFT R        /IIIIIIII/RMXX/
;         /I=1: SHIFT I        /SHXX/IIIIIIII/

; @asr r0, 16 ; mov r0, r0, asr 16
; @lsl r1, r2 ; mov r1, r1, lsl r2

; create shift instructions...

numeric SH.*, lsl, lsr, asr, ror

irps x, lsl lsr asr ror {
 macro @#x a, b \{
  verify.r a
  if b ?is.r
   @arm 0E1A00000h or (a shl 12) or \
    (b shl 8) or (SH.\#x shl 5) or \
    10000b or a
  else if b ?is.i
   verify.shn x, b
   @arm 0E1A00000h or (a shl 12) or \
    (b shl 7) or (SH.\#x shl 5) or a
  else
   'Source must be R/I:' b
  end if
 \}
 macro @#x#s a, b \{ ; s/et flags
  verify.r a
  if b ?is.r
   @arm 0E1B00000h or (a shl 12) or \
    (b shl 8) or (SH.\#x shl 5) or \
    10000b or a
  else if b ?is.i
   verify.shn x, b
   @arm 0E1B00000h or (a shl 12) or \
    (b shl 7) or (SH.\#x shl 5) or a
  else
   'Source must be R/I:' b
  end if
 \}
}

macro verify.shs s {
 if ?not s in <lsl,lsr,asr,ror>
  'Shift expected:' s
 end if
}

macro verify.shn s, n {
 if s eq lsl | s eq ror
  verify.n n, 0, 31
 else
  verify.n n, 1, 32
 end if
}

macro verify.sh s, n {
 verify.shs s
 if ?not n ?is.r
  verify.shn s, n
 end if
}

macro @test.sh {
 @lsl r0, r1
 @lsr r3, r4
 @asr r5, r7
 @ror r5, 7
 @lsls r0, r1
 @lsrs r3, r4
 @asrs r5, r7
 @rors r5, 7
}

;;;;;;;;;;;;;;;;;;;; TRANSFER ;;;;;;;;;;;;;;;;;;;;

; 31-28 27-24 23-0         9876543210
; [CCCC/101L/IIIIIIIIIIIIIIIIIIIIIIII]

; branch + link register...

; calculate 24BIT relative address:

; (((((o)-_begin_)-($-_begin_)-8) shr 2) \
;  and 0FFFFFFh)

; create 32 bxx+blxx instructions...

irps x, eq ne hs lo mi pl vs vc \
 hi ls ge lt gt le al nv {
 macro @b#x o \{
  verify.a (o), 4
  @arm (C.\#x shl 28) or (101b shl 25) \
   or (((((o)-_begin_)-($-_begin_)-8) shr 2) \
   and 0FFFFFFh)
 \}
 macro @bl#x o \{
  verify.a (o), 4
  @arm (C.\#x shl 28) or (101b shl 25) \
   or (((((o)-_begin_)-($-_begin_)-8) shr 2) \
   and 0FFFFFFh) or (1 shl 24)
 \}
}

@bl fix @blal

macro @test.b {
 local a, b, c, d
 a: @beq d
 @nop
 b: @bgt c
 @nop
 c: @blt b
 @nop
 d: @bmi a
 @bl a
}

; branch with link and exchange

macro @blx r {
 verify.cpu ARM.v5T
 verify.r r
 @arm 0E12FFF30h or r
}

; branch and change to Jazelle state

macro @bxj r {
 verify.cpu ARM.v5TEJ
 verify.r r
 @arm 0E12FFF20h or r
}

; software interrupt: swi/svc

macro @swi n {
 verify.n n, 0, 0FFFFFFh
 @arm 0EF000000h or n
}

macro @svc n { @swi n }

; breakpoint

macro @bkpt n {
 verify.cpu ARM.v5T
 verify.i n
 verify.u16 n
 @arm 0E1200000h or \
  (((n shr 4) and 0FFFh) shl 8) \
  or (n and 0Fh) or (7 shl 4)
}

;;;;;;;;;;;;;;;;;;; IMMEDIATE ;;;;;;;;;;;;;;;;;;;;

; load/construct 8-32BIT immediate/address...

macro @ldi a, b {
 local n
 verify.r a
 verify.i b
 if (b)=-1 | (b)=0FFFFFFFFh ; -1/0FFFFFFFFh
  @mvns a, 0		    ; mvns a, 0
 else if (b)>=0 & (b)<=255  ; 8BIT
  @arm (0E3Bh shl 20) or \  ; movs a, b
   (a shl 16) or \
   (a shl 12) or b
 else if \
  ((b) and 0FFFF00FFh)=0    ; any byte
  @arm 0E3B00C00h or \	    ; shift left?
   (a shl 12) or (b shr 8)  ; including
 else if \		    ; powers of 2
  ((b) and 0FF00FFFFh)=0    ; FF0000h
  @arm 0E3B00800h or \
   (a shl 12) or (b shr 16)
 else if \
  ((b) and 00FFFFFFh)=0     ; 0FF000000h
  @arm 0E3B00400h or \
   (a shl 12) or (b shr 24)
 else if \
  ((b) and 0FFFFF00Fh)=0    ; odd bytes...
  @arm 0E3B00E00h or \	    ; FF0h
   (a shl 12) or (b shr 4)
 else if \
  ((b) and 0FFF00FFFh)=0    ; 0FF000h
  @arm 0E3B00A00h or \
   (a shl 12) or (b shr 12)
 else if \
  ((b) and 0F00FFFFFh)=0    ; 0FF00000h
  @arm 0E3B00600h or \
   (a shl 12) or (b shr 20)
 else			    ; build value...
  n=(b) and 0FFh
  @arm (0E3Bh shl 20) or \  ; movs a, b&0FFh
   (a shl 16) or \
   (a shl 12) or n
  n=((b) and 0FF00h) shr 8
  if n<>0
   @arm (0E39h shl 20) \    ; orrs a, a, b&0FF00h
    or (a shl 16) \
    or (a shl 12) \
    or 0C00h or n	    ; orrs if 16/24BIT...
  end if
  n=((b) and 0FF0000h) \
   shr 16
  if n<>0
   @arm (0E39h shl 20) or (a shl 16) or \
    (a shl 12) or 800h or n
  end if
  n=((b) and 0FF000000h) shr 24
  if n<>0
   @arm (0E39h shl 20) or (a shl 16) or \
    (a shl 12) or 400h or n
  end if
 end if
}

macro @test.ldi {
 @ldi r3, -1
 @ldi r3, 0ABh
 @ldi r3, 07F0h
 @ldi r3, 07F00h
 @ldi r3, 07F000h
 @ldi r3, 07F0000h
 @ldi r3, 07F00000h
 @ldi r3, 0FF000000h
 @ldi r3, 080000000h
 @ldi r3, 1234h
}

; movw/movt 16BIT immediate: ARM.v6T2...

; W: [CCCC/0011/0000/IIII/RDXX/IIIIIIIIIIII]
; T: [CCCC/0011/0100/IIII/RDXX/IIIIIIIIIIII]

macro @movwx name, o {
 macro @#name a, b \{
  verify.cpu ARM.v6T2
  verify.r a
  verify.u16 b
  @arm 0E3000000h or (o shl 20) \
   or (a shl 12) or ((b and 0F000h) \
   shl 4) or (b and 0FFFh)
 \}
}

@movwx movw, 0
@movwx movt, 100b

macro @movwt r, i {
 @movw r, (i and 0FFFFh)
 if i>0FFFFh
  @movt r, ((i and 0FFFF0000h) shr 16)
 end if
}

; "move immediate" depending on CPU

macro @movi r, i {
 verify.r r
 verify.i i
 if.cpu ARM.v6T2
  @movwt r, i
 else
  @ldi r, i
 end if
}

; count leading zeros

macro @clz a, b {
 verify.cpu ARM.v5T
 verify.r a, b
 @arm 0E16F0F10h or (a shl 12) or b
}

; get current+saved program status
; register. s=CPSR/SPSR...

macro @mrs r, s {
 verify.r r
 if s eq CPSR
  @arm 0E10F0000h or (r shl 12)
 else if s eq SPSR
  @arm 0E14F0000h or (r shl 12)
 end if
}

; set CPSR/SPSR = register. c=PSR BITs:

; CPSR_f/s/x/c/fs/fsxc
; SPSR_f/s/x/c/fs/fsxc

; no APSR synonms, use CPSR equivelants

numeric CPSR_*,\
 f=28h, s=24h, x=22h, c=21h, fs=2Ch, fsxc=2Fh

numeric SPSR_*,\
 f=68h, s=64h, x=62h, c=61h, fs=6Ch, fsxc=6Fh

macro @msr c, r {
 verify.r r
 if ?not c in \
  <CPSR_f,CPSR_s,CPSR_x,CPSR_c,CPSR_fs,\
   CPSR_fsxc,SPSR_f,SPSR_s,SPSR_x,SPSR_c,\
   SPSR_fs,SPSR_fsxc>
  'Invalid field:' c
 end if
 @arm 0E100F000h or (c shl 16) or r
}

macro @test.mrs {
 @mrs r7, CPSR
 @mrs r7, SPSR
 @msr CPSR_f, r5
 @msr CPSR_s, r5
 @msr CPSR_x, r5
 @msr CPSR_c, r5
 @msr CPSR_fs, r5
 @msr CPSR_fsxc, r5
 @msr SPSR_f, r5
 @msr SPSR_s, r5
 @msr SPSR_x, r5
 @msr SPSR_c, r5
 @msr SPSR_fs, r5
 @msr SPSR_fsxc, r5
}

;;;;;;;;;;;;;;;;;; LOAD & STORE ;;;;;;;;;;;;;;;;;;

; [CCCC/01IP/UBWL/RNXX/RDXX/IIIIIIIIIIII]

; I  - Immediate (1) or register/shift (0)?
; P  - Pre-post? Add/subtract offset before
;      (P=1) or after (P=0) transfer
; U  - Up/down. Add (U=1) or subtract offset
; B  - Byte? 0=Word, 1=Byte
; W  - Write-back address to base?
; L  - Load or store? 1=Load. 0=Store
; RN - Base register
; RD - Source/destiny
; II - 12BIT offset or shift+r (S+M)

macro @ls cc, l, z, r, [p] {
 common
  local ?a, ?b
  ?a=0
  ?b=0
  syntax 0
   match =0 \	       ; ldr r, [b, i, sh #]!
    [b=,i=,sh s]=!, \  ; scaled register
    ?s p \{	       ; pre-index
    verify.r b, i
    verify.i s
    verify.sh sh, s
    ?a=05Ah or 100000b
    ?b=(b shl 16) or \
     (s shl 7) or \
     (SH.\#sh shl 5) \
     or i
   syntax 1
  \}
  match =0 [b=,o]=!, \ ; ldr r, [b, o]!
   ?s p \{
   verify.r b
   if o ?is.r	       ; register pre-index
    ?a=078h or 10b     ; ldr r, [b, ri]!
    ?b=(b shl 16) or o
   else if o ?is.i     ; ldr r, [b, #]!
    verify.12 o
    if o<0
     ?a=050h or 10b    ; negative
     ?b=(b shl 16) \
      or (0-o)
    else
     ?a=058h or 10b    ; positive
     ?b=(b shl 16) \
      or o
    end if
   else
    'Unexpected:' o
   end if
   syntax 1
  \}
  match =0 \	       ; ldr r, [b, i, sh #]
   [b=,i=,sh s], \     ; scaled register
   ?s p \{	       ; post-index
   verify.r b, i
   verify.i s
   verify.sh sh, s
   ?a=078h
   ?b=(b shl 16) or \
    (s shl 7) or \
    (SH.\#sh shl 5) \
    or i
   syntax 1
  \}
  match =0 [b]=,-n, \  ; ldr r, [b], -#
   ?s p \{	       ; negative immediate
   verify.r b	       ; post-index
   verify.i n
   verify.12 n
   ?a=040h
   ?b=(b shl 16) or n
   syntax 1
  \}
  match =0 [b]=,o, \   ; ldr r, [b], o
   ?s p \{
   verify.r b
   if o ?is.r	       ; ldr r, [b], ro
    ?a=068h
    ?b=(b shl 16) or o ; register post-index
    syntax 1
   else if o ?is.i     ; ldr r, [b], #
    verify.12 o
    if o<>0
     ?a=048h
    else
     ?a=058h
    end if
    ?b=(b shl 16) or o ; immediate post-index
    syntax 1
   else
    'Unexpected:' o
   end if
  \}
  match =0 [b=,o], \   ; ldr r, [b, o]
   ?s p \{
   if o ?is.r	       ; ldr r, [b, ri]
    ?a=078h	       ; register post-index
    ?b=(b shl 16) or o
   else if o ?is.i     ; ldr r, [b, #]
    verify.12 o        ; immediate post-index
    if o>=0	       ; positive
     ?a=58h
     ?b=(b shl 16) \
      or o
    else	       ; negative
     ?a=50h
     ?b=(b shl 16) \
      or (0-o)
    end if
   else
    'Unexpected:' o
   end if
   syntax 1
  \}
  match =0 [b], \      ; ldr r, [b]
   ?s p \{
   ?a=58h
   if b ?is.r	       ; base=register
    ?b=(b shl 16)
   else if b ?is.i     ; immediate
    verify.12 b        ; pc relative
    ?b=($-_begin_)-b+8
    if ?b>=0	       ; positive
     ?a=51h
    else	       ; negative
     ?a=59h
     ?b=0-?b
    end if
    ?b=?b or \
     (0Fh shl 16)      ; base=pc
   else
    'Unexpected:' b
   end if
   syntax 1
  \}
 verify ls
 @arm (C.#cc shl 28) or (z shl 22) or \
  (l shl 20) or (?a shl 20) or \
  (r shl 12) or ?b
}

macro @ldr [p]	{ common @ls al, 1, 0, p }
macro @str [p]	{ common @ls al, 0, 0, p }

macro @ldrb [p] { common @ls al, 1, 1, p }
macro @strb [p] { common @ls al, 0, 1, p }

macro @test.ls {
 @ldr r0, [123h]
 @ldr r0, [-123h]
 @ldr r0, [r1]
 @ldr r0, [r1], 1
 @ldr r0, [r1], -1
 @ldr r0, [r1, r2]
 @ldr r0, [r1, 1]
 @ldr r0, [r1, -1]
 @ldr r0, [@15, 0ABCh]
 @ldr r0, [@15, -0ABCh]
 @ldr r0, [r7, 0ABCh]
 @ldr r0, [r7, -0ABCh]
 @ldr r0, [r1, r2, lsl 3]
 @ldr r0, [r1, 777h]!
 @ldr r0, [r1, -777h]!
 @ldr r0, [r1, r2]!
 @ldr r0, [r1, r2, lsl 3]!
}

macro @test.lsb {
 @ldrb r5, [r7]
 @strb r5, [r7]
}

;;;;;;;; LOAD/STORE SIGNED BYTE/HALF/DUAL ;;;;;;;;

; [CCCC/000P/UIWL/RNXX/RDXX/0000/1SH1/RMXX]
; [CCCC/000P/UIWL/RNXX/RDXX/IIII/1SH1/IIII]
;  CCCC/       /L/    /RRRR/    /XXXX/

; @ldrsb r2, [r7]
; @ldrsh r2, [r7, 48h]!
; @ldrh r2, [r7, -48h]
; @ldrd r2, [r7], 8
; @strh r2, [r7], -4
; @strd r3, [r7, 128]

macro @lsx cc, l, x, r, [p] {
 common
  local pi, u, i, n, w
  pi=0			; pre-index?
  u=0			; up/down? add/sub
  i=0			; immediate?
  n=0			; number
  w=0			; write-back?
  verify.r r
  syntax 0
  match =0 [b=,o]!,\	; lsx r, [b,o]!
   ?s p \{
   pi=1 		; pre-index
   u=1			; up/add
   w=1			; write-back
   verify.r b
   if o ?is.r		; base=register
    @arm1 cc, pi,\
     (u shl 3) or (i shl 2) or\
     (w shl 1) or l, b, r,\
     0, x, o
   else if o ?is.i	; immediate
    i=1
    n=o
    if n<0		; negative
     n=0-n
     u=0		; down/sub
    end if
    verify.u8 n
    @arm1 cc, pi,\
     (u shl 3) or (i shl 2) or\
     (w shl 1) or l, b, r,\
     ((n and 0F0h) shr 4),\
     x, (n and 0Fh)
   else
    'Unexpected:' o
   end if
   syntax 1
  \}
  match =0 [b=,o],\	; lsx r, [b,o]
   ?s p \{
   pi=1 		; pre-index
   u=1			; up/add
   verify.r b
   if o ?is.r		; base=register
    @arm1 cc, pi,\
     (u shl 3) or (i shl 2) or\
     (w shl 1) or l, b, r,\
     0, x, o
   else if o ?is.i	; immediate
    i=1
    n=o
    if n<0		; negative
     n=0-n
     u=0		; down/sub
    end if
    verify.u8 n
    @arm1 cc, pi,\
     (u shl 3) or (i shl 2) or\
     (w shl 1) or l, b, r,\
     ((n and 0F0h) shr 4),\
     x, (n and 0Fh)
   else
    'Unexpected:' o
   end if
   syntax 1
  \}
  match =0 [b]=,o, \	; lsx r, [b], o
   ?s p \{
   verify.r b
   if o ?is.r		; lsx r, [b], r
    @arm1 cc, pi,\
     (1000b or l),\
     b, r, 0, x, o
    syntax 1
   else 		; lsx r, [b], -/i/-r
    syntax 0
    match -O, o \\{	; lsx r, [b], -r
     if O ?is.r
      @arm1 cc, pi,\
      l, b, r, 0, x, O
      syntax 1
     else		; lsx r, [b], -i
      u=0
      i=1
      verify.8 O
      @arm1 cc, pi,\
       (u shl 3) or (i shl 2) or\
       (w shl 1) or l, b, r,\
       ((O shr 4) and 0Fh),\
       x, (O and 0Fh)
      syntax 1
     end if
    \\}
    if.syntax 0
     if o ?is.i 	; lsx r, [b], i
      u=1
      i=1
      verify.8 o
      @arm1 cc, pi,\
       (u shl 3) or (i shl 2) or\
       (w shl 1) or l, b, r,\
       ((o shr 4) and 0Fh),\
       x, (o and 0Fh)
      syntax 1
     else
      'Unexpected:' o
     end if
    end if
   end if
  \}
  match =0 [b], ?s p \{ ; lsx r, [b]
   pi=1 		; pre-index
   if b ?is.r		; base=register
    u=1 		; up
    i=1 		; immediate (0)
    @arm1 cc, pi,\
     (u shl 3) or (i shl 2) or\
     (w shl 1) or l, b, r,\
     0, x, 0
   else if b ?is.i	; immediate
    i=1
    n=($-_begin_)-b+8	; pc relative
    verify.8 n
    if n>0		; positive
     u=0
     @arm1 cc, pi,\
      (u shl 3) or (i shl 2) or\
      (w shl 1) or l, 0Fh, r,\
      ((n and 0F0h) shr 4),\
      x, (n and 0Fh)
    else		; negative
     u=1
     n=0-n
     @arm1 cc, pi,\
      (u shl 3) or (i shl 2) or\
      (w shl 1) or l, 0Fh, r,\
      ((n and 0F0h) shr 4),\
      x, (n and 0Fh)
    end if
   else
    'Unexpected:' b
   end if
   syntax 1
  \}
 verify
}

numeric LDR.*,\
 SB=1101b, SH=1111b, H=1011b, D=1101b

numeric STR.*, H=LDR.H, D=LDR.SH

macro @ldrsb r, [p]
 { common @lsx al, 1, LDR.SB, r, p }

macro @ldrsh r, [p]
 { common @lsx al, 1, LDR.SH, r, p }

macro @ldrh r, [p]
 { common @lsx al, 1, LDR.H, r, p }

macro @strh r, [p]
 { common @lsx al, 0, STR.H, r, p }

macro verify.re r {
 if (r and 1)<>0
  'Register must be even' r
 end if
}

macro @ldrd r, [p] {
 common verify.re r
  @lsx al, 0, LDR.D, r, p
}

macro @strd r, [p] {
 common verify.re r
  @lsx al, 1, STR.D, r, p
}

macro @test.lsx {
 @ldrsb r2, [48h]
 @ldrsb r2, [-48h]
 @ldrsb r2, [r7]
 @ldrsh r2, [r7]
 @ldrh r2, [r7]
 @ldrd r2, [r7]
 @strd r2, [r7]
 @ldrsb r2, [r7], 48h
 @ldrsb r2, [r7], -48h
 @ldrsb r2, [r7], r3
 @ldrsb r2, [r7], -r5
 @ldrsb r2, [@pc, r5]
 @ldrsb r2, [@pc, 48h]
 @ldrsb r2, [r7, r5]
 @ldrsb r2, [r7, 48h]
 @ldrsb r2, [r7, -48h]
 @ldrsb r2, [r7, 48h]!
 @ldrsb r2, [r7, -48h]!
}

;;;;;;;;;;;; EASY LOAD/STORE TYPE/SIZE ;;;;;;;;;;;

macro @xls name, a, b, n, i {
 if i eq
  @#name a, b
 else
  @#name a, b, (i*n)
 end if
}

macro @ldx a, b, s, i {
 if s eq i8
  @xls ldrsb, a, b, 1, i
 else if s eq u8
  @xls ldrb, a, b, 1, i
 else if s eq i16
  @xls ldrsh, a, b, 2, i
 else if s eq u16
  @xls ldrh, a, b, 2, i
 else if s eq i32 | s eq u32
  @xls ldr, a, b, 4, i
 else if s eq i64 | s eq u64
  @xls ldrd, a, b, 8, i
 else
  'Invalid type/size:' s
 end if
}

macro @stx a, b, s, i {
 if s eq i8 | s eq u8
  @xls strb, a, b, 1, i
 else if s eq i16 | s eq u16
  @xls strh, a, b, 2, i
 else if s eq i32 | s eq u32
  @xls str, a, b, 4, i
 else if s eq i64 | s eq u64
  @xls strd, a, b, 8, i
 else
  'Invalid type/size:' s
 end if
}

;;;;;;;;;;;;; CONDITIONAL LOAD/STORE ;;;;;;;;;;;;;

; @str:h:ne r1, [r2]

macro @ldr [p] {
 common
  syntax 0
  match :x:c r=,s, p \{ ; ldr:x:c
   if x eq b
    @ls c, 1, 1, r, s
   else if x eq sb
    @lsx c, 1, LDR.SB, r, s
   else if x eq h
    @lsx c, 0, STR.H, r, s
   else if x eq sh
    @lsx c, 1, LDR.SH, r, s
   else
    'Invalid:' x
   end if
   syntax 1
  \}
  match =0 :c r=,s, ?s p \{ ; ldr:c
   @ls c, 1, 0, r, s
   syntax 1
  \}
  if.syntax 0
   @ldr p
  end if
}

macro @str [p] {
 common
  syntax 0
  match :x:c r=,s, p \{ ; str:x:c
   if x eq b
    @ls c, 0, 1, r, s
   else if x eq h
    @lsx c, 0, STR.H, r, s
   else
    'Invalid:' x
   end if
   syntax 1
  \}
  match =0 :c r=,s, ?s p \{ ; str:c
   @ls c, 0, 0, r, s
   syntax 1
  \}
  if.syntax 0
   @str p
  end if
}

;;;;;;;;;;;;;;; LOAD/STORE MULTIPLE ;;;;;;;;;;;;;;

; [CCCC/100P/USWL/RNXX/REGISTERSXXXXXXX]

; P  - Pre-post? Add/subtract offset before
;      (P=1) or after (P=0) transfer
; U  - Up/down. Add (U=1) or subtract offset
; S  - Load PSR or force user mode? 1/0
; W  - Write-back address to base? 1/0
; L  - Load? 1/0

; example: @ldm r0, r1,r2,r3

; [1110/100P/USWL/0000/0000000000001110]
;            1000  r0              321

macro @lsm op, b, [r] {
 common ?rs=0
 forward ?rs=?rs or (1 shl r) ; ?rs|=r
 common @arm (0Eh shl 28) or \
  (b shl 16) or (op shl 20) or ?rs
}

; @ldm/@stm = ldmia/stmdb or ldmfd/stmfd
; full descending stack. !=write-back

macro @ldm r, [p] { common @lsm 89h, r, p }
macro @stm r, [p] { common @lsm 90h, r, p }

macro @ldm! r, [p] { common @lsm 8Bh, r, p }
macro @stm! r, [p] { common @lsm 92h, r, p }

;;;;;;;;;;;;;;;;;;; PUSH + POP ;;;;;;;;;;;;;;;;;;;

; example: @push r0-r3, r5, r6, r7, v7-v8, @lr
; output:  push r0-r3, r5-r7, v7-v8, lr

macro @pp o, c, [p] {
 common
  local r, rs
  r=0
  rs=0
 forward
  syntax 0
  match a-b, p \{
   verify.r a, b
   if a>b
    'Invalid range:' a-b
   end if
   r=a
   while r<=b
    if rs and (1 shl r)<>0
     'Duplicate register in:' a-b
    end if
    rs=rs or (1 shl r)
    r=r+1
   end while
   syntax 1
  \}
  if.syntax 0
   verify.r p
   if rs and (1 shl p)<>0
    'Duplicate register:' p
   end if
   rs=rs or (1 shl p)
  end if
 common
  @arm ((c shl 28) or (@sp shl 16) \
   or (o shl 20) or rs)
}

macro @push [p] { common @pp 92h, C.al, p }
macro @pop [p]	{ common @pp 8Bh, C.al, p }

macro @pushv { @push v1-v8, @lr }
macro @popv  { @pop v1-v8, @pc }

macro @test.pp {
 @push r0-r3, r5, r6, r7, v7-v8, @lr
 @pop r0-r3, r5, r6, r7, v7-v8, @pc
}

;;;;;;;;;;;;;;; DATA + ARITHMETIC ;;;;;;;;;;;;;;;;

; 31-28   25 24-21 20 19-16 15-12 987654/3210
; [CCCC/00/I/OPCODE/S/RNXX/RDXX/OPERAND2/XXXX]

;         /I=0: SHIFT R        /IIIIIIII/RMXX/
;         /I=1: SHIFT I        /SHXX/IIIIIIII/

; @mov r0, r1
; @cmp r0, r2
; @adds r0, r1, r2
; @sub:gt r0, r1, r2
; @bics:mi r1, r2, r3, asr r4

numeric it.*,\
 and, eor, sub, rsb, add, adc, sbc, rsc,\
 tst, teq, cmp, cmn, orr, mov, bic, mvn

numeric it.*,\
 ands, eors, subs, rsbs, adds, adcs, sbcs, rscs,\
 tsts, teqs, cmps, cmns, orrs, movs, bics, mvns

; create "data processing" instruction...

macro @dp it, s {
 macro @#it [p] \{
 \common
  local im
  im=0
  syntax 0
  if it in <tst,teq,cmp,cmn,mov,mvn,\
    tsts,teqs,cmps,cmns,movs,mvns>
   match a=,b=,sh, p \\{
    match x y, sh \\\{
     if ?not x in <lsl,lsr,asr,ror>
      'Operand 3 is invalid:' x
     end if
     syntax 1
    \\\}
    if.syntax 0
     'Operand 3 is invalid'
    end if
   \\}
  end if
   syntax 0
  match =0 :x a=,b=,c=,d, \ ; :x a, b, c, <d>
   ?s p \\{
   match sh n, d \\\{
    verify.sh sh, n
    verify.r a
    if n ?is.r ; shx r
     @arm (C.\\\#x shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (b shl 16) or c or \
      (n shl 8) or (SH.\\\#sh shl 5) or 16
    else if n ?is.i ; shx #
     @arm (C.\\\#x shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (b shl 16) or c or \
      (n shl 7) or (SH.\\\#sh shl 5)
    else
     'Unexpected:' n
    end if
    syntax 1
   \\\}
   if.syntax 0 ; :x a, b, c, ri
    verify.r a, b, c
    if ?not c ?is.r & c ?is.i
     verify.u8 c
     im=1
    end if
    @arm (C.\\#x shl 28) or \
     (it.\\#it shl 21) or (im shl 25) or \
     (s shl 20) or (b shl 16) or \
     (a shl 12) or c
   end if
   syntax 1
  \\}
  match =0 :x a=,b=,c, \ ; :x a, b, <c>
   ?s p \\{
   match sh n, c \\\{
    verify.sh sh, n
    verify.r a, b
    if n ?is.r ; shx r
     @arm (C.\\\#x shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (a shl 16) or b or \
      (n shl 8) or (SH.\\\#sh shl 5) or 16
    else if n ?is.i ; shx #
     @arm (C.\\\#x shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (a shl 16) or b or \
      (n shl 7) or (SH.\\\#sh shl 5)
    else
     'Unexpected:' n
    end if
    syntax 1
   \\\}
   if.syntax 0 ; :x a, b, ri
    verify.r a, b
    if ?not c ?is.r & c ?is.i ; i
     verify.u8 c
     im=1
    end if
    @arm (C.\\#x shl 28) or \
     (it.\\#it shl 21) or (im shl 25) or \
     (s shl 20) or (b shl 16) or \
     (a shl 12) or c
   end if
   syntax 1
  \\}
  match =0 :x a=,b, ?s p \\{ ; :c r, ri
   verify.r a
   if ?not b ?is.r & b ?is.i ; i
    verify.u8 b
    im=1
   end if
   @arm (C.\\#x shl 28) or \
    (it.\\#it shl 21) or (im shl 25) or \
    (s shl 20) or (a shl 16) or \
    (a shl 12) or b
   syntax 1
  \\}
  match =0 a=,b=,c=,d, \ ; a, b, c, <d>
   ?s p \\{
   match sh n, d \\\{
    verify.sh sh, n
    verify.r a, b, c
    if n ?is.r ; shx r
     @arm (C.al shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (b shl 16) or c or \
      (n shl 8) or (SH.\\\#sh shl 5) or 16
    else if n ?is.i ; shx #
     verify.u5 n
     @arm (C.al shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (b shl 16) or c or \
      (n shl 7) or (SH.\\\#sh shl 5)
    else
     'Unexpected:' n
    end if
    syntax 1
   \\\}
   if.syntax 0 ; a, b, c, ri
    verify.r a, b, c
    if ?not c ?is.r & c ?is.i
     verify.u8 c
     im=1
    end if
    @arm (C.al shl 28) or \
     (it.\\#it shl 21) or (im shl 25) or \
     (s shl 20) or (b shl 16) or \
     (a shl 12) or c
   end if
   syntax 1
  \\}
  match =0 a=,b=,c, ?s p \\{ ; a, b, <c>
   match sh n, c \\\{
    verify.sh sh, n
    verify.r a
    if n ?is.r ; shx r
     @arm (C.al shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (a shl 16) or b or \
      (n shl 8) or (SH.\\\#sh shl 5) or 16
    else if n ?is.i ; shx #
     @arm (C.al shl 28) or \
      (it.\\\#it shl 21) or (s shl 20) or \
      (a shl 12) or (a shl 16) or b or \
      (n shl 7) or (SH.\\\#sh shl 5)
    else
     'Unexpected:' n
    end if
    syntax 1
   \\\}
   if.syntax 0 ; a, b, ri
    verify.r a, b
    if ?not c ?is.r & c ?is.i
     verify.u8 c
     im=1
    end if
    @arm (C.al shl 28) or \
     (it.\\#it shl 21) or (im shl 25) or \
     (s shl 20) or (b shl 16) or \
     (a shl 12) or c
   end if
   syntax 1
  \\}
  match =0 a=,b, ?s p \\{ ; a, b
   verify.r a
   verify.o b
   if ?not b ?is.r & b ?is.i
    verify.u8 b
    im=1
   end if
   @arm (C.al shl 28) or \
    (it.\\#it shl 21) or (im shl 25) or \
    (s shl 20) or (a shl 16) or \
    (a shl 12) or b
   syntax 1
  \\}
  verify @\#i
 \}
}

macro @dp [p] {
forward
 @dp p, 0
 @dp p#s, 1
}

; create 512 instructions:

; 32 (16*2 add/s) * 16 conditions each =
; 512 total variations. "add:c" condition
; syntax avoids writing 512 macros!

@dp and, eor, sub, rsb, add, adc, sbc, rsc,\
 tst, teq, cmp, cmn, orr, mov, bic, mvn

@cmp fix @cmps ; automatic
@cmn fix @cmns
@tst fix @tsts
@teq fix @teqs

; test dp instructions...

macro @test.dp {
 @mov r7, r7	 ; a=b
 @and r5, 0FFh	 ; a=a&c
 @eor r5, r6, 15 ; a=b^c
 @sub r5, r6, r7 ; a=b-c
 @rsb r5, r6, r7 ; a=c-b
 @add r5, r6, r7 ; a=b+c
 @adc r5, r6, r7 ; a=b+c
 @sbc r5, r6, r7 ; a=b-c
 @rsc r5, r6, r7 ; a=c-b
 @tst r5, r6	 ; a&b?
 @teq r5, r6	 ; a&b?
 @cmp r5, r6	 ; a=b?
 @cmn r5, r6	 ; a=-b?
 @orr r5, r6, r7 ; a=b|c
 @bic r5, r6, r7 ; a=b&~c
 @mvn r5, r6	 ; a=-b
}

macro @test.dpx {
 @add r5, r6, lsl r7	    ; a=b<<c
 @add r5, r6, lsl 7	    ; a=b<<i
 @add r0, r3, r5, lsr r7    ; a=b+(c>>>d)
 @adds r0, r3, r5, lsr 7    ; a=b+(c>>>i)
 @add:ne r0, r1, asr r7     ; ne? a=(b>>c)
 @adds:lt r0, r1, asr 7     ; lt? a=(b>>i)
 @add:gt r0, r3, r5, ror r7 ; gt? a=b+(c<>>d)
 @adds:mi r0, r3, r5, ror 7 ; mi? a=b+(c<>>7)
}

macro @inc r { @adds r, 1 }
macro @dec r { @subs r, 1 }

macro @neg r { @rsbs r, 0 }
macro @not r { @mvns r, r }

macro @abs r { ; absolute value
 @teq r, 0     ; if a<0, a=0-a
 @rsbs:mi r, 0
}

;;;;;;;;;;;;;;;;;;; SATURATION ;;;;;;;;;;;;;;;;;;;

; [CCCC/0001/QQQQ/RNXX/RDXX/0000/0101/RMXX]

; add, subtract, double, with 32BIT saturation.
; for packed 8/16BIT saturating arithmetic,
; see PACKING (Ctrl+F)

macro @qsat name, o {
 verify.cpu ARM.v5TE
 macro @#name a, b, c \{
  verify.r a, b, c
  @arm 0E1000050h or (o shl 20) \
   or (a shl 12) or b or (c shl 16)
 \}
}

@qsat qadd, 0
@qsat qsub, 2
@qsat qdadd, 4
@qsat qdsub, 6

; [CCCC/0110111/NNNNN/RDXX/IIIII/S/01/RNXX]

; saturate to bit position with optional
; shift before

macro @xsat name, o {
 macro @#name a, n, b, s \{
  verify.cpu ARM.v6
  verify.r a, b
  if s eq ; register
   @arm 0E6000010h or (o shl 16) \
    or (a shl 12) or b
  else
   match sh n, s \\{ ; shift
    if name eq ssat
     if n<1 | n>32
      'Shift # must be 1-32'
     end if
    else if name eq usat
     if n<0 | n>31
      'Shift # must be 0-31'
     end if
    end if
    if sh eq asr
     @arm 0E6001850h or (o shl 16) \
      or (a shl 12) or b
    else if sh eq lsl
     @arm 0E6001810h or (o shl 16) \
      or (a shl 12) or b
    else
     'Shift must be asr/lsl #'
    end if
    syntax 1
   \\}
  end if
  verify
 \}
}

@xsat ssat, 0AEh
@xsat usat, 0EFh

macro @test.sat {
 @qadd r3, r5, r7
 @qsub r3, r5, r7
 @qdadd r3, r5, r7
 @qdsub r3, r5, r7
 @ssat r1, 15, r5
 @ssat r1, 15, r5, asr 12
 @ssat r1, 15, r5, lsl 12
 @usat r1, 15, r5
 @usat r1, 15, r5, asr 12
 @usat r1, 15, r5, lsl 12
}

;;;;;;;;;;;;;;;;;;;; MULTIPLY ;;;;;;;;;;;;;;;;;;;;

; [CCCC/0000000/A/S/RDXX/RNXX/RSXX/1001/RMXX]

; multiply: 32*32

macro @mul a, b, c {
 verify.r a, b
 if c eq
  @arm 0E0000090h or (a shl 16) or b \
   or (a shl 8)
 else
  verify.r c
  @arm 0E0000090h or (a shl 16) or b \
   or (c shl 8)
 end if
}

macro @muls a, b, c {
 verify.r a, b
 if c eq
  @arm 0E0100090h or (a shl 16) or b \
   or (a shl 8)
 else
  verify.r c
  @arm 0E0100090h or (a shl 16) or b \
   or (c shl 8)
 end if
}

macro @mx4 name, o {
 macro @#name a, b, c, d \{
  if name eq umaal
   verify.cpu ARM.v6
  else if name eq mls
   verify.cpu ARM.v6T2
  end if
  verify.r a, b, c, d
  @arm 0E0000090h or (o shl 20) or \
   (a shl 12) or (b shl 16) or \
   c or (d shl 8)
 \}
}

; multiply + accumulate or subtract:
; 32*32+32 or 32*32-32

@mx4 mla, 2
@mx4 mls, 6

; un/signed multiply 32BIT with 64BIT
; result or accumulate

@mx4 umull, 8
@mx4 smull, 0Ch
@mx4 umlal, 0Ah
@mx4 smlal, 0Eh

; unsigned multiply 64BIT + accumulate

@mx4 umaal, 4

; signed 16BIT b/t multiply + 32BIT/64BIT
; accumulate. xy = b=bottom, t=top

macro @smulxy name, o {
 macro @#name a, b, c \{
  verify.cpu ARM.v5TE
  verify.r a, b, c
  @arm 0E1600000h or (o shl 4) or (b) \
   or (a shl 16) or (c shl 8)
 \}
}

macro @smlaxy name, o {
 macro @#name a, b, c, d \{
  verify.cpu ARM.v5TE
  verify.r a, b, c, d
  @arm 0E1000000h or (o shl 4) or b \
   or (a shl 16) or (c shl 8) or (d shl 12)
 \}
}

macro @smlalxy name, o {
 macro @#name a, b, c, d \{
  verify.cpu ARM.v5TE
  verify.r a, b, c, d
  @arm 0E1400000h or (o shl 4) or \
   (a shl 12) or (b shl 16) or c \
   or (d shl 8)
 \}
}

@smulxy smulbb, 8
@smulxy smultt, 0Eh
@smulxy smulbt, 0Ch
@smulxy smultb, 0Ah

@smlaxy smlabb, 8
@smlaxy smlatt, 0Eh
@smlaxy smlabt, 0Ch
@smlaxy smlatb, 0Ah

@smlalxy smlalbb, 8
@smlalxy smlaltt, 0Eh
@smlalxy smlalbt, 0Ch
@smlalxy smlaltb, 0Ah

; dual signed 16BIT multiply with addition
; or subtraction of products + 32BIT/64BIT
; accumulate and optional exchange of halves

macro @smlxd name, o {
 macro @#name a, b, c, d \{
  verify.cpu ARM.v6
  verify.r a, b, c, d
  @arm 0E7000000h or (o shl 4) or \
   (a shl 16) or b or (c shl 8) \
   or (d shl 12)
 \}
}

macro @smlxld name, o {
 macro @#name a, b, c, d \{
  verify.cpu ARM.v6
  verify.r a, b, c, d
  @arm 0E7400000h or (o shl 4) or \
   (a shl 12) or (b shl 16) or c \
   or (d shl 8)
 \}
}

macro @smuxd name, o {
 macro @#name a, b, c \{
  verify.cpu ARM.v6
  verify.r a, b, c
  @arm 0E700F000h or (o shl 4) or \
   (a shl 16) or b or (c shl 8)
 \}
}

@smlxd smlad, 1
@smlxd smlsd, 5

@smlxld smlald, 1
@smlxld smlsld, 5

@smuxd smuad, 1
@smuxd smusd, 5

@smuxd smuadx, 3
@smuxd smusdx, 7

; signed most significant 32BIT multiply
; with optional accumulate or subtract

macro @smmul a, b, c {
 verify.cpu ARM.v6
 verify.r a, b, c
 @arm 0E751F010h or (a shl 16) \
  or b or (c shl 8)
}

macro @smmla a, b, c, d {
 verify.cpu ARM.v6
 verify.r a, b, c, d
 @arm 0E7510010h or (a shl 16) or \
  b or (c shl 8) or (d shl 12)
}

macro @smmls a, b, c, d {
 verify.cpu ARM.v6
 verify.r a, b, c, d
 @arm 0E75100D0h or (a shl 16) or \
  b or (c shl 8) or (d shl 12)
}

; signed multiply wide 32x16 top/bottom

macro @smulx name, o {
 verify.cpu ARM.v5TE
 macro @#name a, b, c \{
  verify.r a, b, c
  @arm 0E1200000h or (o shl 4) or \
   (a shl 16) or b or (c shl 8)
 \}
}

macro @smlax name, o {
 verify.cpu ARM.v5TE
 macro @#name a, b, c, d \{
  verify.r a, b, c, d
  @arm 0E1200000h or (o shl 4) or \
   (a shl 16) or b or (c shl 8) or \
   (d shl 12)
 \}
}

@smulx smulwb, 0Ah
@smulx smulwt, 0Eh

@smlax smlawb, 8
@smlax smlawt, 0Ch

macro @test.mul {
 @mul r0, r1, r5
 @mla r1, r3, r5, r7
 @mls r1, r3, r5, r7
 @umull r1, r3, r5, r7
 @smull r1, r3, r5, r7
 @umlal r1, r3, r5, r7
 @smlal r1, r3, r5, r7
 @umaal r1, r3, r5, r7
 @smulbb r1, r3, r5
 @smultt r1, r3, r5
 @smulbt r1, r3, r5
 @smultb r1, r3, r5
 @smlabb r1, r3, r5, r7
 @smlatt r1, r3, r5, r7
 @smlabt r1, r3, r5, r7
 @smlatb r1, r3, r5, r7
 @smlalbb r1, r3, r5, r7
 @smlaltt r1, r3, r5, r7
 @smlalbt r1, r3, r5, r7
 @smlaltb r1, r3, r5, r7
 @smlad r1, r3, r5, r7
 @smlsd r1, r3, r5, r7
 @smlald r1, r3, r5, r7
 @smlsld r1, r3, r5, r7
 @smuad r1, r3, r5
 @smusd r1, r3, r5
 @smuadx r1, r3, r5
 @smusdx r1, r3, r5
 @smulwb r1, r3, r5
 @smulwt r1, r3, r5
 @smlawb r1, r3, r5, r7
 @smlawt r1, r3, r5, r7
}

; fast multiply r0 by common constant

macro @mul3  { @adds r0, r0, lsl 1 }
macro @mul5  { @adds r0, r0, lsl 2 }

macro @mul10 {
 @mov r0, r0, lsl 1  ; r0=(r0<<1)+(r0<<2)
 @adds r0, r0, lsl 2
}

;;;;;;;;;;;;;;;;;;; DIVISION ;;;;;;;;;;;;;;;;;;;;;

; un/signed 32BIT divide

if.cpu ARM.v7R

macro @xdiv name, x {
 verify.cpu ARM.v7R
 macro @#name a, b, c \{
  verify.r a, b, c
  @arm 0E700F010h or (x shl 20) or \
   (a shl 16) or b or (c shl 8)
 \}
}

@xdiv sdiv, 1
@xdiv udiv, 3

macro @test.div {
 @sdiv r1, r3, r5
 @sdiv r3, r5, r7
 @udiv r1, r3, r5
 @udiv r3, r5, r7
}

; emulate division if CPU < ARM.v7R.
; designed to be expanded once inside
; of a function with parameters...

else

macro @udiv {		; r0/r1
 local a, b
 @mov r2, r0
 @mov r3, r1
 @cmp r3, r2, lsr 1	; while
 a:			; divisor<(n*2)
  @mov:ls r3, r3, lsl 1 ; *2 if lower/same
  @cmp r3, r2, lsr 1	; /2
 @bls a
 @mov r0, 0
 b:
  @cmp r2, r3		; subtract only
  @sub:hs r2, r2, r3	; if carry (hs=cs)
  @adc r0, r0		; *2+C
  @mov r3, r3, lsr 1	; /2
  @cmp r3, r1		; while > divisor
 @bhs b
 @mov r1, r2		; remainder
}

macro @idiv {		; r0/r1
 @push @10
 @mov @10, r0
 @mov r2, r1
 @abs r0		; get absolute
 @abs r1		; values
 @udiv			; divide
 @tst r2, r2, lsl 31	; if sign
 @rsb:ne r0, 0		; negate
 @tst @10, @10, lsl 31
 @rsb:ne r0, 0
 @pop @10
}

macro @test.div {
 @udiv
 @sdiv
}

end if

; fast division by 10. r0/10

macro @div10 {
 @ldi r1, 1999999Ah    ; r1=((2^32)/10)+1
 @sub r0, r0, lsr 30   ; r0=r0-(r0>>>30)
 @umull r2, r0, r1, r0 ; r0=r1*r0
}

; with remainder in r1

macro @div10r {
 @mov r3, r0	       ; dividend
 @div10
 @mov r1, r0, lsl 1    ; multiply by 10:
 @add r1, r1, lsl 2    ; r1=(r0<<1)+(r1<<2)
 @rsb r1, r3	       ; r1=r3-r1
}

;;;;;;;;; BIT FIELD INSERT/CLEAR/EXTRACT ;;;;;;;;;

; BFI:  [CCCC/0111/110/MSBXX/RDXX/LSBXX/001/RNXX]
; BFC:  [CCCC/0111/110/MSBXX/RDXX/LSBXX/001/1111]
; SBFX: [CCCC/0111/101/MSBXX/RDXX/LSBXX/101/RNXX]
; UBFX: [CCCC/0111/111/MSBXX/RDXX/LSBXX/101/RNXX]

macro @bfv r, lsb, w {
 verify.cpu ARM.v6T2
 verify.r r
 verify.n lsb, 0, 31
 verify.n w, 1, 32-lsb
}

macro @bfx x, y, a, b, lsb, w {
 @bfv a, lsb, w
 @arm 0E7000000h or (x shl 21) or \
  (y shl 4)  or (a shl 12) or b or \
  (lsb shl 7) or ((lsb+w-1) shl 16)
}

macro @xbfx x, y, a, b, lsb, w {
 @bfv a, lsb, w
 @arm 0E7000000h or (x shl 21) or \
  (y shl 4) or (a shl 12) or b or \
  (lsb shl 7) or ((w-1) shl 16)
}

macro @bfi a, b, l, w
 { @bfx 110b, 001b, a, b, l, w }

macro @bfc r, l, w { @bfi r, 0Fh, l, w }

macro @sbfx a, b, l, w
 { @xbfx 101b, 101b, a, b, l, w }

macro @ubfx a, b, l, w
 { @xbfx 111b, 101b, a, b, l, w }

macro @test.bf {
 @bfi r1, r3, 4, 7
 @bfi r1, r3, 4, 7
 @sbfx r1, r3, 4, 7
 @ubfx r1, r3, 4, 7
}

;;;;;;;;;; SIGN/ZERO EXTEND/ADD/ROTATE ;;;;;;;;;;;

; sign or zero extend with optional add after
; and rotate before: SXT*, UXT*

; SXTB, SXTH, SXTAB, SXTAH, SXTB16, SXTAB16
; UXTB, UXTH, UXTAB, UXTAH, UXTB16, UXTAB16

; *B:    [CCCC/0110/1S10/1111/RDXX/RO/000111/RMXX]
; *H:    [CCCC/0110/1S11/1111/RDXX/RO/000111/RMXX]
; *AB:   [CCCC/0110/1S10/RNXX/RDXX/RO/000111/RMXX]
; *AH:   [CCCC/0110/1S11/RNXX/RDXX/RO/000111/RMXX]
; *B16:  [CCCC/0110/1S00/1111/RDXX/RO/000111/RMXX]
; *AB16: [CCCC/0110/1S00/RNXX/RDXX/RO/000111/RMXX]

macro verify.ror2 x, r {
 if ~ x eq
  syntax 0
  match sh n, x \{
   if sh eq ror & \
    ((n eq 0) | (n eq 8) | \
    (n eq 16) | (n eq 24))
    if n<>0
     r=n/8
    end if
   else
    'Shift must be ror 0/8/16/24'
   end if
   syntax 1
  \}
  if.syntax 0
   'Unexpected:' x
  end if
 end if
}

macro @xxt2 name, x {
 macro @#name a, b, c \{
  verify.cpu ARM.v6
  local r
  r=0
  verify.r a, b
  verify.ror2 c, r
  @arm 0E6000070h or (x shl 20) \
   or (1111b shl 16) or (a shl 12) \
   or (b) or (r shl 10)
 \}
}

macro @xxt3 name, x {
 macro @#name a, b, c, d \{
  verify.cpu ARM.v6
  local r
  r=0
  verify.r a, b, c
  verify.ror2 d, r
  @arm 0E6000070h or (x shl 20) \
   or (b shl 16) or (a shl 12) \
   or (c) or (r shl 10)
 \}
}

@xxt2 sxtb, 1010b
@xxt2 sxth, 1011b
@xxt2 sxtb16, 1000b
@xxt3 sxtab, 1010b
@xxt3 sxtah, 1011b
@xxt3 sxtab16, 1000b

@xxt2 uxtb, 1110b
@xxt2 uxth, 1111b
@xxt2 uxtb16, 1100b
@xxt3 uxtab, 1110b
@xxt3 uxtah, 1111b
@xxt3 uxtab16, 1100b

macro test.xxt {
 @sxtb r1, r3
 @sxtb r1, r3, ror 16
 @sxth r1, r3
 @sxth r1, r3, ror 16
 @sxtb16 r1, r3
 @sxtb16 r1, r3, ror 16
 @sxtab r1, r3, r7
 @sxtab r1, r3, r7, ror 16
 @sxtah r1, r3, r7
 @sxtah r1, r3, r7, ror 16
 @sxtab16 r1, r3, r7
 @sxtab16 r1, r3, r7, ror 16
 @uxtb r1, r3
 @uxtb r1, r3, ror 16
 @uxth r1, r3
 @uxth r1, r3, ror 16
 @uxtb16 r1, r3
 @uxtb16 r1, r3, ror 16
 @uxtab r1, r3, r7
 @uxtab r1, r3, r7, ror 16
 @uxtah r1, r3, r7
 @uxtah r1, r3, r7, ror 16
 @uxtab16 r1, r3, r7
 @uxtab16 r1, r3, r7, ror 16
}

;;;;;;;;;;;;;;;;;;;; PACKING ;;;;;;;;;;;;;;;;;;;;;

; 31-28   27-20 19-16 15-12 11-987/6/54/3210
; [CCCC/01101000/RNXX/RDXX/IIIIIII/T/01/RMXX]

; combine top/bottom 16BITs with optional
; shift before

macro @pkx name, t {
 macro @#name a, b, c, d \{
  local r
  r=0
  verify.cpu ARM.v6
  verify.r a, b, c
  if d eq
   if t=0
    @arm 0E6800010h or (a shl 12) or \
     (b shl 16) or (c)
   else
    @arm 0E6800010h or (a shl 12) or \
     (b) or (c shl 16)
   end if
  else
   syntax 0
   match sh n, d \\{
    if t=0
     if ?not sh eq lsl
      'Shift must be lsl'
     end if
     verify.n n, 1, 31
     @arm 0E6800010h or (a shl 12) or \
      (b shl 16) or (c) or (n shl 7)
    else
     if ?not sh eq asr
      'Shift must be asr'
     end if
     verify.n n, 1, 32
     @arm 0E6800050h or (a shl 12) or \
      (c) or (b shl 16) or (n shl 7)
    end if
    syntax 1
   \\}
   if.syntax 0
    'Shift expected:' d
   end if
  end if
 \}
}

@pkx pkhbt, 0
@pkx pkhtb, 1

macro @test.pkx {
 @pkhbt r2, r4, r7
 @pkhtb r2, r4, r7
 @pkhbt r2, r4, r7, lsl 8
 @pkhtb r2, r4, r7, asr 8
}

;;;;;;;;;;;;;;;; PACKED ARITHMETIC ;;;;;;;;;;;;;;;

; add/sub un/signed 2 16BIT or 4 8BIT with
; optional h/alve results after

;          31-28 27-20  19-16 15-12  98/7654/3210
; SADD8:  [CCCC/01100001/RNXX/RDXX/1111/1001/RMXX]
; SADD16: [CCCC/01100001/RNXX/RDXX/1111/0001/RMXX]
; SSUB8:  [CCCC/01100001/RNXX/RDXX/1111/1111/RMXX]
; SSUB16: [CCCC/01100001/RNXX/RDXX/1111/0111/RMXX]

; UADD8:  [CCCC/01100101/RNXX/RDXX/1111/1001/RMXX]
; UADD16: [CCCC/01100101/RNXX/RDXX/1111/0001/RMXX]
; USUB8:  [CCCC/01100101/RNXX/RDXX/1111/1111/RMXX]
; USUB16: [CCCC/01100101/RNXX/RDXX/1111/0111/RMXX]

; SHADD8:  [CCCC/01100011/RNXX/RDXX/1111/1001/RMXX]
; SHADD16: [CCCC/01100011/RNXX/RDXX/1111/0001/RMXX]
; SHSUB8:  [CCCC/01100011/RNXX/RDXX/1111/1111/RMXX]
; SHSUB16: [CCCC/01100011/RNXX/RDXX/1111/0111/RMXX]

; UHADD8:  [CCCC/01100111/RNXX/RDXX/1111/1001/RMXX]
; UHADD16: [CCCC/01100111/RNXX/RDXX/1111/0001/RMXX]
; UHSUB8:  [CCCC/01100111/RNXX/RDXX/1111/1111/RMXX]
; UHSUB16: [CCCC/01100111/RNXX/RDXX/1111/0111/RMXX]

macro @xas [name, x, y] {
forward
 macro @#name a, b, c \{
  verify.cpu ARM.v6
  verify.r a, b, c
  @arm1 al, 0110b, x, b, a, 1111b, y, c
 \}
}

@xas sadd8,1,9, sadd16,1,1,\
 ssub8,1,0Fh, ssub16,1,7, uadd8,5,9,\
 uadd16,5,1, usub8,5,0Fh, usub16,5,7,\
 shadd8,3,9, shadd16,3,1, shsub8,3,0Fh,\
 shsub16,3,7, uhadd8,7,9, uhadd16,7,1,\
 uhsub8,7,0Fh, uhsub16,7,7

; un/signed add+subtract or subtract+add
; with exchange and optional halving

; SASX, SSAX, SHASX, SHSAX
; UASX, USAX, UHASX, UHSAX

@xas sasx,1,3, ssax,1,5, shasx,3,3,\
 shsax,3,5, uasx,5,3, usax,5,5,\
 uhasx,7,3, uhsax,7,5

; unsigned sum of absolute differences with
; optional accumulate

macro @usad8 a, b, c {
 verify.cpu ARM.v6
 verify.r a, b, c
 @arm1 al, 0111b, 1000b, a, 1111b, c, 1, b
}

macro @usada8 a, b, c, d {
 verify.cpu ARM.v6
 verify.r a, b, c, d
 @arm1 al, 0111b, 1000b, a, d, c, 1, b
}

; SSAT16, USAT16

macro @xat [name, x, y] {
forward
 macro @#name a, b, c \{
  verify.cpu ARM.v6
  if b ?is.r
   'Operand 2 must be immediate'
  end if
  verify.r a, c
  if x eq 10
   @arm1 al, 0110b, x, b-1, a, 1111b, y, c
  else
   @arm1 al, 0110b, x, b, a, 1111b, y, c
  end if
 \}
}

@xat ssat16,10,3, usat16,0Eh,3

; QASX, QSAX, UQASX, UQSAX

@xas qasx,2,3, qsax,2,5, uqasx,6,3, uqsax,6,5

; QADD8, QADD16, QSUB8, QSUB16
; UQADD8, UQADD16, UQSUB8, UQSUB16

@xas qadd8,2,9, qadd16,2,1, qsub8,2,15,\
 qsub16,2,7, uqadd8,6,9, uqadd16,6,1,\
 uqsub8,6,15, uqsub16,6,7

macro @test.xat {
 @sadd8 r1, r3, r5
 @sadd16 r1, r3, r5
 @ssub8 r1, r3, r5
 @ssub16 r1, r3, r5
 @uadd8 r1, r3, r5
 @uadd16 r1, r3, r5
 @usub8 r1, r3, r5
 @usub16 r1, r3, r5
 @shadd8 r1, r3, r5
 @shadd16 r1, r3, r5
 @shsub8 r1, r3, r5
 @shsub16 r1, r3, r5
 @uhadd8 r1, r3, r5
 @uhadd16 r1, r3, r5
 @uhsub8 r1, r3, r5
 @uhsub16 r1, r3, r5
 @sasx r1, r3, r5
 @ssax r1, r3, r5
 @shasx r1, r3, r5
 @shsax r1, r3, r5
 @uasx r1, r3, r5
 @usax r1, r3, r5
 @uhasx r1, r3, r5
 @uhsax r1, r3, r5
 @usad8 r1, r3, r7
 @usada8 r1, r3, r5, r7
 @ssat16 r1, 8, r3
 @usat16 r1, 8, r3
 @qasx r1, r3, r5
 @qsax r1, r3, r5
 @uqasx r1, r3, r5
 @uqsax r1, r3, r5
 @qadd8 r1, r3, r5
 @qadd16 r1, r3, r5
 @qsub8 r1, r3, r5
 @qsub16 r1, r3, r5
 @uqadd8 r1, r3, r5
 @uqadd16 r1, r3, r5
 @uqsub8 r1, r3, r5
 @uqsub16 r1, r3, r5
}

;;;;;;;;;;;; SWAP, REVERSE BITS/BYTES ;;;;;;;;;;;;

; exchange register and memory

macro @swx o, a, b, c {
 verify.r a, b
 syntax 0
 match [r], c \{
  @arm o or (r shl 16) or (a shl 12) or b
  syntax 1
 \}
 verify
}

macro @swp a, b, c  { @swx 0E1000090h, a, b, c }
macro @swpb a, b, c { @swx 0E1400090h, a, b, c }

; reverse byte/bit order

macro @revx o, a, b {
 verify.cpu ARM.v6
 verify.r a, b
 @arm o or (a shl 12) or b
}

macro @rev a, b   { @revx 0E6BF0F30h, a, b }
macro @rev16 a, b { @revx 0E6BF0FB0h, a, b }
macro @revsh a, b { @revx 0E6FF0FB0h, a, b }

macro @rbit a, b  {
 verify.cpu ARM.v6T2
 verify.r a, b
 @revx 0E6FF0F30h, a, b
}

macro @test.swr {
 @swp r3, r5, [r7]
 @swpb r0, r1, [r2]
 @rev r5, r7
 @rev16 r5, r7
 @revsh r5, r7
 @rbit r5, r7
}

;;;;;;;;;;;;;;;;;;;;; STRING ;;;;;;;;;;;;;;;;;;;;;

; test string routines. inline "instrinsics"
; or expand once inside of function.
; parameters: r0-r3

; strlen(s) - get length, # characters

macro @strlen {
 local x
 @mov r2, r0
 x:
  @ldrb r1, [r0], 1 ; r1=*r0++
  @tst r1, r1	    ; until 0
 @bne x
 @sub r0, r2	    ; r0=r0-r2-1
 @dec r0	    ; return n
}

; strcpy(a, b) - standard copy with 0 after.
; return advanced address

macro @strcpy {
 local x
 x:
  @ldrb r2, [r1], 1 ; r2=*r1++
  @strb r2, [r0], 1 ; *r0++=r2
  @tst r2, r2	    ; until 0
 @bne x
 @dec r0
}

; strcat(a, b) - attach b to a

macro @strcat {
 @mov r3, r0
 @mov r4, r1
 @strlen
 @add r0, r3
 @mov r1, r4
 @strcpy
}

; strcmp(a, b) - lexical comparison

macro @strcmp {
 local x, y
 x:
  @ldrb r2, [r0], 1 ; r2=*r0++
  @ldrb r3, [r1], 1 ; r3=*r1++
  @cmp r2, r3	    ; while equal
  @bne y
  @orrs r2, r3	    ; and both nonzero
 @bne x
 y:
 @subs r0, r2, r3   ; return r0=r2-r3
}

; streq(a, b) - equal?

macro @streq {
 @strcmp
 @mov:eq r0, 1
 @mov:ne r0, 0
}

macro @test.strx {
 @strcat
 @streq
}