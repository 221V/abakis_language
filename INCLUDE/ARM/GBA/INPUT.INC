;;;;;;;;;;;;;;;;;;;;; INPUT ;;;;;;;;;;;;;;;;;;;;;;

align 4

integer keys, last.keys

INPUTR=IORAM+130h ; input control register

; <IGNORE>  RRRR RRRR
; FEDC BA98 7654 3210 ; 10BITs
; XXXX XXLR DULR SEBA

; KEY.A-KEY.L

powers KEY.*, A, B, SELECT, START,\
 RIGHT, LEFT, UP, DOWN, R, L

KEY.MASK=1111111111b ; 10BITs

function update.keys
 alias p=v1, q=v2,\
  mask=v3, ks=v4
 ldi q, !last.keys   ; save previous states
 ldi p, !keys        ; . q=last.keys, p=keys
 . (u32) *q=*p
 . p=INPUTR          ; input register
 . (u16) ks=*p, ~ks  ; ks=~(u16)(*4000130h)
 . mask=KEY.MASK     ; mask keys
 . ks=ks, ks&mask
 ldi p, !keys        ; . p=keys
 . (u32) *p=ks       ; store result
 . a1=ks
endf

; get key state

macro get.key name {
 update.keys
 . a1&KEY.#name
}

macro if.key name {
 get.key name
 if true
}                

; key logic:

; * press = down now, up before
; * release = up now, down before
; * held = down now, down before

;;;;;;;;;;;;;;;;; PORTABLE INPUT ;;;;;;;;;;;;;;;;;

align integer event.type

; keyboard

integer key, old.key, key.event
text keys.t(128)

numeric KEY.ESCAPE=27, KEY.SPACE=' ',\
 KEY.UP=0B2h, KEY.RIGHT=0B3h, KEY.DOWN=0B1h,\
 KEY.LEFT=0B0h, KEY.A='a', KEY.S='s',\
 KEY.D='d', KEY.W='w'

; mouse

integer mouse.event,\
 mouse.1, mouse.2, mouse.x, mouse.y,\
 mouse.drag, mouse.drag.x, mouse.drag.y,\
 mouse.drop, mouse.drop.x, mouse.drop.y,\
 old.mouse.1, old.mouse.2,\
 old.mouse.x, old.mouse.y

macro get.mouse.xy      { mint 37, 0 }
macro get.mouse.buttons { mint 37, 2 }
macro get.mouse.wheel   { mint 37, 3 }

function update.mouse
  . mouse.event=0
  . old.mouse.1=mouse.1, old.mouse.2=mouse.2
  . old.mouse.x=mouse.x, old.mouse.y=mouse.y

  get.mouse.xy
  . r1=r0, r1&0FFFFh, r0>>>16, r0&0FFFFh
  . mouse.x=r0, mouse.y=r1
  get.mouse.buttons
  . r1=r0, r1&1, mouse.1=r1
  . r1=r0, r1&2, mouse.2=r1

  if mouse.1
    if not old.mouse.1, mouse.event='c'
      . mouse.drop=NO
      callf on.mouse
      return 1
    end
  else.if old.mouse.1, mouse.event='r'
    callf on.mouse
    if mouse.drag
      . mouse.drop=YES,\
      mouse.drop.x=mouse.x,\
      mouse.drop.y=mouse.y,\
      mouse.drag=NO
      . mouse.event='d'
      callf on.mouse
    end
    return 1
  end
  . r0=mouse.x, r1=old.mouse.x
  if r0<>r1
    .mouse.move:
    . mouse.event='m'
    callf on.mouse
    if mouse.1
      if not mouse.drag
        . mouse.drag=YES,\
        mouse.drag.x=mouse.x,\
        mouse.drag.y=mouse.y
      end
    else
      . mouse.drop=NO
    end
    return 1
  else
    . r0=mouse.y, r1=old.mouse.y
    if r0<>r1
      go .mouse.move
    end
  end
endf 0

align

integer window.x, window.y

function select.box, box
  . r0=mouse.x, r1=mouse.y
  . r0-window.x, r1-window.y
  callf point.inside, box, r0, r1
endf

macro if.select box { !if select.box, box }
macro else.if.select box
 { !else.if select.box, box }   