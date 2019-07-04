; ABAKIS EXAMPLE. SEE \INCLUDE\

include 'a.inc'

text t(64), s(64), f='LOG.TXT'
text m(64)='NOItUlOVe ReLBmESsa SikaBa'
text a='ABAKIS'

function answer
  if true
    say 'Yes'
  else
    say 'No'
  end
endf

function test.text
  text.copy t, m
  text.reverse t
  text.upper t
  text.attach.c t, '!'
  text.search t, a
  answer
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