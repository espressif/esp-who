#include "dl_tie728_s8.S"

    .align 4
    .text
    .global dl_tie728_s8_resize2d_nearest_2x2_c1
    .type dl_tie728_s8_resize2d_nearest_2x2_c1, @function
dl_tie728_s8_resize2d_nearest_2x2_c1:
    .align 4
    entry sp, 24

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args
    # a5: output_x_offset
    # a6: output_y_offset
    # a7: c_div_x
    # a8: remainder
    # a9: output_shift
    # a10: output_scale

    l32i a5, a4, 20
    l32i a6, a4, 24
    l32i a7, a4, 40
    l32i a8, a4, 44
    l32i a9, a4, 48
    l32i a10, a4, 52

    # a11 (0, 1)
    # a12 (1, 0)
    # a13 (1, 1)
    add a11, a2, a5
    add a12, a2, a6
    add a13, a11, a6

    s8i a10, sp, 0
    ee.vldbc.8.ip q1, sp, 0 # all output_scale
    ee.vld.128.ip q0, a3, 16

    loopgtz a7, 0f
        ee.zero.qacc
        ee.vmulas.s8.qacc.ld.ip q0, a3, 16, q0, q1
        tie728_s8_vector_round_result q2, a9, a14, q7
        ee.vst.128.ip q2, a2, 16
        ee.vst.128.ip q2, a11, 16
        ee.vst.128.ip q2, a12, 16
        ee.vst.128.ip q2, a13, 16
    0:
    retw

    .align 4
    .text
    .global dl_tie728_s8_unaligned_resize2d_nearest_2x2_c1
    .type dl_tie728_s8_unaligned_resize2d_nearest_2x2_c1, @function
dl_tie728_s8_unaligned_resize2d_nearest_2x2_c1:
    .align 4
    entry sp, 24

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args
    # a5: output_x_offset
    # a6: output_y_offset
    # a7: c_div_x
    # a8: remainder
    # a9: output_shift
    # a10: output_scale


    l32i a5, a4, 20
    l32i a6, a4, 24
    l32i a7, a4, 40
    l32i a8, a4, 44
    l32i a9, a4, 48
    l32i a10, a4, 52


    # a11 (0, 1)
    # a12 (1, 0)
    # a13 (1, 1)
    add a11, a2, a5
    add a12, a2, a6
    add a13, a11, a6

    s8i a10, sp, 0
    ee.vldbc.8.ip q3, sp, 0 # all output_scale
    ee.ld.128.usar.ip q0, a3, 16

    loopgtz a7, 0f
        ee.zero.qacc
        ee.ld.128.usar.ip q1, a3, 16
        ee.src.q.qup q2, q0, q1
        ee.vmulas.s8.qacc q2, q3
        tie728_s8_vector_round_result q4, a9, a14, q7
        dl_tie728_s8_unaligned_store0 q4, a2, a14
        dl_tie728_s8_unaligned_store0 q4, a11, a14
        dl_tie728_s8_unaligned_store0 q4, a12, a14
        dl_tie728_s8_unaligned_store0 q4, a13, a14
    0:

    bnez a8, dl_tie728_s8_unaligned_resize2d_nearest_2x2_c1_remainder
    retw

dl_tie728_s8_unaligned_resize2d_nearest_2x2_c1_remainder:
    ee.zero.qacc
    ee.ld.128.usar.ip q1, a3, 16
    ee.src.q.qup q2, q0, q1
    ee.vmulas.s8.qacc q2, q3
    tie728_s8_vector_round_result q4, a9, a14, q7
    dl_tie728_s8_store_remainder q4, a5, a6, a7, a10, a2, a8
    dl_tie728_s8_store_remainder q4, a5, a6, a7, a10, a11, a8
    dl_tie728_s8_store_remainder q4, a5, a6, a7, a10, a12, a8
    dl_tie728_s8_store_remainder q4, a5, a6, a7, a10, a13, a8
    retw
