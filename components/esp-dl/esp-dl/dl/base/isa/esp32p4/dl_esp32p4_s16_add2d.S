#include "dl_esp32p4_s16.S"
#include "dl_esp32p4_common.S"

############################################################################################################################################################
####
#### esp32p4_s16_add2d_11c series
####
############################################################################################################################################################

.macro dl_esp32p4_s16_rescale_add_rescale_output input0, input1, output, output_scale, output_shift
    esp.zero.qacc
    esp.vmulas.s16.qacc \input0, \output_scale
    esp.vmulas.s16.qacc \input1, \output_scale
    esp.srcmb.s16.qacc \output, \output_shift, 1
.endm


    .align 2
    .text
    .global dl_esp32p4_s16_add2d_11c
    .type   dl_esp32p4_s16_add2d_11c, @function
    #.section .iram1
dl_esp32p4_s16_add2d_11c:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_2x_1
    # a5: c_left_x_1

    lw a4, 68(a3)
    lw a5, 72(a3)

    li t0, 1
    blt a4, t0, dl_esp32p4_s16_add2d_small_channel
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16

    add t0, a4, x0
    blez t0, 1f
    0:
    esp.vld.128.ip q2, a1, 16
    esp.vadd.s16.ld.incp q3, a2, q4, q0, q1
    esp.vst.128.ip q4, a0, 16

    esp.vld.128.ip q0, a1, 16
    esp.vadd.s16.ld.incp q1, a2, q5, q2, q3
    esp.vst.128.ip q5, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    esp.vld.128.ip q2, a1, 16
    esp.vadd.s16.ld.incp q3, a2, q4, q0, q1
    esp.vst.128.ip q4, a0, 16
    li t0, 1
    beq a5, t0, 2f #remainder == 2*16byte
    li t0, 2
    beq a5, t0, 3f #remainder == 3*16byte

2:
    esp.vadd.s16 q5, q2, q3
    esp.vst.128.ip q5, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
3:
    esp.vld.128.ip q0, a1, 16
    esp.vadd.s16.ld.incp q1, a2, q5, q2, q3
    esp.vst.128.ip q5, a0, 16

    esp.vadd.s16 q4, q0, q1
    esp.vst.128.ip q4, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret

dl_esp32p4_s16_add2d_small_channel:
    bltz a5, 5f
    add t0, a5, x0
    blez t0, 3f
    2:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vadd.s16 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vadd.s16 q2, q0, q1
    esp.vst.128.ip q2, a0, 16
5:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 2
    .text
    .global dl_esp32p4_s16_rescale_add2d_11c
    .type   dl_esp32p4_s16_rescale_add2d_11c, @function
    #.section .iram1
dl_esp32p4_s16_rescale_add2d_11c:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift

    lw a4, 64(a3)
    lw a5, 88(a3)
    lw t3, 96(a3)
    lw t4, 92(a3)

    li t0, 1
    beq t3, t0, dl_esp32p4_s16_rescale_add2d_output

dl_esp32p4_s16_rescale_add2d_output_scale:
    sh t3, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all output_scale

    # addi a4, a4, 1
    add t0, a4, x0
    blez t0, 3f
    2:
    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q0, q1, q1, q7, t4

    esp.vst.128.ip q1, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16
    esp.srcmb.s16.qacc q1, a5, 1
    dl_esp32p4_s16_rescale_add_rescale_output q0, q1, q1, q7, t4

    esp.vst.128.ip q1, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret

dl_esp32p4_s16_rescale_add2d_output:
    li s1, 1
    sh s1, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all 1

    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16

    add t0, a4, x0
    blez t0, 5f
    4:
    esp.srcmb.s16.qacc q1, a5, 1
    esp.vmulas.s16.qacc.ld.ip q0, a1, 16, q0, q7
    esp.srcmb.s16.qacc q1, t4, 1
    esp.ldqa.s16.128.ip a2, 16
    esp.vst.128.ip q1, a0, 16
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    esp.srcmb.s16.qacc q1, a5, 1
    esp.vmulas.s16.qacc q0, q7
    esp.srcmb.s16.qacc q1, t4, 1

    esp.vst.128.ip q1, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


    .align 2
    .text
    .global dl_esp32p4_s16_add2d_11c_relu
    .type   dl_esp32p4_s16_add2d_11c_relu, @function
    #.section .iram1
