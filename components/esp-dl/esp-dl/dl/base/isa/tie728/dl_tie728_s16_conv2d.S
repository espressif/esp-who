#include "dl_tie728_s16.S"


############################################################################################################################################################
####
#### tie728_s16_conv2d_11cn series
####
############################################################################################################################################################
.macro tie728_s16_conv2d_11c8 input_v0 filter_v0 filter_v1 input_ptr filter_ptr c_div_x_1
    # scalar * vecter and accumulate into QACC
    # input_ptr += (c_div_x_1 + 1) * 16 in the end
    # filter_ptr point to the next 16 bytes in the end

    # input_v0:     8 input elements
    # filter_v0:    8 filter elements
    # filter_v1:    8 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:        input_channel // 8 - 1

    EE.VLD.128.IP \input_v0,  \input_ptr,  16
    EE.VLD.128.IP \filter_v0, \filter_ptr, 16
    EE.VLD.128.IP \filter_v1, \filter_ptr, 16

    loopgtz \c_div_x_1, 0f
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S16.QACC.LD.INCP \input_v0,  \input_ptr,  \filter_v1, \input_v0, 7
    EE.VLD.128.IP               \filter_v1, \filter_ptr, 16
0:
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S16.QACC                                  \filter_v0, \input_v0, 6
    EE.VSMULAS.S16.QACC                                  \filter_v1, \input_v0, 7
.endm




.macro tie728_s16_conv2d_11cn_load_args args filter_ptr c_div_x_1 n_rs3 mac_shift
    l32i \n_rs3,          \args,  96  // output_channel_div_8
    l32i \mac_shift,      \args,  64  // mac_shift
    l32i \filter_ptr,     \args,  48  // filter
    l32i \c_div_x_1,      \args, 100  // input_channel / x - 1
.endm



    .align 4
    .text
    .global dl_tie728_s16_conv2d_11cn_bias
    .type	dl_tie728_s16_conv2d_11cn_bias, @function
    # .section .iram1
dl_tie728_s16_conv2d_11cn_bias:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_11cn_load_args a4, a5, a6, a7, a8

    l32i a11, a4, 68  // bias
    # l32i a12, a4, 76  // activation_alpha
    # l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_11cn_bias_loop:
    mov a15, a3    # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_conv2d_11c8 q0, q1, q2, a15, a5, a6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_11cn_bias_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_11cn_bias_relu
    .type	dl_tie728_s16_conv2d_11cn_bias_relu, @function
    # .section .iram1
dl_tie728_s16_conv2d_11cn_bias_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_11cn_load_args a4, a5, a6, a7, a8

    l32i a11, a4, 68  // bias
    l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_11cn_bias_relu_loop:
    mov a15, a3    # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_conv2d_11c8 q0, q1, q2, a15, a5, a6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_relu  q0, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_11cn_bias_relu_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_11cn_bias_prelu
    .type	dl_tie728_s16_conv2d_11cn_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_conv2d_11cn_bias_prelu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_11cn_load_args a4, a5, a6, a7, a8

    l32i a11, a4, 68  // bias
    l32i a12, a4, 80  // activation_alpha_ptr
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_11cn_bias_prelu_loop:
    mov a15, a3    # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_conv2d_11c8 q0, q1, q2, a15, a5, a6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_prelu  q0, q2, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_11cn_bias_prelu_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_11cn
    .type	dl_tie728_s16_conv2d_11cn, @function
    # .section .iram1
dl_tie728_s16_conv2d_11cn:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_11cn_load_args a4, a5, a6, a7, a8

    # l32i a11, a4, 68  // bias
    # l32i a12, a4, 76  // activation_alpha
    # l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_11cn_loop:
    mov a15, a3    # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_11c8 q0, q1, q2, a15, a5, a6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_11cn_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_11cn_relu
    .type	dl_tie728_s16_conv2d_11cn_relu, @function
    # .section .iram1
dl_tie728_s16_conv2d_11cn_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_11cn_load_args a4, a5, a6, a7, a8

    # l32i a11, a4, 68  // bias
    l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_11cn_relu_loop:
    mov a15, a3    # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_11c8 q0, q1, q2, a15, a5, a6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_relu  q0, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_11cn_relu_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_11cn_prelu
    .type	dl_tie728_s16_conv2d_11cn_prelu, @function
    # .section .iram1
dl_tie728_s16_conv2d_11cn_prelu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_11cn_load_args a4, a5, a6, a7, a8

    # l32i a11, a4, 68  // bias
    l32i a12, a4, 80  // activation_alpha_ptr
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_11cn_prelu_loop:
    mov a15, a3    # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_11c8 q0, q1, q2, a15, a5, a6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_prelu  q0, q1, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_11cn_prelu_loop

    retw






