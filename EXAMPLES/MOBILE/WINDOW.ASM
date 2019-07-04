; Assemble with ABAKISA (FASMARM)

include 'a.inc'

align

textu title='My Window'

;;;;;;;;;;;;;;;;;;;;; WINDOW ;;;;;;;;;;;;;;;;;;;;;

function initialize
  call GetModuleHandleW, 0
  stm a1, hmodule
  call HeapCreate, 0, 0, 0
  stm a1, heap
  call GetSystemMetrics, 0
  stm a1, os.w
  stm a1, screen.w
  stm a1, window.w
  call GetSystemMetrics, 1
  stm a1, os.h
  stm a1, screen.h
  sub a1, 22
  stm a1, window.h
  mov a1, 0
  stm a1, window.x
  mov a1, 22
  stm a1, window.y
endf

function create.window
  ldm a1, hmodule
  ldi v7, wc
  str a1, [v7, ?wc.hInstance]
  mov a1, 0
  str a1, [v7, ?wc.style]
  ldi a1, !window.event
  str a1, [v7, ?wc.lpfnWndProc]
  set.window.color 0F7E1E7h
  ldm v5, window.color
  call CreateSolidBrush, v5
  str a1, [v7, ?wc.hbrBackground]
  ldi a1, main.class.name
  str a1, [v7, ?wc.lpszClassName]
  call RegisterClassW, v7
  tst a1, a1
  beq .error
  ldi v1, main.class.name
  ldi v2, title
  ldi v3, WS_DEFAULT
  ldm v4, window.x
  ldm v5, window.y
  ldm v6, window.w
  ldm v7, window.h
  ldm v8, hmodule
  call CreateWindowExW, 0, v1, v2, v3,\
   v4, v5, v6, v7, 0, 0, v8, 0
  stm a1, hwnd
  tst a1, a1
  beq .error
  .success:
   return 1
  .error:
endf 0

function show.window
  ldm v7, hwnd
  call ShowWindow, v7, 3
  call UpdateWindow, v7
  call InvalidateRect, v7, 0, 0
endf

function on.draw
  ; ...
endf

function main
  initialize
  create.window
  show.window
  @@:
   ldi v7, wm
   call GetMessageW, v7, 0, 0, 0
   tst a1, a1
   beq .r
   call TranslateMessage, v7
   call DispatchMessageW, v7
  bal @b
  .r:
  mov a1, 0
endf

;;;;;;;;;;;;;;;; WINDOW PROCEDURE ;;;;;;;;;;;;;;;;

function window.event
  alias window=v1, message=v2,\
   wp=v3, lp=v4, x=v5
  mov window, a1
  mov message, a2
  mov wp, a3
  mov lp, a4

  ldi x, WM_CREATE
  cmp message, x
  beq .create
  ldi x, WM_DESTROY
  cmp message, x
  beq .destroy
  ldi x, WM_CLOSE
  cmp message, x
  beq .close
  ldi x, WM_PAINT
  cmp message, x
  beq .paint
  ldi x, WM_KEYDOWN
  cmp message, x
  beq .key
  ldi x, WM_LBUTTONDOWN
  cmp message, x
  beq .mouse
  bal .default

  .create:
   ; ...
   bal .default

  .paint:
   begin.paint
   get.orient
   on.draw
   end.paint
   bal .default

  .key:
   ; ...
   bal .default

  .mouse:
   ; ...
   bal .default

  .size:
   ; ...
   bal .default

  .close:
   ldi x, WM_DESTROY
   call SendMessageW,\
    window, x, 0, 0
   bal .default

  .destroy:
   exit

  .default:
   call DefWindowProcW,\
    window, message, wp, lp
   bal .r
  .r0:
  mov a1, 0
  .r:
endf