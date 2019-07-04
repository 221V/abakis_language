; $$$$$$$$$$$$ JASM : JAVA ASSEMBLER $$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? JASM.INC ???????????????????? 

macro syntax v { define ?s v }

macro verify e {
 if ?s eq 0
  'Error: ' e
 end if
}

;;;;;;;;;;;;;;;;;;; VARIABLES ;;;;;;;;;;;;;;;;;;;;

; define variable/s of type...

macro ?DV type, [p] {
 forward
  syntax 0
  match name==value, p \{
   name type value
   syntax 1
  \}
  match =0 name, ?s p \{
   name type 0
   syntax 1
  \}
  verify type
}

macro utf8 [p]
 { common ?DV constant_utf8, p }

macro integer [p]
 { common ?DV constant_integer, p }

macro float [p]
 { common ?DV constant_float, p }

macro string [p] {
 forward
  match name==value, p \{
   _\#name\#_ constant_utf8 value
   name constant_string _\#name\#_
  \}
}

macro cclass [p]
 { common ?DV constant_class, p }

macro module p {
 _#p#_ constant_utf8 `p
 p constant_class _#p#_
}

; push immediate...

macro ipush i {
 if i=-1 | i=0FFFFh   ; -1/0FFFFh
  db 2                ; iconst_m1
 else if i>=0 & i<=5  ; 0 to 5
  db 3+i              ; iconst_0-5
 else if i>=-128 \    ; -128 to 127
  & i<=127
  db 10h, i           ; bipush i8
 else if i>=-32768 \  ; -32768 to 32767
  & i<=32767
  db 11h, (i) shr 8,\ ; sipush i16
  (i) and 0FFh
 else
  'Immediate cannot be encoded'
 end if
}

; get/put s/tatic/f/ield...

macro gets x { getstatic x }
macro puts x { putstatic x }
macro getf x { getfield x }
macro putf x { putfield x }

;;;;;;;;;;;;;;;;;;;;; CLASS ;;;;;;;;;;;;;;;;;;;;;;

macro class [p] {
 common
  syntax 0
  define construct? 0
  match =0 =private name \    ; private name
   =extends super, p \{       ; extends super
   ?name equ name
   ?super equ super
   ?super.i equ super\#.init
   ?flags equ \
    ACC_PRIVATE+ACC_SUPER     ; private
   syntax 1
  \}
  match =0 =private name, \   ; private name
   ?s p \{
   ?name equ name
   ?super equ Object          ; java/lang/Object
   ?super.i equ Object.init
   ?flags equ \
    ACC_PRIVATE+ACC_SUPER     ; private
   syntax 1
  \}
  match =0 name \             ; name
   =extends super,\           ; extends super
   ?s p \{
   ?name equ name
   ?super equ super
   ?super.i equ super\#.init
   ?flags equ \
    ACC_PUBLIC+ACC_SUPER     ; public
   syntax 1
  \}
  match =0 name, ?s p \{     ; default
   ?name equ name
   ?super equ Object         ; java/lang/Object
   ?super.i equ Object.init
   ?flags equ \
    ACC_PUBLIC+ACC_SUPER
   syntax 1
  \}
 verify class

 ; class begins...

 u2 ?flags      ; access flags
 u2 ?name       ; this class
 u2 ?super      ; super class

 interfaces     ; required...
 end_interfaces
 fields
 end_fields

 methods        ; methods begin...
 ; ...          ; method ... endm
}

macro endc {   ; end class

 ; if no constructor, create generic
 ; void .init()

 if construct? eq 0
  method_info ACC_PUBLIC, _init, _VV
   _begin 8, 8
   aload 0          ; this
   calls ?super.i() ; super.init()
  endm
 end if
 end_methods
 attributes
 end_attributes
 restore ?name, ?super, ?super.i, ?flags
}

macro _begin s, l {
 attribute _Code
  u2 s              ; maximum stack size
  u2 l              ; maximum number of locals
  bytecode
   ; ...
}

;;;;;;;;;;;;;;;;;;;; METHODS ;;;;;;;;;;;;;;;;;;;;;

