; $$$$$$$$$$$$$$$ M68K ASSEMBLER $$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ????????????????? GENESIS.INC ??????????????????

format binary as 'BIN'

GAME.NAME equ "MY GAME"

RAM.START=$FF0000
RAM.END=$FFFFFF

VDP.DATA=$C00000
VDP.CONTROL=$C00004
VDP.WRITE.PALETTES=$F0000000
VDP.WRITE.TILES=$40000000
VDP.WRITE.PLANE.A=$40000003
VDP.WRITE.SPRITE.TILES=$60000000
VDP.WRITE.SPRITE.TABLE=$60000003
VDP.READ.SPRITE.TABLE=$20000003
VDP.WRITE.HSCROLL=$50000003
VDP.WRITE.HSCROLL=$40000010

PAD.DATA.A=$A10003
PAD.DATA.B=$A10005
PAD.DATA.C=$A10007

PAD.CONTROL.A=$A10009
PAD.CONTROL.B=$A1000B
PAD.CONTROL.C=$A1000D

PAD.UP    = 0
PAD.DOWN  = 1
PAD.LEFT  = 2
PAD.RIGHT = 3
PAD.A     = 12
PAD.B     = 4
PAD.C     = 5
PAD.START = 13

PSG.OUTPUT=$C00011

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

rom.start:
dd $FFFE00, _main, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd hblank, xerror, vblank, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror
dd xerror, xerror, xerror, xerror

macro texta t, n, c {
 @@: db t
 times n-($-@b) db c
}

header:
texta "SEGA GENESIS", 16, ' '
texta "(C)SEGA 1992.SEP", 16, ' '
texta GAME.NAME, 48, ' '
texta GAME.NAME, 48, ' '
db "GM XXXXXXXX-XX"
dw rom.end-rom.start
db "J"
times 15 db $20
dd 0, rom.end, $FF0000, $FFFFFF
times 64 db $20
db "JUE"
times 13 db $20

purge texta

; __main:   ; $200 in memory
; jmp _main

; <includes, data, functions>

;;;;;;;;;;;;;;;;;;;;;; MAIN ;;;;;;;;;;;;;;;;;;;;;;

_main:

; initialize...

jmp _main

;;;;;;;;;;;;;;;; TIMING, RETRACE ;;;;;;;;;;;;;;;;;

hblank.n dd 0
vblank.n dd 0

wait.vblank.start:
 @@:
  move.w vdp.control, d0 ; d0=status
  and.w =1000b, d0       ; vblank
 bne @b
rts

wait.vblank.end:
 @@:
  move.w vdp.control, d0 ; d0=status
  and.w =1000b, d0       ; vblank
 beq @b
rts

wait.frames:             ; wait.frames(d0=n)
 move.l vblank.n, d1     ; d1=vblank count
 @@:
  move.l vblank.end, d2  ; d2=vblank end
  sub.l d1, d2           ; difference: d2-d1
  cmp.l d0, d2           ; until end
  bge @f
 jmp @b
 @@:
rts

;;;;;;;;;;;;;;;;;;;; GAMEPAD ;;;;;;;;;;;;;;;;;;;;;

read.pad.1:
 move.b PAD.DATA.A, d0   ; read high byte
 rol.w =8, d0            ; copy to d0
 move.b =$40, PAD.DATA.A ; write bit #7
 move.b PAD.DATA.A, d0   ; read low byte
 move.b =0, PAD.DATA.A   ; restore data port
rts

;;;;;;;;;;;;;;; INTERRUPT VECTORS ;;;;;;;;;;;;;;;;

xerror:
 stop =$2700 ; halt
rte

hblank:
 add.l =1, hblank.n
rte

vblank:
 add.l =1, vblank.n
rte

;;;;;;;;;;;;;;;;;;;; Z80 DATA ;;;;;;;;;;;;;;;;;;;;

align 4

z80.driver:
 db $c3,$46,$00,$00,$00,$00,$00,$00,$00,$00
 db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00
 db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00
 db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00
 db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00
 db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00
 db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00
 db $f3,$ed,$56,$31,$00,$20,$3a,$39,$00,$b7
 db $ca,$4c,$00,$21,$3a,$00,$11,$40,$00,$01
 db $06,$00,$ed,$b0,$3e,$00,$32,$39,$00,$3e
 db $b4,$32,$02,$40,$3e,$c0,$32,$03,$40,$3e
 db $2b,$32,$00,$40,$3e,$80,$32,$01,$40,$3a
 db $43,$00,$4f,$3a,$44,$00,$47,$3e,$06,$3d
 db $c2,$81,$00,$21,$00,$60,$3a,$41,$00,$07
 db $77,$3a,$42,$00,$77,$0f,$77,$0f,$77,$0f
 db $77,$0f,$77,$0f,$77,$0f,$77,$0f,$77,$3a
 db $40,$00,$6f,$3a,$41,$00,$f6,$80,$67,$3e
 db $2a,$32,$00,$40,$7e,$32,$01,$40,$21,$40
 db $00,$7e,$c6,$01,$77,$23,$7e,$ce,$00,$77
 db $23,$7e,$ce,$00,$77,$3a,$39,$00,$b7,$c2
 db $4c,$00,$0b,$78,$b1,$c2,$7f,$00,$3a,$45
 db $00,$b7,$ca,$4c,$00,$3d,$3a,$45,$00,$06
 db $ff,$0e,$ff,$c3,$7f,$00
.$=$-z80.driver

rom.end: