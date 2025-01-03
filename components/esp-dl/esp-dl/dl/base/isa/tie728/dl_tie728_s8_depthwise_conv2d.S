#include "dl_tie728_s8.S"


############################################################################################################################################################
####
#### tie728_s8_depthwise_conv2d_33c1 series
####
############################################################################################################################################################
.macro tie728_s8_depthwise_conv2d_33s1 input_v0 filter_v0 input_v1 filter_v1 input_v2 filter_v2 input_ptr filter_ptr dilation_x_offset dilation_y_offset next_33s1
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_33s1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16

    # EE.ZERO.QACC

    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \next_33s1

    EE.VMULAS.S8.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
.endm



.macro tie728_s8_depthwise_conv2d_33s1_last input_v0 filter_v0 input_v1 filter_v1 input_ptr filter_ptr dilation_x_offset dilation_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)

    # EE.ZERO.QACC

    EE.VMULAS.S8.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP               \input_v0,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S8.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP               \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP               \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP               \input_v1,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S8.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP               \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP               \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.IP               \input_v0,  \input_ptr,  0

    EE.VMULAS.S8.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    # block one cycle here

    EE.VMULAS.S8.QACC                                       \input_v0, \filter_v0
.endm



.macro tie728_s8_depthwise_conv2d_33c1_load_args args filter_ptr dilation_x_offset dilation_y_offset next_33s1 c_div_x_1
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_33s1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16

    l32i \filter_ptr,        \args, 48
    l32i \dilation_x_offset, \args, 124
    l32i \dilation_y_offset, \args, 128
    l32i \next_33s1,         \args, 132
    l32i \c_div_x_1,         \args, 100

.endm



    .align 4
    .text
    .global dl_tie728_s8_depthwise_conv2d_33c1
    .type   dl_tie728_s8_depthwise_conv2d_33c1, @function
    # .section .iram1
dl_tie728_s8_depthwise_conv2d_33c1:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_33s1
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: bias_ptr
    # a12:
    # a13:
    # a14:
    # a15:
    tie728_s8_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9

    l32i a10, a4, 64  # mac shift
    l32i a11, a4, 68  # bias

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    blti a10, 0, dl_tie728_s8_depthwise_conv2d_per_channel_33c1


dl_tie728_s8_depthwise_conv2d_per_layer_33c1:

    beqz a11, tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias

    loopgtz a9, tie728_s8_depthwise_conv2d_per_layer_33c1_bias_loop
        EE.ZERO.QACC
        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_vector_round_result q3, a10, a15, q6
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_layer_33c1_bias_loop:

    EE.ZERO.QACC
    tie728_s8_conv2d_128b_vector_bias a11
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_vector_round_result q3, a10, a15, q6
    EE.VST.128.IP q3, a2, 16
    retw


    tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias:
    loopgtz a9, tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_vector_round_result q3, a10, a15, q6
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_vector_round_result q3, a10, a15, q6
    EE.VST.128.IP q3, a2, 16
    retw


dl_tie728_s8_depthwise_conv2d_per_channel_33c1:
    l32i a10, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias

    loopgtz a9, tie728_s8_depthwise_conv2d_per_channel_33c1_bias_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        # tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
        # tie728_s8_conv2d_bias q3, q4, a11
        tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a11, a15, q5
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_channel_33c1_bias_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    # tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
    # tie728_s8_conv2d_bias q3, q4, a11
    tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a11, a15, q5
    EE.VST.128.IP q3, a2, 16
    retw


    tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias:
    loopgtz a9, tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
    EE.VST.128.IP q3, a2, 16
    retw



    .align 4
    .text
    .global dl_tie728_s8_depthwise_conv2d_33c1_relu
    .type   dl_tie728_s8_depthwise_conv2d_33c1_relu, @function
    # .section .iram1
dl_tie728_s8_depthwise_conv2d_33c1_relu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_33s1
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15:
    tie728_s8_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9

    l32i a10, a4, 64  # mac shift
    l32i a11, a4, 68  # bias
    l32i a12, a4, 76  # activation_alpha
    l32i a13, a4, 84  # activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    blti a10, 0, dl_tie728_s8_depthwise_conv2d_per_channel_33c1_relu


dl_tie728_s8_depthwise_conv2d_per_layer_33c1_relu:

    beqz a11, tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_relu

    loopgtz a9, tie728_s8_depthwise_conv2d_per_layer_33c1_bias_relu_loop
        EE.ZERO.QACC
        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_vector_round_result q3, a10, a15, q6
        tie728_s8_conv2d_relu q3, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_layer_33c1_bias_relu_loop:

    EE.ZERO.QACC
    tie728_s8_conv2d_128b_vector_bias a11
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_vector_round_result q3, a10, a15, q6
    tie728_s8_conv2d_relu q3, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw


    tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_relu:
    loopgtz a9, tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_relu_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_vector_round_result q3, a10, a15, q6
        tie728_s8_conv2d_relu q3, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_relu_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_vector_round_result q3, a10, a15, q6
    tie728_s8_conv2d_relu q3, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw


dl_tie728_s8_depthwise_conv2d_per_channel_33c1_relu:
    l32i a10, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_relu

    loopgtz a9, tie728_s8_depthwise_conv2d_per_channel_33c1_bias_relu_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        # tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
        # tie728_s8_conv2d_bias_relu q3, q4, a11, a12, a13
        tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a11, a15, q5
        tie728_s8_conv2d_relu q3, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_channel_33c1_bias_relu_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    # tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
    # tie728_s8_conv2d_bias_relu q3, q4, a11, a12, a13
    tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a11, a15, q5
    tie728_s8_conv2d_relu q3, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw


    tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_relu:
    loopgtz a9, tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_relu_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
        tie728_s8_conv2d_relu q3, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_relu_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
    tie728_s8_conv2d_relu q3, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw



    .align 4
    .text
    .global dl_tie728_s8_depthwise_conv2d_33c1_prelu
    .type   dl_tie728_s8_depthwise_conv2d_33c1_prelu, @function
    # .section .iram1
