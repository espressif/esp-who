#include "dl_esp32p4_s8.S"

############################################################################################################################################################
####
#### esp32p4_s8_depthwise_conv2d_33c1 series
####
############################################################################################################################################################
.macro esp32p4_s8_depthwise_conv2d_33s1 input_v0 filter_v0 input_v1 filter_v1 input_v2 filter_v2 input_ptr filter_ptr dilation_x_offset dilation_y_offset next_33s1
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_33s1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16

    esp.vmulas.s8.qacc.ld.ip \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp            \input_v2,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s8.qacc.ld.ip \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp            \input_v2,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s8.qacc.ld.ip \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp            \input_v2,  \input_ptr,  \next_33s1

    esp.vmulas.s8.qacc.ld.ip \filter_v2, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
    esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset
.endm



.macro esp32p4_s8_depthwise_conv2d_33s1_last input_v0 filter_v0 input_v1 filter_v1 input_ptr filter_ptr dilation_x_offset dilation_y_offset
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)

    esp.vmulas.s8.qacc.ld.ip    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp               \input_v0,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s8.qacc.ld.ip    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp               \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp               \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp               \input_v1,  \input_ptr,  \dilation_y_offset

    esp.vmulas.s8.qacc.ld.ip    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.xp               \input_v0,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    esp.vld.128.xp               \input_v1,  \input_ptr,  \dilation_x_offset

    esp.vmulas.s8.qacc.ld.ip    \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
    esp.vld.128.ip               \input_v0,  \input_ptr,  0

    esp.vmulas.s8.qacc.ld.ip    \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    # block one cycle here

    esp.vmulas.s8.qacc                                       \input_v0, \filter_v0
.endm



