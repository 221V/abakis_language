; $$$$$$$$$$$$$$$ Z77 ASM LIBRARY $$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ????????????????? MACHINE.INC ??????????????????

; write machine code instructions directly to
; memory at runtime

ip equ omega

_opcode fix m.write.c ; byte
_byte	fix m.write.c ; byte
_i16	fix m.write.w ; wide byte
_number fix m.write.n ; standard 32BIT number

; class INSTRUCTION
;  integer opcode, mode, sib, address, number,\
;   opcode.n, mode.n, sib.n, address.n, number.n
; endc ?in

numeric \
 _EAX, _ECX, _EDX, _EBX,\
 _ESP, _EBP, _ESI, _EDI

numeric \
 _AL, _CL, _DL, _BL,\
 _AH, _CH, _DH, _BH

numeric \ ; cccn
 _JO, _JNO, _JNAE, _JAE,\
 _JZ, _JNZ, _JNA, _JA,\
 _JS, _JNS, _JP, _JNP,\
 _JNGE, _JGE, _JNG, _JG

;;;;;;;;;;;;;;;;; MODE, [S*I+B] ;;;;;;;;;;;;;;;;;;

macro _mode a, b, c { ; b is /x opcode extension
 triplet a, b, c
 _byte eax
}

_sib fix _mode

;;;;;;;;;;;;;; ADDRESS/DISPLACEMENT ;;;;;;;;;;;;;;

; write address of label name

macro _address a {
 address? a
 _number eax
}

; calculate relative jmp/call offset (backwards)

function _offset, a, n
 address? a
 . ecx=eax, eax=ip, edx=401000h, edx-200h,\
  eax+edx, eax-ecx, eax+n, -eax
endf

;;;;;;;;;;;;;; INSTRUCTION FORMATS ;;;;;;;;;;;;;;;

macro _opcodes [b] { forward _opcode b }

function opcode_x, o, a, b, c, x, y, z, m, i
  _opcode_ o
  _mode a, b, c
  _sib x, y, z
  _address m
  _number i
endf

; universal macro that converts any instruction

macro _opcode_ [p] {
 forward
  define ?s 0
  match =0 o+r, ?s p \{   ; +r, +cccn
   let eax=o, eax+r
   _opcode eax
   define ?s 1
  \}
  match =0 a:b:c, ?s p \{ ; mm:rrr:mmm, ss:iii:bbb
   _mode a, b, c
   define ?s 1
  \}
  match =0 a==b, ?s p \{
   IF b=8		  ; 8/16/32BIT immediate
    _byte a
   ELSE IF b=16
    _i16 a
   ELSE IF b=32
    _number a
   ELSE IF b='a'	  ; memory address/reference
    _address a
   ELSE
    _offset a, b	  ; calculate relative offset
    IF b=1
     _byte eax		  ; 1 byte displacement
    ELSE
     _number eax	  ; 4 byte displacement
    END IF
   END IF
   define ?s 1
  \}
  IF ?s eq 0		  ; else, just opcode
   IF p eqtype 0
    _opcode p
   END IF
  END IF
}

macro _opcode_xr o, x, r     { _opcode_ o, 11b:x:r }
macro _opcode_rr o, x, r     { _opcode_xr o, r, x }
macro _opcode_2xr x, y, a, b { _opcode_ x, y, 11b:a:b }
macro _opcode_2rr x, y, b, a { _opcode_2xr x, y, b, a }
macro _opcode_2rm x, y, r, m { _opcode_ x, y, 0:r:101b, m='a' }
macro _opcode_ri o, r, i, n  { _opcode_ o, 11b:r:r, i=n }
macro _opcode_xm o, x, m     { _opcode_ o, 0:x:101b, m='a' }
macro _opcode_rm o, r, m     { _opcode_xm o, r, m }

macro _opcode_xmi o, x, m, i, n
 { _opcode_ o, 0:x:101b, m='a', i=n }

