#include "dl_tie728_s16.S"


############################################################################################################################################################
####
#### tie728_s16_min2d_11c series
####
############################################################################################################################################################


    .align 4
    .text
    .global dl_tie728_s16_min2d_11c
    .type   dl_tie728_s16_min2d_11c, @function
    .section .iram1
dl_tie728_s16_min2d_11c:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1


    l32i a6, a5, 64
    blti a6, 0, 5f

    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a4, 16
    loopgtz a6, 0f
    	EE.VMIN.s16.LD.INCP q0, a3, q2, q0, q1
    	EE.VLD.128.IP q1, a4, 16
        EE.VST.128.IP q2, a2, 16
    0:

    EE.VMIN.S16 q2, q0, q1
    EE.VST.128.IP q2, a2, 16
5:
    retw



############################################################################################################################################################
####
#### tie728_s16_unaligned_min2d_11c series
####
############################################################################################################################################################
    .align 4
    .text
    .global dl_tie728_s16_unaligned_min2d_11c
    .type   dl_tie728_s16_unaligned_min2d_11c, @function
    .section .iram1
dl_tie728_s16_unaligned_min2d_11c:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input0_ptr
    # a4: int16_t *input1_ptr
    # a5: void *args
    # a6: c_div_x_1
    # a7: c_remainder


    l32i a6, a5, 64
    l32i a7, a5, 76


    EE.LD.128.USAR.IP q5, a2, 0 #get output_ptr sar_byte
    rur.sar_byte a13

    blti a6, 0, dl_tie718_s16_unaligned_min2d_11c_small_remainder # channel < 16


    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q3, a4, 16
    EE.LD.128.USAR.IP q1, a3, 16

    beqi a13, 0, dl_tie718_s16_unaligned_min2d_11c_0
    beqi a13, 8, dl_tie718_s16_unaligned_min2d_11c_1


    loopgtz a6, 0f
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VMIN.S16 q2, q2, q5
        EE.LD.128.USAR.IP q1, a3, 16
        dl_tie728_128b_unaligned_store0 q2, a2, a13
    0:
    addi a3, a3, -16
    add a3, a3, a7
    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a7
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4

    EE.VMIN.S16 q2, q2, q5
    dl_tie728_128b_unaligned_store0 q2, a2, a13
    j dl_tie718_s16_unaligned_min2d_11c_remainder

dl_tie718_s16_unaligned_min2d_11c_0:

    loopgtz a6, 1f
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VMIN.S16 q2, q2, q5
        EE.LD.128.USAR.IP q1, a3, 16
        EE.VST.128.IP q2, a2, 16
    1:
    addi a3, a3, -16
    add a3, a3, a7
    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a7
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4

    EE.VMIN.S16 q2, q2, q5
    EE.VST.128.IP q2, a2, 16
    j dl_tie718_s16_unaligned_min2d_11c_remainder

dl_tie718_s16_unaligned_min2d_11c_1:

    loopgtz a6, 2f
        EE.SRC.Q.QUP q2, q0, q1

        EE.LD.128.USAR.IP q4, a4, 16
        EE.SRC.Q.QUP q5, q3, q4

        EE.VMIN.S16 q2, q2, q5
        EE.LD.128.USAR.IP q1, a3, 16
        dl_tie728_128b_unaligned_store1 q2, a2
    2:
    addi a3, a3, -16
    add a3, a3, a7
    rur.sar_byte a11 #input0 sar
    EE.SRC.Q.QUP q2, q0, q1

    EE.LD.128.USAR.XP q4, a4, a7
    rur.sar_byte a12 #input1 sar
    EE.SRC.Q.QUP q5, q3, q4

    EE.VMIN.S16 q2, q2, q5
    dl_tie728_128b_unaligned_store1 q2, a2

    j dl_tie718_s16_unaligned_min2d_11c_remainder

dl_tie718_s16_unaligned_min2d_11c_small_remainder:
    EE.LD.128.USAR.XP q0, a3, a7
    rur.sar_byte a11

    EE.LD.128.USAR.XP q3, a4, a7
    rur.sar_byte a12

dl_tie718_s16_unaligned_min2d_11c_remainder:


    beqz a7, dl_tie728_s16_unaligned_min2d_11c_end

    EE.LD.128.USAR.IP q1, a3, 0
    wur.sar_byte a11
    EE.SRC.Q q2, q0, q1

    EE.LD.128.USAR.IP q4, a4, 0
    wur.sar_byte a12
    EE.SRC.Q q5, q3, q4

    EE.VMIN.S16 q2, q2, q5
    srli a7, a7, 1
    dl_tie728_s16_store_remainder q2, a7, a12, a2

dl_tie728_s16_unaligned_min2d_11c_end:
    retw