.macro esp32p4_s8_depthwise_conv2d_33c1_load_args args filter_ptr dilation_x_offset dilation_y_offset next_hwx1 c_div_x_1 mac_shift
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_hwx1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16

    lw  \filter_ptr,         48(\args)
    lw  \dilation_x_offset, 124(\args)
    lw  \dilation_y_offset, 128(\args)
    lw  \next_hwx1,         132(\args)
    lw  \c_div_x_1,         100(\args)
    lw  \mac_shift,         64 (\args)
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_33c1_bias
    .type   dl_esp32p4_s8_depthwise_conv2d_33c1_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_33c1_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_33s1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6: bias_ptr

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_s8_depthwise_conv2d_33c1_load_args a2, a3, a4, a5, t3, t4, t5
    lw  t6, 68(a2)       // bias

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t4, esp32p4_s8_depthwise_conv2d_33c1_bias_loop_last

    # lp.setup  0, t4, 1f
    esp.lp.setup  0, t4, 1f
        esp.zero.qacc
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_33s1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s8_128b_vector_shift_result  q3, t5
    1:  esp.vst.128.ip  q3, a0, 16

    esp32p4_s8_depthwise_conv2d_33c1_bias_loop_last:
        esp.zero.qacc
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_33s1_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp.vst.128.ip  q3, a0, 16
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_33c1_bias_relu
    .type   dl_esp32p4_s8_depthwise_conv2d_33c1_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_33c1_bias_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_33s1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6: bias_ptr

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi  sp, sp, -8
    sw  s0, 4(sp)
    sw  s1, 0(sp)

    esp32p4_s8_depthwise_conv2d_33c1_load_args a2, a3, a4, a5, t3, t4, t5
    lw  t6, 68(a2)       // bias
    lw  s0, 76(a2)       // activation_alpha
    lw  s1, 84(a2)       // activation_shift

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t4, esp32p4_s8_depthwise_conv2d_33c1_bias_relu_loop_last

    # lp.setup  0, t4, 1f
    esp.lp.setup  0, t4, 1f
        esp.zero.qacc
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_33s1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_relu  q3, s0, s1
    1:  esp.vst.128.ip  q3, a0, 16

    esp32p4_s8_depthwise_conv2d_33c1_bias_relu_loop_last:
        esp.zero.qacc
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_33s1_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_relu  q3, s0, s1
        esp.vst.128.ip  q3, a0, 16

    lw  s0, 4(sp)        // restore s0
    lw  s1, 0(sp)        // restore s1
    addi  sp, sp, 8
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_33c1_bias_prelu
    .type   dl_esp32p4_s8_depthwise_conv2d_33c1_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_33c1_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_33s1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6: bias_ptr

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi  sp, sp, -8
    sw  s0, 4(sp)
    sw  s1, 0(sp)

    esp32p4_s8_depthwise_conv2d_33c1_load_args a2, a3, a4, a5, t3, t4, t5
    lw  t6, 68(a2)       // bias
    lw  s0, 80(a2)       // activation_alpha_ptr
    lw  s1, 84(a2)       // activation_shift

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t4, esp32p4_s8_depthwise_conv2d_33c1_bias_prelu_loop_last

    # lp.setup  0, t4, 1f
    esp.lp.setup  0, t4, 1f
        esp.zero.qacc
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_33s1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_prelu  q3, q4, s0, s1
    1:  esp.vst.128.ip  q3, a0, 16

    esp32p4_s8_depthwise_conv2d_33c1_bias_prelu_loop_last:
        esp.zero.qacc
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_33s1_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_prelu  q3, q4, s0, s1
        esp.vst.128.ip  q3, a0, 16

    lw  s0, 4(sp)        // restore s0
    lw  s1, 0(sp)        // restore s1
    addi  sp, sp, 8
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_33c1
    .type   dl_esp32p4_s8_depthwise_conv2d_33c1, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_33c1:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_33s1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6:

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_s8_depthwise_conv2d_33c1_load_args a2, a3, a4, a5, t3, t4, t5

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t4, esp32p4_s8_depthwise_conv2d_33c1_loop_last

    # lp.setup  0, t4, 1f
    esp.lp.setup  0, t4, 1f
        esp.zero.qacc
        esp32p4_s8_depthwise_conv2d_33s1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s8_128b_vector_shift_result  q3, t5
    1:  esp.vst.128.ip  q3, a0, 16

    esp32p4_s8_depthwise_conv2d_33c1_loop_last:
        esp.zero.qacc
        esp32p4_s8_depthwise_conv2d_33s1_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp.vst.128.ip  q3, a0, 16
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_33c1_relu
    .type   dl_esp32p4_s8_depthwise_conv2d_33c1_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_33c1_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_33s1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6:

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi  sp, sp, -8
    sw  s0, 4(sp)
    sw  s1, 0(sp)

    esp32p4_s8_depthwise_conv2d_33c1_load_args a2, a3, a4, a5, t3, t4, t5
    lw  s0, 76(a2)       // activation_alpha
    lw  s1, 84(a2)       // activation_shift

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t4, esp32p4_s8_depthwise_conv2d_33c1_relu_loop_last

    # lp.setup  0, t4, 1f
    esp.lp.setup  0, t4, 1f
        esp.zero.qacc
        esp32p4_s8_depthwise_conv2d_33s1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_relu  q3, s0, s1
    1:  esp.vst.128.ip  q3, a0, 16

    esp32p4_s8_depthwise_conv2d_33c1_relu_loop_last:
        esp.zero.qacc
        esp32p4_s8_depthwise_conv2d_33s1_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_relu  q3, s0, s1
        esp.vst.128.ip  q3, a0, 16

    lw  s0, 4(sp)        // restore s0
    lw  s1, 0(sp)        // restore s1
    addi  sp, sp, 8
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_33c1_prelu
    .type   dl_esp32p4_s8_depthwise_conv2d_33c1_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_33c1_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_33s1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6:

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi  sp, sp, -8
    sw  s0, 4(sp)
    sw  s1, 0(sp)

    esp32p4_s8_depthwise_conv2d_33c1_load_args a2, a3, a4, a5, t3, t4, t5
    lw  s0, 80(a2)       // activation_alpha_ptr
    lw  s1, 84(a2)       // activation_shift

    esp.vld.128.xp  q0, a1, a4
    esp.vld.128.ip  q1, a3, 16
    esp.vld.128.xp  q2, a1, a4

    beqz  t4, esp32p4_s8_depthwise_conv2d_33c1_prelu_loop_last

    # lp.setup  0, t4, 1f
    esp.lp.setup  0, t4, 1f
        esp.zero.qacc
        esp32p4_s8_depthwise_conv2d_33s1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_prelu  q3, q4, s0, s1
    1:  esp.vst.128.ip  q3, a0, 16

    esp32p4_s8_depthwise_conv2d_33c1_prelu_loop_last:
        esp.zero.qacc
        esp32p4_s8_depthwise_conv2d_33s1_last  q0, q1, q2, q3, a1, a3, a4, a5
        esp32p4_s8_128b_vector_shift_result  q3, t5
        esp32p4_s8_128b_vector_prelu  q3, q4, s0, s1
        esp.vst.128.ip  q3, a0, 16

    lw  s0, 4(sp)        // restore s0
    lw  s1, 0(sp)        // restore s1
    addi  sp, sp, 8
    ret



