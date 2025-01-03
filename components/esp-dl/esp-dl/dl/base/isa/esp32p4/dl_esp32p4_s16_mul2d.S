#include "dl_esp32p4_s16.S"
#include "dl_esp32p4_common.S"

############################################################################################################################################################
####
#### esp32p4_s16_mul2d_11c series
####
############################################################################################################################################################

    .align 4
    .text
    .global dl_esp32p4_s16_mul2d_11c
    .type   dl_esp32p4_s16_mul2d_11c, @function
    #.section .iram1
dl_esp32p4_s16_mul2d_11c:
    .align 4
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input0_ptr
    # a2: int8_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: mul_shift


    lw a4, 64(a3)
    lw a5, 100(a3)
    bltz a4, 5f

    ESP.VLD.128.IP q0, a1, 16
    ESP.VLD.128.IP q1, a2, 16
    add t0, a4, x0
    blez t0, 1f
    0:
        ESP.ZERO.QACC
        ESP.VMULAS.S16.QACC.LD.IP q0, a1, 16, q0, q1
        ESP.VLD.128.IP q1, a2, 16
        ESP.SRCMB.S16.QACC q2, a5, 1
        ESP.VST.128.IP q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q0, q1
    ESP.SRCMB.S16.QACC q2, a5, 1
    ESP.VST.128.IP q2, a0, 16
5:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 4
    .text
    .global dl_esp32p4_s16_mul2d_11c_relu
    .type   dl_esp32p4_s16_mul2d_11c_relu, @function
    #.section .iram1
dl_esp32p4_s16_mul2d_11c_relu:
    .align 4
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
    bltz a4, 5f

    ESP.VLD.128.IP q0, a1, 16
    ESP.VLD.128.IP q1, a2, 16
    add t0, a4, x0
    blez t0, 1f
    0:
        ESP.ZERO.QACC
        ESP.VMULAS.S16.QACC.LD.IP q0, a1, 16, q0, q1
        ESP.VLD.128.IP q1, a2, 16
        ESP.SRCMB.S16.QACC q2, a5, 1
        ESP.VRELU.S16 q2, s8, s9
        ESP.VST.128.IP q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q0, q1
    ESP.SRCMB.S16.QACC q2, a5, 1
    ESP.VRELU.S16 q2, s8, s9
    ESP.VST.128.IP q2, a0, 16
5:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


    .align 4
    .text
    .global dl_esp32p4_s16_mul2d_11c_prelu
    .type   dl_esp32p4_s16_mul2d_11c_prelu, @function
    #.section .iram1
dl_esp32p4_s16_mul2d_11c_prelu:
    .align 4
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
    bltz a4, 5f

    ESP.VLD.128.IP q0, a1, 16
    ESP.VLD.128.IP q1, a2, 16
    add t0, a4, x0
    blez t0, 1f
    0:
        ESP.ZERO.QACC
        ESP.VMULAS.S16.QACC.LD.IP q0, a1, 16, q0, q1
        ESP.VLD.128.IP q1, a2, 16

        ESP.VLD.128.IP q3, s8, 16
        ESP.SRCMB.S16.QACC q2, a5, 1
        ESP.VPRELU.S16 q2, q2, q3, s9
        ESP.VST.128.IP q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q0, q1
    ESP.VLD.128.IP q3, s8, 16
    ESP.SRCMB.S16.QACC q2, a5, 1
    ESP.VPRELU.S16 q2, q2, q3, s9
    ESP.VST.128.IP q2, a0, 16
5:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


############################################################################################################################################################
####
#### esp32p4_S16_unaligned_mul2d_11c series
####
############################################################################################################################################################

    .align 4
    .text
    .global dl_esp32p4_s16_unaligned_mul2d_11c
    .type   dl_esp32p4_s16_unaligned_mul2d_11c, @function
    #.section .iram1
dl_esp32p4_s16_unaligned_mul2d_11c:
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
    lw t3, 100(a3)

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


    beqz a5, dl_esp32p4_S16_unaligned_mul2d_11c_end

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

dl_esp32p4_S16_unaligned_mul2d_11c_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret




    .align 4
    .text
    .global dl_esp32p4_s16_unaligned_mul2d_11c_relu
    .type   dl_esp32p4_s16_unaligned_mul2d_11c_relu, @function
    #.section .iram1
dl_esp32p4_s16_unaligned_mul2d_11c_relu:
    .align 4
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



    ESP.LD.128.USAR.IP q5, a0, 0 #get output_ptr sar_byte
    ESP.MOVX.R.SAR.BYTES s1

    bltz a4, dl_tie718_S16_unaligned_mul2d_11c_relu_small_remainder # channel < 8


    ESP.LD.128.USAR.IP q0, a1, 16
    ESP.LD.128.USAR.IP q3, a2, 16
    ESP.LD.128.USAR.IP q1, a1, 16

    beqz s1, dl_tie718_S16_unaligned_mul2d_11c_relu_0
    li t0, 8
    beq s1, t0, dl_tie718_S16_unaligned_mul2d_11c_relu_1


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
        ESP.VRELU.S16 q2, s8, s9
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
    ESP.VRELU.S16 q2, s8, s9
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_tie718_S16_unaligned_mul2d_11c_relu_remainder

dl_tie718_S16_unaligned_mul2d_11c_relu_0:

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
        ESP.VRELU.S16 q2, s8, s9
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
    ESP.VRELU.S16 q2, s8, s9
    ESP.VST.128.IP q2, a0, 16
    j dl_tie718_S16_unaligned_mul2d_11c_relu_remainder

