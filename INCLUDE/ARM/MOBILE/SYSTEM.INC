; $$$$$$$$$$$$$$$$ ABA-KIS 4 ARM $$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ????????????????? SYSTEM.INC ???????????????????

include 'import.inc'
include 'resource.inc'

align 4

void heap

integer os.w, os.h, screen.w, screen.h
integer window.x, window.y, window.w, window.h
integer window.color

; ????????????????? MEMORY.INC ???????????????????

; allocate n
; destroy p

function create.heap
  get heap=HeapCreate, 0, 0, 0
endf

function allocate, n
  ldm v1, heap
  mov v2, a1
  call HeapAlloc, v1, 0, v2
endf

function destroy, p
  ldm v1, heap
  mov v2, a1
  call HeapFree, v1, 0, v2
endf

;;;;;;;;;;;;;;;; COPY, SET, ZERO ;;;;;;;;;;;;;;;;;

; memory.set p, v, n
; memory.copy a, b, n
; memory.zero p, n

; memory.copy(destiny, source, count)

function memory.copy, a, b, n
  cmp a3, 4         ; IF n<4
  blt .8            ; copy 8BIT
  mov v2, a3, lsr 2 ; v2=n/4
  .32:              ; copy 32BIT
   ldr v1, [a2], 4  ; v1=*a2++
   str v1, [a1], 4  ; *a1++=v1
   subs v2, v2, 1   ; v2 # times
  bne .32
  ands a3, a3, 3    ; modulo 4
  beq .!            ; remainder?
  .8:               ; copy 8BIT
   ldrb v1, [a2], 1 ; v1=*a2++
   strb v1, [a1], 1 ; *a1++=v1
   subs a3, a3, 1   ; a3 # times
  bne .8
endf

; memory.set(destiny, value, count).
; assumes 8/16BIT values have been
; expanded to 32BIT

function memory.set, p, v, n
  cmp a3, 4         ; IF n<4
  blt .8            ; copy 8BIT
  mov v2, a3, lsr 2 ; v2=n/4
  .32:              ; copy 32BIT
   str a2, [a1], 4  ; *a1++=a2
   subs v2, 1       ; v2 # times
  bne .32
  ands a3, 3        ; modulo 4
  beq .!            ; remainder?
  .8:               ; copy 8BIT
   strb a2, [a1], 1 ; *a1++=a2
   subs a3, 1       ; a3 # times
  bne .8
endf

macro memory.zero p, n { memory.set p, 0, n }

;;;;;;;;;;;;;;;;;;;; FILE I/O ;;;;;;;;;;;;;;;;;;;;;

; unfinished...

; open file   ; attempt open, fail IF non-existent
; create file ; create, replace IF exists
; close       ; close current file

; seek n      ; seek absolute
; seek n, r   ; seek relative. r=0/1/2=b/c/e

; read/write to/from current file

; read p, n
; write p, n

numeric EOF=-1,\
 CREATE_NEW=1, CREATE_ALWAYS, OPEN_EXISTING,\
 OPEN_ALWAYS, TRUNCATE_EXISTING,\
 GENERIC_READ=80000000h,\
 GENERIC_WRITE=40000000h,\
 FILE_SHARE_READ=1, FILE_SHARE_WRITE,\
 FILE_ATTRIBUTE_NORMAL=80h

numeric SEEK.BEGIN, SEEK.SET, SEEK.END

GENERIC_OPEN=GENERIC_READ or GENERIC_WRITE

align          ; current file:

void file.p    ; pointer for load/save
void file.h    ; handle
integer file.n ; size
integer tmp.rw

macro flush { destroy file.p }

function os.zero.file
  mov a1, 0
  stm a1, file.p
  stm a1, file.h
  stm a1, file.n
  stm a1, tmp.rw
endf

; open/create file. return handle or -1 IF error

macro os.create.file \
  name, access, share, method {
  os.zero.file
  mov v1, name
  ldi v2, access
  ldi v3, share
  ldi v4, method
  call CreateFileW, v1, v2, v3, 0, v4, 0, 0
  stm a1, file.h
}

macro os.open name {
  os.create.file name, GENERIC_OPEN,\
   FILE_SHARE_READ, OPEN_EXISTING
}

macro os.create name {
  os.create.file name, GENERIC_WRITE,\
   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL
}

