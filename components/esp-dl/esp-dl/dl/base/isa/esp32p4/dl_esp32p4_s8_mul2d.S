#include "dl_esp32p4_s8.S"
#include "dl_esp32p4_common.S"


############################################################################################################################################################
####
#### esp32p4_s8_mul2d_11c series
####
############################################################################################################################################################

    .align 2
    .text
    .global dl_esp32p4_s8_mul2d_11c
    .type   dl_esp32p4_s8_mul2d_11c, @function
    #.section .iram1
dl_esp32p4_s8_mul2d_11c:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: mul_shift

    lw a4, 64(a3)
    lw a5, 100(a3)
    lw t3, 76(a3)

    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.vmulas.s8.qacc.ld.ip q0, a1, 16, q0, q1
        esp.vld.128.ip q1, a2, 16
        esp.srcmb.s8.qacc q2, a5, 1
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    esp.zero.qacc
    esp.vmulas.s8.qacc q0, q1
    esp.srcmb.s8.qacc q2, a5, 1

    esp.vst.128.ip q2, a0, 16

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret




    .align 2
    .text
    .global dl_esp32p4_s8_mul2d_11c_relu
    .type   dl_esp32p4_s8_mul2d_11c_relu, @function
    #.section .iram1
dl_esp32p4_s8_mul2d_11c_relu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: mul_shift
    # s8: activation_alpha
    # s9: activation_shift


    lw a4, 64(a3)
    lw a5, 100(a3)
    lw t3, 76(a3)
    lw s8, 52(a3)
    lw s9, 60(a3)


    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.vmulas.s8.qacc.ld.ip q0, a1, 16, q0, q1
        esp.vld.128.ip q1, a2, 16
        esp.srcmb.s8.qacc q2, a5, 1
        esp.vrelu.s8 q2, s8, s9
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    esp.zero.qacc
    esp.vmulas.s8.qacc q0, q1
    esp.srcmb.s8.qacc q2, a5, 1

    esp.vrelu.s8 q2, s8, s9
    esp.vst.128.ip q2, a0, 16

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret




    .align 2
    .text
    .global dl_esp32p4_s8_mul2d_11c_prelu
    .type   dl_esp32p4_s8_mul2d_11c_prelu, @function
    #.section .iram1
dl_esp32p4_s8_mul2d_11c_prelu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: mul_shift
    # s8: activation_alpha_ptr
    # s9: activation_shift


    lw a4, 64(a3)
    lw a5, 100(a3)
    lw s8, 56(a3)
    lw s9, 60(a3)


    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.vmulas.s8.qacc.ld.ip q0, a1, 16, q0, q1
        esp.vld.128.ip q1, a2, 16

        esp.vld.128.ip q3, s8, 16
        esp.srcmb.s8.qacc q2, a5, 1
        esp.vprelu.s8 q2, q2, q3, s9
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    esp.zero.qacc
    esp.vmulas.s8.qacc q0, q1
    esp.vld.128.ip q3, s8, 16
    esp.srcmb.s8.qacc q2, a5, 1

    esp.vprelu.s8 q2, q2, q3, s9
    esp.vst.128.ip q2, a0, 16

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret






############################################################################################################################################################
####
#### esp32p4_s8_unaligned_mul2d_11c series
####
############################################################################################################################################################

    .align 2
    .text
    .global dl_esp32p4_s8_unaligned_mul2d_11c
    .type   dl_esp32p4_s8_unaligned_mul2d_11c, @function
    #.section .iram1
dl_esp32p4_s8_unaligned_mul2d_11c:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: c_remainder
    # t3: mul_shift


    lw a4, 64(a3)
    lw a5, 76(a3)
    lw t3, 100(a3)



    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_tie718_s8_unaligned_mul2d_11c_small_remainder # channel < 16


    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_tie718_s8_unaligned_mul2d_11c_0
    li t0, 8
    beq s1, t0, dl_tie718_s8_unaligned_mul2d_11c_1


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a1, 16
        esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    j dl_tie718_s8_unaligned_mul2d_11c_remainder

dl_tie718_s8_unaligned_mul2d_11c_0:

    add t0, a4, x0
    blez t0, 3f
    2:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a1, 16
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:

    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vst.128.ip q2, a0, 16
    j dl_tie718_s8_unaligned_mul2d_11c_remainder

dl_tie718_s8_unaligned_mul2d_11c_1:

    add t0, a4, x0
    blez t0, 5f
    4:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a1, 16
        esp32p4_s8_64b_unaligned_vector_store q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp32p4_s8_64b_unaligned_vector_store q2, a0

    j dl_tie718_s8_unaligned_mul2d_11c_remainder

dl_tie718_s8_unaligned_mul2d_11c_small_remainder:
    esp.ld.128.usar.xp q0, a1, a5
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.xp q3, a2, a5
    esp.movx.r.sar.bytes s0

dl_tie718_s8_unaligned_mul2d_11c_remainder:


    beqz a5, dl_esp32p4_s8_unaligned_mul2d_11c_end

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.zero.qacc
    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1

    # esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    dl_esp32p4_s8_store_remainder q2, t4, t6, s0, s1, t0, a0, a5