dl_esp32p4_s16_add2d_11c_relu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_2x_1
    # a5: c_left_x
    # s8: activation_alpha
    # s9: activation_shift

    lw a4, 68(a3)
    lw a5, 72(a3)
    lw s8, 52(a3)
    lw s9, 60(a3)


    beqz a4, dl_esp32p4_s16_add2d_relu_small_channel
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16

    add t0, a4, x0
    blez t0, 1f
    0:
    esp.vld.128.ip q2, a1, 16
    esp.vadd.s16.ld.incp q3, a2, q4, q0, q1
    esp.vrelu.s16 q4, s8, s9
    esp.vst.128.ip q4, a0, 16

    esp.vld.128.ip q0, a1, 16
    esp.vadd.s16.ld.incp q1, a2, q5, q2, q3
    esp.vrelu.s16 q5, s8, s9
    esp.vst.128.ip q5, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    esp.vld.128.ip q2, a1, 16
    esp.vadd.s16.ld.incp q3, a2, q4, q0, q1
    esp.vrelu.s16 q4, s8, s9
    esp.vst.128.ip q4, a0, 16
    li t0, 1
    beq a5, t0, 2f #remainder == 2*16byte
    li t0, 2
    beq a5, t0, 3f #remainder == 3*16byte
2:
    esp.vadd.s16 q5, q2, q3
    esp.vrelu.s16 q5, s8, s9
    esp.vst.128.ip q5, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
3:
    esp.vld.128.ip q0, a1, 16
    esp.vadd.s16.ld.incp q1, a2, q5, q2, q3
    esp.vrelu.s16 q5, s8, s9
    esp.vst.128.ip q5, a0, 16

    esp.vadd.s16 q4, q0, q1
    esp.vrelu.s16 q4, s8, s9
    esp.vst.128.ip q4, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret

dl_esp32p4_s16_add2d_relu_small_channel:
    bltz a5, 5f
    add t0, a5, x0
    blez t0, 3f
    2:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vadd.s16 q2, q0, q1
    esp.vrelu.s16 q2, s8, s9
    esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vadd.s16 q2, q0, q1
    esp.vrelu.s16 q2, s8, s9
    esp.vst.128.ip q2, a0, 16
5:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 2
    .text
    .global dl_esp32p4_s16_rescale_add2d_11c_relu
    .type   dl_esp32p4_s16_rescale_add2d_11c_relu, @function
    #.section .iram1
dl_esp32p4_s16_rescale_add2d_11c_relu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift
    # s8: activation_alpha
    # s9: activation_shift

    lw a4, 64(a3)
    lw a5, 88(a3)
    lw t3, 96(a3)
    lw t4, 92(a3)
    lw s8, 52(a3)
    lw s9, 60(a3)

    li t0, 1
    beq t3, t0, dl_esp32p4_s16_rescale_add2d_output_relu

dl_esp32p4_s16_rescale_add2d_output_scale_relu:
    sh t3, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all output_scale

    add t0, a4, x0
    blez t0, 3f
    2:
    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q0, q1, q1, q7, t4

    esp.vrelu.s16 q1, s8, s9
    esp.vst.128.ip q1, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q0, q1, q1, q7, t4

    esp.vrelu.s16 q1, s8, s9
    esp.vst.128.ip q1, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret

dl_esp32p4_s16_rescale_add2d_output_relu:
    li s1, 1
    sh s1, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all 1

    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16

    add t0, a4, x0
    blez t0, 5f
    4:
    esp.srcmb.s16.qacc q1, a5, 1
    esp.vmulas.s16.qacc.ld.ip q0, a1, 16, q0, q7
    esp.srcmb.s16.qacc q1, t4, 1
    esp.ldqa.s16.128.ip a2, 16
    esp.vrelu.s16 q1, s8, s9
    esp.vst.128.ip q1, a0, 16
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    esp.srcmb.s16.qacc q1, a5, 1
    esp.vmulas.s16.qacc q0, q7
    esp.srcmb.s16.qacc q1, t4, 1
    esp.vrelu.s16 q1, s8, s9
    esp.vst.128.ip q1, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 2
    .text
    .global dl_esp32p4_s16_add2d_11c_prelu
    .type   dl_esp32p4_s16_add2d_11c_prelu, @function
    #.section .iram1
