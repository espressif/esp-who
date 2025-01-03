# include "dl_tie728_s16.S"

############################################################################################################################################################
####
#### tie728_s16_add2d_11c series
####
############################################################################################################################################################

.macro dl_tie728_s16_rescale_add_rescale_output input0, input1, output, output_scale, output_shift
    EE.ZERO.QACC
    EE.VMULAS.s16.QACC \input0, \output_scale
    EE.VMULAS.s16.QACC \input1, \output_scale
    EE.SRCMB.S16.QACC \output, \output_shift, 0
.endm


    .align 4
    .text
    .global dl_tie728_s16_add2d_11c
    .type   dl_tie728_s16_add2d_11c, @function
    # .section .iram1
dl_tie728_s16_add2d_11c:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_2x_1
    # a7: c_left_x_1

    l32i a6, a5, 68
    l32i a7, a5, 72

    blti a6, 1, dl_tie728_s16_add2d_small_channel
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16

    loopgtz a6, 0f
    EE.VLD.128.IP q2, a3, 16
    EE.VADDS.S16.LD.INCP q3, a4, q4, q0, q1
    EE.VST.128.IP q4, a2, 16

    EE.VLD.128.IP q0, a3, 16
    EE.VADDS.S16.LD.INCP q1, a4, q5, q2, q3
    EE.VST.128.IP q5, a2, 16
0:
    EE.VLD.128.IP q2, a3, 16
    EE.VADDS.S16.LD.INCP q3, a4, q4, q0, q1
    EE.VST.128.IP q4, a2, 16
    beqi a7, 1, 2f #remainder == 2*16byte
    beqi a7, 2, 3f #remainder == 3*16byte

2:
    EE.VADDS.S16 q5, q2, q3
    EE.VST.128.IP q5, a2, 16
    retw
3:
    EE.VLD.128.IP q0, a3, 16
    EE.VADDS.S16.LD.INCP q1, a4, q5, q2, q3
    EE.VST.128.IP q5, a2, 16

    EE.VADDS.S16 q4, q0, q1
    EE.VST.128.IP q4, a2, 16
    retw

dl_tie728_s16_add2d_small_channel:
    blti a7, 0, 5f
    loopgtz a7, 1f
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16
    EE.VADDS.S16 q2, q0, q1
    EE.VST.128.IP q2, a2, 16
1:
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16
    EE.VADDS.S16 q2, q0, q1
    EE.VST.128.IP q2, a2, 16
5:
    retw



    .align 4
    .text
    .global dl_tie728_s16_rescale_add2d_11c
    .type   dl_tie728_s16_rescale_add2d_11c, @function
    # .section .iram1
dl_tie728_s16_rescale_add2d_11c:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1
    # a7: input_shift
    # a8: output_scale
    # a9: output_shift

    l32i a6, a5, 64
    l32i a7, a5, 88
    l32i a8, a5, 96
    l32i a9, a5, 92

    beqi a8, 1, dl_tie728_s16_rescale_add2d_output

dl_tie728_s16_rescale_add2d_output_scale:
    s16i a8, a1, 0
    EE.VLDBC.16 q7, a1 # all output_scale

    # addi a6, a6, 1
    loopgtz a6, 1f
    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q0, q1, q1, q7, a9

    EE.VST.128.IP q1, a2, 16

1:
    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16
    EE.SRCMB.S16.QACC q1, a7, 0
    dl_tie728_s16_rescale_add_rescale_output q0, q1, q1, q7, a9

    EE.VST.128.IP q1, a2, 16
    retw

dl_tie728_s16_rescale_add2d_output:
    movi a13, 1
    s16i a13, a1, 0
    EE.VLDBC.16 q7, a1 # all 1

    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16

    loopgtz a6, 2f
    EE.SRCMB.S16.QACC q1, a7, 0
    EE.VMULAS.S16.QACC.LD.IP q0, a3, 16, q0, q7
    EE.SRCMB.S16.QACC q1, a9, 0
    EE.LDQA.S16.128.IP a4, 16
    EE.VST.128.IP q1, a2, 16
2:

    EE.SRCMB.S16.QACC q1, a7, 0
    EE.VMULAS.S16.QACC q0, q7
    EE.SRCMB.S16.QACC q1, a9, 0

    EE.VST.128.IP q1, a2, 16
    retw


    .align 4
    .text
    .global dl_tie728_s16_add2d_11c_relu
    .type   dl_tie728_s16_add2d_11c_relu, @function
    # .section .iram1
