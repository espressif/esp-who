#include "dl_esp32p4_s16.S"
#include "dl_esp32p4_common.S"

############################################################################################################################################################
####
#### esp32p4_s16_depthwise_conv2d_33c1 series
####
############################################################################################################################################################
.macro esp32p4_s16_depthwise_conv2d_3381  input_v0, filter_v0, input_v1, filter_v1, input_v2, filter_v2, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset, next_hw81
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(t)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(t)
    # next_hw81 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(t) + 16

    esp.vmulas.s16.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp             \input_v2,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp             \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    esp.vld.128.xp             \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp             \input_v2,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp             \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    esp.vld.128.xp             \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp             \input_v2,  \input_ptr,  \next_hw81

    esp.vmulas.s16.qacc.ld.ip  \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp             \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    esp.vld.128.xp             \input_v1,  \input_ptr,  \dilation_x_offset
.endm



.macro esp32p4_s16_depthwise_conv2d_3381_last  input_v0, filter_v0, input_v1, filter_v1, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(t)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(t)

    esp.vmulas.s16.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp             \input_v0,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp             \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp             \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp             \input_v1,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp             \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp             \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s16.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.ip             \input_v0,  \input_ptr,  0

    esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    # block one cycle here
    esp.vmulas.s16.qacc                                     \input_v0, \filter_v0
.endm



.macro esp32p4_s16_depthwise_conv2d_33c1_load_args  args, filter_ptr, dilation_x_offset, dilation_y_offset, next_hw81, c_div_x_1, mac_shift
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(t)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(t)
    # next_hw81 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(t) + 16

    lw  \filter_ptr,        48 (\args)
    lw  \dilation_x_offset, 124(\args)
    lw  \dilation_y_offset, 128(\args)
    lw  \next_hw81,         132(\args)
    lw  \c_div_x_1,         100(\args)
    lw  \mac_shift,         64 (\args)
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_33c1_bias
    .type	dl_esp32p4_s16_depthwise_conv2d_33c1_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_33c1_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: c_div_x_1
    # t6: bias_ptr

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions):
    # t1(not for extension instructions):
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_s16_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4
    lw  t6, 68(a2)      // bias

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t5, esp32p4_s16_depthwise_conv2d_33c1_bias_loop_last
    // esp.lp.setup  0, t5, 1f
    1:
        esp.zero.qacc
        esp32p4_s16_conv2d_128b_vector_bias  t6
        esp32p4_s16_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp.vst.128.ip  q3, a0, 16
        addi  t5, t5, -1
        bgtz  t5, 1b

    esp32p4_s16_depthwise_conv2d_33c1_bias_loop_last:
        esp.zero.qacc
        esp32p4_s16_conv2d_128b_vector_bias  t6
        esp32p4_s16_depthwise_conv2d_3381_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp.vst.128.ip  q3, a0, 16

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_33c1_bias_relu
    .type	dl_esp32p4_s16_depthwise_conv2d_33c1_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_33c1_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: c_div_x_1
    # t6: bias_ptr

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions):
    # t1(not for extension instructions):
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_8_stacks_2r  s0, s1

    esp32p4_s16_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4
    lw  t6, 68(a2)      // bias
    lw  s0, 76(a2)      // activation_alpha
    lw  s1, 84(a2)      // activation_shift

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t5, esp32p4_s16_depthwise_conv2d_33c1_bias_relu_loop_last
    // esp.lp.setup  0, t5, 1f
    1:
        esp.zero.qacc
        esp32p4_s16_conv2d_128b_vector_bias  t6
        esp32p4_s16_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp32p4_s16_128b_vector_relu  q3, s0, s1
        esp.vst.128.ip  q3, a0, 16
        addi  t5, t5, -1
        bgtz  t5, 1b

    esp32p4_s16_depthwise_conv2d_33c1_bias_relu_loop_last:
        esp.zero.qacc
        esp32p4_s16_conv2d_128b_vector_bias  t6
        esp32p4_s16_depthwise_conv2d_3381_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp32p4_s16_128b_vector_relu  q3, s0, s1
        esp.vst.128.ip  q3, a0, 16

    esp32p4_pop_8_stacks_2r  s0, s1
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_33c1
    .type	dl_esp32p4_s16_depthwise_conv2d_33c1, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_33c1:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: c_div_x_1
    # t6:

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions):
    # t1(not for extension instructions):
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_s16_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t5, esp32p4_s16_depthwise_conv2d_33c1_loop_last
    // esp.lp.setup  0, t5, 1f
    1:
        esp.zero.qacc
        esp32p4_s16_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp.vst.128.ip  q3, a0, 16
        addi  t5, t5, -1
        bgtz  t5, 1b

    esp32p4_s16_depthwise_conv2d_33c1_loop_last:
        esp.zero.qacc
        esp32p4_s16_depthwise_conv2d_3381_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp.vst.128.ip  q3, a0, 16

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_33c1_relu
    .type	dl_esp32p4_s16_depthwise_conv2d_33c1_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_33c1_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: c_div_x_1
    # t6:

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions):
    # t1(not for extension instructions):
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_8_stacks_2r  s0, s1

    esp32p4_s16_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4
    lw  s0, 76(a2)       // activation_alpha
    lw  s1, 84(a2)       // activation_shift

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t5, esp32p4_s16_depthwise_conv2d_33c1_relu_loop_last
    // esp.lp.setup  0, t5, 1f
    1:
        esp.zero.qacc
        esp32p4_s16_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp32p4_s16_128b_vector_relu  q3, s0, s1
        esp.vst.128.ip  q3, a0, 16
        addi  t5, t5, -1
        bgtz  t5, 1b

    esp32p4_s16_depthwise_conv2d_33c1_relu_loop_last:
        esp.zero.qacc
        esp32p4_s16_depthwise_conv2d_3381_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s16_128b_vector_shift_result  q3, t4
        esp32p4_s16_128b_vector_relu  q3, s0, s1
        esp.vst.128.ip  q3, a0, 16

    esp32p4_pop_8_stacks_2r  s0, s1
    ret






