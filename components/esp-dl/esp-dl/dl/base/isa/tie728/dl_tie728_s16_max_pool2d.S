#include "dl_tie728_s16.S"

############################################################################################################################################################
####
#### dl_tie728_s16_max_pool2d series
####
############################################################################################################################################################

    .align 4
    .text
    .global dl_tie728_s16_max_pool2d_22c1
    .type	dl_tie728_s16_max_pool2d_22c1, @function
    .section .iram1
dl_tie728_s16_max_pool2d_22c1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset_bytes
    # a6: input_x_offset_bytes
    # a10: c_div_x_1

    l32i a5, a4, 16     # input_y_offset_bytes
    l32i a6, a4, 20     # input_x_offset_bytes
    l32i a10, a4, 104     # c_div_x_1

    add a7, a3, a6
    add a8, a3, a5
    add a9, a8, a6

0:

    EE.VLD.128.IP q0, a3, 16
    EE.VLD.128.IP q1, a7, 16
    loopgtz a10, 1f
        EE.VMAX.S16.LD.INCP q2, a8, q7, q0, q1
        EE.VMAX.S16.LD.INCP q3, a9, q7, q7, q2
        EE.VMAX.S16.LD.INCP q0, a3, q7, q7, q3
        EE.VST.128.IP q7, a2, 16
        EE.VLD.128.IP q1, a7, 16
1:
    EE.VMAX.S16.LD.INCP q2, a8, q7, q0, q1
    EE.VMAX.S16.LD.INCP q3, a9, q7, q7, q2
    EE.VMAX.S16 q7, q7, q3
    EE.VST.128.IP q7, a2, 16
    retw



    .align 4
    .text
    .global dl_tie728_s16_unaligned_max_pool2d_22c1
    .type	dl_tie728_s16_unaligned_max_pool2d_22c1, @function
    .section .iram1
dl_tie728_s16_unaligned_max_pool2d_22c1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset_bytes
    # a6: input_x_offset_bytes
    # a10: c_div_x_1
    # a12: c_remainder_bytes

    l32i a5, a4, 16     # input_y_offset
    l32i a6, a4, 20     # input_x_offset
    l32i a10, a4, 104
    l32i a12, a4, 60    # c_remainder

    add a7, a3, a6
    add a8, a3, a5
    add a9, a8, a6

    blti a10, 0, dl_tie728_s16_unaligned_max_pool2d_22c1_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    RUR.SAR_BYTE a13

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.IP q1, a3, 0

    beqi a13, 0, 0f
    beqi a13, 8, 8f

    loopgtz a10, 1f
        EE.SRC.Q.LD.IP q2, a7, 16, q0, q1
        EE.LD.128.USAR.IP q3, a7, 0
        EE.SRC.Q.LD.IP q4, a8, 16, q2, q3

        EE.LD.128.USAR.IP q5, a8, 0
        EE.VMAX.S16 q7, q0, q2
        EE.SRC.Q.LD.IP q2, a9, 16, q4, q5

        EE.LD.128.USAR.IP q3, a9, 0
        EE.VMAX.S16 q7, q7, q4
        EE.SRC.Q.LD.IP q0, a3, 16, q2, q3

        EE.LD.128.USAR.IP q1, a3, 0
        EE.VMAX.S16 q7, q7, q2

        dl_tie728_128b_unaligned_store0 q7, a2, a13
1:
    j  dl_tie728_s16_unaligned_max_pool2d_22c1_end

0:
    loopgtz a10, 2f
        EE.SRC.Q.LD.IP q2, a7, 16, q0, q1
        EE.LD.128.USAR.IP q3, a7, 0
        EE.SRC.Q.LD.IP q4, a8, 16, q2, q3

        EE.LD.128.USAR.IP q5, a8, 0
        EE.VMAX.S16 q7, q0, q2
        EE.SRC.Q.LD.IP q2, a9, 16, q4, q5

        EE.LD.128.USAR.IP q3, a9, 0
        EE.VMAX.S16 q7, q7, q4
        EE.SRC.Q.LD.IP q0, a3, 16, q2, q3

        EE.LD.128.USAR.IP q1, a3, 0
        EE.VMAX.S16 q7, q7, q2

        EE.VST.128.IP q7, a2, 16
2:
    j  dl_tie728_s16_unaligned_max_pool2d_22c1_end
