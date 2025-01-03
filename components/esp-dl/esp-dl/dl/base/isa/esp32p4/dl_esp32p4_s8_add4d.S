#include "dl_esp32p4_s8.S"
#include "dl_esp32p4_common.S"


#void dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd
    .type   dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd, @function
    #.section .iram1
dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd:
    .align 2

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: length

    lw a4, 44(a3)
    srai a3, a4, 4

    li t0, 0
loop:
    beq t0, a3, end
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vadd.s8 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop
end:
    ret


#void dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd
    .type   dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd, @function
    #.section .iram1
dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd:
    .align 2

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: length

    lw a4, 44(a3)
    srai a3, a4, 4

    li t0, 0
loop_:
    beq t0, a3, end_
    esp.vld.128.ip q0, a1, 16
    esp.vldbc.8.ip q1, a2, 0
    esp.vadd.s8 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop_
end_:
    ret


#void dl_esp32p4_s8_add4d_bchw_w1_1_w2_16_simdadd(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s8_add4d_bchw_w1_1_w2_16_simdadd
    .type   dl_esp32p4_s8_add4d_bchw_w1_1_w2_16_simdadd, @function
    #.section .iram1
dl_esp32p4_s8_add4d_bchw_w1_1_w2_16_simdadd:
    .align 2

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: length

    lw a4, 44(a3)
    srai a3, a4, 4

    li t0, 0
loop__:
    beq t0, a3, end__
    esp.vldbc.8.ip q0, a1, 0
    esp.vld.128.ip q1, a2, 16
    esp.vadd.s8 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop__
end__:
    ret


    .align 2
    .text
    .global dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned
    .type   dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned, @function
    #.section .iram1
dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift
    # t5: c_remainder

    lw a4, 64(a3)
    lw t5, 76(a3)
    #lw a5, 88(a3)

    #bgez a5, dl_esp32p4_s8_unaligned_rescale_add2d_11c

# input0 exp = input1 exp = output exp

    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_small_remainder # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_0
    li t0, 8
    beq s1, t0, dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_1


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s8 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0
    esp.src.q.qup q5, q3, q4

    esp.vadd.s8 q2, q2, q5
    esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    j dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_remainder

    #output sar = 0
    dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_0:
    add t0, a4, x0
    blez t0, 3f
    2:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s8 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0
    esp.src.q.qup q5, q3, q4

    esp.vadd.s8 q2, q2, q5
    esp.vst.128.ip q2, a0, 16
    j dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_remainder

    # #output sar = 8
    dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_1:
    add t0, a4, x0
    blez t0, 5f
    4:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s8 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp32p4_s8_64b_unaligned_vector_store q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:
    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0
    esp.src.q.qup q5, q3, q4

    esp.vadd.s8 q2, q2, q5
    esp32p4_s8_64b_unaligned_vector_store q2, a0
    j dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_remainder

dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_small_remainder:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0

dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned_remainder:

    beqz t5, dl_esp32p4_s8_unaligned_add2d_end

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.vadd.s8 q2, q2, q5

    dl_esp32p4_s8_store_remainder q2, t4, t6, s0, s1, t0, a0, t5

    dl_esp32p4_s8_unaligned_add2d_end:

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret




    .align 2
    .text
    .global dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned
    .type   dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned, @function
    #.section .iram1
dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr broadcast
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift
    # t5: c_remainder

    lw a4, 64(a3)
    lw t5, 76(a3)
    #lw a5, 88(a3)

    #bgez a5, dl_esp32p4_s8_unaligned_rescale_add2d_11c

# input0 exp = input1 exp = output exp

    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_small_remainder # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    #esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_0
    li t0, 8
    beq s1, t0, dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_1


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a2, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a2, 0

        esp.vadd.s8 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6
    esp.src.q.qup q2, q0, q1

    #esp.ld.128.usar.xp q4, a2, t5
    #esp.movx.r.sar.bytes s0
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0

    esp.vadd.s8 q2, q2, q5
    esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    j dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_remainder

    #output sar = 0
    dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_0:
    add t0, a4, x0
    blez t0, 3f
    2:
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a2, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a2, 0

        esp.vadd.s8 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6
    esp.src.q.qup q2, q0, q1

    #esp.ld.128.usar.xp q4, a2, t5
    #esp.movx.r.sar.bytes s0
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0

    esp.vadd.s8 q2, q2, q5
    esp.vst.128.ip q2, a0, 16
    j dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_remainder

    # #output sar = 8
    dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_1:
    add t0, a4, x0
    blez t0, 5f
    4:
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a2, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a2, 0

        esp.vadd.s8 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp32p4_s8_64b_unaligned_vector_store q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:
    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6
    esp.src.q.qup q2, q0, q1

    #esp.ld.128.usar.xp q4, a2, t5
    #esp.movx.r.sar.bytes s0
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0


    esp.vadd.s8 q2, q2, q5
    esp32p4_s8_64b_unaligned_vector_store q2, a0
    j dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_remainder

dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_small_remainder:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6

    #esp.ld.128.usar.xp q3, a2, t5
    #esp.movx.r.sar.bytes s0
    esp.vldbc.8.ip q5, a2, 0

dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned_remainder:

    beqz t5, dl_esp32p4_s8_unaligned_add2d_end__

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    #esp.ld.128.usar.ip q4, a2, 0
    #esp.movx.w.sar.bytes s0
    #esp.src.q q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0

    esp.vadd.s8 q2, q2, q5

    dl_esp32p4_s8_store_remainder q2, t4, t6, s0, s1, t0, a0, t5

    dl_esp32p4_s8_unaligned_add2d_end__:

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
