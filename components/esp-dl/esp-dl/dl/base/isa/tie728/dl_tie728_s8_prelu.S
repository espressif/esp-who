#include "dl_tie728_s8.S"

    .align 4
    .text
    .global dl_tie728_s8_prelu_11c
    .type   dl_tie728_s8_prelu_11c, @function
    .section .iram1
dl_tie728_s8_prelu_11c:
    .align 4
    entry sp, 16

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args
    # a5: c_div_x = n_div_x
    # a12: activation_alpha_ptr
    # a13: activation_shift
    # a14: output_shift
    # a15: output_scale


    l32i a5, a4, 96   # n_div_x
    l32i a12, a4, 80  # activation_alpha_ptr
    l32i a13, a4, 84  # activation_shift
    l32i a14, a4, 172 # output_shift
    l32i a15, a4, 176 # output_scale

    ee.vld.128.ip q0, a3, 16
    ee.vld.128.ip q1, a12, 16

    ee.zero.q q2 # all 0
    addi a7, a4, 176
    ee.vldbc.8.ip q3, a7, 0 # all output_scale

    loopgtz a5, 0f
        # neg part, alpha * input, right shift: output - alpha - input
        ee.zero.qacc
        ee.vmulas.s8.qacc.ld.ip q1, a12, 16, q0, q1
        tie728_s8_vector_round_result q4, a13, a6, q6
        ee.vcmp.lt.s8 q6, q0, q2
        ee.andq q4, q4, q6

        # pos part, *scale, right shift: output - input
        ee.zero.qacc
        ee.vmulas.s8.qacc q0, q3
        tie728_s8_vector_round_result q5, a14, a6, q6
        ee.vcmp.gt.s8 q6, q0, q2
        ee.andq q5, q5, q6

        ee.vadds.s8.ld.incp q0, a3, q4, q4, q5
        ee.vst.128.ip q4, a2, 16
    0:
    retw


    .align 4
    .text
    .global dl_tie728_s8_unaligned_prelu_11c
    .type   dl_tie728_s8_unaligned_prelu_11c, @function
    .section .iram1
dl_tie728_s8_unaligned_prelu_11c:
    .align 4
    entry sp, 16

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args
    # a5: c_div_x = n_div_x
    # a6: c_remainder
    # a12: activation_alpha_ptr
    # a13: activation_shift
    # a14: output_shift
    # a15: output_scale


    l32i a5, a4, 96   # c_div_x
    l32i a6, a4, 136  # c_remainder
    l32i a12, a4, 80  # activation_alpha_ptr
    l32i a13, a4, 84  # activation_shift
    l32i a14, a4, 172 # output_shift
    l32i a15, a4, 176 # output_scale

    ee.ld.128.usar.ip q0, a3, 16
    ee.ld.128.usar.ip q1, a12, 16

    addi a7, a4, 176
    ee.vldbc.8.ip q7, a7, 0 # all output_scale

    loopgtz a5, 0f
        ee.ld.128.usar.ip q2, a3, 16
        ee.src.q.qup q4, q0, q2

        ee.ld.128.usar.ip q3, a12, 16
        ee.src.q.qup q5, q1, q3

        # neg part, alpha * input, right shift: output - alpha - input
        ee.zero.qacc
        ee.vmulas.s8.qacc q4, q5
        tie728_s8_vector_round_result q5, a13, a8, q6
        ee.zero.q q2
        ee.vcmp.lt.s8 q6, q4, q2
        ee.andq q5, q5, q6

        # pos part, *scale, right shift: output - input
        ee.zero.qacc
        ee.vmulas.s8.qacc q4, q7
        tie728_s8_vector_round_result q3, a14, a8, q6
        ee.vcmp.gt.s8 q6, q4, q2
        ee.andq q3, q3, q6

        ee.vadds.s8 q3, q3, q5
        dl_tie728_s8_unaligned_store0 q3, a2, a8
    0:

    bnez a6, dl_tie728_s8_unaligned_prelu_remainder
    retw

dl_tie728_s8_unaligned_prelu_remainder:
    ee.ld.128.usar.ip q2, a3, 16
    ee.src.q.qup q4, q0, q2

    ee.ld.128.usar.ip q3, a12, 16
    ee.src.q.qup q5, q1, q3

    # neg part, alpha * input, right shift: output - alpha - input
    ee.zero.qacc
    ee.vmulas.s8.qacc q4, q5
    tie728_s8_vector_round_result q5, a13, a8, q6
    ee.zero.q q2
    ee.vcmp.lt.s8 q6, q4, q2
    ee.andq q5, q5, q6

    # pos part, *scale, right shift: output - input
    ee.zero.qacc
    ee.vmulas.s8.qacc q4, q7
    tie728_s8_vector_round_result q3, a14, a8, q6
    ee.vcmp.gt.s8 q6, q4, q2
    ee.andq q3, q3, q6

    ee.vadds.s8 q3, q3, q5
    dl_tie728_s8_store_remainder q3, a3, a4, a5, a7, a2, a6
    retw
