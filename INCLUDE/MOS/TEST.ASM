; test 6502 instructions

format binary as 'BIN'
include 'cpu.inc'

lda =0
beq @f

lda =$44     ; immediate
lda $AA      ; zero page
lda $7F, x   ; zero page, x
lda $4FFF    ; absolute
lda $88BB, x ; absolute, x
lda $24EE, y ; absolute, y
lda ($AC, x) ; (indirect, x)
lda ($DC), y ; (indirect), y
@@:

ldx =$44     ; immediate
ldx $80      ; zero page
ldx $80, y   ; zero page, y
ldx $AAAA    ; absolute
ldx $AAAA, y ; absolute, y

ldy =$44     ; immediate
ldy $80      ; zero page
ldy $80, x   ; zero page, x
ldy $AAAA    ; absolute

sta $AA      ; zero page
sta $7F, x   ; zero page, x
sta $4FFF    ; absolute
sta $88BB, x ; absolute, x
sta $24EE, y ; absolute, y
sta ($AC, x) ; (indirect, x)
sta ($DC), y ; (indirect), y
bne @f

stx $80      ; zero page
stx $80, y   ; zero page, y
stx $AAAA    ; absolute

sty $80      ; zero page
sty $80, x   ; zero page, x
sty $AAAA    ; absolute

inc $80      ; zero page memory
inc $80, x   ; zero page, x
inc $A000    ; absolute memory
inc $8000, x ; absolute, x

dec $80      ; zero page memory
dec $80, x   ; zero page, x
dec $A000    ; absolute memory
dec $8000, x ; absolute, x

adc =$44     ; immediate
adc $AA      ; zero page
adc $7F, x   ; zero page, x
adc $4FFF    ; absolute
adc $88BB, x ; absolute, x
adc $24EE, y ; absolute, y
adc ($AC, x) ; (indirect, x)
adc ($DC), y ; (indirect), y

sbc =$44     ; immediate
sbc $AA      ; zero page
sbc $7F, x   ; zero page, x
sbc $4FFF    ; absolute
sbc $88BB, x ; absolute, x
sbc $24EE, y ; absolute, y
sbc ($AC, x) ; (indirect, x)
sbc ($DC), y ; (indirect), y

cmp =$44     ; immediate
cmp $AA      ; zero page
cmp $7F, x   ; zero page, x
cmp $4FFF    ; absolute
cmp $88BB, x ; absolute, x
cmp $24EE, y ; absolute, y
cmp ($AC, x) ; (indirect, x)
cmp ($DC), y ; (indirect), y

cpx =$88     ; immediate
cpx $BB      ; zero page
cpx $3CCC    ; absolute
cpy =$88     ; immediate
cpy $BB      ; zero page
cpy $3CCC    ; absolute

and =$44     ; immediate
and $AA      ; zero page
and $7F, x   ; zero page, x
and $4FFF    ; absolute
and $88BB, x ; absolute, x
and $24EE, y ; absolute, y
and ($AC, x) ; (indirect, x)
and ($DC), y ; (indirect), y

ora =$44     ; immediate
ora $AA      ; zero page
ora $7F, x   ; zero page, x
ora $4FFF    ; absolute
ora $88BB, x ; absolute, x
ora $24EE, y ; absolute, y
ora ($AC, x) ; (indirect, x)
ora ($DC), y ; (indirect), y

eor =$44     ; immediate
eor $AA      ; zero page
eor $7F, x   ; zero page, x
eor $4FFF    ; absolute
eor $88BB, x ; absolute, x
eor $24EE, y ; absolute, y
eor ($AC, x) ; (indirect, x)
eor ($DC), y ; (indirect), y

asl          ; accumulator (implied)
asl a        ; accumulator (optional)
asl $80      ; zero page
asl $80, x   ; zero page, x
asl $A000    ; absolute
asl $8000, x ; absolute, x

lsr          ; accumulator (implied)
lsr a        ; accumulator (optional)
lsr $80      ; zero page
lsr $80, x   ; zero page, x
lsr $A000    ; absolute
lsr $8000, x ; absolute, x

rol          ; accumulator (implied)
rol a        ; accumulator (optional)
rol $80      ; zero page
rol $80, x   ; zero page, x
rol $A000    ; absolute
rol $8000, x ; absolute, x

ror          ; accumulator (implied)
ror a        ; accumulator (optional)
ror $80      ; zero page
ror $80, x   ; zero page, x
ror $A000    ; absolute
ror $8000, x ; absolute, x

bit $50      ; zero page
bit $4000    ; absolute

beq @f
bne @f
bcc @f
bcs @f
bvc @f
bvs @f
bmi @f
bpl @f
@@:

brk
clc
cld
cli
clv
dex
dey
inx
iny
nop
pha
php
pla
plp
rti
rts
sec
sed
sei
tax
tay
tsx
txa
txs
tya