############################################################################################################################################################
####
#### esp32p4_s8_depthwise_conv2d_hwc1 series
####
############################################################################################################################################################

.macro esp32p4_s8_depthwise_conv2d_1ws1 input_v0, input_v1, input_v2, filter_v0, filter_v1, filter_v2, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset, tmp_value, filter_w, filter_w_rs1_1, filter_y_offset
    beqz  \filter_w_rs1_1, 1f
    # lp.setup  0, \filter_w_rs1_1, 0f
    esp.lp.setup  0, \filter_w_rs1_1, 0f
        esp.vmulas.s8.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s8.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    0:  esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset

    1:
    andi  \tmp_value, \filter_w, 0xFFFFFFFE
    beq  \tmp_value, \filter_w, 2f
        # three 8-input-element left
        esp.vmulas.s8.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v2,  \input_ptr,  \dilation_y_offset

        esp.vmulas.s8.qacc.ld.xp  \filter_v2, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s8.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v2, \filter_v2
        esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset
        j 3f
    2:  # two 8-input-element left
        esp.vmulas.s8.qacc.ld.xp  \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        sub  \input_ptr, \input_ptr, \dilation_x_offset
        add  \input_ptr, \input_ptr, \dilation_y_offset
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s8.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
        esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset
    3:
.endm



.macro esp32p4_s8_depthwise_conv2d_1ws1_last input_v0, input_v1, filter_v0, filter_v1, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset, tmp_value, filter_w, filter_w_rs1_1, next_hws1, filter_y_offset
    beqz  \filter_w_rs1_1, 5f
    # lp.setup  0, \filter_w_rs1_1, 4f
    esp.lp.setup  0, \filter_w_rs1_1, 4f
        esp.vmulas.s8.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_x_offset

        esp.vmulas.s8.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v1
    4:  esp.vld.128.xp            \input_v1,  \input_ptr,  \dilation_x_offset

    5:
    andi  \tmp_value, \filter_w, 0xFFFFFFFE
    beq  \tmp_value, \filter_w, 6f
        # three 8-input-element left
        esp.vmulas.s8.qacc.ld.ip  \filter_v1, \filter_ptr, 16, \input_v0, \filter_v0
        esp.vld.128.xp            \input_v0,  \input_ptr,  \next_hws1

        esp.vmulas.s8.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v1
        # block one cyle here
        esp.vmulas.s8.qacc                                    \input_v0, \filter_v0
        j 7f
    6:  # two 8-input-element left
        esp.vmulas.s8.qacc.ld.xp  \filter_v1, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        sub  \input_ptr, \input_ptr, \dilation_x_offset
        add  \input_ptr, \input_ptr, \next_hws1
        esp.vmulas.s8.qacc                                    \input_v1, \filter_v1
    7:
.endm