dl_tie728_s8_depthwise_conv2d_33c1_prelu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_33s1
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15:
    tie728_s8_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9

    l32i a10, a4, 64  # mac shift
    l32i a11, a4, 68  # bias
    l32i a12, a4, 80  # activation_alpha_ptr
    l32i a13, a4, 84  # activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    blti a10, 0, dl_tie728_s8_depthwise_conv2d_per_channel_33c1_prelu


dl_tie728_s8_depthwise_conv2d_per_layer_33c1_prelu:

    beqz a11, tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_prelu

    loopgtz a9, tie728_s8_depthwise_conv2d_per_layer_33c1_bias_prelu_loop
        EE.ZERO.QACC
        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_vector_round_result q3, a10, a15, q6
        tie728_s8_conv2d_prelu q3, q5, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_layer_33c1_bias_prelu_loop:

    EE.ZERO.QACC
    tie728_s8_conv2d_128b_vector_bias a11
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_vector_round_result q3, a10, a15, q6
    tie728_s8_conv2d_prelu q3, q5, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw


    tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_prelu:
    loopgtz a9, tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_prelu_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_vector_round_result q3, a10, a15, q6
        tie728_s8_conv2d_prelu q3, q5, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_layer_33c1_no_bias_prelu_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_vector_round_result q3, a10, a15, q6
    tie728_s8_conv2d_prelu q3, q5, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw


dl_tie728_s8_depthwise_conv2d_per_channel_33c1_prelu:
    l32i a10, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_prelu

    loopgtz a9, tie728_s8_depthwise_conv2d_per_channel_33c1_bias_prelu_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        # tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
        # tie728_s8_conv2d_bias_prelu q3, q4, a11, q5, a12, a13
        tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a11, a15, q5
        tie728_s8_conv2d_prelu q3, q5, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_channel_33c1_bias_prelu_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    # tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
    # tie728_s8_conv2d_bias_prelu q3, q4, a11, q5, a12, a13
    tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a11, a15, q5
        tie728_s8_conv2d_prelu q3, q5, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw


    tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_prelu:
    loopgtz a9, tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_prelu_loop
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_33s1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
        tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
        tie728_s8_conv2d_prelu q3, q4, a12, a13
        EE.VST.128.IP q3, a2, 16
    tie728_s8_depthwise_conv2d_per_channel_33c1_no_bias_prelu_loop:

    EE.ZERO.QACC
    tie728_s8_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s8_conv2d_per_channel_result q3, q4, a10, a15, q5
    tie728_s8_conv2d_prelu q3, q4, a12, a13
    EE.VST.128.IP q3, a2, 16
    retw











.macro tie728_s8_unaligned_depthwise_conv2d_33s1 input_v0 input_v1 input_v2 input_back_aligned input_ptr filter_v0 filter_v1 filter_v2 filter_ptr dilation_x_offset dilation_y_offset next_33s1
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_33s1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16

    # EE.ZERO.QACC

    # EE.LD.128.USAR.IP \input_v2, \input_ptr, 16
    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v2, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.SRC.Q.LD.IP \input_v2, \input_ptr, 16, \input_v1, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v2, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.SRC.Q.LD.IP \input_v2, \input_ptr, 16, \input_v1, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \next_33s1
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v2, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.SRC.Q.LD.IP \input_v2, \input_ptr, 16, \input_v1, \input_back_aligned
.endm


.macro tie728_s8_unaligned_depthwise_conv2d_33s1_last input_v0 input_v1 input_v2 input_back_aligned input_ptr filter_v0 filter_v1 filter_ptr dilation_x_offset dilation_y_offset next_33s1
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)

    # EE.ZERO.QACC

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v2, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v2, \filter_v0
    EE.SRC.Q.LD.IP \input_v2, \input_ptr, 16, \input_v1, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v1
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v2, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v1, \filter_v0
    EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v1
    EE.SRC.Q.LD.IP \input_v2, \input_ptr, 16, \input_v1, \input_back_aligned


    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \next_33s1
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q \input_v2, \input_v2, \input_back_aligned

    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1

    EE.VMULAS.S8.QACC \input_v2, \filter_v0
.endm


.macro tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_front_aligned filter_back_aligned filter_ptr dilation_x_offset remainder_c
    EE.LD.128.USAR.XP \input_front_aligned, \input_ptr, \remainder_c
    # rur.sar_byte \input_sar #input sar
    # EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    # wur.sar_byte \input_sar #input sar
    EE.VLD.128.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned

    EE.LD.128.USAR.XP \filter_front_aligned, \filter_ptr, \remainder_c
    # rur.sar_byte \filter_sar #filter sar
    # EE.LD.128.USAR.IP \filter_back_aligned, \filter_ptr, 0
    # wur.sar_byte \filter_sar #filter sar
    EE.VLD.128.IP \filter_back_aligned, \filter_ptr, 0
    EE.SRC.Q \filter_v0, \filter_front_aligned, \filter_back_aligned

    EE.VMULAS.S8.QACC \input_v0, \filter_v0
.endm