;;;;;;;;;;;;;;;;;;;; PREFIXES ;;;;;;;;;;;;;;;;;;;;

macro _oso  { _opcode 66h }
macro _aso  { _opcode 67h }
macro _rep  { _opcode 0F3h }

macro _nop  { _opcode 90h }
macro _bug  { _opcode 0CCh } ; int3 breakpoint

;;;;;;;;;;;;;;;; DATA: MOVE, COPY ;;;;;;;;;;;;;;;;

macro _rep_movsb { _opcodes 0F3h, 0A4h }
macro _rep_movsd { _opcodes 0F3h, 0A5h }
macro _rep_stosb { _opcodes 0F3h, 0AAh }
macro _rep_stosd { _opcodes 0F3h, 0ABh }

macro _push_r32 r { _opcode_ 50h+r }
macro _push_i8 i  { _opcode_ 6Ah, i=8 }
macro _push_i32 i { _opcode_ 68h, i=32 }
macro _push_m32 m { _opcode_xm 0FFh, 6, m }
macro _push_a32 m { _opcode_ 68h, m='a' }

macro _pop_r32 r { _opcode_ 58h+r }
macro _pop_m32 m { _opcode_xm 08Fh, 0, m='a' }

macro _mov_r8_r8 a, b { _opcode_rr 88h, a, b }
macro _mov_r8_m8 r, m { _opcode_xm 8Ah, r, m }
macro _mov_r8_i8 r, i { _opcode_ 0B0h+r, i=8 }
macro _mov_m8_i8 m, i { _opcode_xmi 0C6h, 0, m, i, 8 }
macro _mov_al_m8 m    { _opcode_ 0A0h, m='a' }
macro _mov_m8_al m    { _opcode_ 0A2h, m='a' }

macro _mov_r32_r32 a, b { _opcode_rr 89h, a, b }
macro _mov_r32_m32 r, m { _opcode_xm 8Bh, r, m }
macro _mov_r32_i32 r, i { _opcode_ 0B8h+r, i=32 }
macro _mov_m32_i32 m, i { _opcode_xmi 0C7h, 0, m, i, 32 }
macro _mov_eax_m32 m	{ _opcode_ 0A1h, m='a' }
macro _mov_m32_eax m	{ _opcode_ 0A3h, m='a' }

macro _movsx_r32_m8 r, m  { _opcode_2rm 0Fh, 0BEh, r, m }
macro _movsx_r32_m16 r, m { _opcode_2rm 0Fh, 0BFh, r, m }
macro _movzx_r32_m8 r, m  { _opcode_2rm 0Fh, 0B6h, r, m }
macro _movzx_r32_m16 r, m { _opcode_2rm 0Fh, 0B7h, r, m }

macro _lea_r32_m r, m { _opcode_rm 8Dh, r, m }

;;;;;;;;;;;;;;;;;;;; TRANSFER ;;;;;;;;;;;;;;;;;;;;

macro _jmp_r32 r { _opcode_xr 0FFh, 4, r }
macro _jmp_m32 m { _opcode_xm 0FFh, 4, m }
macro _jmp_i8 i  { _opcode_ 0EBh, i=8 }
macro _jmp_i32 i { _opcode_ 0E9h, i=32 }
macro _jmp_o8 a  { _opcode_ 0EBh, a=1 }
macro _jmp_o32 a { _opcode_ 0E9h, a=4 }

macro _jcc_o8 c, a  { _opcode_ 70h+c, a=1 }
macro _jcc_o32 c, a { _opcode_ 0Fh, 80h+c, a=4 }

macro _jz_8 a	{ _jcc_o8 _JZ, a }
macro _jz_32 a	{ _jcc_o32 _JZ, a }
macro _jnz_8 a	{ _jcc_o8 _JNZ, a }
macro _jnz_32 a { _jcc_o32 _JNZ, a }

