#include "dl_tie728_s16.S"


############################################################################################################################################################
####
#### tie728_s16_depthwise_conv2d_33c1 series
####
############################################################################################################################################################
.macro tie728_s16_depthwise_conv2d_3381 input_v0 filter_v0 input_v1 filter_v1 input_v2 filter_v2 input_ptr filter_ptr dilation_x_offset dilation_y_offset next_hw81
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_hw81 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16

    # EE.ZERO.QACC

    EE.VMULAS.S16.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \next_hw81

    EE.VMULAS.S16.QACC.LD.IP \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
.endm



.macro tie728_s16_depthwise_conv2d_3381_last input_v0 filter_v0 input_v1 filter_v1 input_ptr filter_ptr dilation_x_offset dilation_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)

    # EE.ZERO.QACC

    EE.VMULAS.S16.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP               \input_v0,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S16.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP               \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP               \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP               \input_v1,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S16.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP               \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP               \input_v1,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.IP               \input_v0,  \input_ptr,  0

    EE.VMULAS.S16.QACC.LD.IP    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    # block one cycle here

    EE.VMULAS.S16.QACC                                       \input_v0, \filter_v0
.endm



.macro tie728_s16_depthwise_conv2d_33c1_load_args args filter_ptr dilation_x_offset dilation_y_offset next_hw81 c_div_x_1 mac_shift
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_hw81 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16

    l32i \filter_ptr,        \args, 48
    l32i \dilation_x_offset, \args, 124
    l32i \dilation_y_offset, \args, 128
    l32i \next_hw81,         \args, 132
    l32i \c_div_x_1,         \args, 100
    l32i \mac_shift,         \args, 64
.endm



    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_33c1_bias
    .type	dl_tie728_s16_depthwise_conv2d_33c1_bias, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_33c1_bias:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: bias_ptr
    # a12:
    # a13:
    # a14:
    # a15:
    tie728_s16_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a11, a4, 68  // bias_ptr
    # l32i a12, a4, 76  // activation_alpha
    # l32i a13, a4, 84  // activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    loopgtz a9, 1f
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
    tie728_s16_vector_round_result  q3, a10, a15, q6
    EE.VST.128.IP  q3, a2, 16
1:
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_depthwise_conv2d_3381_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s16_vector_round_result  q3, a10, a15, q6
    EE.VST.128.IP  q3, a2, 16

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_33c1_bias_relu
    .type	dl_tie728_s16_depthwise_conv2d_33c1_bias_relu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_33c1_bias_relu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: bias_ptr
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15:
    tie728_s16_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a11, a4, 68  // bias_ptr
    l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    loopgtz a9, 1f
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_relu  q3, a12, a13
    EE.VST.128.IP  q3, a2, 16
1:
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_depthwise_conv2d_3381_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_relu  q3, a12, a13
    EE.VST.128.IP  q3, a2, 16

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_33c1_bias_prelu
    .type	dl_tie728_s16_depthwise_conv2d_33c1_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_33c1_bias_prelu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: bias_ptr
    # a12: activation_alpha_ptr
    # a13: activation_shift
    # a14:
    # a15:
    tie728_s16_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9, a10

    l32i a11, a4, 68  // bias_ptr
    l32i a12, a4, 80  // activation_alpha_ptr
    l32i a13, a4, 84  // activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    loopgtz a9, 1f
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_prelu  q3, q5, a12, a13
    EE.VST.128.IP  q3, a2, 16
1:
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a11
    tie728_s16_depthwise_conv2d_3381_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_prelu  q3, q5, a12, a13
    EE.VST.128.IP  q3, a2, 16

    retw



    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_33c1
    .type	dl_tie728_s16_depthwise_conv2d_33c1, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_33c1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11:
    # a12:
    # a13:
    # a14:
    # a15:
    tie728_s16_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9, a10

    # l32i a11, a4, 68  // bias_ptr
    # l32i a12, a4, 76  // activation_alpha
    # l32i a13, a4, 84  // activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    loopgtz a9, 1f
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
    tie728_s16_vector_round_result  q3, a10, a15, q6
    EE.VST.128.IP  q3, a2, 16
