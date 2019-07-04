; GBA EXAMPLE: CLEAR SCREEN.
; OPEN WITH VISUALBOYADVANCE.
; DEFAULT A/B KEYS ARE Z/X

include 'a.inc'

function on.create
  ; ...
endf

function on.draw
  clear.screen RED
endf

function on.key
  if.key A
    clear.screen GREEN
  end
  if.key B
    clear.screen BLUE
  end
endf

function on.mouse
  ; ...
endf

function on.timer
  ; ...
endf

function on.exit
  ; ...
endf