#include "dl_esp32p4_s8.S"

############################################################################################################################################################
####
#### esp32p4_s8_conv2d_11cn series
####
############################################################################################################################################################
.macro esp32p4_s8_conv2d_11c16 input_v0 input_ptr filter_v0 filter_v1 filter_ptr c_div_x_1
    # scalar * vecter and accumulate into QACC
    # input_ptr += (c_div_x_1 + 1) * 16 in the end
    # filter_ptr point to the next 16 bytes in the end

    # input_v0:     16 input elements
    # filter_v0:    16 filter elements
    # filter_v1:    16 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 16 - 1

    esp.vld.128.ip \input_v0,  \input_ptr,  16
    esp.vld.128.ip \filter_v0, \filter_ptr, 16
    esp.vld.128.ip \filter_v1, \filter_ptr, 16
    beqz           \c_div_x_1, 1f

    # lp.setup 0, \c_div_x_1, 0f
    esp.lp.setup 0, \c_div_x_1, 0f
    # 0:
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 14
        esp.vsmulas.s8.qacc.ld.incp \input_v0,  \input_ptr,  \filter_v1, \input_v0, 15
    0:  esp.vld.128.ip              \filter_v1, \filter_ptr, 16
        # addi                        \c_div_x_1, \c_div_x_1,  -1
        # bgtz                        \c_div_x_1, 0b

    1:
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
        esp.vsmulas.s8.qacc                                  \filter_v0, \input_v0, 14
        esp.vsmulas.s8.qacc                                  \filter_v1, \input_v0, 15
.endm



############################################################################################################################################################
####
#### esp32p4_s8_conv2d_11cn
####
############################################################################################################################################################
.macro esp32p4_s8_conv2d_11cn_load_args args filter_ptr c_div_x_1 n_rs3 mac_shift
    lw  \n_rs3,         96(\args)   // output_channel_div_8
    lw  \mac_shift,     64(\args)   // mac_shift
    lw  \filter_ptr,    48(\args)   // filter
    lw  \c_div_x_1,     100(\args)  // input_channel / x - 1
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_11cn_bias
    .type   dl_esp32p4_s8_conv2d_11cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_11cn_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6:

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_s8_conv2d_11cn_load_args a2, a3, a4, a5, t3
    # Because the subsequent esp.lp.setup loop instruction compares for a value >= 0 and cannot be negative, we subtract 1 in advance here.
    # addi a4, a4, -1
    lw t4, 68(a2)       // bias

    esp32p4_s8_conv2d_11cn_bias_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_11c16 q0, t5, q1, q2, a3, a4
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_11cn_bias_loop
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_11cn_bias_relu
    .type   dl_esp32p4_s8_conv2d_11cn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_11cn_bias_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6:

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi sp, sp, -8
    sw s0, 4(sp)
    sw s1, 0(sp)

    esp32p4_s8_conv2d_11cn_load_args a2, a3, a4, a5, t3
    lw t4, 68(a2)       // bias
    lw s0, 76(a2)       // activation_alpha
    lw s1, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_11cn_bias_relu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_11c16 q0, t5, q1, q2, a3, a4
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_relu q0, s0, s1
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_11cn_bias_relu_loop

    lw s0, 4(sp)        // restore s0
    lw s1, 0(sp)        // restore s1
    addi sp, sp, 8
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_11cn_bias_prelu
    .type   dl_esp32p4_s8_conv2d_11cn_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_11cn_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6:

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi sp, sp, -8
    sw s0, 4(sp)
    sw s1, 0(sp)

    esp32p4_s8_conv2d_11cn_load_args a2, a3, a4, a5, t3
    lw t4, 68(a2)       // bias
    lw s0, 80(a2)       // activation_alpha_ptr
    lw s1, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_11cn_bias_prelu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_11c16 q0, t5, q1, q2, a3, a4
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_prelu q0, q1, s0, s1
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_11cn_bias_prelu_loop

    lw s0, 4(sp)        // restore s0
    lw s1, 0(sp)        // restore s1
    addi sp, sp, 8
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_11cn
    .type   dl_esp32p4_s8_conv2d_11cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_11cn:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6:

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_s8_conv2d_11cn_load_args a2, a3, a4, a5, t3
    # Because the subsequent esp.lp.setup loop instruction compares for a value >= 0 and cannot be negative, we subtract 1 in advance here.
    # addi a4, a4, -1

    esp32p4_s8_conv2d_11cn_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_11c16 q0, t5, q1, q2, a3, a4
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_11cn_loop
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_11cn_relu
    .type   dl_esp32p4_s8_conv2d_11cn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_11cn_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6:

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi sp, sp, -8
    sw s0, 4(sp)
    sw s1, 0(sp)

    esp32p4_s8_conv2d_11cn_load_args a2, a3, a4, a5, t3
    lw s0, 76(a2)       // activation_alpha
    lw s1, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_11cn_relu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_11c16 q0, t5, q1, q2, a3, a4
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_relu q0, s0, s1
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_11cn_relu_loop

    lw s0, 4(sp)        // restore s0
    lw s1, 0(sp)        // restore s1
    addi sp, sp, 8
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_11cn_prelu
    .type   dl_esp32p4_s8_conv2d_11cn_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_11cn_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6:

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    addi sp, sp, -8
    sw s0, 4(sp)
    sw s1, 0(sp)

    esp32p4_s8_conv2d_11cn_load_args a2, a3, a4, a5, t3
    lw s0, 80(a2)       // activation_alpha_ptr
    lw s1, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_11cn_prelu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_11c16 q0, t5, q1, q2, a3, a4
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_prelu q0, q1, s0, s1
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_11cn_prelu_loop

    lw s0, 4(sp)        // restore s0
    lw s1, 0(sp)        // restore s1
    addi sp, sp, 8
    ret






