; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? INTERFACE ???????????????????

WINDOW.X=0
WINDOW.Y=0

SCREEN.W   = WINDOW.W ; 1024
SCREEN.H   = WINDOW.H ; 768-32
SCREEN.BPP = 32
SCREEN.PI  = SCREEN.W*(SCREEN.BPP/8)

function main
  os.get.screen.w
  os.get.screen.h
  set.screen WINDOW.W, WINDOW.H, 32
  try create.vga screen.w, screen.h, BLACK
  define.events create, draw, key, mouse
  create.blank.window screen.w, screen.h
  os.show.cursor
  message.loop
endf

; easy custom tool bar

macro TOOLBAR name, [p] {
  common align
   name: dd p
   name#.n=($-name)/4
}

; tb: tool bar
; xy: location
; wh: size
; wi: width + inset
; n:  # icons

align void item.select=NONE
BOX item.box

function draw.tool.bar, tb, x, y, w, h, wi, n
  locals p, q, s
  . p=tb, s=NONE
  loop n, r0=p, (u32) r0=*r0
    . (u32) r0=*(r0+?image.p)
    draw.bitmap.t r0, x, y, w, h
    set.box item.box, x, y, w, h
    if.select item.box
      . s=p
    end
    . r0=wi, x+r0, p+4
  endl
  . item.select=s
endf s

; check box

align BOX cb.box

function draw.check.box, t, x, y
  . x+4, y+4
  set.box cb.box, x, y, 20, 20
  draw.fade cb.box, WINDOW.COLOR, WHITE
  draw.box.o cb.box, GRAY25
  . x+32, y-2
  draw.text t, x, y
endf

align

BOX icon.box, client.box, scroll.box, dialog.box