############################################################################################################################################################
####
#### tie728_s16_conv2d_33cn series
####
############################################################################################################################################################
.macro tie728_s16_conv2d_33c8 input_v0 filter_v0 filter_v1 input_ptr filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro tie728_s16_conv2d_hwcn_load_args args filter_ptr c_div_x_1  n_rs3 mac_shift dilation_x_offset dilation_y_offset
    tie728_s16_conv2d_11cn_load_args \args, \filter_ptr, \c_div_x_1, \n_rs3, \mac_shift
    l32i \dilation_x_offset, \args, 108  // input dilation x offset
    l32i \dilation_y_offset, \args, 112  // input dilation y offset
.endm



    .align 4
    .text
    .global dl_tie728_s16_conv2d_33cn_bias
    .type	dl_tie728_s16_conv2d_33cn_bias, @function
    # .section .iram1
dl_tie728_s16_conv2d_33cn_bias:
    .align 4
    entry sp, 16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: bias_ptr
    # a12:
    # a13:
    # a14
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a11, a4, 68  // bias
    # l32i a12, a4, 76  // activation_alpha
    # l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_33cn_bias_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_conv2d_33c8 q0, q1, q2, a15, a5, a6, a9, a10
    tie728_s16_vector_round_result  q0, a8, a15, q3
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_33cn_bias_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_33cn_bias_relu
    .type	dl_tie728_s16_conv2d_33cn_bias_relu, @function
    # .section .iram1
dl_tie728_s16_conv2d_33cn_bias_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: bias_ptr
    # a12: activation_alpha
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a11, a4, 68  // bias
    l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_33cn_bias_relu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_conv2d_33c8 q0, q1, q2, a15, a5, a6, a9, a10
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_relu  q0, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_33cn_bias_relu_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_33cn_bias_prelu
    .type	dl_tie728_s16_conv2d_33cn_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_conv2d_33cn_bias_prelu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: bias_ptr
    # a12: activation_alpha_ptr
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a11, a4, 68  // bias
    l32i a12, a4, 80  // activation_alpha_ptr
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_33cn_bias_prelu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_conv2d_33c8 q0, q1, q2, a15, a5, a6, a9, a10
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_prelu  q0, q2, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_33cn_bias_prelu_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_33cn
    .type	dl_tie728_s16_conv2d_33cn, @function
    # .section .iram1
dl_tie728_s16_conv2d_33cn:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11:
    # a12:
    # a13:
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    # l32i a11, a4, 68  // bias
    # l32i a12, a4, 76  // activation_alpha
    # l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_33cn_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_33c8 q0, q1, q2, a15, a5, a6, a9, a10
    tie728_s16_vector_round_result  q0, a8, a15, q3
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_33cn_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_33cn_relu
    .type	dl_tie728_s16_conv2d_33cn_relu, @function
    # .section .iram1
dl_tie728_s16_conv2d_33cn_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11:
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    # l32i a11, a4, 68  // bias
    l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_33cn_relu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_33c8 q0, q1, q2, a15, a5, a6, a9, a10
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_relu  q0, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_33cn_relu_loop

    retw



    .align 4
    .text
    .global dl_tie728_s16_conv2d_33cn_prelu
    .type	dl_tie728_s16_conv2d_33cn_prelu, @function
    # .section .iram1
dl_tie728_s16_conv2d_33cn_prelu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11:
    # a12: activation_alpha_ptr
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    # l32i a11, a4, 68  // bias
    l32i a12, a4, 80  // activation_alpha_ptr
    l32i a13, a4, 84  // activation_shift

tie728_s16_conv2d_33cn_prelu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_33c8 q0, q1, q2, a15, a5, a6, a9, a10
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_prelu  q0, q1, a12, a13
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_33cn_prelu_loop

    retw






############################################################################################################################################################
####
#### tie728_s16_conv2d_hwcn series
####
############################################################################################################################################################
.macro tie728_s16_conv2d_hwc8 input_v0 filter_v0 filter_v1 input_ptr filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset filter_h filter_w args filter_offset_q
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)
    # filter_h
    # filter_w

    l32i \filter_h, \args, 52  # filter_height
    1:
        l32i \filter_w, \args, 56  # filter_width
        beqi \filter_w, 1, 3f
        2:
            tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1
            add \input_ptr, \input_ptr, \dilation_x_offset

            addi \filter_w, \filter_w, -1
            bgei \filter_w, 2, 2b
        3:
        tie728_s16_conv2d_11c8 \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1

        EE.MOVI.32.A \filter_offset_q, \filter_w, 1
        add \filter_ptr, \filter_ptr, \filter_w
        add \input_ptr, \input_ptr, \dilation_y_offset

        addi \filter_h, \filter_h, -1
        bnez \filter_h, 1b

    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h

.endm





    .align 4
    .text
    .global dl_tie728_s16_conv2d_hwcn_bias
    .type	dl_tie728_s16_conv2d_hwcn_bias, @function
    # .section .iram1
