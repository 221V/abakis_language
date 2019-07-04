;;;;;;;;;;;;;;;;;;;;; PRINT ;;;;;;;;;;;;;;;;;;;;;;

; print t, f, ... - a fast print formatted
; text to buffer. intel specific

; %t %s - 'text'; "string"
; %c    - character
; %n %u - 32BIT decimal unsigned
; %b    - 32BIT binary
; %h    - 32BIT hexadecimal
; %r    - return. insert 0Dh, 0Ah
; %0    - 0/NULL
; %%    - %

macro print t, f, [p] {
  common callv !print, t, f, p
}

; note: text.copy/reverse/etc and x2t
; must return end (*p=0) address for this

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

align integer BASE='n'

; !print(n, $t, $f, ...)

!print:
 alias \               ; aliases...
  s=r6, p=r7,\         ; source, destiny
  c=r0, i=r1,\
  va.s=r2,\            ; &va[...] start
  va.n=[esp+4],\       ; # va
  va=[r2+r1*4],\       ; current va[i]
  n=[.a], z=[.z]       ; temporary

  . (u32) c=*(esp+8)   ; text buffer
  . (u32) i=*(esp+12)  ; format
  . va.s=&(esp+16)     ; &va[...] start

  push s p
  . p=c, s=i, i=0      ; index=0

  .get: . c=*s++       ; get c/haracter
  if c=0, go .e, end   ; end? *s=0
  if c<>'%'            ; not %
    .1: . *p++=c       ; copy c
    go .get            ; next
  end
  . c=*s++             ; %x
  if c='%', go .1      ; 2 %% = %
  end

  if c='0', *p++=0     ; '0' = 0/NULL
    go .get
  end

  if c='r'             ; 'r' = return
    .  *p++=\
     0A0Dh, go .get
  end

  if c='c'             ; %c
    . c=va, i++        ; get va[i++]
    . *p++=c
    go .get
  end

  if c='t'
    .t:                ; %t %s - text
    . z=va, i++        ; get va[i++]
    pusha
    get z=\
     text.copy p, z    ; copy, save &
    popa
    . p=z              ; advance text buffer
    go .get
  end
  if c='s', go .t
  end

  if c='n', go .n, end
  if c='u', go .n, end
  if c='h', go .n, end
  if c='b', go .n, end

  go .x                ; unrecognized

  .n:
  . n=va, i++          ; get va[i]
  pusha                ; copy/convert
  if c='n'             ; number to text
    u2t n, p           ; in selected base
  else.if c='u'
    u2t n, p
  else.if c='h'
    h2t n, p
  else.if c='b'
    b2t n, p
  end
  . z=r0               ; save end address
  popa
  . p=z                ; advance text buffer
  go .get

  .x: . *p++='?'       ; else, unrecognized,
  go .get              ; replace with '?'

  .0: . r0=0, go .q    ; error
  .e: . r0=p, *r0=0    ; success. terminate
  .q:                  ; return end
  pop p s
 ret                   ; callv adjusts esp after

endal                  ; end alias

.a dd 0
.z dd 0