.macro tie728_s8_depthwise_conv2d_unaligned_c_slice_updatey input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_front_aligned filter_back_aligned filter_ptr dilation_y_offset remainder_c
    # input remainder is in one 128bit then input_back_aligned = input_front_aligned
    EE.LD.128.USAR.XP \input_front_aligned, \input_ptr, \remainder_c
    # rur.sar_byte \input_sar #input sar
    # EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    # wur.sar_byte \input_sar #input sar
    EE.VLD.128.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned

    EE.LD.128.USAR.XP \filter_front_aligned, \filter_ptr, \remainder_c
    # rur.sar_byte \filter_sar #filter sar
    # EE.LD.128.USAR.IP \filter_back_aligned, \filter_ptr, 0
    # wur.sar_byte \filter_sar #filter sar
    EE.VLD.128.IP \filter_back_aligned, \filter_ptr, 0
    EE.SRC.Q \filter_v0, \filter_front_aligned, \filter_back_aligned

    EE.VMULAS.S8.QACC \input_v0, \filter_v0
.endm


.macro tie728_s8_depthwise_conv2d_33s1_c_remainder input_v0 input_front_aligned input_back_aligned filter_v0 filter_front_aligned filter_back_aligned input_ptr filter_ptr dilation_x_offset dilation_y_offset remainder_c
    # dilation_x_offset = input_channel_with_padding * dilation_x * remainder_c
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * remainder_c

    # EE.ZERO.QACC

    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatey \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_y_offset, \remainder_c

    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatey \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_y_offset, \remainder_c

    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
    tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c

    EE.LD.128.USAR.XP \input_front_aligned, \input_ptr, \remainder_c
    # rur.sar_byte \input_sar #input sar
    # EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 0
    # wur.sar_byte \input_sar #input sar
    EE.VLD.128.IP \input_back_aligned, \input_ptr, 0
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned

    EE.LD.128.USAR.XP \filter_front_aligned, \filter_ptr, \remainder_c
    # rur.sar_byte \filter_sar #filter sar
    # EE.LD.128.USAR.IP \filter_back_aligned, \filter_ptr, 0
    # wur.sar_byte \filter_sar #filter sar
    EE.VLD.128.IP \filter_back_aligned, \filter_ptr, 0
    EE.SRC.Q \filter_v0, \filter_front_aligned, \filter_back_aligned

    EE.VMULAS.S8.QACC \input_v0, \filter_v0

.endm





    .align 4
    .text
    .global dl_tie728_s8_unaligned_depthwise_conv2d_33c1
    .type   dl_tie728_s8_unaligned_depthwise_conv2d_33c1, @function
    # .section .iram1
dl_tie728_s8_unaligned_depthwise_conv2d_33c1:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_33s1 / remainder_c
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15: operation_type /
    tie728_s8_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9


    l32i a10, a4, 64  # mac_shift
    l32i a11, a4, 68  # bias
    #l32i a12, a4, 76  # activation_alpha
    l32i a13, a4, 84  # activation_shift
    l32i a12, a4, 80  # activation_alpha_ptr

    tie728_s8_unaligned_conv2d_operation_type a15, a10, a11, a13, a12, a4

    addi a6, a6, -16
    addi a7, a7, -16

    l32i a14, a4, 4
    blti a14, 16, tie728_s8_unaligned_depthwise_conv2d_33c1_c_loop_end # input_channel < 16

    EE.LD.128.USAR.IP q2, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.VLD.128.IP q4, a5, 16 # filter_v0
    EE.SRC.Q.QUP q0, q2, q3 # input_v0


    EE.LD.128.USAR.IP q2, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    addi a8, a8, -16
    EE.SRC.Q.QUP q1, q2, q3 # input_v1

    EE.LD.128.USAR.IP q2, a3, 16 # input_v2

    EE.LD.128.USAR.IP q6, a2, 0 #get output_ptr sar_byte
    rur.sar_byte a14

    beqi a14, 0, tie728_s8_unaligned_depthwise_conv2d_33c1_0
    beqi a14, 8, tie728_s8_unaligned_depthwise_conv2d_33c1_8


# output sar_byte != 0 && != 8
    beqi a9, 0, tie728_s8_unaligned_depthwise_conv2d_33c1_loop_last



    tie728_s8_unaligned_depthwise_conv2d_33c1_loop:
    # loopgtz a9, tie728_s8_unaligned_depthwise_conv2d_33c1_loop   # Internal error in istack_push_space
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a11, tie728_s8_unaligned_depthwise_conv2d_33c1_loop_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_unaligned_depthwise_conv2d_33c1_loop_no_preload_bias:

        tie728_s8_unaligned_depthwise_conv2d_33s1 q0, q1, q2, q3, a3, q4, q5, q6, a5, a6, a7, a8
        tie728_s8_conv2d_1_1_unaligned_c_result a15, q3, a10, a11, a12, a13, a14, q5, q6
        # tie728_s8_conv2d_per_layer_result q3, a10

        #store to unaligned address
        dl_tie728_s8_unaligned_store0 q3, a2, a14

        addi a9, a9, -1
    bnez a9, tie728_s8_unaligned_depthwise_conv2d_33c1_loop

tie728_s8_unaligned_depthwise_conv2d_33c1_loop_last:

    EE.ZERO.QACC

    # Without modifications specifically for per-channel, there may be issues with per-channel
    beqz a11, tie728_s8_unaligned_depthwise_conv2d_33c1_loop_last_no_preload_bias
        tie728_s8_conv2d_128b_vector_bias a11
    tie728_s8_unaligned_depthwise_conv2d_33c1_loop_last_no_preload_bias:

    tie728_s8_unaligned_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, q4, q5, a5, a6, a7, a8
    tie728_s8_conv2d_1_1_unaligned_c_result a15, q3, a10, a11, a12, a13, a14, q5, q6

    #store to unaligned address
    dl_tie728_s8_unaligned_store0 q3, a2, a14

    j tie728_s8_unaligned_depthwise_conv2d_33c1_c_loop_end