dl_esp32p4_s16_add2d_11c_prelu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_2x_1
    # a5: c_left_x
    # s8: activation_alpha_ptr
    # s9: activation_shift

    lw a4, 68(a3)
    lw a5, 72(a3)
    lw s8, 56(a3)
    lw s9, 60(a3)

    beqz a4, dl_esp32p4_s16_add2d_prelu_small_channel
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16

    add t0, a4, x0
    blez t0, 1f
    0:
    esp.vld.128.ip q2, a1, 16
    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16.ld.incp q3, a2, q4, q0, q1
    esp.vprelu.s16 q4, q4, q6, s9
    esp.vst.128.ip q4, a0, 16

    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16.ld.incp q1, a2, q5, q2, q3
    esp.vprelu.s16 q5, q5, q6, s9
    esp.vst.128.ip q5, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    esp.vld.128.ip q2, a1, 16
    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16.ld.incp q3, a2, q4, q0, q1
    esp.vprelu.s16 q4, q4, q6, s9
    esp.vst.128.ip q4, a0, 16
    li t0, 1
    beq a5, t0, 2f #remainder == 2*16byte
    li t0, 2
    beq a5, t0, 3f #remainder == 3*16byte

2:
    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q5, q2, q3
    esp.vprelu.s16 q5, q5, q6, s9
    esp.vst.128.ip q5, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
3:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16.ld.incp q1, a2, q5, q2, q3
    esp.vprelu.s16 q5, q5, q6, s9
    esp.vst.128.ip q5, a0, 16

    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q4, q0, q1
    esp.vprelu.s16 q4, q4, q6, s9
    esp.vst.128.ip q4, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret

dl_esp32p4_s16_add2d_prelu_small_channel:
    bltz a5, 5f
    add t0, a5, x0
    blez t0, 3f
    2:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q2, q0, q1
    esp.vprelu.s16 q2, q2, q6, s9
    esp.vst.128.ip q2, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a2, 16
    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q2, q0, q1
    esp.vprelu.s16 q2, q2, q6, s9
    esp.vst.128.ip q2, a0, 16
5:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 2
    .text
    .global dl_esp32p4_s16_rescale_add2d_11c_prelu
    .type   dl_esp32p4_s16_rescale_add2d_11c_prelu, @function
    #.section .iram1
dl_esp32p4_s16_rescale_add2d_11c_prelu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift
    # s8: activation_alpha_ptr
    # s9: activation_shift

    lw a4, 64(a3)
    lw a5, 88(a3)
    lw t3, 96(a3)
    lw t4, 92(a3)
    lw s8, 56(a3)
    lw s9, 60(a3)

    li t0, 1
    beq t3, t0, dl_esp32p4_s16_rescale_add2d_output_prelu

dl_esp32p4_s16_rescale_add2d_output_scale_prelu:
    sh t3, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all output_scale

    # addi a4, a4, 1
    add t0, a4, x0
    blez t0, 3f
    2:
    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q0, q1, q1, q7, t4

    esp.vld.128.ip q5, s8, 16

    esp.vprelu.s16 q1, q1, q5, s9
    esp.vst.128.ip q1, a0, 16
        addi t0, t0, -1
        bgtz t0, 2b
    3:
    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q0, q1, q1, q7, t4

    esp.vld.128.ip q5, s8, 16

    esp.vprelu.s16 q1, q1, q5, s9
    esp.vst.128.ip q1, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret

