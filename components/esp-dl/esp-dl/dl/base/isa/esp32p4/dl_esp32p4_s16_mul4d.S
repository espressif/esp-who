#include "dl_esp32p4_s16.S"
#include "dl_esp32p4_common.S"


#void dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul
    .type   dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul, @function
    #.section .iram1
dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul:
    .align 2

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: length

    lw t3, 80(a3)
    esp.movx.w.sar t3
    lw a4, 44(a3)
    srai a3, a4, 3

    li t0, 0
loop:
    beq t0, a3, end
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vmul.s16 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop
end:
    ret


#void dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul
    .type   dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul, @function
    #.section .iram1
dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul:
    .align 2

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: length

    lw t3, 80(a3)
    esp.movx.w.sar t3
    lw a4, 44(a3)
    srai a3, a4, 3

    li t0, 0
loop_:
    beq t0, a3, end_
    esp.vld.128.ip q0, a1, 16
    esp.vldbc.16.ip q1, a2, 0
    esp.vmul.s16 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop_
end_:
    ret

#void dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul(int16_t *output_ptr, int16_t *input0_ptr, int16_t *input1_ptr, int lenght);

    .align 2
    .text
    .global dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul
    .type   dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul, @function
    #.section .iram1
dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul:
    .align 2

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: length

    lw t3, 80(a3)
    esp.movx.w.sar t3
    lw a4, 44(a3)
    srai a3, a4, 3

    li t0, 0
loop__:
    beq t0, a3, end__
    esp.vldbc.16.ip q0, a1, 0
    esp.vld.128.ip q1, a2, 16
    esp.vmul.s16 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
    addi t0, t0, 1
    j loop__
end__:
    ret



    .align 4
    .text
    .global dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul_unaligned
    .type   dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul_unaligned, @function
    #.section .iram1
dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul_unaligned:
    .align 4
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

    ESP.LD.128.USAR.IP q5, a0, 0 #get output_ptr sar_byte
    ESP.MOVX.R.SAR.BYTES s1

    bltz a4, dl_tie718_S16_unaligned_mul2d_11c_small_remainder # channel < 8

    ESP.LD.128.USAR.IP q0, a1, 16
    ESP.LD.128.USAR.IP q3, a2, 16
    ESP.LD.128.USAR.IP q1, a1, 16

    beqz s1, dl_tie718_S16_unaligned_mul2d_11c_0
    li t0, 8
    beq s1, t0, dl_tie718_S16_unaligned_mul2d_11c_1

    add t0, a4, x0
    blez t0, 1f
    0:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        ESP.LD.128.USAR.IP q4, a2, 16
        ESP.SRC.Q.QUP q5, q3, q4

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a1, 16
        dl_esp32p4_128b_unaligned_store0 q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    addi a1, a1, -16
    add a1, a1, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    ESP.LD.128.USAR.XP q4, a2, a5
    ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    ESP.SRC.Q.QUP q5, q3, q4

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_tie718_S16_unaligned_mul2d_11c_remainder

dl_tie718_S16_unaligned_mul2d_11c_0:

    add t0, a4, x0
    blez t0, 3f
    2:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        ESP.LD.128.USAR.IP q4, a2, 16
        ESP.SRC.Q.QUP q5, q3, q4

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a1, 16
        ESP.VST.128.IP q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:

    addi a1, a1, -16
    add a1, a1, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    ESP.LD.128.USAR.XP q4, a2, a5
    ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    ESP.SRC.Q.QUP q5, q3, q4

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VST.128.IP q2, a0, 16
    j dl_tie718_S16_unaligned_mul2d_11c_remainder

dl_tie718_S16_unaligned_mul2d_11c_1:

    add t0, a4, x0
    blez t0, 5f
    4:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        ESP.LD.128.USAR.IP q4, a2, 16
        ESP.SRC.Q.QUP q5, q3, q4

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a1, 16
        dl_esp32p4_128b_unaligned_store1 q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    addi a1, a1, -16
    add a1, a1, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    ESP.LD.128.USAR.XP q4, a2, a5
    ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    ESP.SRC.Q.QUP q5, q3, q4

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    dl_esp32p4_128b_unaligned_store1 q2, a0

    j dl_tie718_S16_unaligned_mul2d_11c_remainder

dl_tie718_S16_unaligned_mul2d_11c_small_remainder:
    ESP.LD.128.USAR.XP q0, a1, a5
    ESP.MOVX.R.SAR.BYTES t6

    ESP.LD.128.USAR.XP q3, a2, a5
    ESP.MOVX.R.SAR.BYTES s0

