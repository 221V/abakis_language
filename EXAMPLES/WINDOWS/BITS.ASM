; CHARACTER BIT STREAM

include 'a.inc'
use bit

text t(128), s(128)
text test.bs.t='Test Read/Write 1-32BITs...'
text test.bs2.t='Test Convert XBIT Stream...'

; write 11110011.00001111.00110011 to
; "bit stream" t then read from it

; read.bs/write.bs are for 1-32BIT values

; read.bs gets/returns an integer value.
; read.bs.t gets/returns binary 'text'
; in 2nd parameter with alignment

function test.bs
  log test.bs.t
  log.r
  reset.bs t
  write.bs t, 11110011b, 8
  write.bs t, 00001111b, 8
  write.bs t, 00110011b, 8

  log t        ; 11110011.00001111.00110011
  read.bs t, 7 ; 1111001, first 7BITs
  b2t r0, s
  log s
  read.bs.t \
   t, s, 11    ; 1.0000111100, next 11BITs
  log s
  log t        ; remaining, '110011'
  log.r
endf

; b2bs/bs2b; convert 'bit stream' to/from
; binary/files/images. aka, load.bs/save.bs

; example 64BIT binary:

binary: db \
 11111111b, 00000000b, 11110000b, 10111101b,\
 00000000b, 11001100b, 11111111b, 00001111b

; example 32BIT stream:

text bst='11111111001111001111000011111111'
	; 111111110011110011110000111 (27)
				   ; ALIGN 32
function test.bs2		   ; 00000
  alias p=r0
  log test.bs2.t
  log.r
  . p=binary	   ; load; convert binary bytes
  b2bs p, t, 60    ; p to 'bit stream' t
  log t 	   ; 60BITs ending with 0000
  reset.bs t	   ; reset *t=0
  bs2b bst, s, 27  ; save; convert 'bit stream'
  b2bs s, t, 27    ; to binary bytes s then
  log t 	   ; back to 'bit stream' t
  align.bs t	   ; align last byte with '0's
  log t
endf

function main
  test.bs
  test.bs2
  show.log
endf