dl_tie718_S16_unaligned_mul2d_11c_relu_1:

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
        ESP.VRELU.S16 q2, s8, s9
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
    ESP.VRELU.S16 q2, s8, s9
    dl_esp32p4_128b_unaligned_store1 q2, a0
    j dl_tie718_S16_unaligned_mul2d_11c_relu_remainder

dl_tie718_S16_unaligned_mul2d_11c_relu_small_remainder:
    ESP.LD.128.USAR.XP q0, a1, a5
    ESP.MOVX.R.SAR.BYTES t6

    ESP.LD.128.USAR.XP q3, a2, a5
    ESP.MOVX.R.SAR.BYTES s0

dl_tie718_S16_unaligned_mul2d_11c_relu_remainder:


    beqz a5, dl_esp32p4_S16_unaligned_mul2d_11c_relu_end

    ESP.LD.128.USAR.IP q1, a1, 0
    ESP.MOVX.W.SAR.BYTES t6
    ESP.SRC.Q q2, q0, q1

    ESP.LD.128.USAR.IP q4, a2, 0
    ESP.MOVX.W.SAR.BYTES s0
    ESP.SRC.Q q5, q3, q4

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q2, q5
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VRELU.S16 q2, s8, s9
    srli a5, a5, 1
    dl_esp32p4_s16_store_remainder q2, a5, s0, a0

dl_esp32p4_S16_unaligned_mul2d_11c_relu_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 4
    .text
    .global dl_esp32p4_s16_unaligned_mul2d_11c_prelu
    .type   dl_esp32p4_s16_unaligned_mul2d_11c_prelu, @function
    #.section .iram1
dl_esp32p4_s16_unaligned_mul2d_11c_prelu:
    .align 4
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



    ESP.LD.128.USAR.IP q5, a0, 0 #get output_ptr sar_byte
    ESP.MOVX.R.SAR.BYTES s1

    bltz a4, dl_tie718_S16_unaligned_mul2d_11c_prelu_small_remainder # channel < 8


    ESP.LD.128.USAR.IP q0, a1, 16
    ESP.LD.128.USAR.IP q3, a2, 16
    ESP.LD.128.USAR.IP q1, a1, 16

    beqz s1, dl_tie718_S16_unaligned_mul2d_11c_prelu_0
    li t0, 8
    beq s1, t0, dl_tie718_S16_unaligned_mul2d_11c_prelu_1


    add t0, a4, x0
    blez t0, 1f
    0:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        ESP.LD.128.USAR.IP q4, a2, 16
        ESP.SRC.Q.QUP q5, q3, q4

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1

        ESP.VLD.128.IP q6, s8, 16
        ESP.LD.128.USAR.IP q1, a1, 16
        ESP.VPRELU.S16 q2, q2, q6, s9
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
    ESP.VLD.128.IP q6, s8, 16
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VPRELU.S16 q2, q2, q6, s9
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_tie718_S16_unaligned_mul2d_11c_prelu_remainder

dl_tie718_S16_unaligned_mul2d_11c_prelu_0:

    add t0, a4, x0
    blez t0, 3f
    2:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        ESP.LD.128.USAR.IP q4, a2, 16
        ESP.SRC.Q.QUP q5, q3, q4

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.VLD.128.IP q6, s8, 16
        ESP.LD.128.USAR.IP q1, a1, 16
        ESP.VPRELU.S16 q2, q2, q6, s9
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
    ESP.VLD.128.IP q6, s8, 16
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VPRELU.S16 q2, q2, q6, s9
    ESP.VST.128.IP q2, a0, 16
    j dl_tie718_S16_unaligned_mul2d_11c_prelu_remainder

dl_tie718_S16_unaligned_mul2d_11c_prelu_1:

    add t0, a4, x0
    blez t0, 5f
    4:
        ESP.ZERO.QACC
        ESP.SRC.Q.QUP q2, q0, q1

        ESP.LD.128.USAR.IP q4, a2, 16
        ESP.SRC.Q.QUP q5, q3, q4

        ESP.VMULAS.S16.QACC q2, q5
        ESP.SRCMB.S16.QACC q2, t3, 1
        ESP.VLD.128.IP q6, s8, 16
        ESP.LD.128.USAR.IP q1, a1, 16
        ESP.VPRELU.S16 q2, q2, q6, s9
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
    ESP.VLD.128.IP q6, s8, 16
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VPRELU.S16 q2, q2, q6, s9
    dl_esp32p4_128b_unaligned_store1 q2, a0
    j dl_tie718_S16_unaligned_mul2d_11c_prelu_remainder


dl_tie718_S16_unaligned_mul2d_11c_prelu_small_remainder:
    ESP.LD.128.USAR.XP q0, a1, a5
    ESP.MOVX.R.SAR.BYTES t6

    ESP.LD.128.USAR.XP q3, a2, a5
    ESP.MOVX.R.SAR.BYTES s0

dl_tie718_S16_unaligned_mul2d_11c_prelu_remainder:

    beqz a5, dl_esp32p4_S16_unaligned_mul2d_11c_prelu_end

    ESP.LD.128.USAR.IP q1, a1, 0
    ESP.MOVX.W.SAR.BYTES t6
    ESP.SRC.Q q2, q0, q1

    ESP.LD.128.USAR.IP q4, a2, 0
    ESP.MOVX.W.SAR.BYTES s0
    ESP.SRC.Q q5, q3, q4

    ESP.ZERO.QACC
    ESP.VMULAS.S16.QACC q2, q5
    ESP.VLD.128.IP q6, s8, 16
    ESP.SRCMB.S16.QACC q2, t3, 1
    ESP.VPRELU.S16 q2, q2, q6, s9
    srli a5, a5, 1
    dl_esp32p4_s16_store_remainder q2, a5, s0, a0

dl_esp32p4_S16_unaligned_mul2d_11c_prelu_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