1:
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_3381_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s16_vector_round_result  q3, a10, a15, q6
    EE.VST.128.IP  q3, a2, 16

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_33c1_relu
    .type	dl_tie728_s16_depthwise_conv2d_33c1_relu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_33c1_relu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11:
    # a12: activation_alpha/_address
    # a13: activation_shift
    # a14:
    # a15:
    tie728_s16_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9, a10

    # l32i a11, a4, 68  // bias_ptr
    l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    loopgtz a9, 1f
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_relu  q3, a12, a13
    EE.VST.128.IP  q3, a2, 16
1:
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_3381_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_relu  q3, a12, a13
    EE.VST.128.IP  q3, a2, 16

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_33c1_prelu
    .type	dl_tie728_s16_depthwise_conv2d_33c1_prelu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_33c1_prelu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11:
    # a12: activation_alpha_ptr
    # a13: activation_shift
    # a14:
    # a15:
    tie728_s16_depthwise_conv2d_33c1_load_args a4, a5, a6, a7, a8, a9, a10

    # l32i a11, a4, 68  // bias_ptr
    l32i a12, a4, 80  // activation_alpha_ptr
    l32i a13, a4, 84  // activation_shift

    EE.VLD.128.XP q0, a3, a6
    EE.VLD.128.IP q1, a5, 16
    EE.VLD.128.XP q2, a3, a6

    loopgtz a9, 1f
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_prelu  q3, q4, a12, a13
    EE.VST.128.IP  q3, a2, 16
1:
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_3381_last q0, q1, q2, q3, a3, a5, a6, a7
    tie728_s16_vector_round_result  q3, a10, a15, q6
    tie728_s16_conv2d_prelu  q3, q4, a12, a13
    EE.VST.128.IP  q3, a2, 16

    retw




############################################################################################################################################################
####
#### tie728_s16_depthwise_conv2d_hwcn series
####
############################################################################################################################################################
.macro tie728_s16_depthwise_conv2d_1w81 input_v0 input_v1 input_v2 filter_v0 filter_v1 filter_v2 input_ptr filter_ptr dilation_x_offset dilation_y_offset filter_h filter_w filter_w_rs1_1 filter_y_offset
    loopgtz \filter_w_rs1_1, 1f
    EE.VMULAS.S16.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
1:
    bbci \filter_w, 0, 2f
    # three 8-input-element left
    EE.VMULAS.S16.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v2,  \input_ptr,  \dilation_y_offset

    EE.VMULAS.S16.QACC.LD.XP \filter_v2, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2     # block one cyle here
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
    j 3f
2:  # two 8-input-element left
    EE.VMULAS.S16.QACC.LD.XP \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
    sub \input_ptr, \input_ptr, \dilation_x_offset
    add \input_ptr, \input_ptr, \dilation_y_offset
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1   # block one cyle here
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
3:
.endm




.macro tie728_s16_depthwise_conv2d_1w81_last input_v0 input_v1 filter_v0 filter_v1 input_ptr filter_ptr dilation_x_offset dilation_y_offset filter_h filter_w filter_w_rs1_1 next_hw81 filter_y_offset
    loopgtz \filter_w_rs1_1, 4f
    EE.VMULAS.S16.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_x_offset

    EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    EE.VLD.128.XP            \input_v1,  \input_ptr,  \dilation_x_offset
4:
    bbci \filter_w, 0, 5f
    # three 8-input-element left
    EE.VMULAS.S16.QACC.LD.IP \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    EE.VLD.128.XP            \input_v0,  \input_ptr,  \next_hw81

    EE.VMULAS.S16.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
    # block one cyle here
    EE.VMULAS.S16.QACC                                    \input_v0, \filter_v0
    j 6f
5:  # two 8-input-element left
    EE.VMULAS.S16.QACC.LD.XP \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
    sub \input_ptr, \input_ptr, \dilation_x_offset
    add \input_ptr, \input_ptr, \next_hw81
    EE.VMULAS.S16.QACC                                    \input_v1, \filter_v1
6:
.endm



