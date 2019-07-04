; TEST MAGIC-ARM ASSEMBLER.
; SEE ARM.INC AND DISASSEMBLY:
; TEST.TXT

include 'z.inc'

.a: @beq .d
@nop
.b: @bgt .c
@nop
.c: @blt .b
@nop
.d: @bmi .a

@ldi r3, -1
@ldi r3, 0ABh
@ldi r3, 07F0h
@ldi r3, 07F00h
@ldi r3, 07F000h
@ldi r3, 07F0000h
@ldi r3, 07F00000h
@ldi r3, 0FF000000h
@ldi r3, 080000000h
@ldi r3, 1234h

@mrs r7, CPSR
@mrs r7, SPSR
@msr CPSR_f, r5
@msr CPSR_s, r5
@msr CPSR_x, r5
@msr CPSR_c, r5
@msr CPSR_fs, r5
@msr CPSR_fsxc, r5
@msr SPSR_f, r5
@msr SPSR_s, r5
@msr SPSR_x, r5
@msr SPSR_c, r5
@msr SPSR_fs, r5
@msr SPSR_fsxc, r5

@ldr r0, [123h]
@ldr r0, [-123h]
@ldr r0, [r1]
@ldr r0, [r1], 1
@ldr r0, [r1], -1
@ldr r0, [r1, r2]
@ldr r0, [r1, 1]
@ldr r0, [r1, -1]
@ldr r0, [@15, 0ABCh]
@ldr r0, [@15, -0ABCh]
@ldr r0, [r7, 0ABCh]
@ldr r0, [r7, -0ABCh]
@ldr r0, [r1, r2, lsl 3]
@ldr r0, [r1, 777h]!
@ldr r0, [r1, -777h]!
@ldr r0, [r1, r2]!
@ldr r0, [r1, r2, lsl 3]!

@ldrb r5, [r7]
@strb r5, [r7]
@ldrsb r2, [48h]
@ldrsb r2, [r7]
@ldrsh r2, [r7]
@ldrh r2, [r7]
@ldrd r2, [r7]
@strd r2, [r7]
@ldrsb r2, [r7], 48h
@ldrsb r2, [r7], -48h
@ldrsb r2, [r7], r3
@ldrsb r2, [r7], -r5
@ldrsb r2, [@pc, r5]
@ldrsb r2, [@pc, 48h]
@ldrsb r2, [r7, r5]
@ldrsb r2, [r7, 48h]
@ldrsb r2, [r7, -48h]
@ldrsb r2, [r7, 48h]!
@ldrsb r2, [r7, -48h]!

@push r0-r3, r5, r6, r7, v7-v8, @lr
@pop r0-r3, r5, r6, r7, v7-v8, @pc

@mov r7, r7	; a=b
@and r5, 0FFh	; a=a&c
@eor r5, r6, 15 ; a=b^c
@sub r5, r6, r7 ; a=b-c
@rsb r5, r6, r7 ; a=c-b
@add r5, r6, r7 ; a=b+c
@adc r5, r6, r7 ; a=b+c
@sbc r5, r6, r7 ; a=b-c
@rsc r5, r6, r7 ; a=c-b
@tst r5, r6	; a&b?
@teq r5, r6	; a&b?
@cmp r5, r6	; a=b?
@cmn r5, r6	; a=-b?
@orr r5, r6, r7 ; a=b|c
@bic r5, r6, r7 ; a=b&~c
@mvn r5, r6	; a=-b

@add r5, r6, lsl r7	   ; a=b<<c
@add r5, r6, lsl 7	   ; a=b<<i
@add r0, r3, r5, lsr r7    ; a=b+(c>>>d)
@adds r0, r3, r5, lsr 7    ; a=b+(c>>>i)
@add:ne r0, r1, asr r7	   ; ne? a=(b>>c)
@adds:lt r0, r1, asr 7	   ; lt? a=(b>>i)
@add:gt r0, r3, r5, ror r7 ; gt? a=b+(c<>>d)
@adds:mi r0, r3, r5, ror 7 ; mi? a=b+(c<>>7)

@qadd r3, r5, r7
@qsub r3, r5, r7
@qdadd r3, r5, r7
@qdsub r3, r5, r7
@ssat r1, 15, r5
@ssat r1, 15, r5, asr 12
@ssat r1, 15, r5, lsl 12
@usat r1, 15, r5
@usat r1, 15, r5, asr 12
@usat r1, 15, r5, lsl 12