############################################################################################################################################################
####
#### esp32p4_s16_depthwise_conv2d_hwcn series
####
############################################################################################################################################################
.macro esp32p4_s16_depthwise_conv2d_1w81  input_v0, input_v1, input_v2, filter_v0, filter_v1, filter_v2, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset, filter_w, filter_w_rs1_1, filter_y_offset, tmp
    beqz  \filter_w_rs1_1, 2f
    // esp.lp.setup  0, \filter_w_rs1_1, (1f - 4)
    mv  \tmp, \filter_w_rs1_1
    1:
        esp.vmulas.s16.qacc.ld.ip \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s16.qacc.ld.ip \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
        esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset
        addi  \tmp, \tmp, -1
        bgtz  \tmp, 1b
    2:
    andi  \tmp, \filter_w, 0xfffffffe
    beq  \filter_w, \tmp, 3f
        # three 8-input-element left
        esp.vmulas.s16.qacc.ld.ip \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v2,  \input_ptr,  \dilation_y_offset

        esp.vmulas.s16.qacc.ld.xp \filter_v2, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s16.qacc.ld.ip \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2 # block one cyle here
        esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset
        j 4f
    3:  # two 8-input-element left
        esp.vmulas.s16.qacc.ld.xp \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        sub \input_ptr, \input_ptr, \dilation_x_offset
        add \input_ptr, \input_ptr, \dilation_y_offset
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s16.qacc.ld.ip \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1 # block one cyle here
        esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset
    4:
.endm



.macro esp32p4_s16_depthwise_conv2d_1w81_last  input_v0, input_v1, filter_v0, filter_v1, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset, filter_w, filter_w_rs1_1, next_hw81, filter_y_offset, tmp
    beqz  \filter_w_rs1_1, 2f
    // esp.lp.setup  0, \filter_w_rs1_1, (1f - 4)
    mv  \tmp, \filter_w_rs1_1
    1:
        esp.vmulas.s16.qacc.ld.ip \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s16.qacc.ld.ip \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
        esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset
        addi  \tmp, \tmp, -1
        bgtz  \tmp, 1b
    2:
    andi  \tmp, \filter_w, 0xfffffffe
    beq  \filter_w, \tmp, 3f
        # three 8-input-element left
        esp.vmulas.s16.qacc.ld.ip \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v0,  \input_ptr,  \next_hw81

        esp.vmulas.s16.qacc.ld.xp \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
        # block one cyle here
        esp.vmulas.s16.qacc                                    \input_v0, \filter_v0
        j 4f
    3:  # two 8-input-element left
        esp.vmulas.s16.qacc.ld.xp \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        sub \input_ptr, \input_ptr, \dilation_x_offset
        add \input_ptr, \input_ptr, \next_hw81
        esp.vmulas.s16.qacc                                    \input_v1, \filter_v1
    4:
