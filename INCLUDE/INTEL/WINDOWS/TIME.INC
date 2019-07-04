; $$$$$$$$$$$$$$$ Z77 ASM LIBRARY $$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? TIME.INC ????????????????????

; system.time is UTC (Universal Time) format
; and local.time is the actual current time
; considering time zone. file.time (64BIT)
; is # of 100-nanosecond intervals since
; January 1, 1601. it may be used to
; compare/arrange times/dates

; year	  - XXXX
; month   - January=1, February=2, etc
; weekday - Sunday=0, Monday=1, etc
; day	  - 1-31
; hour	  - 0-23. if hour>12, pm
; minute  - 0-59
; second  - 0-59
; ms	  - 0-999

macro TIME [a] {
  a: short a#.year, a#.month, a#.weekday,\
   a#.day, a#.hour, a#.minute, a#.second, a#.ms
}

numeric SECOND=1000, MINUTE=SECOND*60,\
 HOUR=MINUTE*60, DAY=HOUR*24,\
 WEEK=DAY*7, YEAR=DAY*365

align

TIME system.time, local.time
FILE.TIME file.time

integer time.year, time.month,\
 time.weekday, time.day,\
 time.hour, time.minute,\
 time.second, time.ms

texts time.days[]='Sunday',\
 'Monday', 'Tuesday', 'Wednesday',\
 'Thursday', 'Friday', 'Saturday',\
 'Today', 'Yesterday', 'Tomorrow'

texts time.months[]='', 'January',\
 'February', 'March', 'April', 'May',\
 'June', 'July', 'August', 'September',\
 'October', 'November', 'December'

void xm.t, weekday.t, month.t
text am.t='AM', pm.t='PM'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; create/update time variables

macro copy.time.v time, v {
 . r0=time
 movzx r1, word [?time.#v+r0]
 . time.#v=r1
}

function update.time, t
  copy.time.v t, hour
  copy.time.v t, minute
  copy.time.v t, second
  copy.time.v t, month
  copy.time.v t, day
  copy.time.v t, year
  copy.time.v t, weekday
  . r0=time.weekday,\
  weekday.t=time.days+r0*4,\
  r0=time.month,\
  month.t=time.months+r0*4
  . r0=time.year
endf

function get.time
  os.get.time
endf

function delay, ms
  os.delay ms
endf

macro get.hour {
  . r0=t, *r0=0, pm=0
  if hour=0, hour=12
  else.if hour>12
    . pm=1, hour-12
  end
}

; t='HH:MM XM'

function parse.time, t, hour, minute
  locals pm
  get.hour
  text.attach.n t, hour, 2
  text.attach.c t, ':'
  text.attach.n t, minute, 2
  text.attach.c t, ' '
  if not pm
    text.attach t, am.t
  else
    text.attach t, pm.t
  end
endf

; t='HH:MM:SS XM'

function parse.time.x,\
 t, hour, minute, second
  locals pm
  get.hour
  text.attach.n t, hour, 2
  text.attach.c t, ':'
  text.attach.n t, minute, 2
  text.attach.c t, ':'
  text.attach.n t, second, 2
  text.attach.c t, ' '
  if not pm
    text.attach t, am.t
  else
    text.attach t, pm.t
  end
endf

; t='MM/DD/YEAR'

function parse.date, t, month, day, year
  . r0=t, *r0=0
  text.attach.n t, month, 2
  text.attach.c t, '/'
  text.attach.n t, day, 2
  text.attach.c t, '/'
  text.attach.n t, year, 4
endf

; get current time & date text

macro get.time.t t {
 parse.time.x t, time.hour,\
  time.minute, time.second
}

macro get.date.t t {
 parse.date t, time.month,\
  time.day, time.year
}

; t='HH:MM:SS XM', RETURN,
; 'MM/DD/YEAR'. note: get/parse
; time returns end (text.attach)

function get.time.date.t, t
  get.time.t t
  . *r0++=0Dh, *r0++=0Ah
  get.date.t r0
endf