# output sar_byte == 0
tie728_s8_unaligned_depthwise_conv2d_33c1_0:

    beqi a9, 0, tie728_s8_unaligned_depthwise_conv2d_33c1_loop0_last

    tie728_s8_unaligned_depthwise_conv2d_33c1_loop0:
    # loopgtz a9, tie728_s8_unaligned_depthwise_conv2d_33c1_loop0
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a11, tie728_s8_unaligned_depthwise_conv2d_33c1_loop0_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_unaligned_depthwise_conv2d_33c1_loop0_no_preload_bias:

        tie728_s8_unaligned_depthwise_conv2d_33s1 q0, q1, q2, q3, a3, q4, q5, q6, a5, a6, a7, a8
        tie728_s8_conv2d_1_1_unaligned_c_result a15, q3, a10, a11, a12, a13, a14, q5, q6

        EE.VST.128.IP q3, a2, 16
    # tie728_s8_unaligned_depthwise_conv2d_33c1_loop0:
        addi a9, a9, -1
    bnez a9, tie728_s8_unaligned_depthwise_conv2d_33c1_loop0

tie728_s8_unaligned_depthwise_conv2d_33c1_loop0_last:
    EE.ZERO.QACC

    # Without modifications specifically for per-channel, there may be issues with per-channel
    beqz a11, tie728_s8_unaligned_depthwise_conv2d_33c1_loop0_last_no_preload_bias
        tie728_s8_conv2d_128b_vector_bias a11
    tie728_s8_unaligned_depthwise_conv2d_33c1_loop0_last_no_preload_bias:

    tie728_s8_unaligned_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, q4, q5, a5, a6, a7, a8
    tie728_s8_conv2d_1_1_unaligned_c_result a15, q3, a10, a11, a12, a13, a14, q5, q6
    EE.VST.128.IP q3, a2, 16

    j tie728_s8_unaligned_depthwise_conv2d_33c1_c_loop_end

# output sar_byte == 8
tie728_s8_unaligned_depthwise_conv2d_33c1_8:

    beqi a9, 0, tie728_s8_unaligned_depthwise_conv2d_33c1_loop8_last
    tie728_s8_unaligned_depthwise_conv2d_33c1_loop8:

    # loopgtz a9, tie728_s8_unaligned_depthwise_conv2d_33c1_loop8
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a11, tie728_s8_unaligned_depthwise_conv2d_33c1_loop8_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_unaligned_depthwise_conv2d_33c1_loop8_no_preload_bias:

        tie728_s8_unaligned_depthwise_conv2d_33s1 q0, q1, q2, q3, a3, q4, q5, q6, a5, a6, a7, a8
        tie728_s8_conv2d_1_1_unaligned_c_result a15, q3, a10, a11, a12, a13, a14, q5, q6

        #store to unaligned address
        dl_tie728_s8_unaligned_store1 q3, a2
        addi a9, a9, -1
    # tie728_s8_unaligned_depthwise_conv2d_33c1_loop8:
    bnez a9, tie728_s8_unaligned_depthwise_conv2d_33c1_loop8

tie728_s8_unaligned_depthwise_conv2d_33c1_loop8_last:
    EE.ZERO.QACC

    # Without modifications specifically for per-channel, there may be issues with per-channel
    beqz a11, tie728_s8_unaligned_depthwise_conv2d_33c1_loop8_last_no_preload_bias
        tie728_s8_conv2d_128b_vector_bias a11
    tie728_s8_unaligned_depthwise_conv2d_33c1_loop8_last_no_preload_bias:

    tie728_s8_unaligned_depthwise_conv2d_33s1_last q0, q1, q2, q3, a3, q4, q5, a5, a6, a7, a8
    tie728_s8_conv2d_1_1_unaligned_c_result a15, q3, a10, a11, a12, a13, a14, q5, q6
    dl_tie728_s8_unaligned_store1 q3, a2


tie728_s8_unaligned_depthwise_conv2d_33c1_c_loop_end:

    l32i a8, a4, 136  # c_remainder

    beqz a8, dl_tie728_s8_unaligned_depthwise_conv2d_33c1_end
    # mov a9, a15 #operation_type

    addi a6, a6, 16
    addi a7, a7, 16

    sub a6, a6, a8
    sub a7, a7, a8

    EE.ZERO.QACC

    # Without modifications specifically for per-channel, there may be issues with per-channel
    beqz a11, tie728_s8_unaligned_depthwise_conv2d_33c1_c_loop_end_no_preload_bias
        tie728_s8_conv2d_128b_vector_bias a11
    tie728_s8_unaligned_depthwise_conv2d_33c1_c_loop_end_no_preload_bias:

    tie728_s8_depthwise_conv2d_33s1_c_remainder q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
    tie728_s8_conv2d_1_1_unaligned_c_result a15, q3, a10, a11, a12, a13, a14, q4, q5

    # store low remainder_c part
    dl_tie728_s8_store_remainder q3, a10, a11, a12, a13, a2, a8

dl_tie728_s8_unaligned_depthwise_conv2d_33c1_end:
    retw






############################################################################################################################################################
####
#### tie728_s8_depthwise_conv2d_hwc1 series
####
############################################################################################################################################################


.macro tie728_s8_depthwise_conv2d_1ws1 input_v0 input_v1 input_v2 filter_v0 filter_v1 filter_v2 input_ptr filter_ptr dilation_x_offset dilation_y_offset filter_h filter_w filter_w_rs1_1 filter_y_offset
    loopgtz \filter_w_rs1_1, 1f
        EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

        EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
        EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
    1:
    bbci \filter_w, 0, 2f
    # three 8-input-element left
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S8.QACC.LD.XP \filter_v2, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2     # block one cyle here
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
    j 3f
