; ABAKIS: FASM MOBILE PORTABLE
; IDE SKETCH

WINDOW.W=800
WINDOW.H=480

include 'a.inc'
use interface

text title.t=+'FASM   File  Edit  ',\
 'Parse  Code  Wizard  Utility  Help'

text search.title.t='Search',\
 search.blank.t='(Text)',\
 search.case.t='Case',\
 search.whole.t='Whole',\
 search.last.t='Last', search.next.t='Next'

align

ICON24 project.i='code', new.i='new',\
 open.i='open', save.i='save', undo.i='undo',\
 redo.i='redo', cut.i='cut', copy.i='copy',\
 paste.i='paste', search.i='search',\
 compile.i='compile', execute.i='run',\
 comment.i='comment', indent.l.i='l',\
 indent.r.i='r', arrow.l.i='left2',\
 arrow.r.i='right2', theme.i='alpha',\
 windows.i='w', settings.i='settings',\
 help.i='help', closes.i='x'
ICON32 close.i='x'

TOOLBAR tool.bar,\
  project.i, new.i, open.i, save.i, undo.i,\
  redo.i, cut.i, copy.i, paste.i, search.i,\
  comment.i, indent.l.i, indent.r.i,\
  compile.i, execute.i, theme.i, windows.i,\
  settings.i, help.i

align FONT main.font='font'

source.code: db \
 'format ELF executable' RET\
 'entry start' RET\
 '' RET\
 'segment readable executable' RET\
 '' RET\
 'start:' RET\
 ' mov r0, 0' RET\
 ' adr r1, t' RET\
 ' mov r2, n' RET\
 ' swi 900004h' RET\
 ' mov r0, 6' RET\
 ' swi 900001h' RET\
 '' RET\
 't: db ''Hello'', 10' RET\
 'n=$-t', 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

function on.create
  set.font main.font
endf

function draw.window
  locals x, y, w, h
  clear.screen WINDOW.COLOR
  . x=0, y=0, w=screen.w, w--
  . h=screen.h, h--
  draw.outline x, y, w, h, WHITE
endf

function draw.title
  locals x, y, w, h
  . x=4, y=4, w=screen.w, w-8, h=40
  set.box box, x, y, w, h
  draw.shade box, ROYAL.BLUE, BLUE
  . r0=h, r0++
  draw.outline x, y, w, r0, WHITE
  draw.image.t compile.i, 7, 12
  set.font.color WHITE
  draw.text title.t, 37, 12
  . r0=screen.w, r0-40
  draw.image.t close.i, r0, 8
  . r0=screen.w, r0-32
  draw.image.t closes.i, r0, 50
endf

function draw.scroll.bar
  locals x, y, w, h
  . x=screen.w, x-36, y=80, w=32
  . h=screen.h, h-84
  set.box box, x, y, w, h
  draw.box.s box, WHITE
  draw.box.o box, GRAY25
endf

function draw.text.box
  locals x, y, w, h
  . x=4, y=80, w=screen.w, w-8, w-36
  . h=screen.h, h-84
  set.box box, x, y, w, h
  draw.box.s box, WHITE
  draw.box.o box, GRAY25
  set.font.color BLACK
  . r0=x, r1=font.w, r2=r1, r2/2
  . r1+r2, r0+r1, x=r0
  . r0=y, r1=font.h, r1/2, r0+r1, y=r0
  draw.text source.code, x, y
endf

function draw.search.box
  locals n, x, y, w, h
  . x=555-4, y=48-4, y+38
  . w=144+62, h=((32)*4)+8
  set.box box, x, y, w, h
  copy.box dialog.box, box
  draw.box.s box, WINDOW.COLOR
  draw.box.o box, GRAY
  . x=box.x, x+4, y=box.y, y+4
  . w=box.w, w-8, h=32
  set.box box, x, y, w, h
  draw.shade box, ROYAL.BLUE, BLUE
  . r0=box.x, r0+4, x=r0
  . r0=box.y, r0+4, y=r0
  set.font.color WHITE
  draw.text search.title.t, x, y
  . x=555, y=48, y+38, y+38
  . w=box.w, w-28, h=28
  set.box box, x, y, w, h
  draw.box.s box, WHITE
  draw.box.o box, GRAY25
  . r0=x, r0+box.w, r0+4, x=r0, y+2
  draw.image.t paste.i, x, y
  . x-4, y-36
  draw.image.t closes.i, x, y
  set.font.color GRAY
  . r0=box.x, r0+4, x=r0
  . r0=box.y, r0+2, y=r0
  draw.text search.blank.t, x, y
  . r0=box.x, r0+1, x=r0
  . r0=box.y, r0+58, y=r0
  set.box box, x, y, 96, 30
  draw.shade box,\
   WINDOW.COLOR.DARK, WINDOW.COLOR
  draw.box.o box, GRAY25
  set.font.color BLACK
  . r0=box.x, r0+37, x=r0
  . r0=box.y, r0+2, y=r0
  draw.text search.last.t, x, y
  move.box.r box
  move.box.right box, 4
  draw.shade box,\
   WINDOW.COLOR.DARK, WINDOW.COLOR
  draw.box.o box, GRAY25
  . r0=box.x, r0+12, x=r0
  . r0=box.y, r0+2, y=r0
  draw.text search.next.t, x, y
  draw.image.t arrow.l.i, 560, 185
  draw.image.t arrow.r.i, 725, 185
  set.font.color BLACK
  . x=dialog.box.x
  draw.check.box search.case.t, x, 153
  get n=get.text.w search.case.t
  . r0=x, r0+n, r0+20, x=r0
  draw.check.box search.whole.t, x, 153
endf

function on.draw
  draw.window
  draw.title
  draw.text.box
  draw.scroll.bar
  draw.tool.bar tool.bar,\
   8, 50, 24, 24, 30, tool.bar.n
  draw.search.box
endf

function on.key
  if key.event='k'
    ; ...
  end
endf

function on.mouse
  if mouse.event='c'
    . r0=&(close.i+?image.box)
    if.select r0
      exit
    end
  else.if mouse.event='rc'
    ; ...
  end
  render
endf

function on.timer
  ; ...
endf

function on.exit
  ; ...
endf