dl_tie728_s16_add2d_11c_relu:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_2x_1
    # a7: c_left_x
    # a14: activation_alpha
    # a15: activation_shift

    l32i a6, a5, 68
    l32i a7, a5, 72
    l32i a14, a5, 52
    l32i a15, a5, 60


    beqz a6, dl_tie728_s16_add2d_relu_small_channel
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16

    loopgtz a6, 0f
    EE.VLD.128.IP q2, a3, 16
    EE.VADDS.S16.LD.INCP q3, a4, q4, q0, q1
    EE.VRELU.S16 q4, a14, a15
    EE.VST.128.IP q4, a2, 16

    EE.VLD.128.IP q0, a3, 16
    EE.VADDS.S16.LD.INCP q1, a4, q5, q2, q3
    EE.VRELU.S16 q5, a14, a15
    EE.VST.128.IP q5, a2, 16
0:
    EE.VLD.128.IP q2, a3, 16
    EE.VADDS.S16.LD.INCP q3, a4, q4, q0, q1
    EE.VRELU.S16 q4, a14, a15
    EE.VST.128.IP q4, a2, 16
    beqi a7, 1, 2f #remainder == 2*16byte
    beqi a7, 2, 3f #remainder == 3*16byte
2:
    EE.VADDS.S16 q5, q2, q3
    EE.VRELU.S16 q5, a14, a15
    EE.VST.128.IP q5, a2, 16
    retw
3:
    EE.VLD.128.IP q0, a3, 16
    EE.VADDS.S16.LD.INCP q1, a4, q5, q2, q3
    EE.VRELU.S16 q5, a14, a15
    EE.VST.128.IP q5, a2, 16

    EE.VADDS.S16 q4, q0, q1
    EE.VRELU.S16 q4, a14, a15
    EE.VST.128.IP q4, a2, 16
    retw

dl_tie728_s16_add2d_relu_small_channel:
    blti a7, 0, 5f
    loopgtz a7, 1f
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16
    EE.VADDS.S16 q2, q0, q1
    EE.VRELU.S16 q2, a14, a15
    EE.VST.128.IP q2, a2, 16
1:
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16
    EE.VADDS.S16 q2, q0, q1
    EE.VRELU.S16 q2, a14, a15
    EE.VST.128.IP q2, a2, 16
5:
    retw



    .align 4
    .text
    .global dl_tie728_s16_rescale_add2d_11c_relu
    .type   dl_tie728_s16_rescale_add2d_11c_relu, @function
    # .section .iram1
dl_tie728_s16_rescale_add2d_11c_relu:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1
    # a7: input_shift
    # a8: output_scale
    # a9: output_shift
    # a14: activation_alpha
    # a15: activation_shift

    l32i a6, a5, 64
    l32i a7, a5, 88
    l32i a8, a5, 96
    l32i a9, a5, 92
    l32i a14, a5, 52
    l32i a15, a5, 60

    beqi a8, 1, dl_tie728_s16_rescale_add2d_output_relu

dl_tie728_s16_rescale_add2d_output_scale_relu:
    s16i a8, a1, 0
    EE.VLDBC.16 q7, a1 # all output_scale

    loopgtz a6, 1f
    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q0, q1, q1, q7, a9

    EE.VRELU.S16 q1, a14, a15
    EE.VST.128.IP q1, a2, 16

1:
    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q0, q1, q1, q7, a9

    EE.VRELU.S16 q1, a14, a15
    EE.VST.128.IP q1, a2, 16
    retw

dl_tie728_s16_rescale_add2d_output_relu:
    movi a13, 1
    s16i a13, a1, 0
    EE.VLDBC.16 q7, a1 # all 1

    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16

    loopgtz a6, 2f
    EE.SRCMB.S16.QACC q1, a7, 0
    EE.VMULAS.S16.QACC.LD.IP q0, a3, 16, q0, q7
    EE.SRCMB.S16.QACC q1, a9, 0
    EE.LDQA.S16.128.IP a4, 16
    EE.VRELU.S16 q1, a14, a15
    EE.VST.128.IP q1, a2, 16
2:

    EE.SRCMB.S16.QACC q1, a7, 0
    EE.VMULAS.S16.QACC q0, q7
    EE.SRCMB.S16.QACC q1, a9, 0
    EE.VRELU.S16 q1, a14, a15
    EE.VST.128.IP q1, a2, 16
    retw



    .align 4
    .text
    .global dl_tie728_s16_add2d_11c_prelu
    .type   dl_tie728_s16_add2d_11c_prelu, @function
    # .section .iram1
