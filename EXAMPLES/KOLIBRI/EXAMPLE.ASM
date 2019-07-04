; EXAMPLE TEMPLATE

WINDOW.W=720
WINDOW.H=512

include 'a.inc'
use interface

text title.t='Hello, Kolibri!'

align FONT main.font='font'

align ICON32 my.i='x'

; on.create: when the program begins

function on.create
  set.font main.font
endf

; on.draw: when the screen is drawn

function on.draw
  clear.screen RED
  draw.text title.t, mouse.x, mouse.y
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