dl_esp32p4_s16_rescale_add2d_output_prelu:
    li s1, 1
    sh s1, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all 1

    esp.ldqa.s16.128.ip a2, 16
    esp.vld.128.ip q0, a1, 16

    add t0, a4, x0
    blez t0, 5f
    4:
    esp.srcmb.s16.qacc q1, a5, 1
    esp.vmulas.s16.qacc.ld.ip q0, a1, 16, q0, q7
    esp.srcmb.s16.qacc q1, t4, 1

    esp.vld.128.ip q6, s8, 16
    esp.ldqa.s16.128.ip a2, 16
    esp.vprelu.s16 q1, q1, q6, s9
    esp.vst.128.ip q1, a0, 16
        addi t0, t0, -1
        bgtz t0, 4b
    5:

    esp.srcmb.s16.qacc q1, a5, 1
    esp.vmulas.s16.qacc q0, q7

    esp.vld.128.ip q6, s8, 16
    esp.srcmb.s16.qacc q1, t4, 1
    esp.vprelu.s16 q1, q1, q6, s9
    esp.vst.128.ip q1, a0, 16
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


############################################################################################################################################################
####
#### esp32p4_s16_unaligned_add2d_11c series
####
############################################################################################################################################################

    .align 2
    .text
    .global dl_esp32p4_s16_unaligned_add2d_11c
    .type   dl_esp32p4_s16_unaligned_add2d_11c, @function
    #.section .iram1
dl_esp32p4_s16_unaligned_add2d_11c:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift
    # t5: c_remainder

    lw a4, 64(a3)
    lw t5, 76(a3)
    lw a5, 88(a3)

    bgez a5, dl_esp32p4_s16_unaligned_rescale_add2d_11c

# input0 exp = input1 exp = output exp

    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_esp32p4_s16_unaligned_add2d_11c_small_remainder # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_esp32p4_s16_unaligned_add2d_11c_0
    li t0, 8
    beq s1, t0, dl_esp32p4_s16_unaligned_add2d_11c_1

    add t0, a4, x0
    blez t0, 1f
    0:
        esp.src.q.qup q2, q0, q1
        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4
        esp.vadd.s16 q2, q2, q5
        esp.ld.128.usar.ip q1, a1, 16
        dl_esp32p4_128b_unaligned_store0 q2, a0, s1
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

    esp.vadd.s16 q2, q2, q5
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder

    #output sar = 0
dl_esp32p4_s16_unaligned_add2d_11c_0:
    add t0, a4, x0
    blez t0, 3f
    2:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s16 q2, q2, q5

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

    esp.vadd.s16 q2, q2, q5
    esp.vst.128.ip q2, a0, 16
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder

    #output sar = 8
dl_esp32p4_s16_unaligned_add2d_11c_1:
    add t0, a4, x0
    blez t0, 5f
    4:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s16 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        dl_esp32p4_128b_unaligned_store1 q2, a0
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

    esp.vadd.s16 q2, q2, q5
    dl_esp32p4_128b_unaligned_store1 q2, a0
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder

dl_esp32p4_s16_unaligned_add2d_11c_small_remainder:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0

dl_esp32p4_s16_unaligned_add2d_11c_remainder:

    beqz t5, dl_esp32p4_s16_unaligned_add2d_end

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.vadd.s16 q2, q2, q5

    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q2, t5, s1, a0

dl_esp32p4_s16_unaligned_add2d_end:

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


## rescaled add
dl_esp32p4_s16_unaligned_rescale_add2d_11c:
    lw t3, 96(a3) # output_scale
    lw t4, 92(a3) # output_shift

    li t0, 1
    beq t3, t0, dl_esp32p4_s16_rescale_unaligned_add2d_output_shift


### rescaled to output by *scale) >> shift
dl_esp32p4_s16_rescale_unaligned_add2d_output_scale:

    sw t3, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all output_scale

    bltz a4, dl_esp32p4_s16_rescale_unaligned_add2d_scale_small_remainder # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    add t0, a4, x0
    blez t0, 7f
    6:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.mov.s16.qacc q5
        esp.srcmb.s16.qacc q1, a5, 1

        dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4

        esp.ld.128.usar.ip q1, a1, 16
        dl_esp32p4_128b_unaligned_store0 q2, a0, s0
        addi t0, t0, -1
        bgtz t0, 6b
    7:

    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4

    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_esp32p4_s16_rescale_unaligned_add2d_scale_remainder


dl_esp32p4_s16_rescale_unaligned_add2d_scale_small_remainder:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar

dl_esp32p4_s16_rescale_unaligned_add2d_scale_remainder:
    beqz t5, dl_esp32p4_s16_unaligned_rescale_add2d_output_scale_end # c remainder

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4

    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q2, t5, s0, a0

