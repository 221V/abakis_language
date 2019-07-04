;;;;;;;;;;;;;;;;;;;;; IMPORT ;;;;;;;;;;;;;;;;;;;;;

; RVAs of dll names and tables, ending with 20
; zero bytes

macro library [names] {
 forward dd 0,0,0,\
  RVA names#_name, RVA names#_table
 common dd 0,0,0,0,0
}

; DLL name + import RVA table. each table ends with 0.
; finally, import names. dw 0 is "ordinal" (N/A)

macro import name, [names] {
 common
  name#_name \		  ; text DLL_name='DLL.DLL'
  db `name#'.DLL', 0
  name#_table:		  ; DLL_table:
 forward
  IF used !#names
   !#names dd RVA _#names ; import name RVAs
   macro names [p] \{	  ; call with no
    \common		  ; invoke prefix
     pushr p
     call [!#names]
   \}
  END IF
 common dd 0		  ; end
 forward
  IF used !#names
   _#names dw 0 	  ; import names
   db `names, 0 	  ; 'import'
  END IF
}

;;;;;;;;;;;;;;;;;;;; IMPORTS ;;;;;;;;;;;;;;;;;;;;;

data import

library MSVCRT, KERNEL32, USER32, SHELL32,\
 GDI32, COMDLG32

import MSVCRT, sprintf

import KERNEL32,\
ExitProcess, GetCommandLineA,\
HeapCreate, HeapAlloc, HeapReAlloc, HeapSize,\
HeapFree, HeapDestroy, VirtualAlloc, VirtualFree,\
GetModuleHandleA, GetModuleFileNameA,\
CreateFileA, GetFileSize, ReadFile, WriteFile,\
SetFilePointer, CloseHandle, CopyFileA,\
MoveFileA, DeleteFileA, GetTickCount,\
GetSystemTime, GetLocalTime, GetFileTime,\
FileTimeToSystemTime, SystemTimeToFileTime,\
FileTimeToLocalFileTime,\
SystemTimeToTzSpecificLocalTime,\
GetFileAttributesExA, CompareFileTimeA,\
GetCurrentDirectoryA, SetCurrentDirectoryA,\
CreateDirectoryA, LoadLibraryA, FreeLibrary,\
GetProcAddress, FindFirstFileA, FindNextFileA,\
FindClose, WaitForSingleObject, Sleep

import USER32,\
GetDC, ReleaseDC,\
MessageBoxA, RegisterClassExA, CreateWindowExA,\
DestroyWindow, ShowWindow, MoveWindow,\
UpdateWindow, GetMessageA, PeekMessageA,\
TranslateMessage, DispatchMessageA,\
SendMessageA, DefWindowProcA, PostQuitMessage,\
WaitMessage, GetAsyncKeyState, LoadImageA,\
LoadIconA, LoadCursorA, SetCursor, ShowCursor,\
SetCursorPos, OpenClipboard, SetClipboardData,\
IsClipboardFormatAvailable, GetClipboardData,\
CloseClipboard, EmptyClipboard,\
GetSystemMetrics, BeginPaint, EndPaint,\
FillRect, InvalidateRect, SetTimer

import SHELL32, ShellExecuteA, ShellExecuteExA

import GDI32, SelectObject, DeleteObject,\
GetObjectA, DeleteDC, TextOutA, CreateFontA,\
CreateFontIndirectA, SetDIBits, BitBlt, StretchBlt,\
CreateBitmap, CreateCompatibleDC

import COMDLG32, GetOpenFileNameA,\
GetSaveFileNameA, ChooseColorA, ChooseFontA

END data