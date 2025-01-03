#include "dl_esp32p4_s8.S"
#include "dl_esp32p4_common.S"


#void dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul
    .type   dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul, @function
    #.section .iram1
dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul:
    .align 2

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: length

    #esp.movx.w.sar a4
    #srai a3, a3, 4
    lw t3, 80(a3)
    esp.movx.w.sar t3
    lw a4, 44(a3)
    srai a3, a4, 4

    li t0, 0
loop:
    beq t0, a3, end
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vmul.s8 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop
end:
    ret


#void dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul
    .type   dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul, @function
    #.section .iram1
dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul:
    .align 2

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: length

    lw t3, 80(a3)
    esp.movx.w.sar t3
    lw a4, 44(a3)
    srai a3, a4, 4

    li t0, 0
loop_:
    beq t0, a3, end_
    esp.vld.128.ip q0, a1, 16
    esp.vldbc.8.ip q1, a2, 0
    esp.vmul.s8 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop_
end_:
    ret

#void dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul(int8_t *output_ptr, int8_t *input0_ptr, int8_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul
    .type   dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul, @function
    #.section .iram1
dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul:
    .align 2

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: length

    lw t3, 80(a3)
    esp.movx.w.sar t3
    lw a4, 44(a3)
    srai a3, a4, 4

    li t0, 0
loop__:
    beq t0, a3, end__
    esp.vldbc.8.ip q0, a1, 0
    esp.vld.128.ip q1, a2, 16
    esp.vmul.s8 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop__
end__:
    ret









    .align 2
    .text
    .global dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned
    .type   dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned, @function
    #.section .iram1
dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr  broadcast
    # a3: void *args
    # a4: c_div_x_1
    # a5: c_remainder
    # t3: mul_shift


    lw a4, 64(a3)
    lw a5, 76(a3)
    lw t3, 80(a3)



    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_tie718_s8_unaligned_mul2d_11c_small_remainder__ # channel < 16


    esp.ld.128.usar.ip q0, a1, 16
    #esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_tie718_s8_unaligned_mul2d_11c_0__
    li t0, 8
    beq s1, t0, dl_tie718_s8_unaligned_mul2d_11c_1__


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a2, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a2, 0

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

    #esp.ld.128.usar.xp q4, a2, a5
    #esp.movx.r.sar.bytes s0 #input1 sar
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    j dl_tie718_s8_unaligned_mul2d_11c_remainder__

dl_tie718_s8_unaligned_mul2d_11c_0__:

    add t0, a4, x0
    blez t0, 3f
    2:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a2, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a2, 0

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

    #esp.ld.128.usar.xp q4, a2, a5
    #esp.movx.r.sar.bytes s0 #input1 sar
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vst.128.ip q2, a0, 16
    j dl_tie718_s8_unaligned_mul2d_11c_remainder__

dl_tie718_s8_unaligned_mul2d_11c_1__:

    add t0, a4, x0
    blez t0, 5f
    4:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a2, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a2, 0

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

    #esp.ld.128.usar.xp q4, a2, a5
    #esp.movx.r.sar.bytes s0 #input1 sar
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp32p4_s8_64b_unaligned_vector_store q2, a0

    j dl_tie718_s8_unaligned_mul2d_11c_remainder__

dl_tie718_s8_unaligned_mul2d_11c_small_remainder__:
    esp.ld.128.usar.xp q0, a1, a5
    esp.movx.r.sar.bytes t6

    #esp.ld.128.usar.xp q3, a2, a5
    #esp.movx.r.sar.bytes s0
    esp.vldbc.8.ip q5, a2, 0


dl_tie718_s8_unaligned_mul2d_11c_remainder__:


    beqz a5, dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned_end__

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    #esp.ld.128.usar.ip q4, a2, 0
    #esp.movx.w.sar.bytes s0
    #esp.src.q q5, q3, q4
    esp.vldbc.8.ip q5, a2, 0
    addi s0, a2, 0

    esp.zero.qacc
    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1

    # esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    dl_esp32p4_s8_store_remainder q2, t4, t6, s0, s1, t0, a0, a5

dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned_end__:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret






    .align 2
    .text
    .global dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned
    .type   dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned, @function
    #.section .iram1
dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned:
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
    lw t3, 80(a3)



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


    beqz a5, dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned_end

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

dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


    .align 2
    .text
    .global dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul_unaligned
    .type   dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul_unaligned, @function
    #.section .iram1
dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul_unaligned:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a2: int8_t *input0_ptr
    # a1: int8_t *input1_ptr  broadcast
    # a3: void *args
    # a4: c_div_x_1
    # a5: c_remainder
    # t3: mul_shift


    lw a4, 64(a3)
    lw a5, 76(a3)
    lw t3, 80(a3)



    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_tie718_s8_unaligned_mul2d_11c_small_remainder__test # channel < 16


    esp.ld.128.usar.ip q0, a2, 16
    #esp.ld.128.usar.ip q3, a1, 16
    esp.ld.128.usar.ip q1, a2, 16

    beqz s1, dl_tie718_s8_unaligned_mul2d_11c_0__test
    li t0, 8
    beq s1, t0, dl_tie718_s8_unaligned_mul2d_11c_1__test


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a1, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a1, 0

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a2, 16
        esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    addi a2, a2, -16
    add a2, a2, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    #esp.ld.128.usar.xp q4, a1, a5
    #esp.movx.r.sar.bytes s0 #input1 sar
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a1, 0
    addi s0, a1, 0

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    j dl_tie718_s8_unaligned_mul2d_11c_remainder__test

dl_tie718_s8_unaligned_mul2d_11c_0__test:

    add t0, a4, x0
    blez t0, 3f
    2:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a1, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a1, 0

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a2, 16
        esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:

    addi a2, a2, -16
    add a2, a2, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    #esp.ld.128.usar.xp q4, a1, a5
    #esp.movx.r.sar.bytes s0 #input1 sar
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a1, 0
    addi s0, a1, 0

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp.vst.128.ip q2, a0, 16
    j dl_tie718_s8_unaligned_mul2d_11c_remainder__test

dl_tie718_s8_unaligned_mul2d_11c_1__test:

    add t0, a4, x0
    blez t0, 5f
    4:
        esp.zero.qacc
        esp.src.q.qup q2, q0, q1

        #esp.ld.128.usar.ip q4, a1, 16
        #esp.src.q.qup q5, q3, q4
        esp.vldbc.8.ip q5, a1, 0

        esp.vmulas.s8.qacc q2, q5
        esp.srcmb.s8.qacc q2, t3, 1
        esp.ld.128.usar.ip q1, a2, 16
        esp32p4_s8_64b_unaligned_vector_store q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    addi a2, a2, -16
    add a2, a2, a5
    esp.zero.qacc
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    #esp.ld.128.usar.xp q4, a1, a5
    #esp.movx.r.sar.bytes s0 #input1 sar
    #esp.src.q.qup q5, q3, q4
    esp.vldbc.8.ip q5, a1, 0
    addi s0, a1, 0

    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1
    esp32p4_s8_64b_unaligned_vector_store q2, a0

    j dl_tie718_s8_unaligned_mul2d_11c_remainder__test

dl_tie718_s8_unaligned_mul2d_11c_small_remainder__test:
    esp.ld.128.usar.xp q0, a2, a5
    esp.movx.r.sar.bytes t6

    #esp.ld.128.usar.xp q3, a1, a5
    #esp.movx.r.sar.bytes s0
    esp.vldbc.8.ip q5, a1, 0


dl_tie718_s8_unaligned_mul2d_11c_remainder__test:


    beqz a5, dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul_unaligned_end__

    esp.ld.128.usar.ip q1, a2, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    #esp.ld.128.usar.ip q4, a1, 0
    #esp.movx.w.sar.bytes s0
    #esp.src.q q5, q3, q4
    esp.vldbc.8.ip q5, a1, 0
    addi s0, a1, 0

    esp.zero.qacc
    esp.vmulas.s8.qacc q2, q5
    esp.srcmb.s8.qacc q2, t3, 1

    # esp32p4_s8_32b_unaligned_vector_store q2, a0, s1
    dl_esp32p4_s8_store_remainder q2, t4, t6, s0, s1, t0, a0, a5

dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul_unaligned_end__:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