2:  # two 8-input-element left
    EE.VMULAS.S8.QACC.LD.XP \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
    sub \input_ptr, \input_ptr, \dilation_x_offset
    add \input_ptr, \input_ptr, \dilation_y_offset
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1   # block one cyle here
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
3:
.endm




.macro tie728_s8_depthwise_conv2d_1ws1_last input_v0 input_v1 filter_v0 filter_v1 input_ptr filter_ptr dilation_x_offset dilation_y_offset filter_h filter_w filter_w_rs1_1 next_hws1 filter_y_offset
    loopgtz \filter_w_rs1_1, 4f
        EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

        EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
        EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
    4:
    bbci \filter_w, 0, 5f
    # three 8-input-element left
    EE.VMULAS.S8.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \next_hws1

    EE.VMULAS.S8.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
    # block one cyle here
    EE.VMULAS.S8.QACC                                    \input_v0, \filter_v0
    j 6f
5:  # two 8-input-element left
    EE.VMULAS.S8.QACC.LD.XP \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
    sub \input_ptr, \input_ptr, \dilation_x_offset
    add \input_ptr, \input_ptr, \next_hws1
    EE.VMULAS.S8.QACC                                    \input_v1, \filter_v1
6:

.endm



.macro tie728_s8_depthwise_conv2d_hws1 input_v0 input_v1 input_v2 filter_v0 filter_v1 filter_v2 input_ptr filter_ptr dilation_x_offset dilation_y_offset next_hws1 filter_h filter_w filter_w_rs1_1 args filter_offset_q filter_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_hws1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16
    # filter_w_rs1_1


    # EE.ZERO.QACC

    l32i \filter_h, \args, 52  # filter_height

    blti \filter_w, 2, 9f
    EE.VLD.128.IP \filter_v0, \filter_ptr, 16
    EE.VLD.128.XP \input_v0,  \input_ptr,  \dilation_x_offset
    EE.VLD.128.XP \input_v1,  \input_ptr,  \dilation_x_offset
    blti \filter_h, 2, 8f
    7:
        tie728_s8_depthwise_conv2d_1ws1 \input_v0, \input_v1, \input_v2, \filter_v0, \filter_v1, \filter_v2, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \filter_h, \filter_w, \filter_w_rs1_1, \filter_y_offset
        addi \filter_h, \filter_h, -1
    bgei \filter_h, 2, 7b
    8:  # last y
    tie728_s8_depthwise_conv2d_1ws1_last \input_v0, \input_v1, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \filter_h, \filter_w, \filter_w_rs1_1, \next_hws1, \filter_y_offset
    j 12f


    # filter_w == 1
    9:

    EE.VLD.128.XP \filter_v0, \filter_ptr, \filter_y_offset
    EE.VLD.128.XP \input_v0,  \input_ptr,  \dilation_y_offset
    blti \filter_h, 2, 11f
    10:
        EE.VMULAS.S8.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_y_offset
        addi \filter_h, \filter_h, -1
    bgei \filter_h, 2, 10b
    11: # last y
    EE.VMULAS.S8.QACC \input_v0, \filter_v0
    sub \input_ptr, \input_ptr, \dilation_y_offset
    add \input_ptr, \input_ptr, \next_hws1

12:
    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm




.macro tie728_s8_depthwise_conv2d_hwc1_load_args args filter_ptr dilation_x_offset dilation_y_offset next_hws1 c_div_x_1 filter_w filter_w_rs1_1
    tie728_s8_depthwise_conv2d_33c1_load_args \args, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \next_hws1, \c_div_x_1
    l32i \filter_w,         \args,  56
    l32i \filter_w_rs1_1,   \args, 148

.endm




    .align 4
    .text
    .global dl_tie728_s8_depthwise_conv2d_hwc1
    .type   dl_tie728_s8_depthwise_conv2d_hwc1, @function
    # .section .iram1
dl_tie728_s8_depthwise_conv2d_hwc1:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hws1
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: bias_ptr
    # a15:

    l32i a15, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a11, 2

    tie728_s8_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a12, a13

    l32i a10, a4, 64  # mac shift
    l32i a14, a4, 68  # bias

    blti a10, 0, dl_tie728_s8_depthwise_conv2d_per_channel_hwc1


dl_tie728_s8_depthwise_conv2d_per_layer_hwc1:

    beqz a14, tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias

    tie728_s8_depthwise_conv2d_per_layer_hwc1_bias_loop:
        EE.ZERO.QACC
        tie728_s8_conv2d_128b_vector_bias a14
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_vector_round_result q3, a10, a11, q6
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_layer_hwc1_bias_loop
    retw


    tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias:
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_vector_round_result q3, a10, a11, q6
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias
    retw


dl_tie728_s8_depthwise_conv2d_per_channel_hwc1:
    l32i a10, a4, 104 # filter_channel_factor address

    beqz a14, tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias

    tie728_s8_depthwise_conv2d_per_channel_hwc1_bias_loop:
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        # tie728_s8_conv2d_per_channel_result q3, q4, a10, a11, q5
        # tie728_s8_conv2d_bias q3, q4, a14
        tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a14, a11, q5
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_channel_hwc1_bias_loop
    retw


    tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias:
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_conv2d_per_channel_result q3, q4, a10, a11, q5
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias
    retw





    .align 4
    .text
    .global dl_tie728_s8_depthwise_conv2d_hwc1_relu
    .type   dl_tie728_s8_depthwise_conv2d_hwc1_relu, @function
    # .section .iram1