dl_tie728_s16_add2d_11c_prelu:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_2x_1
    # a7: c_left_x
    # a14: activation_alpha_ptr
    # a15: activation_shift

    l32i a6, a5, 68
    l32i a7, a5, 72
    l32i a14, a5, 56
    l32i a15, a5, 60

    beqz a6, dl_tie728_s16_add2d_prelu_small_channel
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16

    loopgtz a6, 0f
    EE.VLD.128.IP q2, a3, 16
    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16.LD.INCP q3, a4, q4, q0, q1
    EE.VPRELU.S16 q4, q4, q6, a15
    EE.VST.128.IP q4, a2, 16

    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16.LD.INCP q1, a4, q5, q2, q3
    EE.VPRELU.S16 q5, q5, q6, a15
    EE.VST.128.IP q5, a2, 16
0:
    EE.VLD.128.IP q2, a3, 16
    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16.LD.INCP q3, a4, q4, q0, q1
    EE.VPRELU.S16 q4, q4, q6, a15
    EE.VST.128.IP q4, a2, 16
    beqi a7, 1, 2f #remainder == 2*16byte
    beqi a7, 2, 3f #remainder == 3*16byte

2:
    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q5, q2, q3
    EE.VPRELU.S16 q5, q5, q6, a15
    EE.VST.128.IP q5, a2, 16
    retw
3:
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16.LD.INCP q1, a4, q5, q2, q3
    EE.VPRELU.S16 q5, q5, q6, a15
    EE.VST.128.IP q5, a2, 16

    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q4, q0, q1
    EE.VPRELU.S16 q4, q4, q6, a15
    EE.VST.128.IP q4, a2, 16
    retw

dl_tie728_s16_add2d_prelu_small_channel:
    blti a7, 0, 5f
    loopgtz a7, 1f
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16
    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q2, q0, q1
    EE.VPRELU.S16 q2, q2, q6, a15
    EE.VST.128.IP q2, a2, 16
1:
    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16
    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q2, q0, q1
    EE.VPRELU.S16 q2, q2, q6, a15
    EE.VST.128.IP q2, a2, 16
5:
    retw



    .align 4
    .text
    .global dl_tie728_s16_rescale_add2d_11c_prelu
    .type   dl_tie728_s16_rescale_add2d_11c_prelu, @function
    # .section .iram1
dl_tie728_s16_rescale_add2d_11c_prelu:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1
    # a7: input_shift
    # a8: output_scale
    # a9: output_shift
    # a14: activation_alpha_ptr
    # a15: activation_shift

    l32i a6, a5, 64
    l32i a7, a5, 88
    l32i a8, a5, 96
    l32i a9, a5, 92
    l32i a14, a5, 56
    l32i a15, a5, 60

    beqi a8, 1, dl_tie728_s16_rescale_add2d_output_prelu

dl_tie728_s16_rescale_add2d_output_scale_prelu:
    s16i a8, a1, 0
    EE.VLDBC.16 q7, a1 # all output_scale

    # addi a6, a6, 1
    loopgtz a6, 1f
    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q0, q1, q1, q7, a9

    EE.VLD.128.IP q5, a14, 16

    EE.VPRELU.S16 q1, q1, q5, a15
    EE.VST.128.IP q1, a2, 16

1:
    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q0, q1, q1, q7, a9

    EE.VLD.128.IP q5, a14, 16

    EE.VPRELU.S16 q1, q1, q5, a15
    EE.VST.128.IP q1, a2, 16
    retw

dl_tie728_s16_rescale_add2d_output_prelu:
    movi a13, 1
    s16i a13, a1, 0
    EE.VLDBC.16 q7, a1 # all 1

    EE.LDQA.S16.128.IP a4, 16
    EE.VLD.128.IP q0, a3, 16

    loopgtz a6, 2f
    EE.SRCMB.S16.QACC q1, a7, 0
    EE.VMULAS.S16.QACC.LD.IP q0, a3, 16, q0, q7
    EE.SRCMB.S16.QACC q1, a9, 0

    EE.VLD.128.IP q6, a14, 16
    EE.LDQA.S16.128.IP a4, 16
    EE.VPRELU.S16 q1, q1, q6, a15
    EE.VST.128.IP q1, a2, 16