############################################################################################################################################################
####
#### esp32p4_s8_conv2d_33cn series
####
############################################################################################################################################################
.macro esp32p4_s8_conv2d_33c16 input_v0 filter_v0 filter_v1 input_ptr filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro esp32p4_s8_conv2d_hwcn_load_args args filter_ptr c_div_x_1 n_rs3 mac_shift dilation_x_offset dilation_y_offset
    esp32p4_s8_conv2d_11cn_load_args \args, \filter_ptr, \c_div_x_1, \n_rs3, \mac_shift
    lw \dilation_x_offset, 108(\args)  // input dilation x offset
    lw \dilation_y_offset, 112(\args)  // input dilation y offset
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_33cn_bias
    .type   dl_esp32p4_s8_conv2d_33cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_33cn_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8:
    # s9:
    # s10:
    # s11:

    addi sp, sp, -8
    sw s0, 4(sp)
    sw s1, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t4, 68(a2)       // bias

    esp32p4_s8_conv2d_33cn_bias_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_33c16 q0, q1, q2, t5, a3, a4, s0, s1
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_33cn_bias_loop

    lw s0, 4(sp)        // restore s0
    lw s1, 0(sp)        // restore s1
    addi sp, sp, 8
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_33cn_bias_relu
    .type   dl_esp32p4_s8_conv2d_33cn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_33cn_bias_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: activation_alpha/_address
    # s9: activation_shift
    # s10:
    # s11:

    addi sp, sp, -16
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s8, 4(sp)
    sw s9, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t4, 68(a2)       // bias
    lw s8, 76(a2)       // activation_alpha
    lw s9, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_33cn_bias_relu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_33c16 q0, q1, q2, t5, a3, a4, s0, s1
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_relu q0, s8, s9
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_33cn_bias_relu_loop

    lw s0, 12(sp)       // restore s0
    lw s1, 8(sp)        // restore s1
    lw s8, 4(sp)        // restore s8
    lw s9, 0(sp)        // restore s9
    addi sp, sp, 16
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_33cn_bias_prelu
    .type   dl_esp32p4_s8_conv2d_33cn_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_33cn_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: activation_alpha/_address
    # s9: activation_shift
    # s10:
    # s11:

    addi sp, sp, -16
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s8, 4(sp)
    sw s9, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t4, 68(a2)       // bias
    lw s8, 80(a2)       // activation_alpha_ptr
    lw s9, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_33cn_bias_prelu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_33c16 q0, q1, q2, t5, a3, a4, s0, s1
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_prelu q0, q1, s8, s9
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_33cn_bias_prelu_loop

    lw s0, 12(sp)       // restore s0
    lw s1, 8(sp)        // restore s1
    lw s8, 4(sp)        // restore s8
    lw s9, 0(sp)        // restore s9
    addi sp, sp, 16
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_33cn
    .type   dl_esp32p4_s8_conv2d_33cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_33cn:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8:
    # s9:
    # s10:
    # s11:

    addi sp, sp, -8
    sw s0, 4(sp)
    sw s1, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1

    esp32p4_s8_conv2d_33cn_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_33c16 q0, q1, q2, t5, a3, a4, s0, s1
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_33cn_loop

    lw s0, 4(sp)        // restore s0
    lw s1, 0(sp)        // restore s1
    addi sp, sp, 8
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_33cn_relu
    .type   dl_esp32p4_s8_conv2d_33cn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_33cn_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: activation_alpha/_address
    # s9: activation_shift
    # s10:
    # s11:

    addi sp, sp, -16
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s8, 4(sp)
    sw s9, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw s8, 76(a2)       // activation_alpha
    lw s9, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_33cn_relu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_33c16 q0, q1, q2, t5, a3, a4, s0, s1
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_relu q0, s8, s9
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_33cn_relu_loop

    lw s0, 12(sp)       // restore s0
    lw s1, 8(sp)        // restore s1
    lw s8, 4(sp)        // restore s8
    lw s9, 0(sp)        // restore s9
    addi sp, sp, 16
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_33cn_prelu
    .type   dl_esp32p4_s8_conv2d_33cn_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_33cn_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: activation_alpha/_address
    # s9: activation_shift
    # s10:
    # s11:

    addi sp, sp, -16
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s8, 4(sp)
    sw s9, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw s8, 80(a2)       // activation_alpha_ptr
    lw s9, 84(a2)       // activation_shift

    esp32p4_s8_conv2d_33cn_prelu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_33c16 q0, q1, q2, t5, a3, a4, s0, s1
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_prelu q0, q1, s8, s9
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_33cn_prelu_loop

    lw s0, 12(sp)       // restore s0
    lw s1, 8(sp)        // restore s1
    lw s8, 4(sp)        // restore s8
    lw s9, 0(sp)        // restore s9
    addi sp, sp, 16
    ret