macro os.seek n, r {
  ldm v7, file.h
  ldi v1, n
  call SetFilePointer, v7, v1, 0, r
}

macro os.get.file.size {
  ldm v7, file.h
  call GetFileSize, v7, 0
  stm a1, file.n
}

macro os.close {
  ldm v7, file.h
  call CloseHandle, v7
  os.zero.file
}

function os.read, p, n
  mov v1, a1
  mov v2, a2
  ldi v3, tmp.rw
  ldm v7, file.h
  call ReadFile, v7, v1, v2, v3, 0
endf

function os.write, p, n
  mov v1, a1
  mov v2, a2
  ldi v3, tmp.rw
  ldm v7, file.h
  call WriteFile, v7, v1, v2, v3, 0
endf

function open, name
  mov v1, a1
  try os.open v1
  os.get.file.size
endf 1

function create, name
  mov v1, a1
  try os.create v1
endf 1

macro seek n, r {
  local ?r
  ?r=0
  IF ~ r eq
    ?r=r
  END IF
  os.seek n, ?r
}

function read, p, n
  mov v1, a1
  mov v2, a2
  os.read v1, v2
endf

function write, p, n
  mov v1, a1
  mov v2, a2
  os.write v1, v2
endf

function close
  ldm v1, file.h
  call CloseHandle, v1
endf

;;;;;;;;;;;;;;;;;;;;; WINDOWS ;;;;;;;;;;;;;;;;;;;;

HWND    fix void
HDC     fix void
HBITMAP fix void
HFONT   fix void

