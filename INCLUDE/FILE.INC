; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? FILE.INC ????????????????????

; open file	; try open, fail if no exist
; create file	; create, replace if exists
; exists file	; does file exist?
; close 	; close current file

; seek n	; set location
; get.file.size ; get size

; read p, n	; read/write memory
; write p, n

; load/save entire file, binary or text

; load.file name
; save.file name, p, size
; append.file name, p, size

; load.text name
; save.text name, t

;;;;;;;;;; OPEN, CREATE, CLOSE, EXISTS ;;;;;;;;;;;

function get.file.size
  os.get.file.size
endf

function close
  os.close
  . file.h=-1
endf 1

function open, file
  os.open file
  if r0=-1
    return 0
  end
  get file.n=get.file.size
endf 1

function create, file
  os.create file
  if r0=-1
    return 0
  end
  . file.n=0
endf 1

function exists, file
  open file
  push r0
  close
  pop r0
endf

;;;;;;;;;;;;;;; GET/SET LOCATION ;;;;;;;;;;;;;;;;;

function seek, n, r
  os.seek n, r
endf

macro seek n, r {
 IF r eq
   seek n, SEEK.SET
 ELSE
   seek n, r
 END IF
}

macro tell { seek 0, 1 }

;;;;;;;;;;;;; READ/WRITE DATA/TEXT ;;;;;;;;;;;;;;;

function read, p, n
  os.read p, n
endf

function write, p, n
  os.write p, n
endf

;;;;;;;;;;;;;;;; LOAD/SAVE MEMORY ;;;;;;;;;;;;;;;;

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

;;;;;;;;;;;;;;;;; LOAD/SAVE TEXT ;;;;;;;;;;;;;;;;;

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

;;;;;;;;;;;;;;;;;;;;; LOG ;;;;;;;;;;;;;;;;;;;;;;;;

; log t - append 'text' to log file. only for
; testing, debugging and examples

boolean create.log?=YES,\
 log.star?=YES, log.nl?=YES
text log.file(16)='LOG.TXT', log.star='* '
nl.t db 0Dh, 0Ah, 0

function log, t
  pusha
  if create.log?
    create log.file
    close
    . create.log?=NO
  end
  if t='r'
    go .nl
  else.if log.star?
    append.file log.file, log.star, 2
  end
  text.n t
  append.file log.file, t, r0
  if log.nl?
    .nl: append.file log.file, nl.t, 2
  end
  popa
endf

macro log [t] { forward log t }

macro log.r { log 'r' }

macro show.log { execute log.file }

; log number

function log.n, n
  locale t(96)
  pusha
  . r1=&t
  u2t n, r1
  . r1=&t
  log r1
  popa
endf

function log.h, n
  locale t(96)
  pusha
  . r1=&t
  h2t n, r1
  . r1=&t
  log r1
  popa
endf

function log.b, n
  locale t(96)
  pusha
  . r1=&t
  b2t n, r1
  . r1=&t
  log r1
  popa
endf