.macro esp32p4_s8_depthwise_conv2d_hws1 input_v0, input_v1, input_v2, filter_v0, filter_v1, filter_v2, input_ptr, filter_ptr, dilation_x_offset, dilation_y_offset, next_hws1, filter_h, filter_w, filter_w_rs1_1, args, filter_offset_q, filter_y_offset, tmp_value
    # dilation_x_offset = input_channel_with_padding * dilation_x * sizeof(T)
    # dilation_y_offset = (-(filter_width - 1) * dilation_x * input_channel_with_padding + dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T)
    # next_hws1 = (-(filter_width - 1) * dilation_x * input_channel_with_padding - (filter_height - 1) * dilation_y * input_width_with_padding * input_channel_with_padding) * sizeof(T) + 16
    # filter_w_rs1_1

    lw  \filter_h, 52(\args)  # filter_height

    addi  \tmp_value, \filter_w, -1
    beqz  \tmp_value, 10f
        esp.vld.128.ip  \filter_v0, \filter_ptr, 16
        esp.vld.128.xp  \input_v0,  \input_ptr,  \dilation_x_offset
        esp.vld.128.xp  \input_v1,  \input_ptr,  \dilation_x_offset

        addi  \filter_h, \filter_h, -1
        beqz  \filter_h, 9f
            // lp.setup  1, \filter_h, 8f
            // 8:  esp32p4_s8_depthwise_conv2d_1ws1  \input_v0, \input_v1, \input_v2, \filter_v0, \filter_v1, \filter_v2, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \tmp_value, \filter_w, \filter_w_rs1_1, \filter_y_offset
            8:
                esp32p4_s8_depthwise_conv2d_1ws1  \input_v0, \input_v1, \input_v2, \filter_v0, \filter_v1, \filter_v2, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \tmp_value, \filter_w, \filter_w_rs1_1, \filter_y_offset
                addi  \filter_h, \filter_h, -1
                bgtz  \filter_h, 8b
        9:  # last y
        esp32p4_s8_depthwise_conv2d_1ws1_last  \input_v0, \input_v1, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \tmp_value, \filter_w, \filter_w_rs1_1, \next_hws1, \filter_y_offset
        j  13f

    10:  # filter_w == 1
        esp.vld.128.xp  \filter_v0, \filter_ptr, \filter_y_offset
        esp.vld.128.xp  \input_v0,  \input_ptr,  \dilation_y_offset
        addi  \filter_h, \filter_h, -1
        beqz  \filter_h, 12f
            // lp.setup  1, \filter_h, 11f
            //     esp.vmulas.s8.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
            // 11: esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_y_offset
            11:
                esp.vmulas.s8.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
                esp.vld.128.xp            \input_v0,  \input_ptr,  \dilation_y_offset
                addi  \filter_h, \filter_h, -1
                bgtz  \filter_h, 11b
    12: # last y
        esp.vmulas.s8.qacc  \input_v0, \filter_v0
        sub  \input_ptr, \input_ptr, \dilation_y_offset
        add  \input_ptr, \input_ptr, \next_hws1

    13:
        esp.movi.32.a  \filter_offset_q, \filter_h, 2
        add  \filter_ptr, \filter_ptr, \filter_h
.endm



