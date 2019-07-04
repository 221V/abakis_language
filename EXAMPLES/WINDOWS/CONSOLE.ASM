; ABAKIS EXAMPLE

WINDOW.W=720
WINDOW.H=512

include 'a.inc'
use interface

function on.create
  ; ...
endf

function draw.window
  locals x, y, w, h
  clear.screen BLACK
  . x=0, y=0, w=screen.w, w--
  . h=screen.h, h--
  draw.outline x, y, w, h, GRAY
endf

function draw.title
  locals x, y, w, h
  . x=4, y=4, w=screen.w, w-9, h=40
  set.box box, x, y, w, h
  draw.shade box, BLACK, GRAY25
  draw.outline x, y, w, h, GRAY
endf

function draw.client
  locals x, y, w, h
  . x=4, y=48, w=screen.w, w-9
  . h=screen.h, h-53
  draw.outline x, y, w, h, GRAY25
endf

function on.draw
  draw.window
  draw.title
  draw.client
endf

function on.key
  if key.event='k'
    ; ...
  end
endf

function on.mouse
  if mouse.event='c'
    ; ...
  else.if mouse.event='rc'
    ; ...
  end
  render
endf