@mul r0, r1, r5
@mla r1, r3, r5, r7
@mls r1, r3, r5, r7
@umull r1, r3, r5, r7
@smull r1, r3, r5, r7
@umlal r1, r3, r5, r7
@smlal r1, r3, r5, r7
@umaal r1, r3, r5, r7
@smulbb r1, r3, r5
@smultt r1, r3, r5
@smulbt r1, r3, r5
@smultb r1, r3, r5
@smlabb r1, r3, r5, r7
@smlatt r1, r3, r5, r7
@smlabt r1, r3, r5, r7
@smlatb r1, r3, r5, r7
@smlalbb r1, r3, r5, r7
@smlaltt r1, r3, r5, r7
@smlalbt r1, r3, r5, r7
@smlaltb r1, r3, r5, r7
@smlad r1, r3, r5, r7
@smlsd r1, r3, r5, r7
@smlald r1, r3, r5, r7
@smlsld r1, r3, r5, r7
@smuad r1, r3, r5
@smusd r1, r3, r5
@smuadx r1, r3, r5
@smusdx r1, r3, r5
@smulwb r1, r3, r5
@smulwt r1, r3, r5
@smlawb r1, r3, r5, r7
@smlawt r1, r3, r5, r7

@bfi r1, r3, 4, 7
@bfi r1, r3, 4, 7
@sbfx r1, r3, 4, 7
@ubfx r1, r3, 4, 7

@sxtb r1, r3
@sxtb r1, r3, ror 16
@sxth r1, r3
@sxth r1, r3, ror 16
@sxtb16 r1, r3
@sxtb16 r1, r3, ror 16
@sxtab r1, r3, r7
@sxtab r1, r3, r7, ror 16
@sxtah r1, r3, r7
@sxtah r1, r3, r7, ror 16
@sxtab16 r1, r3, r7
@sxtab16 r1, r3, r7, ror 16
@uxtb r1, r3
@uxtb r1, r3, ror 16
@uxth r1, r3
@uxth r1, r3, ror 16
@uxtb16 r1, r3
@uxtb16 r1, r3, ror 16
@uxtab r1, r3, r7
@uxtab r1, r3, r7, ror 16
@uxtah r1, r3, r7
@uxtah r1, r3, r7, ror 16
@uxtab16 r1, r3, r7
@uxtab16 r1, r3, r7, ror 16

@pkhbt r2, r4, r7
@pkhtb r2, r4, r7
@pkhbt r2, r4, r7, lsl 8
@pkhtb r2, r4, r7, asr 8

@sadd8 r1, r3, r5
@sadd16 r1, r3, r5
@ssub8 r1, r3, r5
@ssub16 r1, r3, r5
@uadd8 r1, r3, r5
@uadd16 r1, r3, r5
@usub8 r1, r3, r5
@usub16 r1, r3, r5
@shadd8 r1, r3, r5
@shadd16 r1, r3, r5
@shsub8 r1, r3, r5
@shsub16 r1, r3, r5
@uhadd8 r1, r3, r5
@uhadd16 r1, r3, r5
@uhsub8 r1, r3, r5
@uhsub16 r1, r3, r5
@sasx r1, r3, r5
@ssax r1, r3, r5
@shasx r1, r3, r5
@shsax r1, r3, r5
@uasx r1, r3, r5
@usax r1, r3, r5
@uhasx r1, r3, r5
@uhsax r1, r3, r5
@usad8 r1, r3, r7
@usada8 r1, r3, r5, r7
@ssat16 r1, 8, r3
@usat16 r1, 8, r3
@qasx r1, r3, r5
@qsax r1, r3, r5
@uqasx r1, r3, r5
@uqsax r1, r3, r5
@qadd8 r1, r3, r5
@qadd16 r1, r3, r5
@qsub8 r1, r3, r5
@qsub16 r1, r3, r5
@uqadd8 r1, r3, r5
@uqadd16 r1, r3, r5
@uqsub8 r1, r3, r5
@uqsub16 r1, r3, r5

@swp r3, r5, [r7]
@swpb r0, r1, [r2]
@rev r5, r7
@rev16 r5, r7
@revsh r5, r7
@rbit r5, r7