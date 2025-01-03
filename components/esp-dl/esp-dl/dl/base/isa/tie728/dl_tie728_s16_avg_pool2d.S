#include "dl_tie728_s16.S"

############################################################################################################################################################
####
#### dl_tie728_s16_avg_pool2d series
####
############################################################################################################################################################
    .align 4
    .text
    .global dl_tie728_s16_avg_pool2d_22c1
    .type	dl_tie728_s16_avg_pool2d_22c1, @function
    .section .iram1
dl_tie728_s16_avg_pool2d_22c1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset_bytes
    # a6: input_x_offset_bytes
    # a11: c_div_x_1
    # a14: mac_shift

    l32i a5, a4, 16     # input_y_offset_bytes
    l32i a6, a4, 20     # input_x_offset_bytes
    l32i a11, a4, 104     # c_div_x_1

    add a7, a3, a6
    add a8, a3, a5
    add a9, a8, a6


    addi a14, a4, 64
    EE.VLDBC.16 q0, a14    # avg_pool_area_inv
    l32i a14, a4, 56       # mac_shift

    EE.VLD.128.IP q1, a3, 16
    EE.VLD.128.IP q2, a7, 16
    loopgtz a11, 1f
        EE.ZERO.QACC
        EE.VMULAS.S16.QACC.LD.IP q3, a8, 16, q0, q1
        EE.VMULAS.S16.QACC.LD.IP q4, a9, 16, q0, q2
        EE.VMULAS.S16.QACC.LD.IP q1, a3, 16, q0, q3
        EE.VMULAS.S16.QACC.LD.IP q2, a7, 16, q0, q4
        # EE.SRCMB.S16.QACC q7, a14, 0
        tie728_s16_vector_round_result  q7, a14, a15, q6
        EE.VST.128.IP q7, a2, 16
    1:
    EE.ZERO.QACC
    EE.VMULAS.S16.QACC.LD.IP q3, a8, 16, q0, q1
    EE.VMULAS.S16.QACC.LD.IP q4, a9, 16, q0, q2
    EE.VMULAS.S16.QACC q0, q3
    EE.VMULAS.S16.QACC q0, q4
    # EE.SRCMB.S16.QACC q7, a14, 0
    tie728_s16_vector_round_result  q7, a14, a15, q6
    EE.VST.128.IP q7, a2, 0
    retw


    .align 4
    .text
    .global dl_tie728_s16_unaligned_avg_pool2d_22c1
    .type	dl_tie728_s16_unaligned_avg_pool2d_22c1, @function
    .section .iram1
dl_tie728_s16_unaligned_avg_pool2d_22c1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset_bytes
    # a6: input_x_offset_bytes
    # a11: c_div_x_1
    # a12: c_remainder_bytes
    # a13: mac_shift

    l32i a5, a4, 16     # input_y_offset_bytes
    l32i a6, a4, 20     # input_x_offset_bytes
    l32i a11, a4, 104   # c_div_x_1
    l32i a12, a4, 60    # c_remainder
    l32i a13, a4, 56    # mac_shift

    add a7, a3, a6
    add a8, a3, a5
    add a9, a8, a6

    blti a11, 0, dl_tie728_s16_unaligned_avg_pool2d_22c1_remainder
    EE.LD.128.USAR.IP q7, a2, 0
    RUR.SAR_BYTE a15

    addi a11, a11, 1

    addi a14, a4, 64
    EE.VLDBC.16 q0, a14    # avg_pool_area_inv

    EE.LD.128.USAR.IP q1, a3, 16
    EE.LD.128.USAR.IP q2, a3, 0
    beqi a15, 0, 0f
    beqi a15, 8, 8f

    loopgtz a11, 3f
    EE.ZERO.QACC

    EE.SRC.Q.LD.IP q3, a7, 16, q1, q2
    EE.LD.128.USAR.IP q4, a7, 0
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q.LD.IP q5, a8, 16, q3, q4
    EE.LD.128.USAR.IP q6, a8, 0
    EE.VMULAS.S16.QACC q0, q3

    EE.SRC.Q.LD.IP q3, a9, 16, q5, q6
    EE.LD.128.USAR.IP q4, a9, 0
    EE.VMULAS.S16.QACC q0, q5

    EE.SRC.Q.LD.IP q1, a3, 16, q3, q4
    EE.LD.128.USAR.IP q2, a3, 0
    EE.VMULAS.S16.QACC q0, q3

    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    dl_tie728_128b_unaligned_store0 q7, a2, a14

3:
    j dl_tie728_s16_unaligned_avg_pool2d_22c1_remainder

