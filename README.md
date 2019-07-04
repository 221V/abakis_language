# abakis_language
abakis macro language on top of fasm

## i am not the author, i just save code & examples

https://board.flatassembler.net/topic.php?t=17343
https://board.flatassembler.net/topic.php?p=174080
https://web.archive.org/web/20170501121221/http://m3ntal.freeiz.com:80/abakis.html


```
while c, c=*p++, endw, p-s, p--

. p=a, q=b, n>>>2
loop n, (u32) *p++=*q++, endl

loop n, c=*s++, *p++=c
  if c=0, break, end
endl, p--

while c, c=*s++, *p++=c, endw, p--

while c=d, c=*p++, d=*s++
  if c=0, break, end
  if d=0, break, end
endw, p=c, p-d

while n, x=n, x&1, x+'0'
  . *p++=x, n>>>1
endw, *p=0
  
```

syntaxes
```
while c, c=*p++, endw, p-s, p--

. p=a, q=b, n>>>2
loop n, (u32) *p++=*q++, endl

while s<e, c=*s, *s++=*e, *e--=c, endw

if n=0, *p++='0', *p=0
  return s
end

loop n, c=*s++, *p++=c
  if c=0, break, end
endl, p--

while c, c=*s++, *p++=c, endw, p--

while c=d, c=*p++, d=*s++
  if c=0, break, end
  if d=0, break, end
endw, p=c, p-d

while n, x=n, x&1, x+'0'
  . *p++=x, n>>>1
endw, *p=0

forever, c=*p++
  if c=0, return n, end
  . x=n, x<<2, n+x, n+n
  . n-'0', n+c
endfv

```

functions
```
; memory copy 32BIT

function memory.copy, a, b, n
  alias p=r0, q=r1
  . p=a, q=b, n>>>2
  loop n, (u32) *p++=*q++, endl
endf

; text.n t - get length, # characters

function text.n, t
  alias p=r0, s=r1, c=r2
  . p=t, s=p, c=1
  while c, c=*p++, endw, p-s, p--
endf

; text.copy a, b - standard copy with
; 0 after. return advanced address

function text.copy, a, b
  alias p=r0, s=r1, c=r2
  . p=a, s=b, c=1
  while c, c=*s++, *p++=c, endw, p--
endf

; text.copy.n a, b, n - copy with maximum
; size specified. return &

function text.copy.n, a, b, n
  alias p=r0, s=r1, c=r2
  . p=a, s=b
  loop n, c=*s++, *p++=c
    if c=0, break, end
  endl, p--
endf                                               

; convert 32BIT binary number to text

function b2t, n, t
  alias p=r0, x=r1
  . p=t
  if n=0, *p++='0', *p=0
    return
  end
  while n, x=n, x&1, x+'0'
    . *p++=x, n>>>1
  endw, *p=0
  text.reverse t
endf

; convert text to 32BIT hexadecimal

function t2h, t
  alias p=r0, c=r1, n=r2
  try text.skip.0 t
  . n=0
  forever, c=*p++
    if c=0, return n, end
    if c<=39h, c-30h
    else.if c>=61h, c-57h
    else, c-37h, end, n<<4, n+c
  endfv
endf

; load/save memory and text

function load.file, name
  catch .0
  try open name
  try file.p=allocate file.n
  try read file.p, file.n
  close
  return file.p
  .0: flush
  close
endf 0

function save.file, name, p, size
  catch .0
  try create name
  try write p, size
  close
  return 1
  .0: close
endf 0

function append.file, name, p, size
  catch .0
  open name
  if false
    try create name
  else
    seek 0, SEEK.END
  end
  try write p, size
  close
  return 1
  .0: close
endf 0

function load.text, name
  alias p=r0
  catch .0
  try open name
  . r0=file.n, r0++
  try file.p=allocate r0
  try read file.p, file.n
  close
  . p=file.p, p+file.n, *p=0
  return file.p
  .0: flush
  close
endf 0

function save.text, name, t
  text.n t
  save.file name, t, r0
endf

```

example
```
; EXAMPLES

; ABAKIS EXAMPLE. SEE \INCLUDE\

; print t, f, ... ; print formatted text

include 'z.inc'

integer age=37

text t(64), name='Zia'

text f=+'Hi, %t. You are %u years old ',\
 'which is %hh/%bb'

function main
  print t, f, name, age, age, age
  say t
endf

; ABAKIS EXAMPLE. SEE \INCLUDE\

include 'z.inc'

text t(64), s(64), f='LOG.TXT'
text m(64)='NOItUlOVe ReLBmESsa SikaBa'

function test.text
  text.copy t, m
  say t
  text.reverse t
  text.upper t
  text.attach.c t, '!'
endf

function test.number
  say.n 4294967295
  say.h 0ABCDEF12h
  say.h 0FFFFFFFFh
  say.b 11110000111100001111b
endf

function test.memory
  locals p
  try p=allocate 33
  memory.zero p, 33
  memory.set p, '1234', 32
  say p
  destroy p
endf 1

function test.file
  locals n
  get n=text.n t
  try create f
  write t, n
  close
  try open f
  read s, n
  say s
  close
  execute f
endf 1

function test.io
  test.memory
  if false
    say 'Memory allocation error'
    return 0
  end
  test.file
  if false
    say 'File I/O error'
    return 0
  end
endf 1

function main
  test.text
  test.number
  try test.io
endf
```

