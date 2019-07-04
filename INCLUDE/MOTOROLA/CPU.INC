; $$$$$$$$$$$$$$$ M68K ASSEMBLER $$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? 68000.INC ???????????????????

; UNFINISHED, UNORGANIZED...

; some experimental 68k...

; warning: this CPU is confusing due to its
; at&t syntax ("opcode source, destiny",
; operands reversed from a=b to b=a!),
; high-endian * byte order, numerous
; addressing modes, direct numbers are memory
; instead of immediate, () is optional in
; some 68k assemblers to be consistent
; with (address) modes but causes an error in
; other assemblers (() is required by
; these macros. * high-endian wouldn't be
; a problem if it were standard)

; it's hard to find a 68k machine code reference

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 32BIT internally with 16BIT data bus and
; 24BIT (16MB) address bus

; data registers: d0-d7
; address registers: a0-a7
; stack=a7, status=sr, flags are
; rightmost 5 BITs: XNZVC
; suffixes: .b=8, .w=16, .l=32
; conditions: cc, cs, eq, ne, ge, gt,
; hi, le, ls, lt, mi, pl, vc, vs

; * prefix immediate values with = instead of #
; * () is required for memory, not optional

; some basic instructions

; nop   - no operation

; lea   - load effective address
; pea   - push effective address

; move  - copy general register (d0-d7)
; movea - copy address register (a0-a7)
; movem - load/store multiple to/from memory
; movep - load/store 8BIT peripheral unit
; ext   - sign extend

; add   - add
; sub   - subtract
; muls  - multiply signed
; mulu  - multiply unsigned
; divs  - divide signed
; divu  - divide unsigned

; asr   - signed shift right
; lsl   - unsigned shift left
; lsr   - unsigned shift right
; rol   - rotate left
; ror   - rotate right

; bitwise: and, or, eor, neg, not,
; tst, btst

; cmp  - compare
; b/c  - branch if condition
; bra  - branch always
; bsr  - branch to subroutine
; db/c - decrement and branch
; jmp  - unconditional
; jsr  - jump to subroutine

; link - create stack frame
; unlk - destroy stack frame

; rtd - return and deallocate
; rte - return from exception
; rtr - return and restore
; rts - return from subroutine

; trap  - software interrupt
; trapv - if overflow

; reset - restore periphals
; stop  - wait for interrupt

;;;;;;;;;;;;;;;;;;; REGISTERS ;;;;;;;;;;;;;;;;;;;;

d0 fix 0
d1 fix 1
d2 fix 2
d3 fix 3
d4 fix 4
d5 fix 5
d6 fix 6
d7 fix 7

a0 fix 0
a1 fix 1
a2 fix 2
a3 fix 3
a4 fix 4
a5 fix 5
a6 fix 6
a7 fix 7

;;;;;;;;;;;;;;;; ADDRESSING MODES ;;;;;;;;;;;;;;;;

macro mode.68k m, oa, ob, [p] {
 common
  m=000b                     ; mode
  define ?s 0
  define ?t 0
  match a=,b, ?s p \{        ; 2 operands
    match =0 (x)+, ?s a \\{ ; (a)+
      assert x in \
       <a0,a1,a2,a3,a4,a5,a6,a7>
      m=011b
      oa=x
      define ?s 1
    \\}
    match =0 (y)+, ?t b \\{ ; (b)+
      assert y in \
       <a0,a1,a2,a3,a4,a5,a6,a7>
      m=011b
      ob=y
      define ?t 1
    \\}
    match =0 -(x), ?s a \\{ ; -(a)
      assert ar in \
       <a0,a1,a2,a3,a4,a5,a6,a7>
      m=100b
      oa=x
      define ?s 1
    \\}
    match =0 -(y), ?t b \\{ ; -(b)
      assert y in \
       <a0,a1,a2,a3,a4,a5,a6,a7>
      m=100b
      ob=y
      define ?t 1
    \\}
    match =0 (x), ?s a \\{   ; (a)
      assert x in \
       <a0,a1,a2,a3,a4,a5,a6,a7>
      m=010b
      oa=x
      define ?s 1
    \\}
    match =0 (y), ?t b \\{   ; (b)
      assert y in \
       <a0,a1,a2,a3,a4,a5,a6,a7>
      m=010b
      ob=y
      define ?t 1
    \\}
  \}
  ; ...
}