8:
    loopgtz a10, 3f
        EE.SRC.Q.LD.IP q2, a7, 16, q0, q1
        EE.LD.128.USAR.IP q3, a7, 0
        EE.SRC.Q.LD.IP q4, a8, 16, q2, q3

        EE.LD.128.USAR.IP q5, a8, 0
        EE.VMAX.S16 q7, q0, q2
        EE.SRC.Q.LD.IP q2, a9, 16, q4, q5

        EE.LD.128.USAR.IP q3, a9, 0
        EE.VMAX.S16 q7, q7, q4
        EE.SRC.Q.LD.IP q0, a3, 16, q2, q3

        EE.LD.128.USAR.IP q1, a3, 0
        EE.VMAX.S16 q7, q7, q2

        dl_tie728_128b_unaligned_store1 q7, a2
3:
    j  dl_tie728_s16_unaligned_max_pool2d_22c1_end

dl_tie728_s16_unaligned_max_pool2d_22c1_end:
    EE.SRC.Q.LD.IP q2, a7, 16, q0, q1
    EE.LD.128.USAR.IP q3, a7, 0
    EE.SRC.Q.LD.IP q4, a8, 16, q2, q3

    EE.LD.128.USAR.IP q5, a8, 0
    EE.VMAX.S16 q7, q0, q2
    EE.SRC.Q.LD.IP q2, a9, 16, q4, q5

    EE.LD.128.USAR.IP q3, a9, 0
    EE.VMAX.S16 q7, q7, q4
    EE.SRC.Q q2, q2, q3
    EE.VMAX.S16 q7, q7, q2

    dl_tie728_128b_unaligned_store0 q7, a2, a13

    beqz a12, 4f

dl_tie728_s16_unaligned_max_pool2d_22c1_remainder:
    EE.LD.128.USAR.XP q0, a3, a12
    EE.VLD.128.IP q1, a3, 0
    EE.SRC.Q q0, q0, q1

    EE.LD.128.USAR.XP q2, a7, a12
    EE.VLD.128.IP q3, a7, 0
    EE.SRC.Q q2, q2, q3

    EE.LD.128.USAR.XP q4, a8, a12
    EE.VLD.128.IP q5, a8, 0
    EE.VMAX.S16 q7, q0, q2
    EE.SRC.Q q4, q4, q5

    EE.LD.128.USAR.XP q0, a9, a12
    EE.VLD.128.IP q1, a9, 0
    EE.VMAX.S16 q7, q7, q4
    EE.SRC.Q q0, q0, q1

    EE.VMAX.S16 q7, q7, q0
    srli a12, a12, 1
    dl_tie728_s16_store_remainder q7, a12, a14, a2
4:
    retw




    .align 4
    .text
    .global dl_tie728_s16_max_pool2d_hwc1
    .type	dl_tie728_s16_max_pool2d_hwc1, @function
    .section .iram1
dl_tie728_s16_max_pool2d_hwc1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset
    # a6: input_x_offset
    # a7: c_div_x_1
    # a8: filter_height
    # a9: filter_width
    # a10: filter_width/2 - 1

    l32i a5, a4, 16     # input_y_offset
    l32i a6, a4, 20     # input_x_offset
    l32i a7, a4, 104    # c_div_x_1
    l32i a8, a4, 48     # filter_height
    l32i a9, a4, 52     # filter_width

    srli a10, a9, 1
    addi a10, a10, -1    # w / 2 - 1

    blti a9, 2, dl_tie728_s16_max_pool2d_h1c1_loop
    blti a7, 1, dl_tie728_s16_max_pool2d_hwc1_small_channel

1:  # loop c
    mov a11, a3
    mov a13, a11
    EE.VLD.128.IP q7, a13, 0
    mov a14, a8

2:   # loop h
    EE.VLD.128.XP q0, a13, a6
    loopgtz a10, 3f       # loop w
    EE.VLD.128.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q0
    EE.VLD.128.XP q0, a13, a6
    EE.VMAX.S16 q7, q7, q1
3:
    bbci a9, 0, 4f
    # w left 3
    EE.VLD.128.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q0
    EE.VLD.128.XP q0, a13, a6
    EE.VMAX.S16 q7, q7, q1
    EE.VMAX.S16 q7, q7, q0
    j 5f

4:  # w left 2
    EE.VLD.128.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q0
    EE.VMAX.S16 q7, q7, q1