0:
   loopgtz a11, 4f
    EE.ZERO.QACC

    EE.SRC.Q.LD.IP q3, a7, 16, q1, q2
    EE.LD.128.USAR.IP q4, a7, 0
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q.LD.IP q5, a8, 16, q3, q4
    EE.LD.128.USAR.IP q6, a8, 0
    EE.VMULAS.S16.QACC q0, q3

    EE.SRC.Q.LD.IP q3, a9, 16, q5, q6
    EE.LD.128.USAR.IP q4, a9, 0
    EE.VMULAS.S16.QACC q0, q5

    EE.SRC.Q.LD.IP q1, a3, 16, q3, q4
    EE.LD.128.USAR.IP q2, a3, 0
    EE.VMULAS.S16.QACC q0, q3

    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    EE.VST.128.IP q7, a2, 16

4:
    j dl_tie728_s16_unaligned_avg_pool2d_22c1_remainder
8:
    loopgtz a11, 5f
    EE.ZERO.QACC

    EE.SRC.Q.LD.IP q3, a7, 16, q1, q2
    EE.LD.128.USAR.IP q4, a7, 0
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q.LD.IP q5, a8, 16, q3, q4
    EE.LD.128.USAR.IP q6, a8, 0
    EE.VMULAS.S16.QACC q0, q3

    EE.SRC.Q.LD.IP q3, a9, 16, q5, q6
    EE.LD.128.USAR.IP q4, a9, 0
    EE.VMULAS.S16.QACC q0, q5

    EE.SRC.Q.LD.IP q1, a3, 16, q3, q4
    EE.LD.128.USAR.IP q2, a3, 0
    EE.VMULAS.S16.QACC q0, q3

    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    dl_tie728_128b_unaligned_store1 q7, a2

5:

dl_tie728_s16_unaligned_avg_pool2d_22c1_remainder:

    beqz a12, 6f
    EE.ZERO.QACC
    EE.SRC.Q.LD.IP q3, a7, 16, q1, q2
    EE.LD.128.USAR.IP q4, a7, 0
    EE.VMULAS.S16.QACC q0, q1
    EE.SRC.Q.LD.IP q5, a8, 16, q3, q4
    EE.LD.128.USAR.IP q6, a8, 0
    EE.VMULAS.S16.QACC q0, q3
    EE.SRC.Q q5, q5, q6
    EE.LD.128.USAR.XP q3, a9, a12
    EE.VLD.128.IP q4, a9, 0
    EE.VMULAS.S16.QACC q0, q5
    EE.SRC.Q q3, q3, q4
    EE.VMULAS.S16.QACC q0, q3
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    srli a12, a12, 1
    dl_tie728_s16_store_remainder q7, a12, a14, a2
6:
    retw



    .align 4
    .text
    .global dl_tie728_s16_avg_pool2d_hwc1
    .type	dl_tie728_s16_avg_pool2d_hwc1, @function
    .section .iram1
dl_tie728_s16_avg_pool2d_hwc1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset
    # a6: input_x_offset
    # a11: c_div_x_1
    # a13: mac_shift


    l32i a5, a4, 16     # input_y_offset
    l32i a6, a4, 20     # input_x_offset
    l32i a8, a4, 48     # filter_height
    l32i a9, a4, 52     # filter_width
    l32i a11, a4, 104   # c_div_x_1
    l32i a13, a4, 56       # mac_shift

    addi a14, a4, 64
    EE.VLDBC.16 q0, a14    # avg_pool_area_inv

    srli a10, a9, 1
    addi a10, a10, -1       # filter_w / 2 - 1

    blti a9, 2, dl_tie728_s16_avg_pool2d_h1c1_loop
    blti a11, 1, dl_tie728_s16_avg_pool2d_hwc1_small_channel

1:  # loop c
    mov a7, a3
    mov a14, a7   # input_ptr
    mov a15, a8   # height
    EE.ZERO.QACC

2:  # loop h
    EE.VLD.128.XP q1, a14, a6
    EE.VLD.128.XP q2, a14, a6
    loopgtz a10, 3f
    EE.VMULAS.S16.QACC.LD.XP q1, a14, a6, q0, q1
    EE.VMULAS.S16.QACC.LD.XP q2, a14, a6, q0, q2
3:
    bbci a9, 0, 4f
    # w left 3
    EE.VMULAS.S16.QACC.LD.XP q1, a14, a6, q0, q1
    EE.VMULAS.S16.QACC q0, q2
    EE.VMULAS.S16.QACC q0, q1
    j 5f

