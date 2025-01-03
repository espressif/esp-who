############################################################################################################################################################
####
#### dl_esp32p4_s8_max_pool2d series
####
############################################################################################################################################################

#include "dl_esp32p4_s8.S"
#include "dl_esp32p4_common.S"

    .align 2
    .text
    .global dl_esp32p4_s8_max_pool2d_22c1
    .type   dl_esp32p4_s8_max_pool2d_22c1, @function
    #.section .iram1
dl_esp32p4_s8_max_pool2d_22c1:
    .align      2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9
    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    lw a3, 16(a2)     # input_y_offset
    lw a4, 20(a2)     # input_x_offset
    lw t6, 104(a2)   # c_div_x_1

    add a5, a1, a4
    add t3, a1, a3
    add t4, t3, a4

    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a5, 16
    esp.vld.128.ip q2, t3, 16
    add t0, t6, x0
    blez t0, 1f
    0:
        esp.vmax.s8.ld.incp q3, t4, q7, q0, q1
        esp.vmax.s8.ld.incp q0, a1, q7, q7, q2
        esp.vmax.s8.ld.incp q1, a5, q7, q7, q3
        esp.vld.128.ip q2, t3, 16
        esp.vst.128.ip q7, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    esp.vmax.s8.ld.incp q3, t4, q7, q0, q1
    esp.vmax.s8 q7, q7, q2
    esp.vmax.s8 q7, q7, q3
    esp.vst.128.ip q7, a0, 16

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret





    .align 2
    .text
    .global dl_esp32p4_s8_unaligned_max_pool2d_22c1
    .type   dl_esp32p4_s8_unaligned_max_pool2d_22c1, @function
    #.section .iram1
dl_esp32p4_s8_unaligned_max_pool2d_22c1:
    .align      2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9
    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    lw a3, 16(a2)     # input_y_offset
    lw a4, 20(a2)     # input_x_offset
    lw t5, 4(a2)     # input_channel
    lw t6, 104(a2)   # c_div_x_1
    lw s0, 60(a2)    # c_remainder

    add a5, a1, a4
    add t3, a1, a3
    add t4, t3, a4

    bltz t6, dl_esp32p4_s8_unaligned_max_pool2d_22c1_remainder #channel < 16

    esp.ld.128.usar.ip q6, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s9

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q1, a1, 0

    beqz s9, 2f
    li t0, 8
    beq s9, t0, 3f

    add t0, t6, x0
    blez t0, 1f
    0:
        esp.src.q.ld.ip q2, a5, 16, q0, q1

        esp.ld.128.usar.ip q3, a5, 0
        esp.src.q.ld.ip q4, t3, 16, q2, q3

        esp.ld.128.usar.ip q5, t3, 0
        esp.vmax.s8 q7, q0, q2
        esp.src.q.ld.ip q2, t4, 16, q4, q5

        esp.ld.128.usar.ip q3, t4, 0
        esp.vmax.s8 q7, q7, q4
        esp.src.q.ld.ip q0, a1, 16, q2, q3

        esp.ld.128.usar.ip q1, a1, 0
        esp.vmax.s8 q7, q7, q2
        esp32p4_s8_32b_unaligned_vector_store q7, a0, s8
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    j dl_esp32p4_s8_unaligned_max_pool2d_22c1_loop_end


2:
    add t0, t6, x0
    blez t0, 1f
    0:
        esp.src.q.ld.ip q2, a5, 16, q0, q1

        esp.ld.128.usar.ip q3, a5, 0
        esp.src.q.ld.ip q4, t3, 16, q2, q3

        esp.ld.128.usar.ip q5, t3, 0
        esp.vmax.s8 q7, q0, q2
        esp.src.q.ld.ip q2, t4, 16, q4, q5

        esp.ld.128.usar.ip q3, t4, 0
        esp.vmax.s8 q7, q7, q4
        esp.src.q.ld.ip q0, a1, 16, q2, q3

        esp.ld.128.usar.ip q1, a1, 0
        esp.vmax.s8 q7, q7, q2
        esp.vst.128.ip q7, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    j dl_esp32p4_s8_unaligned_max_pool2d_22c1_loop_end

