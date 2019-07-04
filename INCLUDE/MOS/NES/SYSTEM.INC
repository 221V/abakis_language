; $$$$$$$$$$$$$$$ Z6502 ASSEMBLER $$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ??????????????????? NES.INC ????????????????????

; Nintendo/NES...

format binary as 'NES'

NTSC=1 ; 0=PAL

; .NES header: 16 bytes

org $C000-16
db "NES", $1A,1,0,1,0
times 8 db 0

_begin: ; $C000 in memory, $10 in file

@0=0    ; first 8 zero page variables
@1=1
@2=2
@3=3
@4=4
@5=5
@6=6
@7=7

;;;;;;;;;;;;;;;;;;;;; SCREEN ;;;;;;;;;;;;;;;;;;;;;

SCREEN.W=256

if NTSC=1
  SCREEN.H=224
  SCREEN.MHZ=60
else
  SCREEN.H=240
  SCREEN.MHZ=50
end if

; PPU: Picture Processing Unit control
; registers. note: after accessing VRAM/
; $2007, PPU will auto-increment the
; VRAMA/$2006 address by 1 or 32 (based
; on bit #2 of PPU.CTRL/$2000)

PPU.CTRL   = $2000
PPU.MASK   = $2001
PPU.STATUS = $2002

VRAM.A     = $2006
VRAM       = $2007
PALETTE.A  = $3F00
PALETTE.B  = $3F10
PALETTE.M  = $3F20

SPRITE.A   = $2003
SPRITE.IO  = $2004
SPRITE.DMC = $4012
SPRITE.LE  = $4013
SPRITE.DMA = $4014
BG.SCROLL  = $2005

; pattern tables store 8x8/8x16 images that
; are 2BPP (4 colors) each

PATTERN.TABLE.0=0
PATTERN.TABLE.1=$1000

NAME.TABLE.W=(SCREEN.W/8)
NAME.TABLE.H=(SCREEN.H/8)

NAME.TABLE.N=$03C0
NAME.TABLE.0=$2000
NAME.TABLE.1=$2400
NAME.TABLE.2=$2800
NAME.TABLE.3=$2C00

ATTRIBUTE.TABLE.N=$40
ATTRIBUTE.TABLE.0=$23C0
ATTRIBUTE.TABLE.1=$27C0
ATTRIBUTE.TABLE.2=$2BC0
ATTRIBUTE.TABLE.3=$2FC0

;;;;;;;;;;;;;;;;;;;;; INPUT ;;;;;;;;;;;;;;;;;;;;;;

; joypads, directions, buttons

JOYPAD.1=$4016
JOYPAD.2=$4017

BUTTON.RIGHT  = 00000001b
BUTTON.LEFT   = 00000010b
BUTTON.DOWN   = 00000100b
BUTTON.UP     = 00001000b
BUTTON.START  = 00010000b
BUTTON.SELECT = 00100000b
BUTTON.B      = 01000000b
BUTTON.A      = 10000000b

;;;;;;;;;;;;;;;;;;;;; SOUND ;;;;;;;;;;;;;;;;;;;;;;

; PAPU: Pseudo Audio Processing Unit.
; F=frequency registers

PAPU.PULSE.1   = $4000
PAPU.PULSE.2   = $4001
PAPU.PULSE.F1  = $4002
PAPU.PULSE.F2  = $4003
PAPU.PULSE.21  = $4004
PAPU.PULSE.22  = $4005
PAPU.PULSE.2F1 = $4006
PAPU.PULSE.2F2 = $4007
PAPU.STATUS    = $4015

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; wait for vertical blank

macro vblank {
  local ..1
  ..1:
    lda PPU.STATUS
  bpl ..1
}

; write to VRAM from pointer @1,
; x # times

write.vram:
  ldy =0
  @@:
  lda (@1), y
  sta VRAM
  iny
  dex
  bne @b
rts

; initialize

macro setup {
  sei              ; disable IRQs
  cld              ; disable decimal mode
  ldx =$FF         ; initialize stack
  txs
  inx              ; x=0
  stx PPU.CTRL     ; zero these
  stx PPU.MASK
  vblank           ; wait
  lda =10000000b   ; set control register 0
  sta PPU.CTRL
  lda =$02         ; initialize video memory
  sta VRAM.A       ; $0200
  lda =$00
  sta VRAM.A
  jsr load.font    ; load font
  lda =$20         ; $2084
  sta VRAM.A
  lda =$84
  sta VRAM.A
}

macro set.palette {
  lda =$3F                ; palette start:
  sta VRAM.A              ; $3F00. write
  lda =$00                ; 2 bytes
  sta VRAM.A
  lda =(palette and 255)  ; palette address
  sta @1
  lda =(palette/256)
  sta @2
  ldx =4                  ; # colors
  jsr write.vram
}

; load font to VRAM. unfinished...

load.font:
  lda =(font and 255)
  sta @1
  lda =(font/256)
  sta @2
  ldx =3
  ldy =0
  @@:
  lda (@1), y
  sta VRAM
  iny
  lda (@1), y
  sta VRAM
  iny
  lda (@1), y
  sta VRAM
  iny
  lda (@1), y
  sta VRAM
  iny
  lda (@1), y
  sta VRAM
  iny
  lda (@1), y
  sta VRAM
  iny
  lda (@1), y
  sta VRAM
  iny
  lda (@1), y
  sta VRAM
  lda =0
  sta VRAM
  sta VRAM
  sta VRAM
  sta VRAM
  sta VRAM
  sta VRAM
  sta VRAM
  sta VRAM
  iny
  bne @b
  inc @2
  dex
  bne @b
rts

; unfinished...

macro draw.text {
  local ..1, ..2
  lda =(t and 255)
  sta @1
  lda =(t/256)
  sta @2
  ldy =0
  ..1:
  lda (@1), y
  beq ..2
  sta VRAM
  iny
  jmp ..1
  ..2:
}

; font. 768 bytes

font:
db $00,$00,$00,$00,$00,$00,$00,$00
db $18,$3C,$3C,$3C,$18,$18,$00,$18
db $6C,$6C,$6C,$00,$00,$00,$00,$00
db $6C,$6C,$FE,$6C,$FE,$6C,$6C,$00
db $30,$7C,$C0,$78,$0C,$F8,$30,$00
db $00,$C6,$CC,$18,$30,$66,$C6,$00
db $38,$6C,$38,$76,$DC,$CC,$76,$00
db $60,$60,$C0,$00,$00,$00,$00,$00
db $18,$30,$60,$60,$60,$30,$18,$00
db $60,$30,$18,$18,$18,$30,$60,$00
db $00,$66,$3C,$FF,$3C,$66,$00,$00
db $00,$30,$30,$FC,$30,$30,$00,$00
db $00,$00,$00,$00,$00,$30,$30,$60
db $00,$00,$00,$FC,$00,$00,$00,$00
db $00,$00,$00,$00,$00,$30,$30,$00
db $06,$0C,$18,$30,$60,$C0,$80,$00
db $38,$4C,$C6,$C6,$C6,$64,$38,$00
db $18,$38,$18,$18,$18,$18,$7E,$00
db $7C,$C6,$0E,$3C,$78,$E0,$FE,$00
db $7E,$0C,$18,$3C,$06,$C6,$7C,$00
db $1C,$3C,$6C,$CC,$FE,$0C,$0C,$00
db $FC,$C0,$FC,$06,$06,$C6,$7C,$00
db $3C,$60,$C0,$FC,$C6,$C6,$7C,$00
db $FE,$C6,$0C,$18,$30,$30,$30,$00
db $7C,$C6,$C6,$7C,$C6,$C6,$7C,$00
db $7C,$C6,$C6,$7E,$06,$0C,$78,$00
db $00,$30,$30,$00,$00,$30,$30,$00
db $00,$30,$30,$00,$00,$30,$30,$60
db $18,$30,$60,$C0,$60,$30,$18,$00
db $00,$00,$FC,$00,$00,$FC,$00,$00
db $60,$30,$18,$0C,$18,$30,$60,$00
db $78,$CC,$0C,$18,$30,$00,$30,$00
db $7C,$C6,$DE,$DE,$DE,$C0,$78,$00
db $38,$6C,$C6,$C6,$FE,$C6,$C6,$00
db $FC,$C6,$C6,$FC,$C6,$C6,$FC,$00
db $3C,$66,$C0,$C0,$C0,$66,$3C,$00
db $F8,$CC,$C6,$C6,$C6,$CC,$F8,$00
db $FE,$C0,$C0,$FC,$C0,$C0,$FE,$00
db $FE,$C0,$C0,$FC,$C0,$C0,$C0,$00
db $3E,$60,$C0,$CE,$C6,$66,$3E,$00
db $C6,$C6,$C6,$FE,$C6,$C6,$C6,$00
db $7E,$18,$18,$18,$18,$18,$7E,$00
db $1E,$06,$06,$06,$C6,$C6,$7C,$00
db $C6,$CC,$D8,$F0,$F8,$DC,$CE,$00
db $60,$60,$60,$60,$60,$60,$7E,$00
db $C6,$EE,$FE,$FE,$D6,$C6,$C6,$00
db $C6,$E6,$F6,$FE,$DE,$CE,$C6,$00
db $7C,$C6,$C6,$C6,$C6,$C6,$7C,$00
db $FC,$C6,$C6,$C6,$FC,$C0,$C0,$00
db $7C,$C6,$C6,$C6,$DE,$CC,$7A,$00
db $FC,$C6,$C6,$CE,$F8,$DC,$CE,$00
db $78,$CC,$C0,$7C,$06,$C6,$7C,$00
db $7E,$18,$18,$18,$18,$18,$18,$00
db $C6,$C6,$C6,$C6,$C6,$C6,$7C,$00
db $C6,$C6,$C6,$EE,$7C,$38,$10,$00
db $C6,$C6,$D6,$FE,$FE,$EE,$C6,$00
db $C6,$EE,$7C,$38,$7C,$EE,$C6,$00
db $66,$66,$66,$3C,$18,$18,$18,$00
db $FE,$0E,$1C,$38,$70,$E0,$FE,$00
db $78,$60,$60,$60,$60,$60,$78,$00
db $C0,$60,$30,$18,$0C,$06,$02,$00
db $78,$18,$18,$18,$18,$18,$78,$00
db $10,$38,$6C,$C6,$00,$00,$00,$00
db $00,$00,$00,$00,$00,$00,$00,$FF
db $30,$30,$18,$00,$00,$00,$00,$00
db $00,$00,$3C,$66,$66,$66,$3B,$00
db $60,$60,$7C,$66,$66,$66,$7C,$00
db $00,$00,$3E,$60,$60,$60,$3E,$00
db $06,$06,$3E,$66,$66,$66,$3E,$00
db $00,$00,$3C,$66,$7E,$60,$3E,$00
db $0E,$18,$18,$7E,$18,$18,$18,$00
db $00,$00,$3E,$66,$66,$3E,$06,$3C
db $60,$60,$60,$7C,$66,$66,$66,$00
db $00,$18,$00,$18,$18,$18,$18,$00
db $00,$06,$00,$06,$06,$06,$66,$3C
db $60,$60,$62,$64,$68,$7C,$66,$00
db $18,$18,$18,$18,$18,$18,$18,$00
db $00,$00,$76,$6B,$6B,$6B,$6B,$00
db $00,$00,$7C,$66,$66,$66,$66,$00
db $00,$00,$3C,$66,$66,$66,$3C,$00
db $00,$00,$7C,$66,$66,$7C,$60,$60
db $00,$00,$3E,$66,$66,$3E,$06,$06
db $00,$00,$6E,$70,$60,$60,$60,$00
db $00,$00,$3C,$40,$3C,$06,$7C,$00
db $30,$30,$FC,$30,$30,$30,$1C,$00
db $00,$00,$66,$66,$66,$66,$3C,$00
db $00,$00,$66,$66,$66,$24,$18,$00
db $00,$00,$63,$6B,$6B,$6B,$36,$00
db $00,$00,$63,$36,$1C,$36,$63,$00
db $00,$00,$66,$66,$2C,$18,$30,$60
db $00,$00,$7E,$0C,$18,$30,$7E,$00
db $1C,$30,$30,$E0,$30,$30,$1C,$00
db $18,$18,$18,$00,$18,$18,$18,$00
db $E0,$30,$30,$1C,$30,$30,$E0,$00
db $76,$DC,$00,$00,$00,$00,$00,$00
db $00,$10,$38,$6C,$C6,$C6,$FE,$00

; end main

macro endm {

  ; palette: $3F00-$3F1F

  palette:
  db $0F, $30, $0F, $0F

  ; interrupt vectors:

  ; * non-maskable: generated each refresh/
  ;   vblank (occurs 60/50 MHZ/times per
  ;   second on NTSC/PAL).

  ; * main: reset or power on.

  ; * irq/brk: standard system/supervisor

  _nmi:
    ; ...
  rti

  _irq:
    ; ...
  rti

  n=_begin+$3FFA
  while $<n
    db 0
  end while

  dw _nmi, main, _irq
}