dl_tie728_s8_depthwise_conv2d_hwc1_relu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hws1
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: bias_ptr
    # a15: activation_alpha

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    tie728_s8_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a12, a13

    l32i a10, a4, 64  # mac shift
    l32i a15, a4, 76  # activation_alpha
    l32i a14, a4, 68  # bias

    EE.MOVI.32.Q q7, a15, 3

    blti a10, 0, dl_tie728_s8_depthwise_conv2d_per_channel_hwc1_relu


dl_tie728_s8_depthwise_conv2d_per_layer_hwc1_relu:

    beqz a14, tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_relu

    tie728_s8_depthwise_conv2d_per_layer_hwc1_bias_relu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_conv2d_128b_vector_bias a14
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_vector_round_result q3, a10, a11, q6
        l32i a11, a4, 84  # activation_shift
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_relu q3, a15, a11
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_layer_hwc1_bias_relu_loop
    retw


    tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_relu:
    l32i a14, a4, 84  # activation_shift
    tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_relu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_vector_round_result q3, a10, a11, q6
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_relu q3, a15, a14
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_relu_loop
    retw


dl_tie728_s8_depthwise_conv2d_per_channel_hwc1_relu:
    l32i a10, a4, 104 # filter_channel_factor address

    beqz a14, tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_relu

    tie728_s8_depthwise_conv2d_per_channel_hwc1_bias_relu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        # tie728_s8_conv2d_per_channel_result q3, q4, a10, a11, q5
        # l32i a11, a4, 84  # activation_shift
        # EE.MOVI.32.A q7, a15, 3
        # tie728_s8_conv2d_bias_relu q3, q4, a14, a15, a11
        tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a14, a11, q5
        l32i a11, a4, 84  # activation_shift
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_relu q3, a15, a11
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_channel_hwc1_bias_relu_loop
    retw

    tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_relu:
    l32i a14, a4, 84  # activation_shift
    tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_relu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_conv2d_per_channel_result q3, q4, a10, a11, q5
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_relu q3, a15, a14
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_relu_loop
    retw





    .align 4
    .text
    .global dl_tie728_s8_depthwise_conv2d_hwc1_prelu
    .type   dl_tie728_s8_depthwise_conv2d_hwc1_prelu, @function
    # .section .iram1
dl_tie728_s8_depthwise_conv2d_hwc1_prelu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hws1
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: bias_ptr
    # a15: activation_alpha_ptr

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    tie728_s8_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a12, a13

    l32i a10, a4, 64  # mac shift
    l32i a15, a4, 80  # activation_alpha_ptr
    l32i a14, a4, 68  # bias
    EE.MOVI.32.Q q7, a15, 3


    blti a10, 0, dl_tie728_s8_depthwise_conv2d_per_channel_hwc1_prelu


dl_tie728_s8_depthwise_conv2d_per_layer_hwc1_prelu:

    beqz a14, tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_prelu

    tie728_s8_depthwise_conv2d_per_layer_hwc1_bias_prelu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_conv2d_128b_vector_bias a14
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_vector_round_result q3, a10, a11, q6
        l32i a11, a4, 84  # activation_shift
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_prelu q3, q5, a15, a11
        EE.MOVI.32.Q q7, a15, 3
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_layer_hwc1_bias_prelu_loop
    retw


    tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_prelu:
    l32i a14, a4, 84  # activation_shift
    tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_prelu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_vector_round_result q3, a10, a11, q6
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_prelu q3, q5, a15, a14
        EE.MOVI.32.Q q7, a15, 3
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_layer_hwc1_no_bias_prelu_loop
    retw


dl_tie728_s8_depthwise_conv2d_per_channel_hwc1_prelu:
    l32i a10, a4, 104 # filter_channel_factor address

    beqz a14, tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_prelu

    tie728_s8_depthwise_conv2d_per_channel_hwc1_bias_prelu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        # tie728_s8_conv2d_per_channel_result q3, q4, a10, a11, q5
        tie728_s8_conv2d_per_channel_with_bias_result q3, q4, a10, a14, a11, q5
        l32i a11, a4, 84  # activation_shift
        EE.MOVI.32.A q7, a15, 3
        # tie728_s8_conv2d_bias_prelu q3, q4, a14, q5, a15, a11
        tie728_s8_conv2d_prelu q3, q5, a15, a11
        EE.MOVI.32.Q q7, a15, 3
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_channel_hwc1_bias_prelu_loop
    retw

    tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_prelu:
    l32i a14, a4, 84  # activation_shift
    tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_prelu_loop:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC
        tie728_s8_depthwise_conv2d_hws1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        tie728_s8_conv2d_per_channel_result q3, q4, a10, a11, q5
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_prelu q3, q5, a15, a14
        EE.MOVI.32.Q q7, a15, 3
        EE.VST.128.IP q3, a2, 16
        addi a9, a9, -1
    bgez a9, tie728_s8_depthwise_conv2d_per_channel_hwc1_no_bias_prelu_loop
    retw







############################################################################################################################################################
####
#### tie728_s8_unaligned_depthwise_conv2d_hwc1 series
####
############################################################################################################################################################

.macro tie728_s8_depthwise_conv2d_unaligned_c_slice_updatey_padding input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_front_aligned filter_back_aligned filter_ptr dilation_y_offset remainder_c filter_y_offset
    EE.LD.128.USAR.XP \input_front_aligned, \input_ptr, \remainder_c

    EE.VLD.128.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned

    EE.LD.128.USAR.XP \filter_front_aligned, \filter_ptr, \remainder_c

    EE.VLD.128.XP \filter_back_aligned, \filter_ptr, \filter_y_offset
    EE.SRC.Q \filter_v0, \filter_front_aligned, \filter_back_aligned

    EE.VMULAS.S8.QACC \input_v0, \filter_v0