dl_tie728_s16_conv2d_hwcn_bias:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height
    # a12: filter_width
    # a13: bias_ptr
    # a14:
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10
    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1 #filter_y_offset
    EE.MOVI.32.Q q6, a11, 2 #filter_n_offset

    l32i a13, a4, 68  // bias

tie728_s16_conv2d_hwcn_bias_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a13
    tie728_s16_conv2d_hwc8 q0, q1, q2, a15, a5, a6, a9, a10, a11, a12, a4, q6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_hwcn_bias_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_conv2d_hwcn_bias_relu
    .type	dl_tie728_s16_conv2d_hwcn_bias_relu, @function
    # .section .iram1
dl_tie728_s16_conv2d_hwcn_bias_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height
    # a12: filter_width
    # a13: bias_ptr
    # a14: activation_alpha
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1 #filter_y_offset
    EE.MOVI.32.Q q6, a11, 2 #filter_n_offset

    l32i a13, a4, 68  // bias
    l32i a14, a4, 76  // activation_alpha

tie728_s16_conv2d_hwcn_bias_relu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a13
    tie728_s16_conv2d_hwc8 q0, q1, q2, a15, a5, a6, a9, a10, a11, a12, a4, q6
    l32i a11, a4, 84  // activation_shift
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_relu  q0, a14, a11
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_hwcn_bias_relu_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_conv2d_hwcn_bias_prelu
    .type	dl_tie728_s16_conv2d_hwcn_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_conv2d_hwcn_bias_prelu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height
    # a12: filter_width
    # a13: bias_ptr
    # a14: activation_alpha_ptr
    # a15: moving_input_ptr
    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1 #filter_y_offset
    EE.MOVI.32.Q q6, a11, 2 #filter_n_offset

    l32i a13, a4, 68  // bias_ptr
    l32i a14, a4, 80  // activation_alpha_ptr

tie728_s16_conv2d_hwcn_bias_prelu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_128b_vector_bias  a13
    tie728_s16_conv2d_hwc8 q0, q1, q2, a15, a5, a6, a9, a10, a11, a12, a4, q6
    l32i a11, a4, 84  // activation_shift
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_prelu  q0, q2, a14, a11
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_hwcn_bias_prelu_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_conv2d_hwcn
    .type	dl_tie728_s16_conv2d_hwcn, @function
    # .section .iram1
dl_tie728_s16_conv2d_hwcn:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height
    # a12: filter_width
    # a13:
    # a14:
    # a15: moving_input_ptr

    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10
    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1 #filter_y_offset
    EE.MOVI.32.Q q6, a11, 2 #filter_n_offset

    # l32i a13, a4, 68  // bias

tie728_s16_conv2d_hwcn_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_hwc8 q0, q1, q2, a15, a5, a6, a9, a10, a11, a12, a4, q6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_hwcn_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_conv2d_hwcn_relu
    .type	dl_tie728_s16_conv2d_hwcn_relu, @function
    # .section .iram1
dl_tie728_s16_conv2d_hwcn_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height
    # a12: filter_width
    # a13: activation_alpha
    # a14: activation_shift
    # a15: moving_input_ptr

    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10
    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1 #filter_y_offset
    EE.MOVI.32.Q q6, a11, 2 #filter_n_offset

    l32i a13, a4, 76  // activation_alpha
    l32i a14, a4, 84  // activation_shift

tie728_s16_conv2d_hwcn_relu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_hwc8 q0, q1, q2, a15, a5, a6, a9, a10, a11, a12, a4, q6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_relu  q0, a13, a14
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_hwcn_relu_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_conv2d_hwcn_prelu
    .type	dl_tie728_s16_conv2d_hwcn_prelu, @function
    # .section .iram1
dl_tie728_s16_conv2d_hwcn_prelu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height
    # a12: filter_width
    # a13: activation_prelu_ptr
    # a14: activation_shift
    # a15: moving_input_ptr

    tie728_s16_conv2d_hwcn_load_args a4, a5, a6, a7, a8, a9, a10
    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1 #filter_y_offset
    EE.MOVI.32.Q q6, a11, 2 #filter_n_offset

    l32i a13, a4, 80  // activation_alpha_ptr
    l32i a14, a4, 84  // activation_shift

tie728_s16_conv2d_hwcn_prelu_loop:
    mov a15, a3     # reload input_ptr
    EE.ZERO.QACC

    tie728_s16_conv2d_hwc8 q0, q1, q2, a15, a5, a6, a9, a10, a11, a12, a4, q6
    tie728_s16_vector_round_result  q0, a8, a15, q3
    tie728_s16_conv2d_prelu  q0, q1, a13, a14
    EE.VST.128.IP  q0, a2, 16

    addi a7, a7, -1
    bnez a7, tie728_s16_conv2d_hwcn_prelu_loop

    retw