.endm



.macro esp32p4_s16_depthwise_conv2d_hw81  input_v0, input_v1, input_v2, filter_v0, filter_v1, filter_v2, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset, next_hw81, filter_h, filter_w, filter_w_rs1_1, args, filter_y_offset, filter_n_offset, tmp
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(t)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(t)
    # next_hw81 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(t) + 16
    # filter_w_rs1_1

    lw  \filter_h, 52(\args)  # filter_height

    addi  \tmp, \filter_w, -1
    beqz  \tmp, 9f
        esp.vld.128.ip  \filter_v0, \filter_ptr, 16
        esp.vld.128.xp  \input_v0,  \input_ptr,  \dilation_x_offset
        esp.vld.128.xp  \input_v1,  \input_ptr,  \dilation_x_offset

        addi  \filter_h, \filter_h, -1
        beqz  \filter_h, 8f
            // esp.lp.setup  1, \filter_h, (7f - 4)
            7:
                esp32p4_s16_depthwise_conv2d_1w81  \input_v0, \input_v1, \input_v2, \filter_v0, \filter_v1, \filter_v2, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \filter_w, \filter_w_rs1_1, \filter_y_offset, \tmp
                addi  \filter_h, \filter_h, -1
                bgtz  \filter_h, 7b
        8:  # last y
        esp32p4_s16_depthwise_conv2d_1w81_last  \input_v0, \input_v1, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \filter_w, \filter_w_rs1_1, \next_hw81, \filter_y_offset, \tmp
        j 12f


    9:  # filter_w == 1
        esp.vld.128.xp  \filter_v0, \filter_ptr, \filter_y_offset
        esp.vld.128.xp  \input_v0,  \input_ptr,  \dilation_y_offset

        addi  \filter_h, \filter_h, -1
        beqz  \filter_h, 11f
            // esp.lp.setup  1, \filter_h, (10f - 4)
            10:
                esp.vmulas.s16.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
                esp.vld.128.xp             \input_v0,  \input_ptr,  \dilation_y_offset
                addi  \filter_h, \filter_h, -1
                bgtz  \filter_h, 10b
        11: # last y
        esp.vmulas.s16.qacc  \input_v0, \filter_v0
        sub  \input_ptr, \input_ptr, \dilation_y_offset
        add  \input_ptr, \input_ptr, \next_hw81

    12:
    add  \filter_ptr, \filter_ptr, \filter_n_offset
.endm