5:
    addi a14, a14, -1
    add a11, a11, a5
    mov a13, a11
    bnez a14, 2b
6:
    EE.VST.128.IP q7, a2, 16
    addi a3, a3, 16
    addi a7, a7, -1
    bnez a7, 1b


dl_tie728_s16_max_pool2d_hwc1_small_channel:

    mov a11, a3
    mov a13, a11
    EE.VLD.128.IP q7, a13, 0
    mov a14, a8

2:   # loop h
    EE.VLD.128.XP q0, a13, a6
    loopgtz a10, 3f       # loop w
    EE.VLD.128.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q0
    EE.VLD.128.XP q0, a13, a6
    EE.VMAX.S16 q7, q7, q1
3:
    bbci a9, 0, 4f
    # w left 3
    EE.VLD.128.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q0
    EE.VLD.128.XP q0, a13, a6
    EE.VMAX.S16 q7, q7, q1
    EE.VMAX.S16 q7, q7, q0
    j 5f

4:  # w left 2
    EE.VLD.128.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q0
    EE.VMAX.S16 q7, q7, q1

5:
    addi a14, a14, -1
    add a11, a11, a5
    mov a13, a11
    bnez a14, 2b
6:
    EE.VST.128.IP q7, a2, 16
    retw

dl_tie728_s16_max_pool2d_h1c1_loop:
    blti a7, 1, dl_tie728_s16_max_pool2d_h1c1_small_channel
1:
    mov a13, a3
    EE.VLD.128.IP q7, a13, 0
    loopgtz a8, 2f
    EE.VLD.128.XP q0, a13, a5
    EE.VMAX.S16 q7, q7, q0
2:
    EE.VST.128.IP q7, a2, 16
    addi a3, a3, 16
    addi a7, a7, -1
    bnez a7, 1b

dl_tie728_s16_max_pool2d_h1c1_small_channel:
    mov a13, a3
    EE.VLD.128.IP q7, a13, 0
    loopgtz a8, 1f
    EE.VLD.128.XP q0, a13, a5
    EE.VMAX.S16 q7, q7, q0
1:
    EE.VST.128.IP q7, a2, 16
    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_max_pool2d_hwc1
    .type	dl_tie728_s16_unaligned_max_pool2d_hwc1, @function
    .section .iram1
dl_tie728_s16_unaligned_max_pool2d_hwc1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args
    # a5: input_y_offset
    # a6: input_x_offset
    # a7: c_div_x_1
    # a8: filter_height
    # a9: filter_width
    # a10: filter_width/2 - 1
    # a12: c_remainder_bytes

    l32i a5, a4, 16     # input_y_offset
    l32i a6, a4, 20     # input_x_offset
    l32i a7, a4, 104    # c_div_x_1
    l32i a8, a4, 48     # filter_height
    l32i a9, a4, 52     # filter_width
    l32i a12, a4, 60    # c_remainder_bytes

    srli a10, a9, 1
    addi a10, a10, -1    # w/2-1

    addi a6, a6, -16

    EE.LD.128.USAR.IP q0, a2, 0
    RUR.SAR_BYTE a15

    addi a7, a7, 1

    blti a9, 2, dl_tie728_s16_unaligned_max_pool2d_h1c1_loop
    blti a7, 1, dl_tie728_s16_unaligned_max_pool2d_hwc1_small_channel

    1:  # loop c
    mov a11, a3
    mov a13, a11
    EE.LD.128.USAR.IP q0, a13, 16
    EE.LD.128.USAR.IP q1, a13, -16
    EE.SRC.Q q7, q0, q1
    mov a14, a8

2:   # loop h
    EE.LD.128.USAR.IP q0, a13, 16
    EE.LD.128.USAR.XP q1, a13, a6
    loopgtz a10, 3f       # loop w
    EE.SRC.Q.LD.IP q2, a13, 16, q0, q1
    EE.LD.128.USAR.XP q3, a13, a6
    EE.VMAX.S16 q7, q7, q0

    EE.SRC.Q.LD.IP q0, a13, 16, q2, q3
    EE.LD.128.USAR.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q2