2:

    EE.SRCMB.S16.QACC q1, a7, 0
    EE.VMULAS.S16.QACC q0, q7

    EE.VLD.128.IP q6, a14, 16
    EE.SRCMB.S16.QACC q1, a9, 0
    EE.VPRELU.S16 q1, q1, q6, a15
    EE.VST.128.IP q1, a2, 16
    retw


############################################################################################################################################################
####
#### tie728_s16_unaligned_add2d_11c series
####
############################################################################################################################################################

    .align 4
    .text
    .global dl_tie728_s16_unaligned_add2d_11c
    .type   dl_tie728_s16_unaligned_add2d_11c, @function
    # .section .iram1
dl_tie728_s16_unaligned_add2d_11c:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1
    # a7: input_shift
    # a8: output_scale
    # a9: output_shift
    # a10: c_remainder

    l32i a6, a5, 64
    l32i a10, a5, 76
    l32i a7, a5, 88

    bgei a7, 0, dl_tie728_s16_unaligned_rescale_add2d_11c

# input0 exp = input1 exp = output exp

    EE.LD.128.USAR.IP q5, a2, 0 #get output_ptr sar_byte
    rur.sar_byte a13

    blti a6, 0, dl_tie728_s16_unaligned_add2d_11c_small_remainder # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    beqi a13, 0, dl_tie728_s16_unaligned_add2d_11c_0
    beqi a13, 8, dl_tie728_s16_unaligned_add2d_11c_1

    loopgtz a6, 0f #dl_tie728_s16_unaligned_add2d_11c
        EE.SRC.Q.QUP q2, q0, q1
        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4
        EE.VADDS.S16 q2, q2, q5
        EE.LD.128.USAR.IP q1, a3, 16
        dl_tie728_128b_unaligned_store0 q2, a2, a13
0:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VADDS.S16 q2, q2, q5
    dl_tie728_128b_unaligned_store0 q2, a2, a13
    j dl_tie728_s16_unaligned_add2d_11c_remainder

    #output sar = 0
dl_tie728_s16_unaligned_add2d_11c_0:
    loopgtz a6, 1f #dl_tie728_s16_unaligned_add2d_11c_loop0
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VADDS.S16 q2, q2, q5

        EE.LD.128.USAR.IP q1, a3, 16
        EE.VST.128.IP q2, a2, 16
1:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VADDS.S16 q2, q2, q5
    EE.VST.128.IP q2, a2, 16
    j dl_tie728_s16_unaligned_add2d_11c_remainder

    # #output sar = 8
dl_tie728_s16_unaligned_add2d_11c_1:
    loopgtz a6, 2f #dl_tie728_s16_unaligned_add2d_11c_loop1
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VADDS.S16 q2, q2, q5

        EE.LD.128.USAR.IP q1, a3, 16
        dl_tie728_128b_unaligned_store1 q2, a2
2:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VADDS.S16 q2, q2, q5
    dl_tie728_128b_unaligned_store1 q2, a2
    j dl_tie728_s16_unaligned_add2d_11c_remainder

dl_tie728_s16_unaligned_add2d_11c_small_remainder:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11

    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12

dl_tie728_s16_unaligned_add2d_11c_remainder:

    beqz a10, dl_tie728_s16_unaligned_add2d_end

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.VADDS.S16 q2, q2, q5

    srli a10, a10, 1
    dl_tie728_s16_store_remainder q2, a10, a13, a2

dl_tie728_s16_unaligned_add2d_end:

    retw


## rescaled add
dl_tie728_s16_unaligned_rescale_add2d_11c:
    l32i a8, a5, 96 # output_scale
    l32i a9, a5, 92 # output_shift

    beqi a8, 1, dl_tie728_s16_rescale_unaligned_add2d_output_shift


### rescaled to output by *scale) >> shift
dl_tie728_s16_rescale_unaligned_add2d_output_scale:

    s32i a8, a1, 0
    EE.VLDBC.16 q7, a1 # all output_scale

    blti a6, 0, dl_tie728_s16_rescale_unaligned_add2d_scale_small_remainder # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    loopgtz a6, 3f #dl_tie728_s16_rescale_unaligned_add2d_11c_scale
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.MOV.S16.QACC q5
        EE.SRCMB.S16.QACC q1, a7, 0

        dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9

        EE.LD.128.USAR.IP q1, a3, 16
        dl_tie728_128b_unaligned_store0 q2, a2, a12
    3:

    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9

    dl_tie728_128b_unaligned_store0 q2, a2, a13
    j dl_tie728_s16_rescale_unaligned_add2d_scale_remainder