3:
    add t0, t6, x0
    blez t0, 1f
    0:
        esp.src.q.ld.ip q2, a5, 16, q0, q1

        esp.ld.128.usar.ip q3, a5, 0
        esp.src.q.ld.ip q4, t3, 16, q2, q3

        esp.ld.128.usar.ip q5, t3, 0
        esp.vmax.s8 q7, q0, q2
        esp.src.q.ld.ip q2, t4, 16, q4, q5

        esp.ld.128.usar.ip q3, t4, 0
        esp.vmax.s8 q7, q7, q4
        esp.src.q.ld.ip q0, a1, 16, q2, q3

        esp.ld.128.usar.ip q1, a1, 0
        esp.vmax.s8 q7, q7, q2
        esp32p4_s8_64b_unaligned_vector_store q7, a0
        addi t0, t0, -1
        bgtz t0, 0b
    1:


dl_esp32p4_s8_unaligned_max_pool2d_22c1_loop_end:
    esp.src.q.ld.ip q2, a5, 16, q0, q1

    esp.ld.128.usar.ip q3, a5, 0
    esp.src.q.ld.ip q4, t3, 16, q2, q3

    esp.ld.128.usar.ip q5, t3, 0
    esp.vmax.s8 q7, q0, q2
    esp.src.q.ld.ip q2, t4, 16, q4, q5

    esp.ld.128.usar.ip q3, t4, 0
    esp.vmax.s8 q7, q7, q4
    esp.src.q q2, q2, q3
    esp.vmax.s8 q7, q7, q2

    esp32p4_s8_32b_unaligned_vector_store q7, a0, s8

    beqz s0, dl_esp32p4_s8_unaligned_max_pool2d_22c1_end

dl_esp32p4_s8_unaligned_max_pool2d_22c1_remainder:
    esp.ld.128.usar.xp q0, a1, s0
    esp.vld.128.ip q1, a1, 0
    esp.src.q q0, q0, q1

    esp.ld.128.usar.xp q2, a5, s0
    esp.vld.128.ip q3, a5, 0
    esp.src.q q2, q2, q3

    esp.ld.128.usar.xp q4, t3, s0
    esp.vld.128.ip q5, t3, 0
    esp.vmax.s8 q7, q0, q2
    esp.src.q q4, q4, q5

    esp.ld.128.usar.xp q2, t4, s0
    esp.vld.128.ip q3, t4, 0
    esp.vmax.s8 q7, q7, q4
    esp.src.q q2, q2, q3

    esp.vmax.s8 q7, q7, q2

    dl_esp32p4_s8_store_remainder q7, t3, t4, t5, t6, t0, a0, s0

dl_esp32p4_s8_unaligned_max_pool2d_22c1_end:

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret





.macro dl_esp32p4_s8_max_pool2d_hw
    4:

        esp.vld.128.xp q0, s1, a4
        add t0, t5, x0
        blez t0, 1f
        0:
            esp.vld.128.xp q1, s1, a4
            esp.vmax.s8 q7, q7, q0

            esp.vld.128.xp q0, s1, a4
            esp.vmax.s8 q7, q7, q1
            addi t0, t0, -1
            bgtz t0, 0b
        1:

        andi t0, t4, 1
        beqz t0, 2f

        #three left
        esp.vld.128.xp q1, s1, a4
        esp.vmax.s8 q7, q7, q0
        esp.vld.128.xp q0, s1, a4
        esp.vmax.s8 q7, q7, q1
        esp.vmax.s8 q7, q7, q0
        j 3f

        2: # two left
        esp.vld.128.xp q1, s1, a4
        esp.vmax.s8 q7, q7, q0
        esp.vmax.s8 q7, q7, q1

        3:
        addi s8, s8, -1
        add a5, a5, a3
        add s1, a5, x0
    bnez s8, 4b
.endm


    .align 2
    .text
    .global dl_esp32p4_s8_max_pool2d_hwc1
    .type	dl_esp32p4_s8_max_pool2d, @function
    #.section .iram1