macro _call_i32 i { _opcode_ 0E8h, i=32 }
macro _call_m32 m { _opcode_xm 0FFh, 2, m }
macro _call_o32 a { _opcode_ 0E8h, a=4 }

macro _ret    { _opcode 0C3h }
macro _retn n { _opcode_ 0C2h, n=16 }

;;;;;;;;;;;;;;;;;;; ARITHMETIC ;;;;;;;;;;;;;;;;;;;

macro _inc_r8 r  { _opcode_xr 0FEh, 0, r }
macro _inc_m8 m  { _opcode_xm 0FEh, 0, m }
macro _inc_r32 r { _opcode_ 40h+r }
macro _inc_m32 m { _opcode_xm 0FFh, 0, m }

macro _dec_r8 r  { _opcode_xr 0FEh, 1, r }
macro _dec_m8 m  { _opcode_xm 0FEh, 1, m }
macro _dec_r32 r { _opcode_ 48h+r }
macro _dec_m32 m { _opcode_xm 0FFh, 1, m }

macro _shl_r32_i8 r, i { _opcode_ 0C1h, 11b:4:r, i=8 }
macro _shl_m32_i8 m, i { _opcode_ 0C1h, 0:4:101b, m='a', i=8 }

macro _shr_r32_i8 r, i { _opcode_ 0C1h, 11b:5:r, i=8 }
macro _shr_m32_i8 m, i { _opcode_ 0C1h, 0:5:101b, m='a', i=8 }

macro _sar_r32_i8 r, i { _opcode_ 0C1h, 11b:7:r, i=8 }
macro _sar_m32_i8 m, i { _opcode_ 0C1h, 0:7:101b, m='a', i=8 }

macro _neg_r32 r { _opcode_xr 0F7h, 3, r }
macro _not_r32 r { _opcode_xr 0F7h, 2, r }

macro _imul_r32 r	 { _opcode_xr 0F7h, 5, r }
macro _imul_r32_r32 a, b { _opcode_2rr 0Fh, 0AFh, a, b }
macro _imul_r32_m32 r, m { _opcode_2rm 0Fh, 0AFh, r, m }
macro _imul_r32_i32 r, i { _opcode_ri 69h, r, i, 32 }
macro _imul_r32_i8 r, i  { _opcode_ri 6Bh, r, i, 8 }
macro _mul_r32 r	 { _opcode_xr 0F7h, 4, r }
macro _mul_m32 m	 { _opcode_xm 0F7h, 4, m }

macro _idiv_r32 r { _opcode_xr 0F7h, 7, r }
macro _idiv_m32 m { _opcode_xm 0F7h, 7, m }
macro _div_r32 r  { _opcode_xr 0F7h, 6, r }
macro _div_m32 m  { _opcode_xm 0F7h, 6, m }

macro _basic_x86 o, c {
 ?x=c shr 3 and 111b
 macro _#o#_al_i8 i	 \{ _opcode_ c+4, i=8 \}
 macro _#o#_r8_r8 r, s	 \{ _opcode_rr c, r, s \}
 macro _#o#_r8_m8 r, m	 \{ _opcode_rm c+2, r, m \}
 macro _#o#_r8_i8 r, i	 \{ _opcode_ 80h, 11b:?x:r, i=8 \}
 macro _#o#_m8_r8 m, r	 \{ _opcode_ c, 0:r:101b, m='a' \}
 macro _#o#_m8_i8 m, i	 \{ _opcode_ 80h, 0:?x:101b, m='a', i=8 \}
 macro _#o#_eax_i32 i	 \{ _opcode_ c+5, i=32 \}
 macro _#o#_r32_r32 r, s \{ _opcode_rr c+1, r, s \}
 macro _#o#_r32_m32 r, m \{ _opcode_rm c+3, r, m \}
 macro _#o#_r32_i8 r, i  \{ _opcode_ 83h, 11b:?x:r, i=8 \}
 macro _#o#_r32_i32 r, i \{ _opcode_ 81h, 11b:?x:r, i=32 \}
 macro _#o#_m32_r32 m, r \{ _opcode_ c+1, 0:r:101b, m='a' \}
 macro _#o#_m32_i32 m, i \{ _opcode_ 81h, 0:?x:101b, m='a', i=32 \}
}