3:
    bbci a9, 0, 4f
    # w left 3
    EE.SRC.Q.LD.IP q2, a13, 16, q0, q1
    EE.LD.128.USAR.XP q3, a13, a6
    EE.VMAX.S16 q7, q7, q0

    EE.SRC.Q.LD.IP q0, a13, 16, q2, q3
    EE.LD.128.USAR.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q2

    EE.SRC.Q q0, q0, q1
    EE.VMAX.S16 q7, q7, q0
    j 5f

4:  # w left 2
    EE.SRC.Q.LD.IP q2, a13, 16, q0, q1
    EE.LD.128.USAR.XP q3, a13, a6
    EE.VMAX.S16 q7, q7, q0

    EE.SRC.Q q2, q2, q3
    EE.VMAX.S16 q7, q7, q2

5:
    addi a14, a14, -1
    add a11, a11, a5
    mov a13, a11
    bnez a14, 2b
6:
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
    addi a7, a7, -1
    bnez a7, 1b


dl_tie728_s16_unaligned_max_pool2d_hwc1_small_channel:
    beqz a12, 9f

    mov a11, a3
    mov a13, a11
    EE.LD.128.USAR.IP q0, a13, 16
    EE.LD.128.USAR.IP q1, a13, -16
    EE.SRC.Q q7, q0, q1
    mov a14, a8

2:   # loop h
    EE.LD.128.USAR.IP q0, a13, 16
    EE.LD.128.USAR.XP q1, a13, a6
    loopgtz a10, 3f       # loop w
    EE.SRC.Q.LD.IP q2, a13, 16, q0, q1
    EE.LD.128.USAR.XP q3, a13, a6
    EE.VMAX.S16 q7, q7, q0

    EE.SRC.Q.LD.IP q0, a13, 16, q2, q3
    EE.LD.128.USAR.XP q1, a13, a6
    EE.VMAX.S16 q7, q7, q2
3:
    bbci a9, 0, 4f
    # w left 3
    EE.SRC.Q.LD.IP q2, a13, 16, q0, q1
    EE.LD.128.USAR.XP q3, a13, a6
    EE.VMAX.S16 q7, q7, q0

    EE.SRC.Q q2, q2, q3
    EE.LD.128.USAR.XP q0, a13, a12
    EE.VLD.128.IP q1, a13, 0
    EE.VMAX.S16 q7, q7, q2

    EE.SRC.Q q0, q0, q1
    EE.VMAX.S16 q7, q7, q0
    j 5f

4:  # w left 2
    EE.SRC.Q q0, q0, q1
    EE.LD.128.USAR.XP q2, a13, a12
    EE.VLD.128.IP q3, a13, 0
    EE.VMAX.S16 q7, q7, q0

    EE.SRC.Q q2, q2, q3
    EE.VMAX.S16 q7, q7, q2

5:
    addi a14, a14, -1
    add a11, a11, a5
    mov a13, a11
    bnez a14, 2b
6:
    srli a12, a12, 1
    dl_tie728_s16_store_remainder q7, a12, a14, a2
9:
    retw


dl_tie728_s16_unaligned_max_pool2d_h1c1_loop:
    addi a5, a5, -16
    blti a7, 1, dl_tie728_s16_unaligned_max_pool2d_h1c1_small_channel
1:
    mov a13, a3
    EE.LD.128.USAR.IP q0, a13, 16
    EE.LD.128.USAR.IP q1, a13, -16
    EE.SRC.Q q7, q0, q1

    loopgtz a8, 2f
    EE.LD.128.USAR.IP q0, a13, 16
    EE.LD.128.USAR.XP q1, a13, a5
    EE.SRC.Q q0, q0, q1
    EE.VMAX.S16 q7, q7, q0
2:
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
    addi a7, a7, -1
    bnez a7, 1b

dl_tie728_s16_unaligned_max_pool2d_h1c1_small_channel:
    beqz a12, 5f

    mov a13, a3
    EE.LD.128.USAR.XP q0, a13, a12
    EE.VLD.128.IP q1, a13, 0
    EE.SRC.Q q7, q0, q1
    sub a13, a13, a12
    addi a5, a5, 16
    sub a5, a5, a12

    loopgtz a8, 1f
    EE.LD.128.USAR.XP q0, a13, a12
    EE.VLD.128.XP q1, a13, a5
    EE.SRC.Q q0, q0, q1
    EE.VMAX.S16 q7, q7, q0
1:
    srli a12, a12, 1
    dl_tie728_s16_store_remainder q7, a12, a14, a2
5:
    retw