macro callx type, [p] {
 common match method(), p \{
  invoke\#type method \} }

macro call [p] { common callx virtual, p }

macro calls [p] { common callx special, p }

_public  = ACC_PUBLIC
_private = ACC_PRIVATE
_static  = ACC_STATIC

; create method...

macro method [p] {
 common
  ?rt equ               ; return type
  syntax 0
  match =create(), p \{ ; method create()
   method_info \        ; init()
    _public, _init, _VV
   _begin 8, 8
   aload 0              ; this
   calls ?super.i()     ; super.init
   define construct? 1  ; yes, has one
   syntax 1
  \}
  match =0 =main(), ?s p \{
   method_info \
    _public+_static, main, _ASV
   _begin 8, 8
   syntax 1
  \}
  match =0 =main(=String[] a), ?s p \{
   method_info \
    _public+_static, main, _ASV
   _begin 8, 8
   syntax 1
  \}
  match =0 =paint(=Graphics g), ?s p \{
   method_info _public, _paint, _GV
   _begin 8, 8
   syntax 1
  \}
  match =0 name(), ?s p \{
   method_info \
    _public+_static, name, _VV
   _begin 8, 8
   syntax 1
  \}
  verify method
}

macro endm {       ; end method
   ; ...
   ; return#?rt      ; return type
   return
   restore ?rt
   end_bytecode
   exceptions
   end_exceptions
   attributes
   end_attributes
  end_attribute
 end_method_info
}

macro System.out.println s {
 gets System.out
 ldc s
 call PrintStream_println()
}

;;;;;;;;;;;;;;;;;; IF STATEMENT ;;;;;;;;;;;;;;;;;;

; .if =   ; if_icmpne .end
;   ; ...
; .end

; jump if NOT condition to l

macro .jifn l, [c] {
 common
  syntax 0
  match =0 ==, ?s c  \{ if_icmpne l ; if =
   syntax 1 \}
  match =0 =0, ?s c  \{ if_icmpne l ; if 0
   syntax 1 \}
  match =0 <>, ?s c  \{ if_icmpeq l ; if <>
   syntax 1 \}
  match =0 =!0, ?s c \{ if_icmpeq l ; if !0
   syntax 1 \}
  match =0 >==, ?s c \{ if_icmplt l ; if >=
   syntax 1 \}
  match =0 >, ?s c   \{ if_icmple l ; if >
   syntax 1 \}
  match =0 <==, ?s c \{ if_icmpgt l ; if <=
   syntax 1 \}
  match =0 <, ?s c   \{ if_icmpge l ; if <
   syntax 1 \}

 verify 'Invalid expression'
}

; HL IF/ELSE

macro .if.begin {
 local ..start, ..else, ..end
 ?IF equ
 ?START equ ..start
 ?ELSE equ ..else
 ?END equ ..end
 ?START:
}

macro .if [c] {
 common
  .if.begin
  .jifn ?ELSE, c
}

macro .else {
 jmp ?END
 ?ELSE:
 restore ?IF
 ?IF equ ,
}

macro .else.if [c] {
 common
  jmp ?END
  ?ELSE:
  restore ?ELSE
  local ..else
  ?ELSE equ ..else
  .jifn ?ELSE, c
}

macro .end {
 if ?IF eq
  ?ELSE:
 end if
 ?END:
 restore ?IF, ?START, ?ELSE, ?END
}

;;;;;;;;;;;;;;;;; .CLASS BEGINS ;;;;;;;;;;;;;;;;;;

u4 0CAFEBABEh ; magic
u2 0, 49      ; version: minor, major

;;;;;;;;;;;;;;;;; CONSTANT POOL ;;;;;;;;;;;;;;;;;;

macro constants {
constant_pool

utf8 _Code='Code', main='main',\
 _init='<init>'

include 'include\import.inc' ; import table

; parameters and return types...

utf8 \
 _VV='()V', _ZV='(Z)V', _IIV='(II)V',\
 _ASV='([Ljava/lang/String;)V',\
 _SV='(Ljava/lang/String;)V',\
 _CV='(Ljava/awt/Color;)V',\
 _GV='(Ljava/awt/Graphics;)V',\
 _SIIV='(Ljava/lang/String;II)V'

; class types...

utf8 _ST='Ljava/lang/String;',\
 _CT='Ljava/awt/Color;'
}

macro endcs { end_constant_pool }