; defines 104 (8*13) instructions...

_basic_x86 add, 00h
_basic_x86 or,	08h
_basic_x86 adc, 10h
_basic_x86 sbb, 18h
_basic_x86 and, 20h
_basic_x86 sub, 28h
_basic_x86 xor, 30h
_basic_x86 cmp, 38h

;;;;;;;;;;;;;;;;;;; VARIABLE ;;;;;;;;;;;;;;;;;;;;;

; create LL internal variable/s with just a
; label and data. no type or scope

macro _integer [p] {
 forward
  match name==value, p \{
   create.label name
   m.write.n value
  \}
}

macro _text [p] {
 forward
  match name==value, p \{
   create.label name
   m.write.string value
  \}
}

;;;;;;;;;;;;;;;;;;; PROCEDURE ;;;;;;;;;;;;;;;;;;;;

; create LL procedure. HL code must manage names,
; sizes and ebp +/- offsets. p/l are parameters
; and locals sizes or 0 IF none. note: this is
; much easier to do at runtime than with macros

function _procedure, name, p, l
  call \
   !create.label, name
  . eax=p, eax|l	  ; parameters
  IF true		  ; or locals?
    _push_r32 _EBP	  ; prologue
    _mov_r32_r32 \
     _EBP, _ESP
  END
  IF l			  ; locals?
    _sub_r32_i32 _ESP, l
  END
endf

function _endp, p, l
  locals s
  let eax=p, eax|l	  ; parameters
  IF true		  ; or locals?
    _mov_r32_r32 \	  ; epilogue
     _ESP, _EBP
    _pop_r32 _EBP
    _retn p		   ; retn size
  ELSE
    _ret
  END
endf

; load stack parameter or local by offset
; (x=i*4) to/from 32BIT register.
; x=positive for parameters, negative
; for locals. a/address? 0=mov/value
; or 1=lea/address. invalid for _put_s

function _get_s, r, x, a ; r=[ebp+/-x]
  IF a
    _opcode 8Dh ; lea r, m
  ELSE
    _opcode 8Bh ; mov r, m
  END
  _sib 10b, r, 101b
  _number x
endf

function _put_s, r, x ; [ebp+/-x]=r
  _opcode 89h	       ; mov m, r
  _sib 10b, r, 101b
  _number x
endf

; example procedure that accepts 2 parameters

; text inc.add='?p0'	    ; name

; _PS=2*4		    ; sizes of parameters
; _LS=0 		    ; and locals

; _procedure inc.add,\	    ; !inc.add(a,b)
; _PS, _LS
; _get_s _EAX, (0*4)+_PS, 0 ; mov eax, [ebp+8]
; _get_s _ECX, (1*4)+_PS, 0 ; mov ecx, [ebp+12]
; _inc_r32 _EAX
; _add_r32_r32 _EAX, _ECX
; _endp _PS, _LS

; _push 4	  ; example call...
; _push 5
; _call_a inc.add ; returns 10

; equivelant:

; function inc.add, a, b
;   let eax=a, ecx=b, eax++, eax+ecx
; endf

; output:

; 004010EC 55		push ebp
; 004010ED 89E5 	mov  ebp, esp
; 004010EF 8B8508000000 mov  eax, [ebp+8]
; 004010F5 8B8D0C000000 mov  ecx, [ebp+12]
; 004010FB 40		inc  eax
; 004010FC 01C8 	add  eax, ecx
; 004010FE 89EC 	mov  esp, ebp
; 00401100 5D		pop  ebp
; 00401101 C20800	retn 8