;  match (a)+=,(b)+, p \{     ; (ar)+, (ar)+
;    if ~ a in \              ; both must be ar
;      <a0,a1,a2,a3,\
;      a4,a5,a6,a7> | \
;      ~ b in <a0,a1,a2,a3,\
;      a4,a5,a6,a7>
;      'Syntax error'
;    end if
;    m=011b
;    define ?s 1
;  \}
;  match (a)+=,(b)+, p \{     ; (ar)+, (ar)+
;    if ~ a in \              ; both must be ar
;      <a0,a1,a2,a3,\
;      a4,a5,a6,a7> | \
;      ~ b in <a0,a1,a2,a3,\
;      a4,a5,a6,a7>
;      'Syntax error'
;    end if
;    m=011b
;    define ?s 1
;  \}

;    if a in \
;      <a0,a1,a2,a3,\
;      a4,a5,a6,a7>
;      if b in \              ; ar
;        <a0,a1,a2,a3,\
;        a4,a5,a6,a7>
;        m=001b
;        define ?s 1
;      end if
;    end if


;    if a in <d0,d1,d2,d3,\
;      d4,d5,d6,d7>
;      if b in \              ; dr, dr
;        <d0,d1,d2,d3,\
;        d4,d5,d6,d7>
;        m=000b
;        define ?s 1
;      else if b in \         ; dr, ar
;        <a0,a1,a2,a3,\
;        a4,a5,a6,a7>
;        m=001b
;        define ?s 1
;      end if
;    else if a in \           ; ar, ?
;      <a0,a1,a2,a3,\
;      a4,a5,a6,a7>
;      if b in \              ; ar, dr
;        <d0,d1,d2,d3,\
;        d4,d5,d6,d7>
;        dh16 (mask or (b shl 9)) or \
;         1000b or a
;        define ?s 1
;
;      else if b in \         ; ar, ar
;        <a0,a1,a2,a3,\
;        a4,a5,a6,a7>
;        dh16 (mask or (b shl 9)) or \
;         1000000b or (a or 1000b)
;        define ?s 1
;      end if
;    end if

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; write high endian

macro dh16 i {
 dw ((i and $FF) shl 8) or \
  ((i and $FF00) shr 8)
}

macro move.l [p] {
 common
  local mask
  mask=$2000
  define ?s 0
  match =0 a=,b, ?s p \{     ; ?, ?
    if a in <d0,d1,d2,d3,\
      d4,d5,d6,d7>
      if b in \              ; dr, dr
        <d0,d1,d2,d3,\
        d4,d5,d6,d7>
        dh16 (mask or (b shl 9)) or a
        define ?s 1
      else if b in \         ; dr, ar
        <a0,a1,a2,a3,\
        a4,a5,a6,a7>
        dh16 (mask or (b shl 9)) or \
         1000000b or a
        define ?s 1
      end if
    else if a in \           ; ar, ?
      <a0,a1,a2,a3,\
      a4,a5,a6,a7>
      if b in \              ; ar, dr
        <d0,d1,d2,d3,\
        d4,d5,d6,d7>
        dh16 (mask or (b shl 9)) or \
         1000b or a
        define ?s 1
      else if b in \         ; ar, ar
        <a0,a1,a2,a3,\
        a4,a5,a6,a7>
        dh16 (mask or (b shl 9)) or \
         1000000b or (a or 1000b)
        define ?s 1
      end if
    end if
  \}
  if ?s eq 0
    'Syntax error: ' name
  end if
}

macro nop { db $4E, $71 }

macro trap i { db $4E, $40 or i }

macro rts { db $4E, $75 }
macro rte { db $4E, $73 }

macro stop [p] {}

;;;;;;;;;;;;;;;;;;;;; BRANCH ;;;;;;;;;;;;;;;;;;;;;

macro jmp [p] {}
macro beq [p] {}
macro bne [p] {}
macro bge [p] {}

;;;;;;;;;;;;;;;;;;;;; MOVE.X ;;;;;;;;;;;;;;;;;;;;;