dl_esp32p4_s8_max_pool2d_hwc1:
    .align      2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9
    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    lw a3, 16(a2)     # input_y_offset
    lw a4, 20(a2)     # input_x_offset
    lw a5, 4(a2)      # input_channel
    lw t3, 48(a2)     # filter_height
    lw t4, 52(a2)     # filter_width
    lw t6, 104(a2)   # c_div_x_1

    srli t5, t4, 1
    addi t5, t5, -1   # filter_w / 2 - 1

    li t0, 1
    beq t4, t0, dl_esp32p4_s8_max_pool2d_h1c1 #filter_width == 1
    li t0, 1
    blt t6, t0, dl_esp32p4_s8_max_pool2d_hw_small_channel
    5:
        add a5, a1, x0
        add s1, a5, x0
        esp.vld.128.ip q7, s1, 0
        add s8, t3, x0

        dl_esp32p4_s8_max_pool2d_hw

        esp.vst.128.ip q7, a0, 16
        addi a1, a1, 16
        addi t6, t6, -1
    bnez t6, 5b

dl_esp32p4_s8_max_pool2d_hw_small_channel:
    add a5, a1, x0
    add s1, a5, x0
    esp.vld.128.ip q7, s1, 0
    add s8, t3, x0

    dl_esp32p4_s8_max_pool2d_hw
    esp.vst.128.ip q7, a0, 16

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


dl_esp32p4_s8_max_pool2d_h1c1:
    addi t3, t3, -1
    li t0, 1
    blt t6, t0, dl_esp32p4_s8_max_pool2d_h1_small_channel
    2:
        add s1, a1, x0
        esp.vld.128.xp q7, s1, a3
        add t0, t3, x0
        blez t0, 1f
        0:
            esp.vld.128.xp q0, s1, a3
            esp.vmax.s8 q7, q7, q0
            addi t0, t0, -1
            bgtz t0, 0b
        1:

        esp.vst.128.ip q7, a0, 16
        addi a1, a1, 16
        addi t6, t6, -1
    bnez t6, 2b

dl_esp32p4_s8_max_pool2d_h1_small_channel:
    add s1, a1, x0
    esp.vld.128.xp q7, s1, a3
    add t0, t3, x0
    blez t0, 1f
    0:
        esp.vld.128.xp q0, s1, a3
        esp.vmax.s8 q7, q7, q0
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    esp.vst.128.ip q7, a0, 16

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret




    .align 2
    .text
    .global dl_esp32p4_s8_unaligned_max_pool2d_hwc1
    .type   dl_esp32p4_s8_unaligned_max_pool2d_hwc1, @function
    #.section .iram1
dl_esp32p4_s8_unaligned_max_pool2d_hwc1:
    .align      2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9
    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    lw a3, 16(a2)     # input_y_offset
    lw a4, 20(a2)     # input_x_offset
    lw a5, 4(a2)      # input_channel
    lw t3, 48(a2)     # filter_height
    lw t4, 52(a2)     # filter_width
    lw t6, 104(a2)   # c_div_x_1
    lw s0, 60(a2)    # c_remainder

    srli t5, t4, 1
    addi t5, t5, -1   # filter_w / 2 - 1

    addi a4, a4, -16

    esp.ld.128.usar.ip q6, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s9

    addi t6, t6, 1

    li t0, 1
    beq t4, t0, dl_esp32p4_s8_unaligned_max_pool2d_h1c1 #filter_width == 1
    li t0, 1
    blt t6, t0, dl_esp32p4_s8_unaligned_max_pool2d_hw_remainder

    9:
        add a5, a1, x0
        add s1, a5, x0
        esp.ld.128.usar.ip q6, s1, 16
        esp.ld.128.usar.ip q7, s1, -16
        esp.src.q q7, q6, q7
        add s8, t3, x0

        4:
            esp.ld.128.usar.ip q0, s1, 16
            esp.ld.128.usar.xp q1, s1, a4
            add t0, t5, x0
            blez t0, 1f
            0:
                esp.src.q.ld.ip q2, s1, 16, q0, q1
                esp.ld.128.usar.xp q1, s1, a4
                esp.vmax.s8 q7, q7, q0

                esp.src.q.ld.ip q0, s1, 16, q2, q1
                esp.ld.128.usar.xp q1, s1, a4
                esp.vmax.s8 q7, q7, q2
                addi t0, t0, -1
                bgtz t0, 0b
            1:

            andi t0, t4, 1
            beqz t0, 2f

            #three left
            esp.src.q.ld.ip q2, s1, 16, q0, q1
            esp.ld.128.usar.xp q1, s1, a4
            esp.vmax.s8 q7, q7, q0

            esp.src.q.ld.ip q0, s1, 16, q2, q1
            esp.ld.128.usar.xp q1, s1, a4
            esp.vmax.s8 q7, q7, q2

            esp.src.q q0, q0, q1
            esp.vmax.s8 q7, q7, q0

            j 3f

            2:# two left
            esp.src.q.ld.ip q2, s1, 16, q0, q1
            esp.ld.128.usar.xp q1, s1, a4
            esp.vmax.s8 q7, q7, q0

            esp.src.q q2, q2, q1
            esp.vmax.s8 q7, q7, q2

            3:
            addi s8, s8, -1
            add a5, a5, a3
            add s1, a5, x0
        bnez s8, 4b

        beqz s9, 5f
        li t0, 8
        beq s9, t0, 6f

        esp32p4_s8_32b_unaligned_vector_store q7, a0, s8
        j 7f

        5:
        esp.vst.128.ip q7, a0, 16
        j 7f
        6:
        esp32p4_s8_64b_unaligned_vector_store q7, a0

        7:
        addi a1, a1, 16
        addi t6, t6, -1
    bnez t6, 9b

