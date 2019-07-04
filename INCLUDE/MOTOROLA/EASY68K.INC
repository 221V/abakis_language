;;;;;;;;;;;;;;;;;;; EASY 68K ;;;;;;;;;;;;;;;;;;;;;

; template for Easy68k assembler+simulator.
; search google. supports file I/O, mouse/key
; input and graphics

; to test, press F9 or click "Assemble" (right
; arrow). in the message box that appears,
; click Execute. in Sim68k, press F9 or "Run"

; warning: in some older assemblers, certain
; lines MUST be indented by 2+ spaces or it
; will display an error: "Invalid opcode"

; see Help > Simulator I/O and "Jimmy Mardells
; 68000 reference"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

START ORG $1000
    
  lea text, a1   
  move #14, d0  ; task=14, display text at a1
  trap #15
   
SIMHALT

; <functions here>

text dc.b 'EXAMPLE', 0

  END     START

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

START ORG $1000
  
  bsr test_memset
  bsr newl
  bsr test_memcpy
  bsr newl
  bsr test_strcpy
  bsr newl
  bsr test_strlen

SIMHALT

; push/pop all except a7/sp.
; movem.l=load/store multiple

pusha macro
  movem.l d0-d7/a0-a6, -(a7)
  endm

popa macro
  movem.l (a7)+, d0-d7/a0-a6
  endm

; memcpy(a0, a1, d0)

memcpy:   
  cmp.l #4, d0         ; d0<4?
  blo mc8
  movem.l d0, -(a7)    ; push d0
  lsr.l #2, d0         ; n/4
  mc32:
   move.l (a1)+, (a0)+ ; (u32) *a0++=*a1++
   sub.l #1, d0        ; until 0
  bne mc32
  movem.l (a7)+, d0    ; pop d0
  and.l #3, d0         ; modulo 4
  mc8:
   move.b (a1)+, (a0)+ ; (u8) *a0++=*a1++
   sub.l #1, d0        ; until 0
  bne mc8
  mce:
  rts                  ; return

; memset8(a0, d0, d1)
  
memset8:   
  cmp.l #0, d1         ; d0=0?
  beq ms8e
  ms8:
   move.b d0, (a0)+    ; (u8) *a0++=d0
   sub.l #1, d1        ; until 0
  bne ms8
  ms8e:
  rts                  ; return

; memset16(a0, d0, d1)
  
memset16:
  cmp.l #0, d1         ; d0=0?
  beq ms16e
  ms16:
   move.w d0, (a0)+    ; (u16) *a0++=d0
   sub.l #1, d1        ; until 0
  bne ms16
  ms16e:
  rts                  ; return

; memset32(a0, d0, d1)
  
memset32:
  cmp.l #0, d1         ; d0=0?
  beq ms32e
  ms32:
   move.l d0, (a0)+    ; (u32) *a0++=d0
   sub.l #1, d1        ; until 0
  bne ms32
  ms32e:
  rts                  ; return

; strlen(a0); return in d0

strlen:
  move.l #0, d0
  sl:
   move.b (a0)+, d1    ; (u8) d1=*a0++
   add.l #1, d0        ; d0++
   cmp.b #0, d1        ; until 0
  bne sl
  sub.l #1, d0         ; d0--
  rts                  ; return

; strcpy(a0, a1)

strcpy:
  move.b (a1)+, d0     ; (u8) d0=*a1++
  move.b d0, (a0)+     ; *a0++=d0
  cmp.b #0, d0         ; until 0   
  bne strcpy           ; copy
  rts                  ; return

; new line

newl:
  lea crlf, a1         ; new line
  move #14, d0         ; task=14, display text at a1
  trap #15             ; irq
  rts                  ; return

; testing...

test_memcpy:
  lea buffer, a0       ; a0=&buffer
  lea text, a1         ; a1=&text
  move.l #11, d0       ; d0=#
  bsr memcpy           ; copy
  lea buffer, a1       ; a1=&buffer
  move #14, d0         ; task=14, display text at a1
  trap #15             ; irq
  rts                  ; return

test_memset:
  lea buffer, a0       ; a0=&buffer ; initialize
  move.l #0, d0        ; d0=value
  move.l #128, d1      ; d1=#
  bsr memset8
  lea buffer, a0       ; a0=&buffer ; copy 7 A's
  move.l #$41, d0      ; d0=value
  move.l #7, d1        ; d1=#
  bsr memset8
  lea buffer, a1       ; a1=&buffer
  move #14, d0         ; task=14, display text at a1
  trap #15             ; irq
  rts                  ; return
  
test_strlen:
  lea text, a0         ; a0=&text
  bsr strlen           ; length
  move.l d0, d1        ; d1=d0
  move #3, d0          ; task=3, display number in d1
  trap #15             ; irq
  rts                  ; return

test_strcpy:
  lea buffer, a0       ; a0=&buffer
  lea text, a1         ; a1=&text
  bsr strcpy           ; copy
  lea buffer, a1       ; a1=&buffer
  move #14, d0         ; task=14, display text at a1
  trap #15             ; irq
  rts                  ; return

  ORG   (*+3)&-4       ; align

buffer ds.b 128
number dc.l 1234567
text dc.b 'EXAMPLE123', 0
crlf dc.b $0D, $0A, 0

  END     START

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; example function with parameters

      offset  4+4     ; parameters
sum   ds.l    1 
n2    ds.l    1 
n1    ds.l    1 
      org     *       ; end offset

adder:
  link a0, #0         ; create stack frame
  move.l d0, -(sp)    ; save d0
  move.l (n1,a0), d0  ; access n1 parameter
  add.l (n2,a0), d0   ; add n2 parameter
  move.l d0, (sum,a0) ; store result
  move.l (sp)+, d0    ; restore d0
  unlk a0             ; destroy stack frame
  rts