4:  # w left 2
    EE.VMULAS.S16.QACC q0, q1
    EE.VMULAS.S16.QACC q0, q2
5:
    addi a15, a15, -1
    add a7, a7, a5
    mov a14, a7
    bnez a15, 2b
6:
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    EE.VST.128.IP q7, a2, 16
    addi a3, a3, 16
    addi a11, a11, -1
    bnez a11, 1b

dl_tie728_s16_avg_pool2d_hwc1_small_channel:
    mov a7, a3
    mov a14, a7   # input_ptr
    mov a15, a8   # height
    EE.ZERO.QACC

2:  # loop h
    EE.VLD.128.XP q1, a14, a6
    EE.VLD.128.XP q2, a14, a6
    loopgtz a10, 3f
    EE.VMULAS.S16.QACC.LD.XP q1, a14, a6, q0, q1
    EE.VMULAS.S16.QACC.LD.XP q2, a14, a6, q0, q2
3:
    bbci a9, 0, 4f
    # w left 3
    EE.VMULAS.S16.QACC.LD.XP q1, a14, a6, q0, q1
    EE.VMULAS.S16.QACC q0, q2
    EE.VMULAS.S16.QACC q0, q1
    j 5f

4:  # w left 2
    EE.VMULAS.S16.QACC q0, q1
    EE.VMULAS.S16.QACC q0, q2
5:
    addi a15, a15, -1
    add a7, a7, a5
    mov a14, a7
    bnez a15, 2b
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    EE.VST.128.IP q7, a2, 0
    retw

dl_tie728_s16_avg_pool2d_h1c1_loop:
    blti a11, 1, dl_tie728_s16_max_pool2d_h1c1_small_channel
1:
    mov a14, a3
    EE.ZERO.QACC
    EE.VLD.128.XP q1, a14, a5
    loopgtz a8, 2f
    EE.VMULAS.S16.QACC.LD.XP q1, a14, a5, q0, q1
2:
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    EE.VST.128.IP q7, a2, 16
    addi a3, a3, 16
    addi a11, a11, -1
    bnez a11, 1b

dl_tie728_s16_max_pool2d_h1c1_small_channel:
    mov a14, a3
    EE.ZERO.QACC
    EE.VLD.128.XP q1, a14, a5
    loopgtz a8, 2f
    EE.VMULAS.S16.QACC.LD.XP q1, a14, a5, q0, q1
2:
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a15, q6
    EE.VST.128.IP q7, a2, 16
    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_avg_pool2d_hwc1
    .type	dl_tie728_s16_unaligned_avg_pool2d_hwc1, @function
    .section .iram1
dl_tie728_s16_unaligned_avg_pool2d_hwc1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset
    # a6: input_x_offset
    # a11: c_div_x_1
    # a12: c_remainder_bytes
    # a13: mac_shift


    l32i a5, a4, 16     # input_y_offset
    l32i a6, a4, 20     # input_x_offset
    l32i a8, a4, 48     # filter_height
    l32i a9, a4, 52     # filter_width
    l32i a11, a4, 104   # c_div_x_1
    l32i a12, a4, 60    # c_remainder
    l32i a13, a4, 56       # mac_shift

    addi a14, a4, 64
    EE.VLDBC.16 q0, a14    # avg_pool_area_inv

    srli a10, a9, 1
    addi a10, a10, -1       # filter_w / 2 - 1

    addi a6, a6, -16
    addi a11, a11, 1
    EE.LD.128.USAR.IP q7, a2, 0
    RUR.SAR_BYTE a15

    blti a9, 2, dl_tie728_s16_unaligned_avg_pool2d_h1c1_loop
    blti a11, 1, dl_tie728_s16_unaligned_avg_pool2d_hwc1_small_channel

1:  # loop c
    mov a7, a3
    mov a14, a7   # input_ptr
    mov a4, a8   # height
    EE.ZERO.QACC
2:  # loop h
    EE.LD.128.USAR.IP q1, a14, 16
    EE.LD.128.USAR.XP q2, a14, a6
    loopgtz a10, 3f
    EE.SRC.Q.LD.IP q3, a14, 16, q1, q2
    EE.LD.128.USAR.XP q4, a14, a6
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q.LD.IP q1, a14, 16, q3, q4
    EE.LD.128.USAR.XP q2, a14, a6
    EE.VMULAS.S16.QACC q0, q3