.macro esp32p4_s8_depthwise_conv2d_hwc1_load_args  args, filter_ptr, dilation_x_offset, dilation_y_offset, next_hws1, c_div_x_1, mac_shift, filter_w, filter_w_rs1_1
    esp32p4_s8_depthwise_conv2d_33c1_load_args  \args, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \next_hws1, \c_div_x_1, \mac_shift
    lw  \filter_w,         56(\args)
    lw  \filter_w_rs1_1,  148(\args)
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_hwc1_bias
    .type   dl_esp32p4_s8_depthwise_conv2d_hwc1_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_hwc1_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hws1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6: bias_ptr

    # s0: filter_h / filter_n_offset
    # s1: filter_w
    # s8: filter_w_rs1_1
    # s9: filter_y_offset
    # s10:
    # s11: tmp_value

    addi  sp, sp, -20
    sw  s0, 16(sp)
    sw  s1, 12(sp)
    sw  s8, 8(sp)
    sw  s9, 4(sp)
    sw  s11, 0(sp)

    lw  s9, 60(a2)      // filter_y_offset
    lw  s0, 144(a2)     // filter_n_offset
    esp.movi.32.q  q7, s0, 2

    lw  t6, 68(a2)      // bias
    esp32p4_s8_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t4, t5, s1, s8

    esp32p4_s8_depthwise_conv2d_hwc1_bias_loop:
        esp.zero.qacc
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_hws1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, s0, s1, s8, a2, q7, s9, s11
        esp32p4_s8_128b_vector_shift_result  q0, t5
        esp.vst.128.ip  q0, a0, 16
        addi  t4, t4, -1
        bgez  t4, esp32p4_s8_depthwise_conv2d_hwc1_bias_loop

    lw  s0, 16(sp)      // restore s0
    lw  s1, 12(sp)      // restore s1
    lw  s8, 8(sp)       // restore s8
    lw  s9, 4(sp)       // restore s9
    lw  s11, 0(sp)      // restore s11
    addi  sp, sp, 20
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_relu
    .type   dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hws1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6: bias_ptr

    # s0: filter_h / filter_n_offset
    # s1: filter_w
    # s8: filter_w_rs1_1
    # s9: filter_y_offset / activation_alpha
    # s10: activation_shift
    # s11: tmp_value

    addi  sp, sp, -24
    sw  s0, 20(sp)
    sw  s1, 16(sp)
    sw  s8, 12(sp)
    sw  s9, 8(sp)
    sw  s10, 4(sp)
    sw  s11, 0(sp)

    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp.movi.32.q  q7, s9, 3

    lw  s9, 60(a2)      // filter_y_offset
    lw  s0, 144(a2)     // filter_n_offset
    esp.movi.32.q  q7, s9, 1
    esp.movi.32.q  q7, s0, 2

    lw  t6, 68(a2)      // bias
    esp32p4_s8_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t4, t5, s1, s8

    esp32p4_s8_depthwise_conv2d_hwc1_bias_relu_loop:
        esp.zero.qacc
        esp.movi.32.a  q7, s9, 1    // filter_y_offset
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_hws1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, s0, s1, s8, a2, q7, s9, s11
        esp32p4_s8_128b_vector_shift_result  q0, t5
        esp.movi.32.a  q7, s9, 3    // activation_alpha
        esp32p4_s8_128b_vector_relu  q0, s9, s10
        esp.vst.128.ip  q0, a0, 16

        addi  t4, t4, -1
        bgez  t4, esp32p4_s8_depthwise_conv2d_hwc1_bias_relu_loop

    lw  s0, 20(sp)      // restore s0
    lw  s1, 16(sp)      // restore s1
    lw  s8, 12(sp)      // restore s8
    lw  s9, 8(sp)       // restore s9
    lw  s10, 4(sp)      // restore s10
    lw  s11, 0(sp)      // restore s11
    addi  sp, sp, 24
    ret


        .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu
    .type   dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hws1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6: bias_ptr

    # s0: filter_h / filter_n_offset
    # s1: filter_w
    # s8: filter_w_rs1_1
    # s9: filter_y_offset / activation_alpha
    # s10: activation_shift
    # s11: tmp_value

    addi  sp, sp, -24
    sw  s0, 20(sp)
    sw  s1, 16(sp)
    sw  s8, 12(sp)
    sw  s9, 8(sp)
    sw  s10, 4(sp)
    sw  s11, 0(sp)

    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp.movi.32.q  q7, s9, 3

    lw  s9, 60(a2)      // filter_y_offset
    lw  s0, 144(a2)     // filter_n_offset
    esp.movi.32.q  q7, s9, 1
    esp.movi.32.q  q7, s0, 2

    lw  t6, 68(a2)      // bias
    esp32p4_s8_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t4, t5, s1, s8

    esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu_loop:
        esp.zero.qacc
        esp.movi.32.a  q7, s9, 1    // filter_y_offset
        esp32p4_s8_conv2d_128b_vector_bias  t6
        esp32p4_s8_depthwise_conv2d_hws1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, s0, s1, s8, a2, q7, s9, s11
        esp32p4_s8_128b_vector_shift_result  q0, t5
        esp.movi.32.a  q7, s9, 3    // activation_alpha_ptr
        esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
        esp.vst.128.ip  q0, a0, 16

        addi  t4, t4, -1
        bgez  t4, esp32p4_s8_depthwise_conv2d_hwc1_bias_prelu_loop

    lw  s0, 20(sp)      // restore s0
    lw  s1, 16(sp)      // restore s1
    lw  s8, 12(sp)      // restore s8
    lw  s9, 8(sp)       // restore s9
    lw  s10, 4(sp)      // restore s10
    lw  s11, 0(sp)      // restore s11
    addi  sp, sp, 24
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_hwc1
    .type   dl_esp32p4_s8_depthwise_conv2d_hwc1, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_hwc1:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hws1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6:

    # s0: filter_h / filter_n_offset
    # s1: filter_w
    # s8: filter_w_rs1_1
    # s9: filter_y_offset
    # s10:
    # s11: tmp_value

    addi  sp, sp, -20
    sw  s0, 16(sp)
    sw  s1, 12(sp)
    sw  s8, 8(sp)
    sw  s9, 4(sp)
    sw  s11, 0(sp)

    lw  s9, 60(a2)      // filter_y_offset
    lw  s0, 144(a2)     // filter_n_offset
    esp.movi.32.q  q7, s0, 2
    esp32p4_s8_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t4, t5, s1, s8

    esp32p4_s8_depthwise_conv2d_hwc1_loop:
        esp.zero.qacc
        esp32p4_s8_depthwise_conv2d_hws1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, s0, s1, s8, a2, q7, s9, s11
        esp32p4_s8_128b_vector_shift_result  q0, t5
        esp.vst.128.ip  q0, a0, 16
        addi  t4, t4, -1
        bgez  t4, esp32p4_s8_depthwise_conv2d_hwc1_loop

    lw  s0, 16(sp)      // restore s0
    lw  s1, 12(sp)      // restore s1
    lw  s8, 8(sp)       // restore s8
    lw  s9, 4(sp)       // restore s9
    lw  s11, 0(sp)      // restore s11
    addi  sp, sp, 20
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_hwc1_relu
    .type   dl_esp32p4_s8_depthwise_conv2d_hwc1_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_hwc1_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hws1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6:

    # s0: filter_h / filter_n_offset
    # s1: filter_w
    # s8: filter_w_rs1_1
    # s9: filter_y_offset / activation_alpha
    # s10: activation_shift
    # s11: tmp_value

    addi  sp, sp, -24
    sw  s0, 20(sp)
    sw  s1, 16(sp)
    sw  s8, 12(sp)
    sw  s9, 8(sp)
    sw  s10, 4(sp)
    sw  s11, 0(sp)

    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp.movi.32.q  q7, s9, 3

    lw  s9, 60(a2)      // filter_y_offset
    lw  s0, 144(a2)     // filter_n_offset
    esp.movi.32.q  q7, s9, 1
    esp.movi.32.q  q7, s0, 2
    esp32p4_s8_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t4, t5, s1, s8

    esp32p4_s8_depthwise_conv2d_hwc1_relu_loop:
        esp.zero.qacc
        esp.movi.32.a  q7, s9, 1    // filter_y_offset
        esp32p4_s8_depthwise_conv2d_hws1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, s0, s1, s8, a2, q7, s9, s11
        esp32p4_s8_128b_vector_shift_result  q0, t5
        esp.movi.32.a  q7, s9, 3    // activation_alpha
        esp32p4_s8_128b_vector_relu  q0, s9, s10
        esp.vst.128.ip  q0, a0, 16

        addi  t4, t4, -1
        bgez  t4, esp32p4_s8_depthwise_conv2d_hwc1_relu_loop

    lw  s0, 20(sp)      // restore s0
    lw  s1, 16(sp)      // restore s1
    lw  s8, 12(sp)      // restore s8
    lw  s9, 8(sp)       // restore s9
    lw  s10, 4(sp)      // restore s10
    lw  s11, 0(sp)      // restore s11
    addi  sp, sp, 24
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_depthwise_conv2d_hwc1_prelu
    .type   dl_esp32p4_s8_depthwise_conv2d_hwc1_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_depthwise_conv2d_hwc1_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: input dilation x offset
    # a5: input dilation y offset
    # t3: next_hws1
    # t4: c_div_x_1
    # t5: mac_shift
    # t6:

    # s0: filter_h / filter_n_offset
    # s1: filter_w
    # s8: filter_w_rs1_1
    # s9: filter_y_offset / activation_alpha
    # s10: activation_shift
    # s11: tmp_value

    addi  sp, sp, -24
    sw  s0, 20(sp)
    sw  s1, 16(sp)
    sw  s8, 12(sp)
    sw  s9, 8(sp)
    sw  s10, 4(sp)
    sw  s11, 0(sp)

    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp.movi.32.q  q7, s9, 3

    lw  s9, 60(a2)      // filter_y_offset
    lw  s0, 144(a2)     // filter_n_offset
    esp.movi.32.q  q7, s9, 1
    esp.movi.32.q  q7, s0, 2
    esp32p4_s8_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, t4, t5, s1, s8

    esp32p4_s8_depthwise_conv2d_hwc1_prelu_loop:
        esp.zero.qacc
        esp.movi.32.a  q7, s9, 1    // filter_y_offset
        esp32p4_s8_depthwise_conv2d_hws1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3, s0, s1, s8, a2, q7, s9, s11
        esp32p4_s8_128b_vector_shift_result  q0, t5
        esp.movi.32.a  q7, s9, 3    // activation_alpha_ptr
        esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
        esp.vst.128.ip  q0, a0, 16

        addi  t4, t4, -1
        bgez  t4, esp32p4_s8_depthwise_conv2d_hwc1_prelu_loop

    lw  s0, 20(sp)      // restore s0
    lw  s1, 16(sp)      // restore s1
    lw  s8, 12(sp)      // restore s8
    lw  s9, 8(sp)       // restore s9
    lw  s10, 4(sp)      // restore s10
    lw  s11, 0(sp)      // restore s11
    addi  sp, sp, 24
    ret