dl_esp32p4_s8_unaligned_mul2d_11c_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret






    .align 2
    .text
    .global dl_esp32p4_s8_unaligned_mul2d_11c_relu
    .type   dl_esp32p4_s8_unaligned_mul2d_11c_relu, @function
    #.section .iram1
dl_esp32p4_s8_unaligned_mul2d_11c_relu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: c_remainder
    # t3: mul_shift
    # s8: activation_alpha
    # s9: activation_shift


    lw a4, 64(a3)
    lw a5, 76(a3)
    lw t3, 100(a3)
    lw s8, 52(a3)
    lw s9, 60(a3)



    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_tie718_s8_unaligned_mul2d_11c_relu_small_remainder # channel < 16


    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_tie718_s8_unaligned_mul2d_11c_relu_0
    li t0, 8
    beq s1, t0, dl_tie718_s8_unaligned_mul2d_11c_relu_1


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s8 q2, s8, s9
        esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vrelu.s8 q2, s8, s9
    esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    j dl_tie718_s8_unaligned_mul2d_11c_relu_remainder

dl_tie718_s8_unaligned_mul2d_11c_relu_0:

    add t0, a4, x0
    blez t0, 3f
    2:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s8 q2, s8, s9
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vrelu.s8 q2, s8, s9
    esp.vst.128.ip q2, a0, 16
    j dl_tie718_s8_unaligned_mul2d_11c_relu_remainder

dl_tie718_s8_unaligned_mul2d_11c_relu_1:

    add t0, a4, x0
    blez t0, 5f
    4:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s8 q2, s8, s9
        esp32p4_s8_64b_unaligned_vector_store q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:
    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vrelu.s8 q2, s8, s9
    esp32p4_s8_64b_unaligned_vector_store q2, a0
    j dl_tie718_s8_unaligned_mul2d_11c_relu_remainder

dl_tie718_s8_unaligned_mul2d_11c_relu_small_remainder:
    esp.ld.128.usar.xp q0, a1, a5
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.xp q3, a2, a5
    esp.movx.r.sar.bytes s0

dl_tie718_s8_unaligned_mul2d_11c_relu_remainder:


    beqz a5, dl_esp32p4_s8_unaligned_mul2d_11c_relu_end

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.zero.qacc
    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vrelu.s8 q2, s8, s9
    # esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    dl_esp32p4_s8_store_remainder q2, t4, t6, s0, s1, t0, a0, a5

dl_esp32p4_s8_unaligned_mul2d_11c_relu_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret




    .align 2
    .text
    .global dl_esp32p4_s8_unaligned_mul2d_11c_prelu
    .type   dl_esp32p4_s8_unaligned_mul2d_11c_prelu, @function
    #.section .iram1
dl_esp32p4_s8_unaligned_mul2d_11c_prelu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: c_remainder
    # t3: mul_shift
    # s8: activation_alpha_ptr
    # s9: activation_shift


    lw a4, 64(a3)
    lw a5, 76(a3)
    lw t3, 100(a3)
    lw s8, 56(a3)
    lw s9, 60(a3)



    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_tie718_s8_unaligned_mul2d_11c_prelu_small_remainder # channel < 16


    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_tie718_s8_unaligned_mul2d_11c_prelu_0
    li t0, 8
    beq s1, t0, dl_tie718_s8_unaligned_mul2d_11c_prelu_1


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1

        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s8 q2, q2, q6, s9
        esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.vld.128.ip q6, s8, 16
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vprelu.s8 q2, q2, q6, s9
    esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    j dl_tie718_s8_unaligned_mul2d_11c_prelu_remainder

dl_tie718_s8_unaligned_mul2d_11c_prelu_0:

    add t0, a4, x0
    blez t0, 3f
    2:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s8 q2, q2, q6, s9
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.vld.128.ip q6, s8, 16
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vprelu.s8 q2, q2, q6, s9
    esp.vst.128.ip q2, a0, 16
    j dl_tie718_s8_unaligned_mul2d_11c_prelu_remainder

dl_tie718_s8_unaligned_mul2d_11c_prelu_1:

    add t0, a4, x0
    blez t0, 5f
    4:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s8 q2, q2, q6, s9
        esp32p4_s8_64b_unaligned_vector_store q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:
    addi a1, a1, -16
    add a1, a1, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, a5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.vmulas.s8.qacc q2, q5
    esp.vld.128.ip q6, s8, 16
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vprelu.s8 q2, q2, q6, s9
    esp32p4_s8_64b_unaligned_vector_store q2, a0
    j dl_tie718_s8_unaligned_mul2d_11c_prelu_remainder


dl_tie718_s8_unaligned_mul2d_11c_prelu_small_remainder:
    esp.ld.128.usar.xp q0, a1, a5
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.xp q3, a2, a5
    esp.movx.r.sar.bytes s0

dl_tie718_s8_unaligned_mul2d_11c_prelu_remainder:

    beqz a5, dl_esp32p4_s8_unaligned_mul2d_11c_prelu_end

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.zero.qacc
    esp.vmulas.s8.qacc q2, q5
    esp.vld.128.ip q6, s8, 16
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vprelu.s8 q2, q2, q6, s9
    # esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    dl_esp32p4_s8_store_remainder q2, t4, t6, s0, s1, t0, a0, a5

dl_esp32p4_s8_unaligned_mul2d_11c_prelu_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