dl_tie718_S16_unaligned_mul2d_11c_remainder:


    beqz a5, dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul_unaligned_end

    ESP.LD.128.USAR.IP q1, a1, 0
    ESP.MOVX.W.SAR.BYTES t6
    ESP.SRC.Q q2, q0, q1

    ESP.LD.128.USAR.IP q4, a2, 0
    ESP.MOVX.W.SAR.BYTES s0
    ESP.SRC.Q q5, q3, q4

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1

    srli a5, a5, 1
    dl_esp32p4_s16_store_remainder q2, a5, s0, a0

dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul_unaligned_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


    .align 4
    .text
    .global dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned
    .type   dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned, @function
    #.section .iram1
dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned:
    .align 4
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

    ESP.LD.128.USAR.IP q5, a0, 0 #get output_ptr sar_byte
    ESP.MOVX.R.SAR.BYTES s1

    bltz a4, dl_tie718_S16_unaligned_mul2d_11c_small_remainder_ # channel < 8

    ESP.LD.128.USAR.IP q0, a1, 16
    #ESP.LD.128.USAR.IP q3, a2, 16
    ESP.LD.128.USAR.IP q1, a1, 16

    beqz s1, dl_tie718_S16_unaligned_mul2d_11c_0_
    li t0, 8
    beq s1, t0, dl_tie718_S16_unaligned_mul2d_11c_1_

    add t0, a4, x0
    blez t0, 1f
    0:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        #ESP.LD.128.USAR.IP q4, a2, 16
        #ESP.SRC.Q.QUP q5, q3, q4
        esp.vldbc.16.ip q5, a2, 0

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a1, 16
        dl_esp32p4_128b_unaligned_store0 q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    addi a1, a1, -16
    add a1, a1, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    #ESP.LD.128.USAR.XP q4, a2, a5
    #ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    #ESP.SRC.Q.QUP q5, q3, q4
    esp.vldbc.16.ip q5, a2, 0
    addi s0, a2, 0

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_tie718_S16_unaligned_mul2d_11c_remainder_

dl_tie718_S16_unaligned_mul2d_11c_0_:

    add t0, a4, x0
    blez t0, 3f
    2:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        #ESP.LD.128.USAR.IP q4, a2, 16
        #ESP.SRC.Q.QUP q5, q3, q4
        esp.vldbc.16.ip q5, a2, 0

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a1, 16
        ESP.VST.128.IP q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:

    addi a1, a1, -16
    add a1, a1, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    #ESP.LD.128.USAR.XP q4, a2, a5
    #ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    #ESP.SRC.Q.QUP q5, q3, q4
    esp.vldbc.16.ip q5, a2, 0
    addi s0, a2, 0

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VST.128.IP q2, a0, 16
    j dl_tie718_S16_unaligned_mul2d_11c_remainder_

dl_tie718_S16_unaligned_mul2d_11c_1_:

    add t0, a4, x0
    blez t0, 5f
    4:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        #ESP.LD.128.USAR.IP q4, a2, 16
        #ESP.SRC.Q.QUP q5, q3, q4
        esp.vldbc.16.ip q5, a2, 0

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a1, 16
        dl_esp32p4_128b_unaligned_store1 q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    addi a1, a1, -16
    add a1, a1, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    #ESP.LD.128.USAR.XP q4, a2, a5
    #ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    #ESP.SRC.Q.QUP q5, q3, q4
    esp.vldbc.16.ip q5, a2, 0
    addi s0, a2, 0

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    dl_esp32p4_128b_unaligned_store1 q2, a0

    j dl_tie718_S16_unaligned_mul2d_11c_remainder_

dl_tie718_S16_unaligned_mul2d_11c_small_remainder_:
    ESP.LD.128.USAR.XP q0, a1, a5
    ESP.MOVX.R.SAR.BYTES t6

    #ESP.LD.128.USAR.XP q3, a2, a5
    #ESP.MOVX.R.SAR.BYTES s0
    esp.vldbc.16.ip q5, a2, 0


dl_tie718_S16_unaligned_mul2d_11c_remainder_:


    beqz a5, dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned_end

    ESP.LD.128.USAR.IP q1, a1, 0
    ESP.MOVX.W.SAR.BYTES t6
    ESP.SRC.Q q2, q0, q1

    #ESP.LD.128.USAR.IP q4, a2, 0
    #ESP.MOVX.W.SAR.BYTES s0
    #ESP.SRC.Q q5, q3, q4
    esp.vldbc.16.ip q5, a2, 0
    addi s0, a2, 0

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1

    srli a5, a5, 1
    dl_esp32p4_s16_store_remainder q2, a5, s0, a0

dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 4
    .text
    .global dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul_unaligned
    .type   dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul_unaligned, @function
    #.section .iram1
dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul_unaligned:
    .align 4
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a2: int8_t *input0_ptr
    # a1: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: c_remainder
    # t3: mul_shift


    lw a4, 64(a3)
    lw a5, 76(a3)
    lw t3, 80(a3)

    ESP.LD.128.USAR.IP q5, a0, 0 #get output_ptr sar_byte
    ESP.MOVX.R.SAR.BYTES s1

    bltz a4, dl_tie718_S16_unaligned_mul2d_11c_small_remainder_test # channel < 8

    ESP.LD.128.USAR.IP q0, a2, 16
    #ESP.LD.128.USAR.IP q3, a1, 16
    ESP.LD.128.USAR.IP q1, a2, 16

    beqz s1, dl_tie718_S16_unaligned_mul2d_11c_0_test
    li t0, 8
    beq s1, t0, dl_tie718_S16_unaligned_mul2d_11c_1_test

    add t0, a4, x0
    blez t0, 1f
    0:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        #ESP.LD.128.USAR.IP q4, a1, 16
        #ESP.SRC.Q.QUP q5, q3, q4
        esp.vldbc.16.ip q5, a1, 0

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a2, 16
        dl_esp32p4_128b_unaligned_store0 q2, a0, s1
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    addi a2, a2, -16
    add a2, a2, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    #ESP.LD.128.USAR.XP q4, a1, a5
    #ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    #ESP.SRC.Q.QUP q5, q3, q4
    esp.vldbc.16.ip q5, a1, 0
    addi s0, a1, 0

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_tie718_S16_unaligned_mul2d_11c_remainder_test

dl_tie718_S16_unaligned_mul2d_11c_0_test:

    add t0, a4, x0
    blez t0, 3f
    2:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        #ESP.LD.128.USAR.IP q4, a1, 16
        #ESP.SRC.Q.QUP q5, q3, q4
        esp.vldbc.16.ip q5, a1, 0

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a2, 16
        ESP.VST.128.IP q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:

    addi a2, a2, -16
    add a2, a2, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    #ESP.LD.128.USAR.XP q4, a1, a5
    #ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    #ESP.SRC.Q.QUP q5, q3, q4
    esp.vldbc.16.ip q5, a1, 0
    addi s0, a1, 0

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VST.128.IP q2, a0, 16
    j dl_tie718_S16_unaligned_mul2d_11c_remainder_test

dl_tie718_S16_unaligned_mul2d_11c_1_test:

    add t0, a4, x0
    blez t0, 5f
    4:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        #ESP.LD.128.USAR.IP q4, a1, 16
        #ESP.SRC.Q.QUP q5, q3, q4
        esp.vldbc.16.ip q5, a1, 0

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.LD.128.USAR.IP q1, a2, 16
        dl_esp32p4_128b_unaligned_store1 q2, a0
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    addi a2, a2, -16
    add a2, a2, a5
    ESP.ZERO.QACC
    ESP.MOVX.R.SAR.BYTES t6 #input0 sar
    ESP.SRC.Q.QUP q2, q0, q1

    #ESP.LD.128.USAR.XP q4, a1, a5
    #ESP.MOVX.R.SAR.BYTES s0 #input1 sar
    #ESP.SRC.Q.QUP q5, q3, q4
    esp.vldbc.16.ip q5, a1, 0
    addi s0, a1, 0

    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    dl_esp32p4_128b_unaligned_store1 q2, a0

    j dl_tie718_S16_unaligned_mul2d_11c_remainder_test

dl_tie718_S16_unaligned_mul2d_11c_small_remainder_test:
    ESP.LD.128.USAR.XP q0, a2, a5
    ESP.MOVX.R.SAR.BYTES t6

    #ESP.LD.128.USAR.XP q3, a1, a5
    #ESP.MOVX.R.SAR.BYTES s0
    esp.vldbc.16.ip q5, a1, 0


dl_tie718_S16_unaligned_mul2d_11c_remainder_test:


    beqz a5, dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul_unaligned_end

    ESP.LD.128.USAR.IP q1, a2, 0
    ESP.MOVX.W.SAR.BYTES t6
    ESP.SRC.Q q2, q0, q1

    #ESP.LD.128.USAR.IP q4, a1, 0
    #ESP.MOVX.W.SAR.BYTES s0
    #ESP.SRC.Q q5, q3, q4
    esp.vldbc.16.ip q5, a1, 0
    addi s0, a1, 0

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1

    srli a5, a5, 1
    dl_esp32p4_s16_store_remainder q2, a5, s0, a0

dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul_unaligned_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