############################################################################################################################################################
####
#### esp32p4_s8_conv2d_hwcn series
####
############################################################################################################################################################
.macro esp32p4_s8_conv2d_hwc16  input_v0 filter_v0 filter_v1 input_ptr filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset filter_h filter_w args filter_offset_q
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)
    # filter_h
    # filter_w

    lw  \filter_h, 52(\args)  # filter_height
    2:
        lw  \filter_w, 56(\args)  # filter_width
        addi  \filter_w, \filter_w, -1
        beqz  \filter_w, 4f
            // lp.setup 1, \filter_w, 3f
            //     esp32p4_s8_conv2d_11c16  \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
            // 3:  add  \input_ptr, \input_ptr, \dilation_x_offset
            3:
                esp32p4_s8_conv2d_11c16  \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
                add  \input_ptr, \input_ptr, \dilation_x_offset
                addi  \filter_w, \filter_w, -1
                bgtz  \filter_w, 3b
        4:
        esp32p4_s8_conv2d_11c16  \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
        esp.movi.32.a  \filter_offset_q, \filter_w, 1
        add  \filter_ptr, \filter_ptr, \filter_w
        add  \input_ptr, \input_ptr, \dilation_y_offset

        addi  \filter_h, \filter_h, -1
        bnez  \filter_h, 2b

    esp.movi.32.a  \filter_offset_q, \filter_h, 2
    add  \filter_ptr, \filter_ptr, \filter_h
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_hwcn_bias
    .type   dl_esp32p4_s8_conv2d_hwcn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_hwcn_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: filter_height
    # s9: filter_width
    # s10:
    # s11:

    addi sp, sp, -16
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s8, 4(sp)
    sw s9, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t4, 68(a2)       // bias
    lw s9, 60(a2)       // filter_y_offset
    lw s8, 144(a2)
    esp.movi.32.q q6, s9, 1  // filter_y_offset
    esp.movi.32.q q6, s8, 2  // filter_n_offset

    esp32p4_s8_conv2d_hwcn_bias_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_hwc16  q0, q1, q2, t5, a3, a4, s0, s1, s8, s9, a2, q6
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_hwcn_bias_loop

    lw s0, 12(sp)       // restore s0
    lw s1, 8(sp)        // restore s1
    lw s8, 4(sp)        // restore s8
    lw s9, 0(sp)        // restore s9
    addi sp, sp, 16
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_hwcn_bias_relu
    .type   dl_esp32p4_s8_conv2d_hwcn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_hwcn_bias_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6: activation_alpha/_address

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: filter_height
    # s9: filter_width
    # s10: activation_shift
    # s11:

    addi sp, sp, -20
    sw s0, 16(sp)
    sw s1, 12(sp)
    sw s8, 8(sp)
    sw s9, 4(sp)
    sw s10, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t4, 68(a2)       // bias
    lw t6, 76(a2)       // activation_alpha
    lw s10, 84(a2)      // activation_shift
    lw s9, 60(a2)       // filter_y_offset
    lw s8, 144(a2)
    esp.movi.32.q q6, s9, 1  // filter_y_offset
    esp.movi.32.q q6, s8, 2  // filter_n_offset

    esp32p4_s8_conv2d_hwcn_bias_relu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_hwc16  q0, q1, q2, t5, a3, a4, s0, s1, s8, s9, a2, q6
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_relu q0, t6, s10
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_hwcn_bias_relu_loop

    lw s0, 16(sp)       // restore s0
    lw s1, 12(sp)       // restore s1
    lw s8, 8(sp)        // restore s8
    lw s9, 4(sp)        // restore s9
    lw s10, 0(sp)       // restore s10
    addi sp, sp, 20
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_hwcn_bias_prelu
    .type   dl_esp32p4_s8_conv2d_hwcn_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_hwcn_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4: bias_ptr
    # t5: moving_input_ptr
    # t6: activation_alpha/_address

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: filter_height
    # s9: filter_width
    # s10: activation_shift
    # s11:

    addi sp, sp, -20
    sw s0, 16(sp)
    sw s1, 12(sp)
    sw s8, 8(sp)
    sw s9, 4(sp)
    sw s10, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t4, 68(a2)       // bias
    lw t6, 80(a2)       // activation_alpha_ptr
    lw s10, 84(a2)      // activation_shift
    lw s9, 60(a2)       // filter_y_offset
    lw s8, 144(a2)
    esp.movi.32.q q6, s9, 1  // filter_y_offset
    esp.movi.32.q q6, s8, 2  // filter_n_offset

    esp32p4_s8_conv2d_hwcn_bias_prelu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_128b_vector_bias t4
        esp32p4_s8_conv2d_hwc16  q0, q1, q2, t5, a3, a4, s0, s1, s8, s9, a2, q6
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_prelu q0, q1, t6, s10
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_hwcn_bias_prelu_loop

    lw s0, 16(sp)       // restore s0
    lw s1, 12(sp)       // restore s1
    lw s8, 8(sp)        // restore s8
    lw s9, 4(sp)        // restore s9
    lw s10, 0(sp)       // restore s10
    addi sp, sp, 20
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_hwcn
    .type   dl_esp32p4_s8_conv2d_hwcn, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_hwcn:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6:

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: filter_height
    # s9: filter_width
    # s10:
    # s11:

    addi sp, sp, -16
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s8, 4(sp)
    sw s9, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw s9, 60(a2)       // filter_y_offset
    lw s8, 144(a2)
    esp.movi.32.q q6, s9, 1  // filter_y_offset
    esp.movi.32.q q6, s8, 2  // filter_n_offset

    esp32p4_s8_conv2d_hwcn_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_hwc16  q0, q1, q2, t5, a3, a4, s0, s1, s8, s9, a2, q6
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_hwcn_loop

    lw s0, 12(sp)       // restore s0
    lw s1, 8(sp)        // restore s1
    lw s8, 4(sp)        // restore s8
    lw s9, 0(sp)        // restore s9
    addi sp, sp, 16
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_hwcn_relu
    .type   dl_esp32p4_s8_conv2d_hwcn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_hwcn_relu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6: activation_alpha/_address

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: filter_height
    # s9: filter_width
    # s10: activation_shift
    # s11:

    addi sp, sp, -20
    sw s0, 16(sp)
    sw s1, 12(sp)
    sw s8, 8(sp)
    sw s9, 4(sp)
    sw s10, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t6, 76(a2)       // activation_alpha
    lw s10, 84(a2)      // activation_shift
    lw s9, 60(a2)       // filter_y_offset
    lw s8, 144(a2)
    esp.movi.32.q q6, s9, 1  // filter_y_offset
    esp.movi.32.q q6, s8, 2  // filter_n_offset

    esp32p4_s8_conv2d_hwcn_relu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_hwc16  q0, q1, q2, t5, a3, a4, s0, s1, s8, s9, a2, q6
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_relu q0, t6, s10
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_hwcn_relu_loop

    lw s0, 16(sp)       // restore s0
    lw s1, 12(sp)       // restore s1
    lw s8, 8(sp)        // restore s8
    lw s9, 4(sp)        // restore s9
    lw s10, 0(sp)       // restore s10
    addi sp, sp, 20
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_conv2d_hwcn_prelu
    .type   dl_esp32p4_s8_conv2d_hwcn_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_conv2d_hwcn_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_div_x_1
    # a5: n_rs3
    # t3: mac_shift
    # t4:
    # t5: moving_input_ptr
    # t6: activation_alpha/_address

    # s0: input dilation x offset
    # s1: input dilation y offset
    # s8: filter_height
    # s9: filter_width
    # s10: activation_shift
    # s11:

    addi sp, sp, -20
    sw s0, 16(sp)
    sw s1, 12(sp)
    sw s8, 8(sp)
    sw s9, 4(sp)
    sw s10, 0(sp)

    esp32p4_s8_conv2d_hwcn_load_args a2, a3, a4, a5, t3, s0, s1
    lw t6, 80(a2)       // activation_alpha_ptr
    lw s10, 84(a2)      // activation_shift
    lw s9, 60(a2)       // filter_y_offset
    lw s8, 144(a2)
    esp.movi.32.q q6, s9, 1  // filter_y_offset
    esp.movi.32.q q6, s8, 2  // filter_n_offset

    esp32p4_s8_conv2d_hwcn_prelu_loop:
        mv t5, a1       // reload input_ptr
        esp.zero.qacc

        esp32p4_s8_conv2d_hwc16  q0, q1, q2, t5, a3, a4, s0, s1, s8, s9, a2, q6
        esp32p4_s8_128b_vector_shift_result q0, t3
        esp32p4_s8_128b_vector_prelu q0, q1, t6, s10
        esp.vst.128.ip q0, a0, 16
        addi a5, a5, -1
        bnez a5, esp32p4_s8_conv2d_hwcn_prelu_loop

    lw s0, 16(sp)       // restore s0
    lw s1, 12(sp)       // restore s1
    lw s8, 8(sp)        // restore s8
    lw s9, 4(sp)        // restore s9
    lw s10, 0(sp)       // restore s10
    addi sp, sp, 20
    ret