dl_tie728_s16_rescale_unaligned_add2d_scale_small_remainder:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12 #input1 sar

dl_tie728_s16_rescale_unaligned_add2d_scale_remainder:
    beqz a10, dl_tie728_s16_unaligned_rescale_add2d_output_scale_end # c remainder

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9

    srli a10, a10, 1
    dl_tie728_s16_store_remainder q2, a10, a12, a2

dl_tie728_s16_unaligned_rescale_add2d_output_scale_end:
    retw


### rescaled to output by right shift
dl_tie728_s16_rescale_unaligned_add2d_output_shift:
    movi a13, 1
    s16i a13, a1, 0
    EE.VLDBC.16 q7, a1 # all 1

    blti a6, 0, dl_tie728_s16_rescale_unaligned_add2d_shift_small_remainder # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    loopgtz a6, 4f #dl_tie728_s16_rescale_unaligned_add2d_11c_shift
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4
        EE.MOV.S16.QACC q5

        EE.SRCMB.S16.QACC q5, a7, 0
        EE.VMULAS.S16.QACC q2, q7
        EE.SRCMB.S16.QACC q5, a9, 0

        EE.LD.128.USAR.IP q1, a3, 16
        dl_tie728_128b_unaligned_store0 q5, a2, a13
    4:
    addi a3, a3, -16
    add a3, a3, a10

    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4
    EE.MOV.S16.QACC q5

    EE.SRCMB.S16.QACC q5, a7, 0
    EE.VMULAS.S16.QACC q2, q7
    EE.SRCMB.S16.QACC q5, a9, 0

    dl_tie728_128b_unaligned_store0 q5, a2, a13
    j dl_tie728_s16_rescale_unaligned_add2d_shift_remainder



dl_tie728_s16_rescale_unaligned_add2d_shift_small_remainder:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12 #input1 sar

dl_tie728_s16_rescale_unaligned_add2d_shift_remainder:
    beqz a10, dl_tie728_s16_unaligned_rescale_add2d_output_shift_end # c remainder

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q5, a7, 0
    EE.VMULAS.S16.QACC q2, q7
    EE.SRCMB.S16.QACC q5, a9, 0

    srli a10, a10, 1
    dl_tie728_s16_store_remainder q5, a10, a13, a2

dl_tie728_s16_unaligned_rescale_add2d_output_shift_end:
    retw



    .align 4
    .text
    .global dl_tie728_s16_unaligned_add2d_11c_relu
    .type   dl_tie728_s16_unaligned_add2d_11c_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_add2d_11c_relu:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1
    # a7: input_shift
    # a8: output_scale
    # a9: output_shift
    # a10: c_remainder
    # a14: activation_alpha
    # a15: activation_shift

    l32i a6, a5, 64
    l32i a10, a5, 76
    l32i a7, a5, 88
    l32i a14, a5, 52
    l32i a15, a5, 60

    bgei a7, 0, dl_tie728_s16_unaligned_rescale_add2d_11c_relu

# input0 exp = input1 exp = output exp

    EE.LD.128.USAR.IP q5, a2, 0 #get output_ptr sar_byte
    rur.sar_byte a13

    blti a6, 0, dl_tie728_s16_unaligned_add2d_11c_small_remainder_relu # channel < 8

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    beqi a13, 0, dl_tie728_s16_unaligned_add2d_11c_relu_0
    beqi a13, 8, dl_tie728_s16_unaligned_add2d_11c_relu_1


    loopgtz a6, 0f #dl_tie728_s16_unaligned_add2d_11c_relu
        EE.SRC.Q.QUP q2, q0, q1
        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4
        EE.VADDS.S16 q2, q2, q5
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S16 q2, a14, a15
        dl_tie728_128b_unaligned_store0 q2, a2, a13
0:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VADDS.S16 q2, q2, q5
    EE.VRELU.S16 q2, a14, a15
    dl_tie728_128b_unaligned_store0 q2, a2, a13
    j dl_tie728_s16_unaligned_add2d_11c_remainder_relu

    #output sar = 0
dl_tie728_s16_unaligned_add2d_11c_relu_0:
    loopgtz a6, 1f #dl_tie728_s16_unaligned_add2d_11c_loop0_relu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VADDS.S16 q2, q2, q5

        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S16 q2, a14, a15
        EE.VST.128.IP q2, a2, 16
1:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VADDS.S16 q2, q2, q5
    EE.VRELU.S16 q2, a14, a15
    EE.VST.128.IP q2, a2, 16
    j dl_tie728_s16_unaligned_add2d_11c_remainder_relu

    # #output sar = 8
dl_tie728_s16_unaligned_add2d_11c_relu_1:
    loopgtz a6, 2f #dl_tie728_s16_unaligned_add2d_11c_loop1_relu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VADDS.S16 q2, q2, q5

        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S16 q2, a14, a15
        dl_tie728_128b_unaligned_store1 q2, a2
2:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VADDS.S16 q2, q2, q5
    EE.VRELU.S16 q2, a14, a15
    dl_tie728_128b_unaligned_store1 q2, a2
    j dl_tie728_s16_unaligned_add2d_11c_remainder_relu

dl_tie728_s16_unaligned_add2d_11c_small_remainder_relu:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11

    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12

dl_tie728_s16_unaligned_add2d_11c_remainder_relu:

    beqz a10, dl_tie728_s16_unaligned_add2d_end_relu

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.VADDS.S16 q2, q2, q5
    EE.VRELU.S16 q2, a14, a15
    srli a10, a10, 1
    dl_tie728_s16_store_remainder q2, a10, a13, a2

dl_tie728_s16_unaligned_add2d_end_relu:

    retw


## rescaled add
dl_tie728_s16_unaligned_rescale_add2d_11c_relu:
    l32i a8, a5, 96 # output_scale
    l32i a9, a5, 92 # output_shift

    beqi a8, 1, dl_tie728_s16_rescale_unaligned_add2d_output_shift_relu


### rescaled to output by *scale) >> shift
dl_tie728_s16_rescale_unaligned_add2d_output_scale_relu:

    s32i a8, a1, 0
    EE.VLDBC.16 q7, a1 # all output_scale

    blti a6, 0, dl_tie728_s16_rescale_unaligned_add2d_scale_small_remainder_relu # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    loopgtz a6, 3f #dl_tie728_s16_rescale_unaligned_add2d_11c_scale_relu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.MOV.S16.QACC q5
        EE.SRCMB.S16.QACC q1, a7, 0

        dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S16 q2, a14, a15
        dl_tie728_128b_unaligned_store0 q2, a2, a12
    3:

    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9

    EE.VRELU.S16 q2, a14, a15
    dl_tie728_128b_unaligned_store0 q2, a2, a13
    j dl_tie728_s16_rescale_unaligned_add2d_scale_remainder_relu


dl_tie728_s16_rescale_unaligned_add2d_scale_small_remainder_relu:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12 #input1 sar

dl_tie728_s16_rescale_unaligned_add2d_scale_remainder_relu:
    beqz a10, dl_tie728_s16_unaligned_rescale_add2d_output_scale_end_relu # c remainder

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q1, a7, 0

    dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9

    EE.VRELU.S16 q2, a14, a15
    srli a10, a10, 1
    dl_tie728_s16_store_remainder q2, a10, a13, a2

dl_tie728_s16_unaligned_rescale_add2d_output_scale_end_relu:
    retw


### rescaled to output by right shift
dl_tie728_s16_rescale_unaligned_add2d_output_shift_relu:
    movi a13, 1
    s16i a13, a1, 0
    EE.VLDBC.16 q7, a1 # all 1

    blti a6, 0, dl_tie728_s16_rescale_unaligned_add2d_shift_small_remainder_relu # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    loopgtz a6, 4f #dl_tie728_s16_rescale_unaligned_add2d_11c_shift_relu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4
        EE.MOV.S16.QACC q5

        EE.SRCMB.S16.QACC q5, a7, 0
        EE.VMULAS.S16.QACC q2, q7
        EE.SRCMB.S16.QACC q5, a9, 0

        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S16 q5, a14, a15
        dl_tie728_128b_unaligned_store0 q5, a2, a13
    4:
    addi a3, a3, -16
    add a3, a3, a10

    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4
    EE.MOV.S16.QACC q5

    EE.SRCMB.S16.QACC q5, a7, 0
    EE.VMULAS.S16.QACC q2, q7
    EE.SRCMB.S16.QACC q5, a9, 0

    EE.VRELU.S16 q5, a14, a15
    dl_tie728_128b_unaligned_store0 q5, a2, a13
    j dl_tie728_s16_rescale_unaligned_add2d_shift_remainder_relu



dl_tie728_s16_rescale_unaligned_add2d_shift_small_remainder_relu:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12 #input1 sar