.macro tie728_s16_depthwise_conv2d_hw81 input_v0 input_v1 input_v2 filter_v0 filter_v1 filter_v2 input_ptr filter_ptr dilation_x_offset dilation_y_offset next_hw81 filter_h filter_w filter_w_rs1_1 args filter_offset_q filter_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_hw81 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16
    # filter_w_rs1_1


    # EE.ZERO.QACC

    l32i \filter_h, \args, 52  # filter_height

    blti \filter_w, 2, 9f
    EE.VLD.128.IP \filter_v0, \filter_ptr, 16
    EE.VLD.128.XP \input_v0,  \input_ptr,  \dilation_x_offset
    EE.VLD.128.XP \input_v1,  \input_ptr,  \dilation_x_offset
    blti \filter_h, 2, 8f
7:
    tie728_s16_depthwise_conv2d_1w81 \input_v0, \input_v1, \input_v2, \filter_v0, \filter_v1, \filter_v2, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \filter_h, \filter_w, \filter_w_rs1_1, \filter_y_offset
    addi \filter_h, \filter_h, -1
    bgei \filter_h, 2, 7b

8:  # last y
    tie728_s16_depthwise_conv2d_1w81_last \input_v0, \input_v1, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \filter_h, \filter_w, \filter_w_rs1_1, \next_hw81, \filter_y_offset
    j 12f

# filter_w == 1
9:
    EE.VLD.128.XP \filter_v0, \filter_ptr, \filter_y_offset
    EE.VLD.128.XP \input_v0,  \input_ptr,  \dilation_y_offset
    blti \filter_h, 2, 11f
    10:
        EE.VMULAS.S16.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        EE.VLD.128.XP            \input_v0,  \input_ptr,  \dilation_y_offset
        addi \filter_h, \filter_h, -1
    bgei \filter_h, 2, 10b
    11: # last y
    EE.VMULAS.S16.QACC \input_v0, \filter_v0
    sub \input_ptr, \input_ptr, \dilation_y_offset
    add \input_ptr, \input_ptr, \next_hw81

12:
    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm




.macro tie728_s16_depthwise_conv2d_hwc1_load_args args filter_ptr dilation_x_offset dilation_y_offset next_hw81 c_div_x_1 mac_shift filter_w filter_w_rs1_1
    tie728_s16_depthwise_conv2d_33c1_load_args \args, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \next_hw81, \c_div_x_1, \mac_shift
    l32i \filter_w,         \args,  56
    l32i \filter_w_rs1_1,   \args, 148
.endm




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_hwc1_bias
    .type	dl_tie728_s16_depthwise_conv2d_hwc1_bias, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_hwc1_bias:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h, bias_ptr
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: bias_ptr
    # a15:

    l32i a11, a4, 144
    l32i a15, a4, 60
    EE.MOVI.32.Q q7, a11, 2

    tie728_s16_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a10, a12, a13

    l32i a14, a4, 68  // bias_ptr
    # l32i a14, a4,  80  // activation_alpha_ptr
    # l32i a15, a4,  84  // activation_shift

tie728_s16_depthwise_conv2d_hwc1_bias_loop:
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a14
    tie728_s16_depthwise_conv2d_hw81 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
    tie728_s16_vector_round_result  q0, a10, a11, q6
    EE.VST.128.IP  q0, a2, 16

    addi a9, a9, -1
    bgez a9, tie728_s16_depthwise_conv2d_hwc1_bias_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu
    .type	dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_hwc1_bias_relu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h, activation_shift
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: bias_ptr
    # a15: activation_alpha

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    tie728_s16_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a10, a12, a13

    l32i a15, a4, 76  // activation_alpha
    l32i a14, a4, 68  // bias_ptr

    EE.MOVI.32.Q q7, a10, 3

tie728_s16_depthwise_conv2d_hwc1_bias_relu_loop:
    EE.MOVI.32.A q7, a10, 1
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a14
    tie728_s16_depthwise_conv2d_hw81 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a10
    EE.MOVI.32.A q7, a10, 3
    tie728_s16_vector_round_result  q0, a10, a11, q6
    l32i a11, a4,  84  // activation_shift
    tie728_s16_conv2d_relu  q0, a15, a11
    EE.VST.128.IP  q0, a2, 16

    addi a9, a9, -1
    bgez a9, tie728_s16_depthwise_conv2d_hwc1_bias_relu_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_hwc1_bias_prelu
    .type	dl_tie728_s16_depthwise_conv2d_hwc1_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_hwc1_bias_prelu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h, activation_shift
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: bias_ptr
    # a15: activation_alpha_ptr

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    tie728_s16_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a10, a12, a13

    l32i a15, a4, 80  // activation_alpha_ptr
    l32i a14, a4, 68  // bias_ptr
    EE.MOVI.32.Q q7, a10, 3