3:
    bbci a9, 0, 4f
    # w left 3
    EE.SRC.Q.LD.IP q3, a14, 16, q1, q2
    EE.LD.128.USAR.XP q4, a14, a6
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q.LD.IP q1, a14, 16, q3, q4
    EE.LD.128.USAR.XP q2, a14, a6
    EE.VMULAS.S16.QACC q0, q3

    EE.SRC.Q q1, q1, q2
    EE.VMULAS.S16.QACC q0, q1
    j 5f

4:  # w left 2
    EE.SRC.Q.LD.IP q3, a14, 16, q1, q2
    EE.LD.128.USAR.XP q4, a14, a6
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q q3, q3, q4
    EE.VMULAS.S16.QACC q0, q3
5:
    addi a4, a4, -1
    add a7, a7, a5
    mov a14, a7
    bnez a4, 2b
6:
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a14, q6

    beqi a15, 0, 7f
    beqi a15, 8, 8f
    dl_tie728_128b_unaligned_store0 q7, a2, a14
    j 9f
7:
    EE.VST.128.IP q7, a2, 16
    j 9f
8:
    dl_tie728_128b_unaligned_store1 q7, a2
9:
    addi a3, a3, 16
    addi a11, a11, -1
    bnez a11, 1b

dl_tie728_s16_unaligned_avg_pool2d_hwc1_small_channel:
    beqz a12, 9f

    mov a7, a3
    mov a14, a7   # input_ptr
    mov a4, a8   # height
    EE.ZERO.QACC
2:  # loop h
    EE.LD.128.USAR.IP q1, a14, 16
    EE.LD.128.USAR.XP q2, a14, a6
    loopgtz a10, 3f
    EE.SRC.Q.LD.IP q3, a14, 16, q1, q2
    EE.LD.128.USAR.XP q4, a14, a6
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q.LD.IP q1, a14, 16, q3, q4
    EE.LD.128.USAR.XP q2, a14, a6
    EE.VMULAS.S16.QACC q0, q3

3:
    bbci a9, 0, 4f
    # w left 3
    EE.SRC.Q.LD.IP q3, a14, 16, q1, q2
    EE.LD.128.USAR.XP q4, a14, a6
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q q3, q3, q4
    EE.LD.128.USAR.XP q1, a14, a12
    EE.VLD.128.IP q2, a14, 0
    EE.VMULAS.S16.QACC q0, q3

    EE.SRC.Q q1, q1, q2
    EE.VMULAS.S16.QACC q0, q1
    j 5f

4:  # w left 2
    EE.SRC.Q q1, q1, q2
    EE.LD.128.USAR.XP q3, a14, a12
    EE.VLD.128.IP q4, a14, 0
    EE.VMULAS.S16.QACC q0, q1

    EE.SRC.Q q3, q3, q4
    EE.VMULAS.S16.QACC q0, q3
5:
    addi a4, a4, -1
    add a7, a7, a5
    mov a14, a7
    bnez a4, 2b
6:
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a14, q6
    srli a12, a12, 1
    dl_tie728_s16_store_remainder q7, a12, a14, a2
9:

    retw

dl_tie728_s16_unaligned_avg_pool2d_h1c1_loop:
    addi a5, a5, -16
    blti a11, 1, dl_tie728_s16_unaligned_avg_pool2d_h1c1_small_channel
1:
    mov a14, a3
    EE.ZERO.QACC

    loopgtz a8, 2f
    EE.LD.128.USAR.IP q1, a14, 16
    EE.VLD.128.XP q2, a14, a5
    EE.SRC.Q q1, q1, q2
    EE.VMULAS.S16.QACC q0, q1
2:
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a14, q6
    beqi a15, 0, 3f
    beqi a15, 8, 4f
    dl_tie728_128b_unaligned_store0 q7, a2, a14
    j 5f
3:
    EE.VST.128.IP q7, a2, 16
    j 5f
4:
    dl_tie728_128b_unaligned_store1 q7, a2
5:
    addi a3, a3, 16
    addi a11, a11, -1
    bnez a11, 1b

dl_tie728_s16_unaligned_avg_pool2d_h1c1_small_channel:
    beqz a12, 5f
1:
    mov a14, a3
    addi a5, a5, 16
    sub a5, a5, a12
    EE.ZERO.QACC

    loopgtz a8, 2f
    EE.LD.128.USAR.XP q1, a14, a12
    EE.VLD.128.XP q2, a14, a5
    EE.SRC.Q q1, q1, q2
    EE.VMULAS.S16.QACC q0, q1
2:
    # EE.SRCMB.S16.QACC q7, a13, 0
    tie728_s16_vector_round_result  q7, a13, a14, q6
    srli a12, a12, 1
    dl_tie728_s16_store_remainder q7, a12, a14, a2
5:
    retw
