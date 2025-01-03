#include "dl_tie728_s8.S"

    .align 4
    .text
    .global dl_tie728_s8_relu_11c
    .type   dl_tie728_s8_relu_11c, @function
    .section .iram1
dl_tie728_s8_relu_11c:
    .align 4
    entry sp, 16

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args
    # a5: c_rs1_1: c / 2x - 1
    # a6: c_rs2_1: c_left_1
    # a14: activation_alpha
    # a15: activation_shift


    l32i a5, a4, 88
    l32i a6, a4, 92
    l32i a7, a4, 136
    l32i a14, a4, 76  # activation_alpha
    l32i a15, a4, 84  # activation_shift


    loopgtz a5, 0f
        EE.VLD.128.IP q0, a3, 16
        EE.VLD.128.IP q1, a3, 16

        EE.VRELU.S8 q0, a14, a15
        EE.VST.128.IP q0, a2, 16

        EE.VRELU.S8 q1, a14, a15
        EE.VST.128.IP q1, a2, 16
    0:


    loopgtz a6, 1f
        EE.VLD.128.IP q0, a3, 16
        EE.VRELU.S8 q0, a14, a15
        EE.VST.128.IP q0, a2, 16
    1:


    EE.VLD.128.IP q0, a3, 16

    EE.VRELU.S8 q0, a14, a15
    EE.VST.128.IP q0, a2, 16

    retw





    .align 4
    .text
    .global dl_tie728_s8_unaligned_relu_11c
    .type   dl_tie728_s8_unaligned_relu_11c, @function
    .section .iram1
dl_tie728_s8_unaligned_relu_11c:
    .align 4
    entry sp, 16

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args
    # a5: c_div_x_1
    # a6: c_remainder
    # a14: activation_alpha
    # a15: activation_shift


    l32i a5, a4, 100
    l32i a6, a4, 136
    l32i a14, a4, 76  # activation_alpha
    l32i a15, a4, 84  # activation_shift


    EE.LD.128.USAR.IP q5, a2, 0 #get output_ptr sar_byte
    rur.sar_byte a13

    blti a5, 0, dl_tie718_s8_unaligned_relu_11c_small_remainder # channel < 16


    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q1, a3, 16

    beqi a13, 0, dl_tie718_s8_unaligned_relu_11c_0
    beqi a13, 8, dl_tie718_s8_unaligned_relu_11c_1


    loopgtz a5, 0f
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S8 q2, a14, a15
        dl_tie728_s8_unaligned_store0 q2, a2, a13
    0:
    addi a3, a3, -16
    add a3, a3, a6
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1
    EE.VRELU.S8 q2, a14, a15
    dl_tie728_s8_unaligned_store0 q2, a2, a13
    j dl_tie718_s8_unaligned_relu_11c_remainder


dl_tie718_s8_unaligned_relu_11c_0:
    loopgtz a5, 0f
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S8 q2, a14, a15
        EE.VST.128.IP q2, a2, 16
    0:
    addi a3, a3, -16
    add a3, a3, a6
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1
    EE.VRELU.S8 q2, a14, a15
    EE.VST.128.IP q2, a2, 16
    j dl_tie718_s8_unaligned_relu_11c_remainder


dl_tie718_s8_unaligned_relu_11c_1:

    loopgtz a5, 0f
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q1, a3, 16
        EE.VRELU.S8 q2, a14, a15
        dl_tie728_s8_unaligned_store1 q2, a2
    0:
    addi a3, a3, -16
    add a3, a3, a6
    rur.sar_byte a11
    EE.SRC.Q.QUP q2, q0, q1
    EE.VRELU.S8 q2, a14, a15
    dl_tie728_s8_unaligned_store1 q2, a2
    j dl_tie718_s8_unaligned_relu_11c_remainder


dl_tie718_s8_unaligned_relu_11c_small_remainder:
    EE.LD.128.USAR.XP q0, a3, a6
    rur.sar_byte a11


dl_tie718_s8_unaligned_relu_11c_remainder:

    beqz a6, dl_tie728_s8_unaligned_relu_11c_end

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.VRELU.S8 q2, a14, a15
    # dl_tie728_s8_unaligned_store0 q2, a2, a13
    dl_tie728_s8_store_remainder q2, a9, a11, a12, a13, a2, a6

dl_tie728_s8_unaligned_relu_11c_end:
    retw