dl_tie728_s16_rescale_unaligned_add2d_shift_remainder_relu:
    beqz a10, dl_tie728_s16_unaligned_rescale_add2d_output_shift_end_relu # c remainder

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q5, a7, 0
    EE.VMULAS.S16.QACC q2, q7
    EE.SRCMB.S16.QACC q5, a9, 0

    EE.VRELU.S16 q5, a14, a15
    srli a10, a10, 1
    dl_tie728_s16_store_remainder q5, a10, a13, a2

dl_tie728_s16_unaligned_rescale_add2d_output_shift_end_relu:
    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_add2d_11c_prelu
    .type   dl_tie728_s16_unaligned_add2d_11c_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_add2d_11c_prelu:
    .align 4
    entry sp, 32

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1
    # a7: input_shift
    # a8: output_scale
    # a9: output_shift
    # a10: c_remainder
    # a14: activation_alpha_ptr
    # a15: activation_shift

    l32i a6, a5, 64
    l32i a10, a5, 76
    l32i a7, a5, 88
    l32i a14, a5, 56
    l32i a15, a5, 60

    bgei a7, 0, dl_tie728_s16_unaligned_rescale_add2d_11c_prelu

# input0 exp = input1 exp = output exp

    EE.LD.128.USAR.IP q5, a2, 0 #get output_ptr sar_byte
    rur.sar_byte a13

    blti a6, 0, dl_tie728_s16_unaligned_add2d_11c_small_remainder_prelu # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    beqi a13, 0, dl_tie728_s16_unaligned_add2d_11c_prelu_0
    beqi a13, 8, dl_tie728_s16_unaligned_add2d_11c_prelu_1


    loopgtz a6, 0f #dl_tie728_s16_unaligned_add2d_11c_prelu
        EE.SRC.Q.QUP q2, q0, q1
        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4
        EE.VADDS.S16 q2, q2, q5

        EE.VLD.128.IP q6, a14, 16
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VPRELU.S16 q2, q2, q6, a15
        dl_tie728_128b_unaligned_store0 q2, a2, a13
0:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q2, q2, q5
    EE.VPRELU.S16 q2, q2, q6, a15
    dl_tie728_128b_unaligned_store0 q2, a2, a13
    j dl_tie728_s16_unaligned_add2d_11c_remainder_prelu

    #output sar = 0
dl_tie728_s16_unaligned_add2d_11c_prelu_0:
    loopgtz a6, 1f #dl_tie728_s16_unaligned_add2d_11c_loop0_prelu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VADDS.S16 q2, q2, q5

        EE.VLD.128.IP q6, a14, 16
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VPRELU.S16 q2, q2, q6, a15
        EE.VST.128.IP q2, a2, 16
1:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q2, q2, q5
    EE.VPRELU.S16 q2, q2, q6, a15
    EE.VST.128.IP q2, a2, 16
    j dl_tie728_s16_unaligned_add2d_11c_remainder_prelu

    # #output sar = 8
dl_tie728_s16_unaligned_add2d_11c_prelu_1:
    loopgtz a6, 2f #dl_tie728_s16_unaligned_add2d_11c_loop1_prelu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VADDS.S16 q2, q2, q5

        EE.VLD.128.IP q6, a14, 16
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VPRELU.S16 q2, q2, q6, a15
        dl_tie728_128b_unaligned_store1 q2, a2
2:
    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12
    EE.SRC.Q.QUP q5, q3, q4

    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q2, q2, q5
    EE.VPRELU.S16 q2, q2, q6, a15
    dl_tie728_128b_unaligned_store1 q2, a2
    j dl_tie728_s16_unaligned_add2d_11c_remainder_prelu

dl_tie728_s16_unaligned_add2d_11c_small_remainder_prelu:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11

    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12

dl_tie728_s16_unaligned_add2d_11c_remainder_prelu:

    beqz a10, dl_tie728_s16_unaligned_add2d_end_prelu

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.VLD.128.IP q6, a14, 16
    EE.VADDS.S16 q2, q2, q5
    EE.VPRELU.S16 q2, q2, q6, a15
    srli a10, a10, 1
    dl_tie728_s16_store_remainder q2, a10, a13, a2

dl_tie728_s16_unaligned_add2d_end_prelu:

    retw


## rescaled add
dl_tie728_s16_unaligned_rescale_add2d_11c_prelu:
    l32i a8, a5, 96 # output_scale
    l32i a9, a5, 92 # output_shift

    beqi a8, 1, dl_tie728_s16_rescale_unaligned_add2d_output_shift_prelu