dl_esp32p4_s16_unaligned_rescale_add2d_output_scale_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


### rescaled to output by right shift
dl_esp32p4_s16_rescale_unaligned_add2d_output_shift:
    li s1, 1
    sh s1, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all 1

    bltz a4, dl_esp32p4_s16_rescale_unaligned_add2d_shift_small_remainder # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    add t0, a4, x0
    blez t0, 9f
    8:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4
        esp.mov.s16.qacc q5

        esp.srcmb.s16.qacc q5, a5, 1
        esp.vmulas.s16.qacc q2, q7
        esp.srcmb.s16.qacc q5, t4, 1

        esp.ld.128.usar.ip q1, a1, 16
        dl_esp32p4_128b_unaligned_store0 q5, a0, s1
        addi t0, t0, -1
        bgtz t0, 8b
    9:
    addi a1, a1, -16
    add a1, a1, t5

    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4
    esp.mov.s16.qacc q5

    esp.srcmb.s16.qacc q5, a5, 1
    esp.vmulas.s16.qacc q2, q7
    esp.srcmb.s16.qacc q5, t4, 1

    dl_esp32p4_128b_unaligned_store0 q5, a0, s1
    j dl_esp32p4_s16_rescale_unaligned_add2d_shift_remainder



dl_esp32p4_s16_rescale_unaligned_add2d_shift_small_remainder:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar

dl_esp32p4_s16_rescale_unaligned_add2d_shift_remainder:
    beqz t5, dl_esp32p4_s16_unaligned_rescale_add2d_output_shift_end # c remainder

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q5, a5, 1
    esp.vmulas.s16.qacc q2, q7
    esp.srcmb.s16.qacc q5, t4, 1

    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q5, t5, s1, a0

dl_esp32p4_s16_unaligned_rescale_add2d_output_shift_end:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret



    .align 2
    .text
    .global dl_esp32p4_s16_unaligned_add2d_11c_relu
    .type   dl_esp32p4_s16_unaligned_add2d_11c_relu, @function
    #.section .iram1
dl_esp32p4_s16_unaligned_add2d_11c_relu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift
    # t5: c_remainder
    # s8: activation_alpha
    # s9: activation_shift

    lw a4, 64(a3)
    lw t5, 76(a3)
    lw a5, 88(a3)
    lw s8, 52(a3)
    lw s9, 60(a3)

    bgez a5, dl_esp32p4_s16_unaligned_rescale_add2d_11c_relu

# input0 exp = input1 exp = output exp

    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_esp32p4_s16_unaligned_add2d_11c_small_remainder_relu # channel < 8

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_esp32p4_s16_unaligned_add2d_11c_relu_0
    li t0, 8
    beq s1, t0, dl_esp32p4_s16_unaligned_add2d_11c_relu_1


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.src.q.qup q2, q0, q1
        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4
        esp.vadd.s16 q2, q2, q5
        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s16 q2, s8, s9
        dl_esp32p4_128b_unaligned_store0 q2, a0, s1
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

    esp.vadd.s16 q2, q2, q5
    esp.vrelu.s16 q2, s8, s9
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder_relu

    #output sar = 0
dl_esp32p4_s16_unaligned_add2d_11c_relu_0:
    add t0, a4, x0
    blez t0, 3f
    2:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s16 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s16 q2, s8, s9
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

    esp.vadd.s16 q2, q2, q5
    esp.vrelu.s16 q2, s8, s9
    esp.vst.128.ip q2, a0, 16
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder_relu

    #output sar = 8
dl_esp32p4_s16_unaligned_add2d_11c_relu_1:
    add t0, a4, x0
    blez t0, 5f
    4:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s16 q2, q2, q5

        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s16 q2, s8, s9
        dl_esp32p4_128b_unaligned_store1 q2, a0
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

    esp.vadd.s16 q2, q2, q5
    esp.vrelu.s16 q2, s8, s9
    dl_esp32p4_128b_unaligned_store1 q2, a0
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder_relu

dl_esp32p4_s16_unaligned_add2d_11c_small_remainder_relu:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0