.endm



.macro tie728_s8_unaligned_depthwise_conv2d_1ws1 input_v0 input_v1 input_back_aligned input_ptr filter_v0 filter_ptr dilation_x_offset dilation_y_offset filter_w filter_w_rs1_1 filter_y_offset

    loopgtz \filter_w_rs1_1, 1f
        EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
        EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back_aligned

        EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
        EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned
    1:
    bbci \filter_w, 0, 2f

    # three 8-input-element left
    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.VMULAS.S8.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v0
    EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q \input_v0, \input_v1, \input_back_aligned
    EE.LD.128.USAR.IP \input_v1, \input_ptr, 16

    j 3f
2:  # two 8-input-element left
    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.VMULAS.S8.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
    EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned
3:
.endm



.macro tie728_s8_unaligned_depthwise_conv2d_1ws1_last input_v0 input_v1 input_back_aligned input_ptr filter_v0 filter_ptr dilation_x_offset filter_w filter_w_rs1_1 next_hws1 filter_y_offset
    loopgtz \filter_w_rs1_1, 4f
        EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
        EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back_aligned

        EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
        EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back_aligned
    4:
    bbci \filter_w, 0, 5f

    # three 16byte left
    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VMULAS.S8.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
    EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back_aligned

    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \next_hws1
    EE.VMULAS.S8.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v0
    EE.SRC.Q \input_v0, \input_v0, \input_back_aligned

    EE.VMULAS.S8.QACC \input_v0, \filter_v0

    j 6f
5:  # two 16byte left
    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \next_hws1
    EE.VMULAS.S8.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
    EE.SRC.Q \input_v1, \input_v1, \input_back_aligned

    EE.VMULAS.S8.QACC                                    \input_v1, \filter_v0
6:

.endm




.macro tie728_s8_unaligned_depthwise_conv2d_hws1 input_v0 input_v1 input_back_aligned input_ptr filter_v0 filter_ptr dilation_x_offset dilation_y_offset next_hws1 filter_h filter_w filter_w_rs1_1 args filter_offset_q filter_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_hws1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16
    # filter_w_rs1_1


    # EE.ZERO.QACC

    l32i \filter_h, \args, 52  # filter_height


    blti \filter_w, 2, 9f

    EE.LD.128.USAR.IP \input_v1, \input_ptr, 16
    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_x_offset
    EE.VLD.128.IP \filter_v0, \filter_ptr, 16 # filter_v0
    EE.SRC.Q \input_v0, \input_v1, \input_back_aligned # input_v0

    EE.LD.128.USAR.IP \input_v1, \input_ptr, 16

    blti \filter_h, 2, 8f
    7:
        tie728_s8_unaligned_depthwise_conv2d_1ws1 \input_v0, \input_v1, \input_back_aligned, \input_ptr, \filter_v0, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \filter_w, \filter_w_rs1_1 \filter_y_offset
        addi \filter_h, \filter_h, -1
    bgei \filter_h, 2, 7b
    8:  # last y
    tie728_s8_unaligned_depthwise_conv2d_1ws1_last \input_v0, \input_v1, \input_back_aligned, \input_ptr, \filter_v0, \filter_ptr, \dilation_x_offset, \filter_w, \filter_w_rs1_1, \next_hws1 \filter_y_offset
    j 12f


    # filter_w == 1
    9:
    EE.LD.128.USAR.IP \input_v1, \input_ptr, 16
    EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
    EE.VLD.128.XP \filter_v0, \filter_ptr, \filter_y_offset # filter_v0
    EE.SRC.Q \input_v0, \input_v1, \input_back_aligned # input_v0


    blti \filter_h, 2, 11f
    addi \filter_h, \filter_h, -1
    loopgtz \filter_h, 10f
        EE.LD.128.USAR.IP \input_v1, \input_ptr, 16
        EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \dilation_y_offset
        EE.VMULAS.S8.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        EE.SRC.Q \input_v0, \input_v1, \input_back_aligned
    10:


    11: # last y
    EE.VMULAS.S8.QACC \input_v0, \filter_v0
    sub \input_ptr, \input_ptr, \dilation_y_offset
    add \input_ptr, \input_ptr, \next_hws1

12:
    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm




.macro tie728_s8_unaligned_depthwise_conv2d_hws1_c_remainder input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_front_aligned filter_back_aligned filter_ptr dilation_x_offset dilation_y_offset filter_h filter_w filter_w_rs1_1 remainder_c args filter_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * remainder_c
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * remainder_c

    # EE.ZERO.QACC

    l32i \filter_h, \args, 52 # filter_height

    blti \filter_w, 2, 5f

    4:
        loopgtz \filter_w_rs1_1, 1f
            tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
            tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
        1:
        bbci \filter_w, 0, 2f
        # 3 left
        tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
        tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
        tie728_s8_depthwise_conv2d_unaligned_c_slice_updatey_padding \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_y_offset, \remainder_c, \filter_y_offset
        j 3f
        2:
        # 2 left
        tie728_s8_depthwise_conv2d_unaligned_c_slice_updatex \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_x_offset, \remainder_c
        tie728_s8_depthwise_conv2d_unaligned_c_slice_updatey_padding \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_y_offset, \remainder_c, \filter_y_offset
        3:
        addi \filter_h, \filter_h, -1
    bgei \filter_h, 1, 4b

    j 7f


    5:
    # filter_w == 1
    loopgtz \filter_h, 6f
        tie728_s8_depthwise_conv2d_unaligned_c_slice_updatey_padding \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \dilation_y_offset, \remainder_c, \filter_y_offset
    6:


7:

.endm



    .align 4
    .text
    .global dl_tie728_s8_unaligned_depthwise_conv2d_hwc1
    .type   dl_tie728_s8_unaligned_depthwise_conv2d_hwc1, @function
    # .section .iram1
dl_tie728_s8_unaligned_depthwise_conv2d_hwc1:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hws1
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: bias_ptr
    # a15: activation_alpha_ptr

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2


    l32i a10, a4, 64  # mac shift
    l32i a14, a4, 68  # bias
    # l32i a12, a4, 76  # activation_alpha
    l32i a11, a4, 84  # activation_shift
    l32i a15, a4, 80  # activation_alpha_ptr

    tie728_s8_unaligned_conv2d_operation_type a12, a10, a14, a11, a15, a4
    EE.MOVI.32.Q q7, a12, 0 #operation type
    EE.MOVI.32.Q q7, a15, 3

    tie728_s8_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a12, a13

    addi a6, a6, -16
    addi a7, a7, -16
    addi a8, a8, -16



    # EE.LD.128.USAR.IP q5, a2, 0 #get output_ptr sar_byte
    # rur.sar_byte a11

    # beqi a9, 0, tie728_s8_unaligned_depthwise_conv2d_hwc1_loop_last

    l32i a11, a4, 4 #input_channel
    blti a11, 16, tie728_s8_unaligned_depthwise_conv2d_hwc1_c_loop_end #input_channel < 16

    l32i a11, a4, 84  # activation_shift
    blti a11, 0, tie728_s8_unaligned_depthwise_conv2d_hwc1_no_activation

tie728_s8_unaligned_depthwise_conv2d_hwc1_with_activation:
    tie728_s8_unaligned_depthwise_conv2d_hwc1_loop:
        l32i a13, a4, 148 # filter_w_rs1_1
        l32i a12, a4, 56 # filter_w
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a14, tie728_s8_unaligned_depthwise_conv2d_hwc1_loop_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a14
        tie728_s8_unaligned_depthwise_conv2d_hwc1_loop_no_preload_bias:
        tie728_s8_unaligned_depthwise_conv2d_hws1 q0, q1, q2, a3, q3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15

        EE.MOVI.32.A q7, a13, 0 #operation type
        l32i a12, a4, 84  # activation_shift
        EE.MOVI.32.A q7, a15, 3
        tie728_s8_conv2d_1_1_unaligned_c_result a13, q4, a10, a14, a15, a12, a11, q5, q6
        EE.MOVI.32.Q q7, a15, 3
        #store to unaligned address
        dl_tie728_s8_unaligned_store0 q4, a2, a11

        addi a9, a9, -1
    bgez a9, tie728_s8_unaligned_depthwise_conv2d_hwc1_loop

    j tie728_s8_unaligned_depthwise_conv2d_hwc1_c_loop_end


tie728_s8_unaligned_depthwise_conv2d_hwc1_no_activation:
    # EE.MOVI.32.A q7, a15, 0 #operation type

    tie728_s8_unaligned_depthwise_conv2d_hwc1_loop_no_act:
        EE.MOVI.32.A q7, a15, 1
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a14, tie728_s8_unaligned_depthwise_conv2d_hwc1_loop_no_act_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a14
        tie728_s8_unaligned_depthwise_conv2d_hwc1_loop_no_act_no_preload_bias:
        tie728_s8_unaligned_depthwise_conv2d_hws1 q0, q1, q2, a3, q3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
        EE.MOVI.32.A q7, a15, 0
        tie728_s8_conv2d_1_1_unaligned_c_result a15, q4, a10, a14, a11, a11, a11, q5, q6
        #store to unaligned address
        dl_tie728_s8_unaligned_store0 q4, a2, a11

        addi a9, a9, -1
    bgez a9, tie728_s8_unaligned_depthwise_conv2d_hwc1_loop_no_act

    j tie728_s8_unaligned_depthwise_conv2d_hwc1_c_loop_end



tie728_s8_unaligned_depthwise_conv2d_hwc1_c_loop_end:
    l32i a8, a4, 136  # c_remainder

    beqz a8, dl_tie728_s8_unaligned_depthwise_conv2d_hwc1_end

    l32i a12, a4, 160
    l32i a5, a4,  168 # filter_ptr unaligned
    EE.MOVI.32.Q q7, a12, 1

    addi a6, a6, 16
    addi a7, a7, 16
    sub a6, a6, a8
    sub a7, a7, a8

    EE.MOVI.32.A q7, a9, 0 # operation type


    l32i a13, a4, 148 # filter_w_rs1_1
    l32i a12, a4, 56 # filter_w
    EE.MOVI.32.A q7, a15, 1
    EE.ZERO.QACC

    # Without modifications specifically for per-channel, there may be issues with per-channel
    beqz a14, tie728_s8_unaligned_depthwise_conv2d_hwc1_c_loop_end_no_preload_bias
        tie728_s8_conv2d_128b_vector_bias a14
    tie728_s8_unaligned_depthwise_conv2d_hwc1_c_loop_end_no_preload_bias:
    tie728_s8_unaligned_depthwise_conv2d_hws1_c_remainder q0, q1, q2, a3, q3, q4, q5, a5, a6, a7, a11, a12, a13, a8, a4, a15

    l32i a12, a4, 84  # activation_shift
    EE.MOVI.32.A q7, a15, 3
    tie728_s8_conv2d_1_1_unaligned_c_result a9, q3, a10, a14, a15, a12, a11, q4, q5

    # store low remainder_c part
    dl_tie728_s8_store_remainder q3, a10, a11, a12, a13, a2, a8

dl_tie728_s8_unaligned_depthwise_conv2d_hwc1_end:
    # addi a2, a2, -16

    retw