macro POINT [a] { a: integer a#.x, a#.y }

macro BOX [a]
 { a: integer a#.x, a#.y, a#.w, a#.h }

macro RECT [a] { a: integer a#.left,\
 a#.top, a#.right, a#.bottom }

assume ?rect=RECT

macro MSG [a] {
 a: integer a#.hwnd, a#.message,\
 a#.wParam, a#.lParam, a#.time
 POINT a#.pt
}

assume ?wm=MSG

macro WNDCLASSEX [a] {
 a: integer a#.style, a#.lpfnWndProc,\
 a#.cbClsExtra, a#.cbWndExtra, a#.hInstance,\
 a#.hIcon, a#.hCursor, a#.hbrBackground,\
 a#.lpszMenuName, a#.lpszClassName
 WNDCLASSEX.$=$-a
}

macro virtuals name, [p] {
 common
  name:
 virtual at 0
  forward
   name#.#p dw ?
  common
 END virtual
}

virtuals ?wc, style, lpfnWndProc,\
 cbClsExtra, cbWndExtra, hInstance,\
 hIcon, hCursor, hbrBackground,\
 lpszMenuName, lpszClassName

; assume ?wc=WNDCLASSEX

macro PAINTSTRUCT [a] {
 a: integer a#.hdc, a#.fErase
 RECT a#.rcPaint
 integer a#.fRestore, a#.fIncUpdate
 text a#.rgbReserved(32)
}

macro BITMAP [a] {
 a: integer a#.bmType,\
  a#.bmWidth, a#.bmHeight, a#.bmWidthBytes
 short a#.bmPlanes, a#.bmBitsPixel
 void a#.bmBits
 BITMAP.$=$-a
}

; assume ?bitmap=BITMAP

macro BITMAPINFOHEADER [a] {
 a: integer a#.biSize, a#.biWidth, a#.biHeight
 short a#.biPlanes, a#.biBitCount
 integer a#.biCompression, a#.biSizeImage,\
  a#.biXPelsPerMeter, a#.biYPelsPerMeter,\
  a#.biClrUsed, a#.biClrImportant
 BITMAPINFOHEADER.$=$-a
}

macro BITMAPINFO [a] {
  BITMAPINFOHEADER a
  integer bmiColors
  BITMAPINFO.$=BITMAPINFOHEADER.$+4
}

macro LOGBRUSH [a] {
 a: integer a#.style, a#.color, a#.lbHatch
}

assume ?lb=LOGBRUSH

macro LOGFONT [a] {
 a: integer a#.nHeight, a#.nWidth,\
  a#.nEscapement, a#.nOrientation,\
  a#.fnWeight, a#.fdwItalic,\
  a#.fdwUnderline, a#.fdwStrikeOut,\
  a#.fdwCharSet, a#.fdwOutputPrecision,\
  a#.fdwClipPrecision, a#.fdwQuality,\
  a#.fdwPitchAndFamily
 text a#.lpszFace(32*2)
}

assume ?lf=LOGFONT

; window messages

numeric WM_*, \
 CREATE=1, DESTROY=2, MOVE=3, SIZE=5,\
 SETFOCUS=7, KILLFOCUS=8, GETTEXT=0Dh,\
 SETTEXT=0Ch, GETTEXTLENGTH=0Eh,\
 PAINT=0Fh, CLOSE=10h, QUIT=12h, CUT=300h,\
 COPY=301h, PASTE=302h, CLEAR=303h,\
 SETFONT=30h, COMMAND=111h, TIMER=0113h

; window styles

numeric WS_*, \
 POPUP=80000000h, CHILD=40000000h,\
 MINIMIZE=20000000h, VISIBLE=10000000h,\
 MAXIMIZE=1000000h, CAPTION=0C00000h,\
 BORDER=800000h, DLGFRAME=400000h,\
 VSCROLL=200000h, HSCROLL=100000h,\
 SYSMENU=80000h, THICKFRAME=40000h, \
 MINIMIZEBOX=20000h, MAXIMIZEBOX=10000h,\
 EX_TOPMOST=8, EX_TOOLWINDOW=80h,\
 EX_WINDOWEDGE=100h, EX_CLIENTEDGE=00200h

WS_BLANK   = WS_VISIBLE+WS_POPUP
WS_DEFAULT = WS_VISIBLE+WS_CAPTION+\
             WS_MINIMIZEBOX+WS_SYSMENU

CS_DBLCLKS=8

; keyboard+mouse messages

numeric WM_*,\
 KEYDOWN=100h, KEYUP, CHAR, DEADCHAR,\
 SYSKEYDOWN, SYSKEYUP, SYSCHAR

numeric WM_*,\
 MOUSEMOVE=200h, LBUTTONDOWN, LBUTTONUP,\
 LBUTTONDBLCLK, RBUTTONDOWN, RBUTTONUP,\
 RBUTTONDBLCLK, MBUTTONDOWN, MBUTTONUP,\
 MBUTTONDBLCLK, MOUSEWHEEL

; virtual key codes. function keys=(6Fh+N).
; example: F1=70h (6Fh+1)

numeric K.*,\
 FUNCTION=6Fh, LEFT=25h, UP=26h, RIGHT=27h,\
 DOWN=28h, ESCAPE=1Bh, SPACE=20h, DELETE=2Eh,\
 CONTROL=11h, LCONTROL=0A2h, RCONTROL=0A3h,\
 LALT=0A4h, RALT=0A5h, BACK=8, TAB=9,\
 RETURN=0Dh, END=23h, HOME=24h,\
 A='A', S='S', D='D', W='W'

SRCCOPY=00CC0020h

macro os.show.cursor { ShowCursor 1 }

align

void hmodule, hwnd, hedit,\
 hdc, mdc, hbrush, hfont

integer screen.orient

RECT rect

WNDCLASSEX wc  ; window
MSG wm         ; message
PAINTSTRUCT ps ; for PAINT
BITMAP bm      ; for draw.bitmap.w

LOGBRUSH lbrush
LOGFONT lfont

align 2

textu main.class.name='WC1'
textu edit.class.name='EDIT'
textu font.name.t='Tahoma'

FONT.H=16
FONT.WEIGHT=700
FONT.ITALIC=0

function get.screen.size
  call GetSystemMetrics, 0
  stm a1, screen.w
  call GetSystemMetrics, 1
  stm a1, screen.h
endf

; return 0=landscape, 1=portrait/phone
; and update

function get.orient
  get.screen.size
  ldm v1, screen.w
  ldm v2, screen.h
  mov a1, 0
  cmp v1, v2
  movlo a1, 1
  stm a1, screen.orient
endf

;;;;;;;;;;;;;;;;; WINDOWS MACROS ;;;;;;;;;;;;;;;;;

macro exit { call PostQuitMessage, 0 }

macro say t {
 ldi r7, t
 call MessageBoxW, 0, r7, r7, 0
}

align
text buffer.t(256)

function sayt, t
  ldi v7, buffer.t
  mov v8, a1
  @@:
   ldrb a1, [v8], 1
   strh a1, [v7], 2
   tst a1, a1
  bne @b
  ldi v7, buffer.t
  call MessageBoxW, 0, v7, v7, 0
endf

function sayu, t
  mov v7, a1
  call MessageBoxW, 0, v7, v7, 0
endf

macro message.loop {
  local ..l
  ..l:
   ldi v7, wm
   call GetMessageW, v7, 0, 0, 0
   tst a1, a1
   beq .r
   call TranslateMessage, v7
   call DispatchMessageW, v7
  bal ..l
  .r:
  ldi v7, wm
  ldr a1, [v7, ?wm.wParam]
}

macro set.window.color c {
  ldi a1, c
  stm a1, window.color
}

;;;;;;;;;;;;;;;;;;;; DRAWING ;;;;;;;;;;;;;;;;;;;;;

macro begin.paint {
  ldm v7, hwnd
  ldi v8, ps
  call BeginPaint, v7, v8
  stm a1, hdc
}

macro end.paint {
  ldm v7, hwnd
  ldi v8, ps
  call EndPaint, v7, v8
}

macro create.brush b, c {
  ldi v7, c
  call CreateSolidBrush, v7
  stm a1, b
}

macro select.brush b {
  ldm v7, hdc
  ldm v8, b
  call SelectObject, v7, v8
}

function draw.box ;, x, y, w, h
  alias x=v1, y=v2, w=v3, h=v4,\
   p=v5, dc=v7
  mov x, a1
  mov y, a2
  mov w, a3
  mov h, a4
  select.brush hbrush
  ldm dc, hdc
  call Rectangle, dc, x, y, w, h
endf

function draw.circle, x, y, w, h
;  alias x=v1, y=v2, w=v3, h=v4,\
;   p=v5, dc=v7
;  mov x, a1
;  mov y, a2
;  mov w, a3
;  mov h, a4
;  select.brush hbrush
;  ldm dc, hdc
;  call Ellipse, dc, x, y, w, h
endf

;;;;;;;;;;;;;;;;;;;;;; FONT ;;;;;;;;;;;;;;;;;;;;;;

; unfinished...

function create.font.x ;,\
; name, size, weight, italic
  alias x=a1, y=a2, z=a3, p=v7,\
   name=v1, size=v2, weight=v4, italic=v5
  mov name, a1
  mov size, a2
  mov weight, a3
  mov italic, a4
  ldi p, lfont
  mov x, size
  str x, [p, ?lf.nHeight]
  mov x, 0
  str x, [p, ?lf.nWidth]
  str x, [p, ?lf.nEscapement]
  str x, [p, ?lf.nOrientation]
  mov y, weight
  str y, [p, ?lf.fnWeight]
  mov y, italic
  str y, [p, ?lf.fdwItalic]
  str x, [p, ?lf.fdwUnderline]
  str x, [p, ?lf.fdwStrikeOut]
  str x, [p, ?lf.fdwCharSet]
  str x, [p, ?lf.fdwOutputPrecision]
  str x, [p, ?lf.fdwClipPrecision]
  ldi y, 4
  str y, [p, ?lf.fdwQuality]
  ldi y, 8
  str y, [p, ?lf.fdwPitchAndFamily]
  add x, p, ?lf.lpszFace
  mov y, name
  @@:
   ldrh z, [y], 2
   strh z, [x], 2
   tst z, z
  bne @b
  ldi p, lfont
  call CreateFontIndirectW, p
  stm a1, hfont
endf

function create.font
  ldi a1, font.name.t
  ldi a2, 12          ; FONT.HEIGHT
  ldi a3, 800         ; FONT.WEIGHT
  ldi a4, 0           ; FONT.ITALIC
  create.font.x
endf

;;;;;;;;;;;;;;;;;;;;;; IMAGE ;;;;;;;;;;;;;;;;;;;;;

macro IMAGE [a] {
 a:
 void a#.p
 integer a#.x, a#.y, a#.w, a#.h,\
  a#.bpp, a#.key, a#.alpha
 ; ...
 HBITMAP a#.hbmp
 BITMAP a#.bmp
}

virtuals ?bitmap,\
  bmType, bmWidth, bmHeight, bmWidthBytes,\
  bmPlanesBits, bmBits

virtuals ?image,\
  p, x, y, w, h, bpp,\
  key, alpha, hbmp,\
  bmp, bmWidth, bmHeight, bmWidthBytes,\
  bmPlanesBits, bmBits

IMAGE.$=?image.bmp-?image

function load.image, name, file
  alias x=v3, bmp=v5, hbmp=v6, image=v7
  mov image, a1
  add bmp, a1, ?image.bmp
  add hbmp, a1, ?image.hbmp
  mov v1, a2
  call SHLoadDIBitmap, v1
  str a1, [image, ?image.hbmp]
  mov v1, a1
  call GetObjectW, v1, BITMAP.$, bmp
  ldr x, [bmp, ?bitmap.bmWidth]
  str x, [image, ?image.w]
  ldr x, [bmp, ?bitmap.bmHeight]
  str x, [image, ?image.h]
endf

macro load.image name, file {
  IF file eq
    load.image name, name#.file
  ELSE
    load.image name, file
  END IF
}

macro load.images [name]
 { forward load.image name#.i }

function move.image, name, x, y
  str a2, [a1, ?image.x]
  str a3, [a1, ?image.y]
endf

function draw.image, name
  mov v7, a1
  ldm v1, hdc
  call CreateCompatibleDC, v1
  stm a1, mdc
  mov v1, a1
  ldr v2, [v7, ?image.hbmp]
  call SelectObject, v1, v2
  ldm v1, hdc
  ldr v2, [v7, ?image.x]
  ldr v3, [v7, ?image.y]
  ldr v4, [v7, ?image.w]
  ldr v5, [v7, ?image.h]
  ldm v6, mdc
  ldi v7, 0CC0020h
  call StretchBlt, v1, v2, v3, v4, v5,\
   v6, 0, 0, v4, v5, v7
  call DeleteDC, v6
endf

macro draw.image name, x, y {
  IF ~x eq
    move.image name, x, y
  END IF
  draw.image name
}

;;;;;;;;;;;;;;;;;;;; TOOLBAR ;;;;;;;;;;;;;;;;;;;;;

macro TOOL.BAR name, [p] {
  common
   align
   name: dw p
   name#.n=($-name)/4
}

;;;;;;;;;;;;;;;;;;;;;; MENU ;;;;;;;;;;;;;;;;;;;;;;

macro MENU [p] { common }

MENU \
 file='File', new='New', open='Open...',\
 close='Close', save='Save',\
 save_as='Save As...', exit='Exit',\
 edit='Edit', undo='Undo', cut='Cut',\
 copy='Copy', paste='Paste',\
 delete='Delete', select_all='Select All',\
 execute='Execute', help='Help'

;;;;;;;;;;;;;;;;;;;; EDIT BOX ;;;;;;;;;;;;;;;;;;;;

numeric ES_*, LEFT, MULTILINE=4,\
 AUTOVSCROLL=40h, AUTOHSCROLL=80h,\
 NOHIDESEL=0100h, WANTRETURN=1000h

WS_EDIT=WS_CHILD+WS_VISIBLE+\
 WS_HSCROLL+WS_VSCROLL+WS_BORDER+\
 ES_MULTILINE+ES_NOHIDESEL+ES_AUTOVSCROLL

align

edit.box: integer .x, .y, .w, .h

function adjust.edit.box
  alias p=v7,\
   x=v1, y=v2, w=v3, h=v4
  ldi p, edit.box
  mov x, 3
  mov y, 32
  ldm w, window.w
  sub w, 8
  ldm h, window.h
  sub h, 60
  str x, [p, ?box.x]
  str y, [p, ?box.y]
  str w, [p, ?box.w]
  str h, [p, ?box.h]
endf

function create.edit.box
  alias p=v7,\
   x=v3, y=v4, w=v5, h=v6,\
   name=v1, style=v2, module=a4
  ldi name, edit.class.name
  ldi style, WS_EDIT
  get.orient
  adjust.edit.box
  ldi p, edit.box
  ldr x, [p, ?box.x]
  ldr y, [p, ?box.y]
  ldr w, [p, ?box.w]
  ldr h, [p, ?box.h]
  ldm p, hwnd
  ldm module, hmodule
  call CreateWindowExW, 0, name, 0,\
   style, x, y, w, h, p, 0, module, 0
  stm a1, hedit
endf

function set.edit.font
  set.font
  ldm v1, hedit
  mov v2, a1
  call SendMessageW, v1, WM_SETFONT, v2, 1
endf

function set.edit.text, t
  ldm v1, hedit
  ldi v2, WM_SETTEXT
  mov v3, a1
  call SendMessageW, v1, v2, 0, v3
endf