macro move.x name, o, [p] {
 common
  define ?s 0
  match ==i=,r, p \{         ; =i16, dr
    if r in \
      <a0,a1,a2,a3,\
      a4,a5,a6,a7>
      'Invalid operand'
    end if
    db o or (r shl 1)
    db $3C
    if name eq move.l
      db (i shr 24) and $FF
      db (i shr 16) and $FF
      db (i shr 8) and $FF
      db (i and $FF)
    else
      db (i shr 8) and $FF
      db (i and $FF)
      define ?s 1
    end if
  \}
  match =0 a=,b, ?s p \{     ; ?, ?
    if a in <d0,d1,d2,d3,\
      d4,d5,d6,d7>
      if b in \              ; dr, dr
        <d0,d1,d2,d3,\
        d4,d5,d6,d7>
        db o or (b shl 1)
        db a
        define ?s 1
      else if b in \         ; dr, ar
        <a0,a1,a2,a3,\
        a4,a5,a6,a7>
        if name eq move.b
          'Invalid operand'
        end if
        db o or (b shl 1)
        db (4 shl 4) or a
        define ?s 1
      end if
    else if a in \           ; ar, ?
      <a0,a1,a2,a3,\
      a4,a5,a6,a7>
      if name eq move.b
        'Invalid operand'
      end if
      if b in \              ; ar, dr
        <d0,d1,d2,d3,\
        d4,d5,d6,d7>
        db o or (b shl 1)
        db a or 8
        define ?s 1
      else if b in \         ; ar, ar
        <a0,a1,a2,a3,\
        a4,a5,a6,a7>
        db o or (b shl 1)
        db (4 shl 4) \
         or (a or 8)
        define ?s 1
      end if
    end if
  \}
  if ?s eq 0
    'Syntax error: ' name
  end if
}

macro move.b [p] { common move.x move.b, $10, p }
macro move.w [p] { common move.x move.w, $30, p }
;macro move.l [p] { common move.x move.l, $20, p }

macro test.move.x {
  move.b =$12, d7       ; d7=i8 (i,d)
  move.w =$1234, d7     ; d7=i16 (i,d)
  move.l =$12345678, d7 ; d7=i32 (i,d)

  move.b d2, d1  ; d1=d2 (d,d)
  move.b d4, d3  ; d3=d4
  move.b d6, d5  ; d5=d6

  move.w d2, d1  ; d1=d2 (d,d)
  move.w d4, d3  ; d3=d4
  move.w d6, d5  ; d5=d6
  move.w d2, a1  ; a1=d2 (d,a)
  move.w d4, a3  ; a3=d4
  move.w d6, a5  ; a5=d6
  move.w a2, d1  ; d1=a2 (a,d)
  move.w a4, d3  ; d3=a4
  move.w a6, d5  ; d5=a6
  move.w a2, a1  ; a1=a2 (a,a)
  move.w a4, a3  ; a3=a4
  move.w a6, a5  ; a5=a6

  move.l d2, d1  ; d1=d2 (d,d)
  move.l d4, d3  ; d3=d4
  move.l d6, d5  ; d5=d6
  move.l d2, a1  ; a1=d2 (d,a)
  move.l d4, a3  ; a3=d4
  move.l d6, a5  ; a5=d6
  move.l a2, d1  ; d1=a2 (a,d)
  move.l a4, d3  ; d3=a4
  move.l a6, d5  ; d5=a6
  move.l a2, a1  ; a1=a2 (a,a)
  move.l a4, a3  ; a3=a4
  move.l a6, a5  ; a5=a6

  ; move.x (a2), d1     ; d1=*a2
  ; move.x d2, (a1)     ; *a1=d2
  ; move.x (a2)+, d1    ; d1=*a2++
  ; move.x d2, -(a1)    ; *a1--=d2
  ; move.x (a2), (a1)   ; *a1=*a2
  ; move.x (a2)+, (a1)+ ; *a1++=*a2++
}

;;;;;;;;;;;;;;;;;;;;;; PUSH ;;;;;;;;;;;;;;;;;;;;;;

; syntax: push d0, d1, d2
; output: movem.l d0/d1/d2, -(a7)

macro push [p] {}

; syntax: push d0, d1, d2
; output: movem.l (a7)+, d0/d1/d2

macro pop [p] {}

;;;;;;;;;;;;;;;;;;;;;; ADD ;;;;;;;;;;;;;;;;;;;;;;;

macro add.l [p] {}
macro sub.l [p] {}
macro and.w [p] {}
macro rol.w [p] {}
macro subx.l [p] {}
macro cmp.l [p] {}