dl_esp32p4_s16_unaligned_add2d_11c_remainder_relu:

    beqz t5, dl_esp32p4_s16_unaligned_add2d_end_relu

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.vadd.s16 q2, q2, q5
    esp.vrelu.s16 q2, s8, s9
    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q2, t5, s1, a0

dl_esp32p4_s16_unaligned_add2d_end_relu:

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


## rescaled add
dl_esp32p4_s16_unaligned_rescale_add2d_11c_relu:
    lw t3, 96(a3) # output_scale
    lw t4, 92(a3) # output_shift

    li t0, 1
    beq t3, t0, dl_esp32p4_s16_rescale_unaligned_add2d_output_shift_relu


### rescaled to output by *scale) >> shift
dl_esp32p4_s16_rescale_unaligned_add2d_output_scale_relu:

    sw t3, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all output_scale

    bltz a4, dl_esp32p4_s16_rescale_unaligned_add2d_scale_small_remainder_relu # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    add t0, a4, x0
    blez t0, 7f
    6:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.mov.s16.qacc q5
        esp.srcmb.s16.qacc q1, a5, 1

        dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4
        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s16 q2, s8, s9
        dl_esp32p4_128b_unaligned_store0 q2, a0, s0
        addi t0, t0, -1
        bgtz t0, 6b
    7:

    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4

    esp.vrelu.s16 q2, s8, s9
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_esp32p4_s16_rescale_unaligned_add2d_scale_remainder_relu


dl_esp32p4_s16_rescale_unaligned_add2d_scale_small_remainder_relu:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar

dl_esp32p4_s16_rescale_unaligned_add2d_scale_remainder_relu:
    beqz t5, dl_esp32p4_s16_unaligned_rescale_add2d_output_scale_end_relu # c remainder

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q1, a5, 1

    dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4

    esp.vrelu.s16 q2, s8, s9
    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q2, t5, s1, a0

dl_esp32p4_s16_unaligned_rescale_add2d_output_scale_end_relu:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


### rescaled to output by right shift
dl_esp32p4_s16_rescale_unaligned_add2d_output_shift_relu:
    li s1, 1
    sh s1, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all 1

    bltz a4, dl_esp32p4_s16_rescale_unaligned_add2d_shift_small_remainder_relu # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    add t0, a4, x0
    blez t0, 9f
    8:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4
        esp.mov.s16.qacc q5

        esp.srcmb.s16.qacc q5, a5, 1
        esp.vmulas.s16.qacc q2, q7
        esp.srcmb.s16.qacc q5, t4, 1

        esp.ld.128.usar.ip q1, a1, 16
        esp.vrelu.s16 q5, s8, s9
        dl_esp32p4_128b_unaligned_store0 q5, a0, s1
        addi t0, t0, -1
        bgtz t0, 8b
    9:
    addi a1, a1, -16
    add a1, a1, t5

    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4
    esp.mov.s16.qacc q5

    esp.srcmb.s16.qacc q5, a5, 1
    esp.vmulas.s16.qacc q2, q7
    esp.srcmb.s16.qacc q5, t4, 1

    esp.vrelu.s16 q5, s8, s9
    dl_esp32p4_128b_unaligned_store0 q5, a0, s1
    j dl_esp32p4_s16_rescale_unaligned_add2d_shift_remainder_relu



dl_esp32p4_s16_rescale_unaligned_add2d_shift_small_remainder_relu:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar

dl_esp32p4_s16_rescale_unaligned_add2d_shift_remainder_relu:
    beqz t5, dl_esp32p4_s16_unaligned_rescale_add2d_output_shift_end_relu # c remainder

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q5, a5, 1
    esp.vmulas.s16.qacc q2, q7
    esp.srcmb.s16.qacc q5, t4, 1

    esp.vrelu.s16 q5, s8, s9
    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q5, t5, s1, a0

dl_esp32p4_s16_unaligned_rescale_add2d_output_shift_end_relu:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret




    .align 2
    .text
    .global dl_esp32p4_s16_unaligned_add2d_11c_prelu
    .type   dl_esp32p4_s16_unaligned_add2d_11c_prelu, @function
    #.section .iram1