dl_esp32p4_s8_unaligned_max_pool2d_hw_remainder:
    beqz s0, dl_esp32p4_s8_unaligned_max_pool2d_hw_remainder_end

    add a5, a1, x0
    add s1, a5, x0
    esp.ld.128.usar.ip q6, s1, 16
    esp.ld.128.usar.ip q7, s1, -16
    esp.src.q q7, q6, q7
    add s8, t3, x0
    addi a4, a4, 16
    sub a4, a4, s0

    2:
        add t0, t4, x0
        blez t0, 1f
        0:
            esp.ld.128.usar.xp q0, s1, s0
            esp.vld.128.xp q1, s1, a4
            esp.src.q q0, q0, q1
            esp.vmax.s8 q7, q7, q0
            addi t0, t0, -1
            bgtz t0, 0b
        1:
        addi s8, s8, -1
        add a5, a5, a3
        add s1, a5, x0
    bnez s8, 2b

    dl_esp32p4_s8_store_remainder q7, t3, t4, t5, t6, t0, a0, s0
dl_esp32p4_s8_unaligned_max_pool2d_hw_remainder_end:

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


dl_esp32p4_s8_unaligned_max_pool2d_h1c1:
    addi a3, a3, -16
    addi t3, t3, -1
    li t0, 1
    blt t6, t0, dl_esp32p4_s8_unaligned_max_pool2d_h1_remainder

    5:
        add s1, a1, x0
        esp.ld.128.usar.ip q6, s1, 16
        esp.vld.128.xp q7, s1, a3
        esp.src.q q7, q6, q7

        add t0, t3, x0
        blez t0, 1f
        0:
            esp.ld.128.usar.ip q0, s1, 16
            esp.ld.128.usar.xp q1, s1, a3
            esp.src.q q0, q0, q1
            esp.vmax.s8 q7, q7, q0
            addi t0, t0, -1
            bgtz t0, 0b
        1:

        beqz s9, 2f
        li t0, 8
        beq s9, t0, 3f

        esp32p4_s8_32b_unaligned_vector_store q7, a0, s8
        j 4f
        2:
        esp.vst.128.ip q7, a0, 16
        j 4f
        3:
        esp32p4_s8_64b_unaligned_vector_store q7, a0

        4:
        addi a1, a1, 16
        addi t6, t6, -1
    bnez t6, 5b

dl_esp32p4_s8_unaligned_max_pool2d_h1_remainder:

    beqz s0, dl_esp32p4_s8_unaligned_max_pool2d_h1_remainder_end

    addi a3, a3, 16
    sub a3, a3, s0

    add s1, a1, x0
    esp.ld.128.usar.xp q6, s1, s0
    esp.vld.128.xp q7, s1, a3
    esp.src.q q7, q6, q7

    add t0, t3, x0
    blez t0, 1f
    0:
        esp.ld.128.usar.xp q0, s1, s0
        esp.vld.128.xp q1, s1, a3
        esp.src.q q0, q0, q1
        esp.vmax.s8 q7, q7, q0
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    dl_esp32p4_s8_store_remainder q7, t3, t4, t5, t6, t0, a0, s0

dl_esp32p4_s8_unaligned_max_pool2d_h1_remainder_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
