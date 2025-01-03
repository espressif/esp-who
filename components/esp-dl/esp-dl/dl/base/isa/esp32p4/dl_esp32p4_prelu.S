#include "dl_esp32p4_s8.S"
#include "dl_esp32p4_common.S"

    .align 2
    .text
    .global dl_esp32p4_s8_prelu_11c
    .type   dl_esp32p4_s8_prelu_11c, @function
dl_esp32p4_s8_prelu_11c:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args
    # a3: c_div_x = n_div_x
    # s0: activation_alpha_ptr
    # s1: activation_shift
    # s8: output_shift
    # s9: output_scale


    lw a3, 96(a2)   # n_div_x
    lw s0, 80(a2)  # activation_alpha_ptr
    lw s1, 84(a2)  # activation_shift
    lw s8, 172(a2) # output_shift
    lw s9, 176(a2) # output_scale

    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, s0, 16

    esp.zero.q q2 # all 0
    addi a5, a2, 176
    esp.vldbc.8.ip q3, a5, 0 # all output_scale

    add t0, a3, x0
    blez t0, 1f
    0:
        # neg part, alpha * input, right shift: output - alpha - input
        esp.vprelu.s8 q4, q0, q1, s1
        esp.vcmp.lt.s8 q6, q0, q2
        esp.andq q4, q4, q6

        # pos part, *scale, right shift: output - input
        esp.zero.qacc
        esp.vmulas.s8.qacc.ld.ip q1, s0, 16, q0, q3
        esp.srcmb.s8.qacc q5, s8, 1
        esp.vcmp.gt.s8 q6, q0, q2
        esp.andq q5, q5, q6

        esp.vadd.s8.ld.incp q0, a1, q4, q4, q5
        esp.vst.128.ip q4, a0, 16
        addi t0, t0, -1
        bgtz t0, 0b
    1:
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret


    .align 2
    .text
    .global dl_esp32p4_s8_unaligned_prelu_11c
    .type   dl_esp32p4_s8_unaligned_prelu_11c, @function
dl_esp32p4_s8_unaligned_prelu_11c:
    .align 2
    esp32p4_push_128_stacks_4r  s0, s1, s8, s9

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args
    # a3: c_div_x = n_div_x
    # a4: c_remainder
    # s0: activation_alpha_ptr
    # s1: activation_shift
    # s8: output_shift
    # s9: output_scale


    lw a3, 96(a2)   # c_div_x
    lw a4, 136(a2)  # c_remainder
    lw s0, 80(a2)  # activation_alpha_ptr
    lw s1, 84(a2)  # activation_shift
    lw s8, 172(a2) # output_shift
    lw s9, 176(a2) # output_scale

    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q1, s0, 16

    addi a5, a2, 176
    esp.vldbc.8.ip q7, a5, 0 # all output_scale

    add t0, a3, x0
    blez t0, 1f
    0:
        esp.ld.128.usar.ip q2, a1, 16
        esp.src.q.qup q4, q0, q2

        esp.ld.128.usar.ip q3, s0, 16
        esp.src.q.qup q5, q1, q3

        # neg part, alpha * input, right shift: output - alpha - input
        esp.vprelu.s8 q5, q4, q5, s1
        esp.zero.q q2
        esp.vcmp.lt.s8 q6, q4, q2
        esp.andq q5, q5, q6

        # pos part, *scale, right shift: output - input
        esp.zero.qacc
        esp.vmulas.s8.qacc q4, q7
        esp.srcmb.s8.qacc q3, s8, 1
        esp.vcmp.gt.s8 q6, q4, q2
        esp.andq q3, q3, q6

        esp.vadd.s8 q3, q3, q5
        esp32p4_s8_32b_unaligned_vector_store q3, a0, t3
        addi t0, t0, -1
        bgtz t0, 0b
    1:

    bnez a4, dl_esp32p4_s8_unaligned_prelu_remainder
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret

dl_esp32p4_s8_unaligned_prelu_remainder:
    esp.ld.128.usar.ip q2, a1, 16
    esp.src.q.qup q4, q0, q2

    esp.ld.128.usar.ip q3, s0, 16
    esp.src.q.qup q5, q1, q3

    # neg part, alpha * input, right shift: output - alpha - input
    esp.vprelu.s8 q5, q4, q5, s1
    esp.zero.q q2
    esp.vcmp.lt.s8 q6, q4, q2
    esp.andq q5, q5, q6

    # pos part, *scale, right shift: output - input
    esp.zero.qacc
    esp.vmulas.s8.qacc q4, q7
    esp.srcmb.s8.qacc q3, s8, 1
    esp.vcmp.gt.s8 q6, q4, q2
    esp.andq q3, q3, q6

    esp.vadd.s8 q3, q3, q5
    dl_esp32p4_s8_store_remainder q3, a1, a2, a3, a5, a6, a0, a4
    esp32p4_pop_128_stacks_4r  s0, s1, s8, s9
    ret