dl_esp32p4_s16_unaligned_add2d_11c_prelu:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int16_t *output_ptr
    # a1: int16_t *input0_ptr
    # a2: int16_t *input1_ptr
    # a3: void *args
    # a4: c_div_x_1
    # a5: input_shift
    # t3: output_scale
    # t4: output_shift
    # t5: c_remainder
    # s8: activation_alpha_ptr
    # s9: activation_shift

    lw a4, 64(a3)
    lw t5, 76(a3)
    lw a5, 88(a3)
    lw s8, 56(a3)
    lw s9, 60(a3)

    bgez a5, dl_esp32p4_s16_unaligned_rescale_add2d_11c_prelu

# input0 exp = input1 exp = output exp

    esp.ld.128.usar.ip q5, a0, 0 #get output_ptr sar_byte
    esp.movx.r.sar.bytes s1

    bltz a4, dl_esp32p4_s16_unaligned_add2d_11c_small_remainder_prelu # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    beqz s1, dl_esp32p4_s16_unaligned_add2d_11c_prelu_0
    li t0, 8
    beq s1, t0, dl_esp32p4_s16_unaligned_add2d_11c_prelu_1


    add t0, a4, x0
    blez t0, 1f
    0:
        esp.src.q.qup q2, q0, q1
        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4
        esp.vadd.s16 q2, q2, q5

        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s16 q2, q2, q6, s9
        dl_esp32p4_128b_unaligned_store0 q2, a0, s1
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

    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q2, q2, q5
    esp.vprelu.s16 q2, q2, q6, s9
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder_prelu

    #output sar = 0
dl_esp32p4_s16_unaligned_add2d_11c_prelu_0:
    add t0, a4, x0
    blez t0, 3f
    2:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s16 q2, q2, q5

        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s16 q2, q2, q6, s9
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

    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q2, q2, q5
    esp.vprelu.s16 q2, q2, q6, s9
    esp.vst.128.ip q2, a0, 16
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder_prelu

    #output sar = 8
dl_esp32p4_s16_unaligned_add2d_11c_prelu_1:
    add t0, a4, x0
    blez t0, 5f
    4:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.vadd.s16 q2, q2, q5

        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s16 q2, q2, q6, s9
        dl_esp32p4_128b_unaligned_store1 q2, a0
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

    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q2, q2, q5
    esp.vprelu.s16 q2, q2, q6, s9
    dl_esp32p4_128b_unaligned_store1 q2, a0
    j dl_esp32p4_s16_unaligned_add2d_11c_remainder_prelu

dl_esp32p4_s16_unaligned_add2d_11c_small_remainder_prelu:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0

dl_esp32p4_s16_unaligned_add2d_11c_remainder_prelu:

    beqz t5, dl_esp32p4_s16_unaligned_add2d_end_prelu

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.vld.128.ip q6, s8, 16
    esp.vadd.s16 q2, q2, q5
    esp.vprelu.s16 q2, q2, q6, s9
    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q2, t5, s1, a0

dl_esp32p4_s16_unaligned_add2d_end_prelu:

    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


## rescaled add
dl_esp32p4_s16_unaligned_rescale_add2d_11c_prelu:
    lw t3, 96(a3) # output_scale
    lw t4, 92(a3) # output_shift

    li t0, 1
    beq t3, t0, dl_esp32p4_s16_rescale_unaligned_add2d_output_shift_prelu


### rescaled to output by *scale) >> shift
dl_esp32p4_s16_rescale_unaligned_add2d_output_scale_prelu:

    sw t3, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all output_scale
    # ssr t4 #output shift
    # li s1, 0

    bltz a4, dl_esp32p4_s16_rescale_unaligned_add2d_scale_small_remainder_prelu # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    add t0, a4, x0
    blez t0, 7f
    6:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4

        esp.mov.s16.qacc q5
        esp.srcmb.s16.qacc q1, a5, 1

        dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4
        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s16 q2, q2, q6, s9
        dl_esp32p4_128b_unaligned_store0 q2, a0, s0
        addi t0, t0, -1
        bgtz t0, 6b
    7:

    addi a1, a1, -16
    add a1, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q1, a5, 1

    esp.vld.128.ip q6, s8, 16
    dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4

    esp.vprelu.s16 q2, q2, q6, s9
    dl_esp32p4_128b_unaligned_store0 q2, a0, s1
    j dl_esp32p4_s16_rescale_unaligned_add2d_scale_remainder_prelu


