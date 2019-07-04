; $$$$$$$$$$$$$$$ Z6502 ASSEMBLER $$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? 6502.INC ????????????????????

;                   MMM"""AMV
;                   M'   AMV
;                   '   AMV
;                     AMV   ,
;     .6*"          AMV   ,M   MOS
;   ,M'            AMVmmmmMM
;  ,Mbmmm.    M******  ,pP""Yq.   pd*"*b.
;  6M'  `Mb. .M       6W'    `Wb (O)   j8
;  MI     M8 |bMMAg.  8M      M8     ,;j9
;  WM.   ,M9      `Mb YA.    ,A9  ,-='
;   WMbmmd9        jM  `Ybmmd9'  Ammmmmmm
;           (O)  ,M9
;            6mmm9

; a               - accumulator
; x, y            - general
; status          - flags: SV_BDIZC
; program counter
; stack register

; $00-$FF     - zero page memory
; $0100-$01FF - stack, 1K

; in some emulators:

; $FE         - random byte
; $FF         - last ASCII key
; $0200-$05FF - screen, 32x32

; syntax is standard except replace # with =
; for immediate operands:

; lda =$7F  ; a=i (immediate, prefix with =)
; lda $7F   ; a=[$7F] (zero page)
; lda $7FAA ; a=[$7FAA] (absolute)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

macro verify a {
  if ?s eq 0
    'Error: ' a
  end if
}

macro verify.i i {
  if ~ i eqtype 0
    'Number expected:' i
  end if
}

macro verify.n n, min, max {
  if n<min | n>max
    'Number exceeds range:' min-max
  end if
} 

macro verify.u8 n  { verify.n n, 0, 255 } 
macro verify.i8 n  { verify.n n, -128, 127 }
macro verify.u16 n { verify.n n, 0, 65535 } 

;;;;;;;;;;;;;; ONE-BYTE INSTRUCTIONS ;;;;;;;;;;;;;

macro brk { db $00 } ; non-maskable interrupt
macro clc { db $18 } ; clear carry
macro cld { db $D8 } ; clear decimal
macro cli { db $58 } ; clear interrupt
macro clv { db $B8 } ; clear overflow
macro dex { db $CA } ; x--
macro dey { db $88 } ; y--
macro inx { db $E8 } ; x++
macro iny { db $C8 } ; y++
macro nop { db $EA } ; no operation
macro pha { db $48 } ; push a
macro php { db $08 } ; push status
macro pla { db $68 } ; pull/pop a
macro plp { db $28 } ; pull/pop status
macro rti { db $40 } ; return from interrupt
macro rts { db $60 } ; return from subroutine
macro sec { db $38 } ; set carry
macro sed { db $F8 } ; set decimal
macro sei { db $78 } ; set interrupt
macro tax { db $AA } ; x=a
macro tay { db $A8 } ; y=a
macro tsx { db $BA } ; x=s
macro txa { db $8A } ; a=x
macro txs { db $9A } ; s=x
macro tya { db $98 } ; a=y

;;;;;;;;;;;;;;;;;;;;; BRANCH ;;;;;;;;;;;;;;;;;;;;;

macro jmp [a] {
 common
 define ?s 0
  match (x), a \{ ; indirect
    db $6C
    dw x
    define ?s 1
  \}
  match =0 x,\    ; absolute
   ?s a \{
    db $4C
    dw x
  \}
}

macro bxx o, a
 { db o, (255-($-a)) and $FF }

macro beq a { bxx $F0, a }
macro bne a { bxx $D0, a }
macro bcc a { bxx $90, a }
macro bcs a { bxx $B0, a }
macro bvc a { bxx $50, a }
macro bvs a { bxx $70, a }
macro bmi a { bxx $30, a }
macro bpl a { bxx $10, a }

; jmp to subroutine

macro jsr a {
  db $20
  dw a         ; absolute
}

;;;;;;;; LDA, STA, ADC, SBC, ORA, CMP, ETC ;;;;;;;

; opc =$44     ; immediate (=i)
; opc $AA      ; zero page
; opc $7F, x   ; zero page, x
; opc $4FFF    ; absolute
; opc $88BB, x ; absolute, x
; opc $24EE, y ; absolute, y
; opc ($AC, x) ; (indirect, x)
; opc ($DC), y ; (indirect), y

macro o1.6502 name, aaa, [p] {
 common
  local i, mode, size
  define ?s 0
  match =0 ==a, ?s p \{      ; immediate
    i=a
    mode=010b
    size=1
    if name eq sta
      'Error: ' a
    end if
    define ?s 1
  \}
  match =0 (a=,=x), ?s p \{  ; (indirect, x)
    i=a
    mode=000b
    size=1
    define ?s 1
  \}
  match =0 (a)=,=y, ?s p \{  ; (indirect), y
    i=a
    mode=100b
    size=1
    define ?s 1
  \}
  match =0 a=,b, ?s p \{     ; ?, ?
    i=a
    verify.i a               ; i, ?
    verify.u16 a
    if b eq x                ; i, x
      if a<256               ; zero page, x
        mode=101b
        size=1
      else                   ; absolute, x
        mode=111b
        size=2
      end if
    else if b eq y           ; absolute, y
      mode=110b
      size=2
    else
      'Error: ' b
    end if
    define ?s 1
  \}
  match =0 a, ?s p \{
    i=a
    verify.u16 a
    if a<256                 ; zero page
      mode=001b
      size=1
    else                     ; absolute
      mode=011b
      size=2
    end if
    define ?s 1
  \}
  verify name
  db (aaa shl 5) or \
   (mode shl 2) or 1
  if size=1
    db i
  else
    dw i
  end if
}

macro ora [p] { common o1.6502 ora, 000b, p }
macro and [p] { common o1.6502 and, 001b, p }
macro eor [p] { common o1.6502 eor, 010b, p }
macro adc [p] { common o1.6502 adc, 011b, p }
macro sta [p] { common o1.6502 sta, 100b, p }
macro lda [p] { common o1.6502 lda, 101b, p }
macro cmp [p] { common o1.6502 cmp, 110b, p }
macro sbc [p] { common o1.6502 sbc, 111b, p }

;;;;;;;;;;;;;;;;;;;; INC, DEC ;;;;;;;;;;;;;;;;;;;;

; opc $80      ; zero page
; opc $80, x   ; zero page, x
; opc $A000    ; absolute
; opc $8000, x ; absolute, x

macro o2.6502.a name, aaa, [p] {
 common
  local i, mode, size
  define ?s 0
  match =0 a=,=x,\      ; ?, x
   ?s p \{
    i=a
    if i<256
      size=1
      mode=101b
    else
      size=2
      mode=111b
    end if
    define ?s 1
  \}
  match =0 a, ?s p \{   ; x
    i=a
    if i<256
      size=1
      mode=001b
    else
      size=2
      mode=011b
    end if
    define ?s 1
  \}
  verify name
  verify.u16 i
  db (aaa shl 5) or \
   (mode shl 2) or 2
  if size=1             ; zero page
    db i
  else                  ; absolute
    dw i
  end if
}

macro inc [p] { common o2.6502.a inc, 111b, p }
macro dec [p] { common o2.6502.a dec, 110b, p }

;;;;;;;;;;;;;;; ASL, LSR, ROL, ROR ;;;;;;;;;;;;;;;

; opc          ; accumulator
; opc $80      ; zero page
; opc $80, x   ; zero page, x
; opc $A000    ; absolute
; opc $8000, x ; absolute, x

macro o2.6502.b name, aaa, [p] {
 common
  local i, mode, size
  define ?s 0
  match =0 a=,=x,\      ; ?, x
   ?s p \{
    i=a
    if i<256
      size=1
      mode=101b
    else
      size=2
      mode=111b
    end if
    define ?s 1
  \}
  match =0 _a, ?s p \{  ; x
    if _a eq a          ; accumulator
      size=1
      mode=010b
    else
      i=_a
      if i<256
        size=1
        mode=001b
      else
        size=2
        mode=011b
      end if
    end if
    define ?s 1
  \}
  if ?s eq 0            ; accumulator
    size=1
    mode=010b
    define ?s 1
  end if
  verify name
  db (aaa shl 5) or \
   (mode shl 2) or 2
  if mode<>010b         ; not accumulator?
    verify.u16 i
    if size=1           ; zero page
      db i
    else                ; absolute
      dw i
    end if
  end if
}

macro asl [p] { common o2.6502.b asl, 000b, p }
macro rol [p] { common o2.6502.b rol, 001b, p }
macro lsr [p] { common o2.6502.b lsr, 010b, p }
macro ror [p] { common o2.6502.b ror, 011b, p }

;;;;;;;;;;;;;;;;;;;; STX, LDX ;;;;;;;;;;;;;;;;;;;;

; opc =$44     ; immediate (=i) *
; opc $80      ; zero page
; opc $80, y   ; zero page, y
; opc $AAAA    ; absolute
; opc $AAAA, y ; absolute, y *

; * ldx only

macro o2.6502.c name, aaa, [p] {
 common
  local i, mode, size
  define ?s 0
  match =0 ==a, ?s p \{      ; immediate
    i=a
    mode=000b
    size=1
    if name eq stx
      'Error: ' a
    end if
    define ?s 1
  \}
  match =0 a=,b, ?s p \{     ; ?, ?
    i=a
    verify.i a               ; i, ?
    verify.u16 a
    if b eq y                ; i, y
      if a<256               ; zero page, y
        mode=101b
        size=1
      else                   ; absolute, y
        mode=111b
        size=2
        if name eq stx
          'Error: ' a
        end if
      end if
    else
      'Error: ' b
    end if
    define ?s 1
  \}
  match =0 a, ?s p \{
    i=a
    verify.i i
    verify.u16 i
    if i<256                 ; zero page
      mode=001b
      size=1
    else
      mode=011b              ; absolute
      size=2
    end if
    define ?s 1
  \}
  verify name
  db (aaa shl 5) or \
   (mode shl 2) or 2
  if size=1
    db i
  else
    dw i
  end if
}

macro stx [p] { common o2.6502.c stx, 100b, p }
macro ldx [p] { common o2.6502.c ldx, 101b, p }

;;;;;;;;;;;;;;;;;;;; STY, LDY ;;;;;;;;;;;;;;;;;;;;

; opc =$44     ; immediate (=i)
; opc $AA      ; zero page
; opc $7F, x   ; zero page, x
; opc $4FFF    ; absolute
; opc $88BB, x ; absolute, x

macro o0.6502 name, aaa, [p] {
 common
  local i, mode, size
  define ?s 0
  match =0 ==a, ?s p \{      ; immediate
    i=a
    mode=000b
    size=1
    if name eq sty
      'Error: ' a
    end if
    define ?s 1
  \}
  match =0 a=,b, ?s p \{     ; ?, ?
    i=a
    verify.i a               ; i, ?
    verify.u16 a
    if b eq x                ; i, x
      if a<256               ; zero page, x
        mode=101b
        size=1
      else                   ; absolute, x
        mode=111b
        size=2
      end if
    else
      'Error: ' b
    end if
    define ?s 1
  \}
  match =0 a, ?s p \{
    i=a
    verify.u16 a
    if a<256                 ; zero page
      mode=001b
      size=1
    else                     ; absolute
      mode=011b
      size=2
    end if
    define ?s 1
  \}
  verify name
  db (aaa shl 5) or \
   (mode shl 2)
  if size=1
    db i
  else
    dw i
  end if
}

macro sty [p] { common o0.6502 sty, 100b, p }
macro ldy [p] { common o0.6502 ldy, 101b, p }

;;;;;;;;;;;;;;;;;;;; CPX, CPY ;;;;;;;;;;;;;;;;;;;;

; opc =$44     ; immediate (=i)
; opc $AA      ; zero page
; opc $4FFF    ; absolute

macro o0.6502.a name, aaa, [p] {
 common
  local i, mode, size
  define ?s 0
  match =0 ==a, ?s p \{      ; immediate
    i=a
    mode=000b
    size=1
    define ?s 1
  \}
  match =0 a, ?s p \{
    i=a
    verify.u16 a
    if a<256                 ; zero page
      mode=001b
      size=1
    else                     ; absolute
      mode=011b
      size=2
    end if
    define ?s 1
  \}
  verify name
  db (aaa shl 5) or \
   (mode shl 2)
  if size=1
    db i
  else
    dw i
  end if
}

macro cpx [p] { common o0.6502.a cpx, 111b, p }
macro cpy [p] { common o0.6502.a cpy, 110b, p }

;;;;;;;;;;;;;;;;;;;;;;; BIT ;;;;;;;;;;;;;;;;;;;;;;

macro bit a {
  define ?s 0
  verify.u16 a
  if a eqtype 0
    if a<256    ; zero page
      db $24
      db a
    else        ; absolute
      db $2C
      dw a
    end if
    define ?s 1
  end if
  verify bit
}