### rescaled to output by *scale) >> shift
dl_tie728_s16_rescale_unaligned_add2d_output_scale_prelu:

    s32i a8, a1, 0
    EE.VLDBC.16 q7, a1 # all output_scale
    # ssr a9 #output shift
    # movi a13, 0

    blti a6, 0, dl_tie728_s16_rescale_unaligned_add2d_scale_small_remainder_prelu # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    loopgtz a6, 3f #dl_tie728_s16_rescale_unaligned_add2d_11c_scale_prelu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.MOV.S16.QACC q5
        EE.SRCMB.S16.QACC q1, a7, 0

        dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9
        EE.VLD.128.IP q6, a14, 16
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VPRELU.S16 q2, q2, q6, a15
        dl_tie728_128b_unaligned_store0 q2, a2, a12
    3:

    addi a3, a3, -16
    add a3, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q1, a7, 0

    EE.VLD.128.IP q6, a14, 16
    dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9

    EE.VPRELU.S16 q2, q2, q6, a15
    dl_tie728_128b_unaligned_store0 q2, a2, a13
    j dl_tie728_s16_rescale_unaligned_add2d_scale_remainder_prelu


dl_tie728_s16_rescale_unaligned_add2d_scale_small_remainder_prelu:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12 #input1 sar

dl_tie728_s16_rescale_unaligned_add2d_scale_remainder_prelu:
    beqz a10, dl_tie728_s16_unaligned_rescale_add2d_output_scale_end_prelu # c remainder

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q1, a7, 0

    EE.VLD.128.IP q6, a14, 16
    dl_tie728_s16_rescale_add_rescale_output q2, q1, q2, q7, a9

    EE.VPRELU.S16 q2, q2, q6, a15
    srli a10, a10, 1
    dl_tie728_s16_store_remainder q2, a10, a13, a2

dl_tie728_s16_unaligned_rescale_add2d_output_scale_end_prelu:
    retw


### rescaled to output by right shift
dl_tie728_s16_rescale_unaligned_add2d_output_shift_prelu:
    movi a13, 1
    s16i a13, a1, 0
    EE.VLDBC.16 q7, a1 # all 1

    blti a6, 0, dl_tie728_s16_rescale_unaligned_add2d_shift_small_remainder_prelu # channel < 16

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    loopgtz a6, 4f #dl_tie728_s16_rescale_unaligned_add2d_11c_shift_prelu
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4
        EE.MOV.S16.QACC q5

        EE.SRCMB.S16.QACC q5, a7, 0
        EE.VMULAS.S16.QACC q2, q7
        EE.SRCMB.S16.QACC q5, a9, 0

        EE.VLD.128.IP q6, a14, 16
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VPRELU.S16 q5, q5, q6, a15
        dl_tie728_128b_unaligned_store0 q5, a2, a13
    4:
    addi a3, a3, -16
    add a3, a3, a10

    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a10
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4
    EE.MOV.S16.QACC q5

    EE.SRCMB.S16.QACC q5, a7, 0
    EE.VMULAS.S16.QACC q2, q7
    EE.VLD.128.IP q6, a14, 16
    EE.SRCMB.S16.QACC q5, a9, 0

    EE.VPRELU.S16 q5, q5, q6, a15
    dl_tie728_128b_unaligned_store0 q5, a2, a13
    j dl_tie728_s16_rescale_unaligned_add2d_shift_remainder_prelu



dl_tie728_s16_rescale_unaligned_add2d_shift_small_remainder_prelu:
    EE.LD.128.USAR.XP q0, a3, a10
    rur.sar_byte a11 #input0 sar
    EE.LD.128.USAR.XP q3, a4, a10
    rur.sar_byte a12 #input1 sar

dl_tie728_s16_rescale_unaligned_add2d_shift_remainder_prelu:
    beqz a10, dl_tie728_s16_unaligned_rescale_add2d_output_shift_end_prelu # c remainder

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.MOV.S16.QACC q5
    EE.SRCMB.S16.QACC q5, a7, 0
    EE.VMULAS.S16.QACC q2, q7
    EE.VLD.128.IP q6, a14, 16
    EE.SRCMB.S16.QACC q5, a9, 0

    EE.VPRELU.S16 q5, q5, q6, a15
    srli a10, a10, 1
    dl_tie728_s16_store_remainder q5, a10, a13, a2

dl_tie728_s16_unaligned_rescale_add2d_output_shift_end_prelu:
    retw