.macro esp32p4_s16_depthwise_conv2d_hwc1_load_args  args, filter_ptr, dilation_x_offset, dilation_y_offset, next_hw81, c_div_x_1, mac_shift, filter_w, filter_w_rs1_1
    esp32p4_s16_depthwise_conv2d_33c1_load_args \args, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \next_hw81, \c_div_x_1, \mac_shift
    lw \filter_w,         56(\args)
    lw \filter_w_rs1_1,  148(\args)
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_hwc1_bias
    .type	dl_esp32p4_s16_depthwise_conv2d_hwc1_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_hwc1_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: filter_y_offset
    # t6: bias_ptr

    # a6(not for extension instructions): filter_w
    # a7(not for extension instructions): filter_w_rs1_1
    # t0(not for extension instructions): filter_n_offset
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): filter_h
    # s2(not for extension instructions): tmp value
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_4_stacks_1r  s2

    esp32p4_s16_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t1, t4, a6, a7
    lw  t6, 68(a2)      // bias
    lw  t5, 60 (a2)     // filter_y_offset
    lw  t0, 144(a2)     // filter_n_offset

    esp32p4_s16_depthwise_conv2d_hwc1_bias_loop:
        esp.zero.qacc
        esp32p4_s16_conv2d_128b_vector_bias  t6
        esp32p4_s16_depthwise_conv2d_hw81  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, t2, a6, a7, a2, t5, t0, s2
        esp32p4_s16_128b_vector_shift_result  q0, t4
        esp.vst.128.ip  q0, a0, 16

        addi  t1, t1, -1
        bgez  t1, esp32p4_s16_depthwise_conv2d_hwc1_bias_loop
    esp32p4_pop_4_stacks_1r  s2
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu
    .type	dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_hwc1_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: filter_y_offset
    # t6: bias_ptr

    # a6(not for extension instructions): filter_w
    # a7(not for extension instructions): filter_w_rs1_1
    # t0(not for extension instructions): filter_n_offset
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): filter_h
    # s2(not for extension instructions): tmp value
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: activation_alpha
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s2

    esp32p4_s16_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t1, t4, a6, a7
    lw  t6, 68(a2)      // bias
    lw  t5, 60 (a2)     // filter_y_offset
    lw  t0, 144(a2)     // filter_n_offset
    lw  s0,  76 (a2)    // activation_alpha
    lw  s1,  84 (a2)    // activation_shift

    esp32p4_s16_depthwise_conv2d_hwc1_bias_relu_loop:
        esp.zero.qacc
        esp32p4_s16_conv2d_128b_vector_bias  t6
        esp32p4_s16_depthwise_conv2d_hw81  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, t2, a6, a7, a2, t5, t0, s2
        esp32p4_s16_128b_vector_shift_result  q0, t4
        esp32p4_s16_128b_vector_relu  q0, s0, s1
        esp.vst.128.ip  q0, a0, 16

        addi  t1, t1, -1
        bgez  t1, esp32p4_s16_depthwise_conv2d_hwc1_bias_relu_loop
    esp32p4_pop_12_stacks_3r  s0, s1, s2
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_hwc1
    .type	dl_esp32p4_s16_depthwise_conv2d_hwc1, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_hwc1:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: filter_y_offset
    # t6:

    # a6(not for extension instructions): filter_w
    # a7(not for extension instructions): filter_w_rs1_1
    # t0(not for extension instructions): filter_n_offset
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): filter_h
    # s2(not for extension instructions): tmp value
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_4_stacks_1r  s2

    esp32p4_s16_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t1, t4, a6, a7
    lw  t5, 60 (a2)     // filter_y_offset
    lw  t0, 144(a2)     // filter_n_offset

    esp32p4_s16_depthwise_conv2d_hwc1_loop:
        esp.zero.qacc
        esp32p4_s16_depthwise_conv2d_hw81  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, t2, a6, a7, a2, t5, t0, s2
        esp32p4_s16_128b_vector_shift_result  q0, t4
        esp.vst.128.ip  q0, a0, 16

        addi  t1, t1, -1
        bgez  t1, esp32p4_s16_depthwise_conv2d_hwc1_loop
    esp32p4_pop_4_stacks_1r  s2
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_depthwise_conv2d_hwc1_relu
    .type	dl_esp32p4_s16_depthwise_conv2d_hwc1_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_depthwise_conv2d_hwc1_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hw81
    # t4: mac_shift
    # t5: filter_y_offset
    # t6:

    # a6(not for extension instructions): filter_w
    # a7(not for extension instructions): filter_w_rs1_1
    # t0(not for extension instructions): filter_n_offset
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): filter_h
    # s2(not for extension instructions): tmp value
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: activation_alpha
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s2

    esp32p4_s16_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t1, t4, a6, a7
    lw  t5, 60 (a2)     // filter_y_offset
    lw  t0, 144(a2)     // filter_n_offset
    lw  s0,  76 (a2)    // activation_alpha
    lw  s1,  84 (a2)    // activation_shift

    esp32p4_s16_depthwise_conv2d_hwc1_relu_loop:
        esp.zero.qacc
        esp32p4_s16_depthwise_conv2d_hw81  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, t2, a6, a7, a2, t5, t0, s2
        esp32p4_s16_128b_vector_shift_result  q0, t4
        esp32p4_s16_128b_vector_relu  q0, s0, s1
        esp.vst.128.ip  q0, a0, 16

        addi  t1, t1, -1
        bgez  t1, esp32p4_s16_depthwise_conv2d_hwc1_relu_loop
    esp32p4_pop_12_stacks_3r  s0, s1, s2
    ret
