; $$$$$$$$$$$$$$$$$$$$ HTML $$$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ;;;;;;;;;;;;; SYNTAX HIGHLIGHTER ;;;;;;;;;;;;;;;

; EXAMPLE USAGE: SAVE AS: EXAMPLE.ASM
; GENERATES EXAMPLE.HTML

; include 'html.inc'
;
; source.file equ 'SOURCE.TXT'
; title.t equ 'Title'
;
; comment.color equ '#A0A0A0'
; number.color equ '#4E00E7'
; symbol.color equ '#0000FF'
; string.color equ '#FF1777'
;
; html.begin
; html.code source.file
; html.end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

format binary as 'HTML'

is.symbol? equ \
 a=';' | a=27h | a=22h | a='=' | a=',' | \
 a='+' | a='-' | a='*' | a='/' | a='%' | \
 a='\' | a='(' | a=')' | a='{' | a='}' | \
 a='[' | a=']' | a='<' | a='>' | a='~' | \
 a='&' | a='|' | a='^' | a=':' | a='#' | a='`'

is.symbolic? equ \
 a='_' | a='.' | a='?' | a='!' | a='@' | a='$'

is.space?  equ (a=' ' | a=9)
is.return? equ (a=0Dh | a=0Ah)
is.white?  equ (is.space? | is.return?)
is.string? equ (a=27h | a=22h)
is.upper?  equ (a>='A' & a<='Z')
is.lower?  equ (a>='a' & a<='z')
is.alpha?  equ (is.upper? | is.lower?)
is.name?   equ (is.alpha? | is.symbolic?)
is.number? equ (a>='0' & a<='9')
is.hex.a?  equ (a>='a' & a<='f')
is.hex.A?  equ (a>='A' & a<='F')
is.digit?  equ (is.hex.a? | is.hex.A?)
is.suffix? equ (a='h' | a='H' | a='b' | a='B')
is.end?    equ (is.white? | is.symbol?)

R equ 0Dh,0Ah

macro htmlx [p] { forward db p }

macro html [p] { common htmlx p, R }

macro html.begin title {
 html '<!DOCTYPE html>', '<html>', '<head>'
 html '<meta http-equiv="content-type"'
 html 'context="text/html; charset=windows-1252">'
 html '<title>', title.t, '</title>', '</head>'
 html '<body bgcolor="#FFFFFF">', '<b>', R

 html '<style>', R, 'body {'
 html ' font-size: 18px;'
 html ' font-weight: bold;'
 html ' width: 720px;'
 html ' font-family: "Consolas",'
 html ' "Bitstream Vera Sans Mono",'
 html ' "Courier New", "Courier",'
 html ' "Andale Mono WT", "Andale Mono",'
 html ' "Lucida Console", "DejaVu Sans Mono",'
 html ' "Monaco", monospace;'
 html '}', R

 html '.comment {'
 htmlx ' color: ', comment.color, ';', R
 html ' font-style: italic;'
 html '}', R

 htmlx '.symbol { color: ', symbol.color, '; }', R
 htmlx '.number { color: ', number.color, '; }', R
 htmlx '.string { color: ', string.color, '; }', R
 html '</style>', R
}

macro html.text t, a {
 db '<font color="#000000" size="4" '
 db 'face="Times New Roman">', R
 db '<p align="', `a, '">', R
 db t, R, '</p>', '</font>', R, R
}

macro html.subject s, a {
 db '<font color="#000000" size="5" '
 db 'face="Times New Roman">', R
 db '<p align="', `a, '"><u><i>', R
 db s, R, '</i></u></p>', '</font>', R, R
}

macro html.end
 { html R, '</body>', '</html>' }

macro style name
 { db '<span class=''', `name, '''>' }

macro end.style { db '</span>' }

macro html.db {
 if a='<'
  db '&lt;'
 else if a='>'
  db '&gt;'
 else
  db a
 end if
}

;;;;;;;;; SOURCE CODE, SYNTAX HIGHLIGHT ;;;;;;;;;;

; load/extract/insert source file with
; syntax highlighting

macro html.code name {
 local p, i, line.n,\
  comment?, number?, string?

 db '<pre style="background:white; color:black;', R
 db 'border:2px solid white; padding:.2em .2em;">', R

 virtual at 0
  p:: file name
  file.$=$
 end virtual

 line.n=1
 comment?=0
 number?=0
 string?=0
 i=0
 while i<file.$
  load a byte from p:i
  if a=0Dh
   line.n=line.n+1
   string?=0
  end if
  if comment?
   if a=0Dh
    comment?=0
    end.style
   end if
   html.db
  else if number?
   if is.end?
    number?=0
    end.style
   end if
   db a
  else if string?
   if is.string?
    string?=0
    db a
    end.style
   else
    html.db
   end if
  else if is.number?
   c=a
   if number?=0
    if i>0
     load a byte from p:i-1
     if ((is.end? | \
      ~is.name?) & \
      ~is.number?)
      number?=1
      style number
     end if
    else
     number?=1
     style number
    end if
   end if
   db c
  else if is.string?
   string?=1
   style string
   db a
  else if is.symbol?
   if a=';'
    comment?=1
    style comment
    db a
   else
    style symbol
    html.db
    end.style
   end if
  else
   db a
  end if
  i=i+1
 end while

 db '</pre>'
}