dl_esp32p4_s16_rescale_unaligned_add2d_scale_small_remainder_prelu:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar

dl_esp32p4_s16_rescale_unaligned_add2d_scale_remainder_prelu:
    beqz t5, dl_esp32p4_s16_unaligned_rescale_add2d_output_scale_end_prelu # c remainder

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q1, a5, 1

    esp.vld.128.ip q6, s8, 16
    dl_esp32p4_s16_rescale_add_rescale_output q2, q1, q2, q7, t4

    esp.vprelu.s16 q2, q2, q6, s9
    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q2, t5, s1, a0

dl_esp32p4_s16_unaligned_rescale_add2d_output_scale_end_prelu:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


### rescaled to output by right shift
dl_esp32p4_s16_rescale_unaligned_add2d_output_shift_prelu:
    li s1, 1
    sh s1, 0(sp)
    add s11, sp, x0
    esp.vldbc.16.ip q7, s11, 0 # all 1

    bltz a4, dl_esp32p4_s16_rescale_unaligned_add2d_shift_small_remainder_prelu # channel < 16

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q3, a2, 16
    esp.ld.128.usar.ip q1, a1, 16

    add t0, a4, x0
    blez t0, 9f
    8:
        esp.src.q.qup q2, q0, q1

        esp.ld.128.usar.ip q4, a2, 16
        esp.src.q.qup q5, q3, q4
        esp.mov.s16.qacc q5

        esp.srcmb.s16.qacc q5, a5, 1
        esp.vmulas.s16.qacc q2, q7
        esp.srcmb.s16.qacc q5, t4, 1

        esp.vld.128.ip q6, s8, 16
        esp.ld.128.usar.ip q1, a1, 16
        esp.vprelu.s16 q5, q5, q6, s9
        dl_esp32p4_128b_unaligned_store0 q5, a0, s1
        addi t0, t0, -1
        bgtz t0, 8b
    9:
    addi a1, a1, -16
    add a1, a1, t5

    esp.movx.r.sar.bytes t6 #input0 sar
    esp.src.q.qup q2, q0, q1

    esp.ld.128.usar.xp q4, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar
    esp.src.q.qup q5, q3, q4
    esp.mov.s16.qacc q5

    esp.srcmb.s16.qacc q5, a5, 1
    esp.vmulas.s16.qacc q2, q7
    esp.vld.128.ip q6, s8, 16
    esp.srcmb.s16.qacc q5, t4, 1

    esp.vprelu.s16 q5, q5, q6, s9
    dl_esp32p4_128b_unaligned_store0 q5, a0, s1
    j dl_esp32p4_s16_rescale_unaligned_add2d_shift_remainder_prelu



dl_esp32p4_s16_rescale_unaligned_add2d_shift_small_remainder_prelu:
    esp.ld.128.usar.xp q0, a1, t5
    esp.movx.r.sar.bytes t6 #input0 sar
    esp.ld.128.usar.xp q3, a2, t5
    esp.movx.r.sar.bytes s0 #input1 sar

dl_esp32p4_s16_rescale_unaligned_add2d_shift_remainder_prelu:
    beqz t5, dl_esp32p4_s16_unaligned_rescale_add2d_output_shift_end_prelu # c remainder

    esp.ld.128.usar.ip q1, a1, 0
    esp.movx.w.sar.bytes t6
    esp.src.q q2, q0, q1

    esp.ld.128.usar.ip q4, a2, 0
    esp.movx.w.sar.bytes s0
    esp.src.q q5, q3, q4

    esp.mov.s16.qacc q5
    esp.srcmb.s16.qacc q5, a5, 1
    esp.vmulas.s16.qacc q2, q7
    esp.vld.128.ip q6, s8, 16
    esp.srcmb.s16.qacc q5, t4, 1

    esp.vprelu.s16 q5, q5, q6, s9
    srli t5, t5, 1
    dl_esp32p4_s16_store_remainder q5, t5, s1, a0

dl_esp32p4_s16_unaligned_rescale_add2d_output_shift_end_prelu:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