tie728_s16_depthwise_conv2d_hwc1_bias_prelu_loop:
    EE.MOVI.32.A q7, a10, 1
    EE.ZERO.QACC
    tie728_s16_conv2d_128b_vector_bias  a14
    tie728_s16_depthwise_conv2d_hw81 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a10
    EE.MOVI.32.A q7, a10, 3
    tie728_s16_vector_round_result  q0, a10, a11, q6
    l32i a11, a4,  84  // activation_shift
    tie728_s16_conv2d_prelu  q0, q2, a15, a11
    EE.VST.128.IP  q0, a2, 16

    addi a9, a9, -1
    bgez a9, tie728_s16_depthwise_conv2d_hwc1_bias_prelu_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_hwc1
    .type	dl_tie728_s16_depthwise_conv2d_hwc1, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_hwc1:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14:
    # a15:

    l32i a11, a4, 144
    l32i a15, a4, 60
    EE.MOVI.32.Q q7, a11, 2

    tie728_s16_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a10, a12, a13

    # l32i a14, a4,  80  // activation_alpha_ptr
    # l32i a15, a4,  84  // activation_shift

tie728_s16_depthwise_conv2d_hwc1_loop:
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_hw81 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a15
    tie728_s16_vector_round_result  q0, a10, a11, q6
    EE.VST.128.IP  q0, a2, 16

    addi a9, a9, -1
    bgez a9, tie728_s16_depthwise_conv2d_hwc1_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_hwc1_relu
    .type	dl_tie728_s16_depthwise_conv2d_hwc1_relu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_hwc1_relu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: activation_alpha
    # a15: activation_shift

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    tie728_s16_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a10, a12, a13

    l32i a14, a4,  76  // activation_alpha
    l32i a15, a4,  84  // activation_shift

    EE.MOVI.32.Q q7, a10, 3

tie728_s16_depthwise_conv2d_hwc1_relu_loop:
    EE.MOVI.32.A q7, a10, 1
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_hw81 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a10
    EE.MOVI.32.A q7, a10, 3
    tie728_s16_vector_round_result  q0, a10, a11, q6
    tie728_s16_conv2d_relu  q0, a14, a15
    EE.VST.128.IP  q0, a2, 16

    addi a9, a9, -1
    bgez a9, tie728_s16_depthwise_conv2d_hwc1_relu_loop

    retw




    .align 4
    .text
    .global dl_tie728_s16_depthwise_conv2d_hwc1_prelu
    .type	dl_tie728_s16_depthwise_conv2d_hwc1_prelu, @function
    # .section .iram1
dl_tie728_s16_depthwise_conv2d_hwc1_prelu:
    .align      4
    entry       sp,     16
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    # a5: int16_t *filter_ptr
    # a6: input dilation x offset
    # a7: input dilation y offset
    # a8: next_hw81
    # a9: c_div_x_1
    # a10: mac_shift
    # a11: filter_h, bias_ptr
    # a12: filter_w
    # a13: filter_w_rs1_1
    # a14: activation_alpha_ptr
    # a15: activation_shift

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    tie728_s16_depthwise_conv2d_hwc1_load_args a4, a5, a6, a7, a8, a9, a10, a12, a13

    l32i a14, a4,  80  // activation_alpha_ptr
    l32i a15, a4,  84  // activation_shift

    EE.MOVI.32.Q q7, a10, 3

tie728_s16_depthwise_conv2d_hwc1_prelu_loop:
    EE.MOVI.32.A q7, a10, 1
    EE.ZERO.QACC
    tie728_s16_depthwise_conv2d_hw81 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8, a11, a12, a13, a4, q7, a10
    EE.MOVI.32.A q7, a10, 3
    tie728_s16_vector_round_result  q0, a10, a11, q6
    tie728_s16_conv2d_prelu  q0, q1, a14, a15
    EE.VST.128.IP  q0, a2, 16

    addi a9, a9, -1
    bgez a9, tie728_s16_depthwise_conv2d_hwc1_prelu_loop

    retw
