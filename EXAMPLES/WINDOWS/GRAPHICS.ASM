; EXAMPLE TEMPLATE

WINDOW.W=720
WINDOW.H=512

include 'a.inc'
use interface

function test.gradients
  . r0=screen.w, r0/2, r1=screen.h, r1/2
  set.box box, 0, 0, r0, r1
  draw.shade box, DARK.BLUE, BLUE
  move.box.r box
  draw.shade box, RED, BLUE
  move.box.l box
  move.box.d box
  draw.shade box, GREEN, PINK
  move.box.r box
  draw.shade box, VIOLET, YELLOW
endf

; on.create: when the program begins

function on.create
  ; ...
endf

; on.draw: when the screen is drawn

function on.draw
  clear.screen BLACK
  test.gradients
endf

; on.key: when a key event occurs

function on.key
  if key.event='k'  ; keypress
    ; ...
    render
  end
endf

; on.mouse: when a mouse event occurs

function on.mouse
  if mouse.event='m' ; move
    ; ...
    render
  end
  if mouse.event='c' ; click
    ; ...
    render
  end
  if mouse.event='r' ; release
    ; ...
    render
  end
endf

; on.timer: every timer interval

function on.timer
  ; ...
endf

; on.exit: when the program ends

function on.exit
  ; ...
endf