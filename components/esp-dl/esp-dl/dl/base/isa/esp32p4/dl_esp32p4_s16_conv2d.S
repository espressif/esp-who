#include "dl_esp32p4_s16.S"
#include "dl_esp32p4_common.S"


############################################################################################################################################################
####
#### esp32p4_s16_conv2d_11cn series
####
############################################################################################################################################################
.macro esp32p4_s16_conv2d_11c8  input_v0, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, tmp
    # scalar * vecter and accumulate into qacc
    # input_ptr += (c_div_x_1 + 1) * 16 in the end
    # filter_ptr point to the next 16 bytes in the end

    # input_v0:     8 input elements
    # filter_v0:    8 filter elements
    # filter_v1:    8 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 8 - 1

    esp.vld.128.ip  \input_v0,  \input_ptr,  16
    esp.vld.128.ip  \filter_v0, \filter_ptr, 16
    esp.vld.128.ip  \filter_v1, \filter_ptr, 16
    beqz            \c_div_x_1, 1f

    # esp.lp.setup  0, \c_div_x_1, 0f
    mv  \tmp, \c_div_x_1
    0:
        esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
        esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
        esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
        esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
        esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
        esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
        esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
        esp.vsmulas.s16.qacc.ld.incp  \input_v0,  \input_ptr,  \filter_v1, \input_v0, 7
        esp.vld.128.ip                \filter_v1, \filter_ptr, 16
        addi  \tmp, \tmp, -1
        bgtz  \tmp, 0b

    1:
    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s16.qacc                                   \filter_v0, \input_v0, 6
    esp.vsmulas.s16.qacc                                   \filter_v1, \input_v0, 7
.endm



.macro esp32p4_s16_conv2d_11cn_load_args  args, filter_ptr, c_div_x_1, n_rs3, mac_shift
    lw  \n_rs3,      96(\args)  // output_channel_div_8
    lw  \mac_shift,  64(\args)  // mac_shift
    lw  \filter_ptr, 48(\args)  // filter
    lw  \c_div_x_1,  100(\args) // input_channel / x - 1
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_11cn_bias
    .type   dl_esp32p4_s16_conv2d_11cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_11cn_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5: bias_ptr
    # t3:
    # t4:
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
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

    esp32p4_s16_conv2d_11cn_load_args  a2, a3, t1, t6, a4
    lw  a5, 68(a2)      // bias

    esp32p4_s16_conv2d_11cn_bias_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_128b_vector_bias  a5
        esp32p4_s16_conv2d_11c8  q0, q1, q2, t5, a3, t1, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_11cn_bias_loop

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_11cn_bias_relu
    .type   dl_esp32p4_s16_conv2d_11cn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_11cn_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5: bias_ptr
    # t3: activation_alpha/_address
    # t4: activation_shift
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
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

    esp32p4_s16_conv2d_11cn_load_args  a2, a3, t1, t6, a4
    lw  a5, 68(a2)      // bias
    lw  t3, 76(a2)      // activation_alpha
    lw  t4, 84(a2)      // activation_shift

    esp32p4_s16_conv2d_11cn_bias_relu_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_128b_vector_bias  a5
        esp32p4_s16_conv2d_11c8  q0, q1, q2, t5, a3, t1, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_vector_relu  q0, t3, t4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_11cn_bias_relu_loop

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_11cn
    .type   dl_esp32p4_s16_conv2d_11cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_11cn:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5:
    # t3:
    # t4:
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
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

    esp32p4_s16_conv2d_11cn_load_args  a2, a3, t1, t6, a4

    esp32p4_s16_conv2d_11cn_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_11c8  q0, q1, q2, t5, a3, t1, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_11cn_loop

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_11cn_relu
    .type   dl_esp32p4_s16_conv2d_11cn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_11cn_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5:
    # t3: activation_alpha/_address
    # t4: activation_shift
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
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

    esp32p4_s16_conv2d_11cn_load_args  a2, a3, t1, t6, a4
    lw  t3, 76(a2)      // activation_alpha
    lw  t4, 84(a2)      // activation_shift

    esp32p4_s16_conv2d_11cn_relu_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_11c8  q0, q1, q2, t5, a3, t1, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_vector_relu  q0, t3, t4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_11cn_relu_loop

    ret






############################################################################################################################################################
####
#### esp32p4_s16_conv2d_33cn series
####
############################################################################################################################################################

.macro esp32p4_s16_conv2d_33c8  input_v0, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, dilation_x_offset, dilation_y_offset, tmp
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro esp32p4_s16_conv2d_hwcn_load_args  args, filter_ptr, c_div_x_1, n_rs3, mac_shift, dilation_x_offset, dilation_y_offset
    esp32p4_s16_conv2d_11cn_load_args  \args, \filter_ptr, \c_div_x_1, \n_rs3, \mac_shift
    lw  \dilation_x_offset, 108(\args)  // input dilation x offset
    lw  \dilation_y_offset, 112(\args)  // input dilation y offset
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_33cn_bias
    .type   dl_esp32p4_s16_conv2d_33cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_33cn_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5: bias_ptr
    # t3:
    # t4:
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
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

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6
    lw  a5, 68(a2)      // bias

    esp32p4_s16_conv2d_33cn_bias_loop:
        mv  t5, a1     // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_128b_vector_bias  a5
        esp32p4_s16_conv2d_33c8  q0, q1, q2, t5, a3, t1, t2, a6, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_33cn_bias_loop

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_33cn_bias_relu
    .type   dl_esp32p4_s16_conv2d_33cn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_33cn_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5: bias_ptr
    # t3: activation_alpha/_address
    # t4: activation_shift
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
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

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6
    lw  a5, 68(a2)      // bias
    lw  t3, 76(a2)      // activation_alpha
    lw  t4, 84(a2)      // activation_shift

    esp32p4_s16_conv2d_33cn_bias_relu_loop:
        mv  t5, a1     // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_128b_vector_bias  a5
        esp32p4_s16_conv2d_33c8  q0, q1, q2, t5, a3, t1, t2, a6, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_vector_relu  q0, t3, t4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_33cn_bias_relu_loop

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_33cn
    .type   dl_esp32p4_s16_conv2d_33cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_33cn:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5:
    # t3:
    # t4:
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
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

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6

    esp32p4_s16_conv2d_33cn_loop:
        mv  t5, a1     // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_33c8  q0, q1, q2, t5, a3, t1, t2, a6, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_33cn_loop

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_33cn_relu
    .type   dl_esp32p4_s16_conv2d_33cn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_33cn_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5:
    # t3: activation_alpha/_address
    # t4: activation_shift
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp_value
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
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

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6
    lw  t3, 76(a2)      // activation_alpha
    lw  t4, 84(a2)      // activation_shift

    esp32p4_s16_conv2d_33cn_relu_loop:
        mv  t5, a1     // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_33c8  q0, q1, q2, t5, a3, t1, t2, a6, t0
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_vector_relu  q0, t3, t4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_33cn_relu_loop

    ret






############################################################################################################################################################
####
#### esp32p4_s16_conv2d_hwcn series
####
############################################################################################################################################################
.macro esp32p4_s16_conv2d_hwc8  input_v0, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, dilation_x_offset, dilation_y_offset, filter_h, filter_w, args, filter_y_offset, filter_n_offset, tmp
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
            //     esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
            // 3:  add  \input_ptr, \input_ptr, \dilation_x_offset
            3:
                esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
                add  \input_ptr, \input_ptr, \dilation_x_offset
                addi  \filter_w, \filter_w, -1
                bgtz  \filter_w, 3b
        4:
        esp32p4_s16_conv2d_11c8  \input_v0, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \tmp
        add  \filter_ptr, \filter_ptr, \filter_y_offset
        add  \input_ptr, \input_ptr, \dilation_y_offset

        addi  \filter_h, \filter_h, -1
        bnez  \filter_h, 2b

    add  \filter_ptr, \filter_ptr, \filter_n_offset
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_hwcn_bias
    .type   dl_esp32p4_s16_conv2d_hwcn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_hwcn_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5: bias_ptr
    # t3:
    # t4:
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions): filter_height
    # t0(not for extension instructions): filter_width
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
    # s2(not for extension instructions): filter_y_offset
    # s3(not for extension instructions): filter_n_offset
    # s4(not for extension instructions): tmp_value
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s2, s3, s4

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6
    lw  s2, 60(a2)      // filter_y_offset
    lw  s3, 144(a2)     // filter_n_offset
    lw  a5, 68(a2)      // bias

    esp32p4_s16_conv2d_hwcn_bias_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_128b_vector_bias  a5
        esp32p4_s16_conv2d_hwc8  q0, q1, q2, t5, a3, t1, t2, a6, a7, t0, a2, s2, s3, s4
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_hwcn_bias_loop

    esp32p4_pop_12_stacks_3r  s2, s3, s4
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_hwcn_bias_relu
    .type   dl_esp32p4_s16_conv2d_hwcn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_hwcn_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5: bias_ptr
    # t3: activation_alpha/_address
    # t4: activation_shift
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions): filter_height
    # t0(not for extension instructions): filter_width
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
    # s2(not for extension instructions): filter_y_offset
    # s3(not for extension instructions): filter_n_offset
    # s4(not for extension instructions): tmp_value
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s2, s3, s4

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6
    lw  s2, 60(a2)      // filter_y_offset
    lw  s3, 144(a2)     // filter_n_offset
    lw  a5, 68(a2)      // bias
    lw  t3, 76(a2)      // activation_alpha
    lw  t4, 84(a2)      // activation_shift

    esp32p4_s16_conv2d_hwcn_bias_relu_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_128b_vector_bias  a5
        esp32p4_s16_conv2d_hwc8  q0, q1, q2, t5, a3, t1, t2, a6, a7, t0, a2, s2, s3, s4
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_vector_relu  q0, t3, t4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_hwcn_bias_relu_loop

    esp32p4_pop_12_stacks_3r  s2, s3, s4
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_hwcn
    .type   dl_esp32p4_s16_conv2d_hwcn, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_hwcn:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5:
    # t3:
    # t4:
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions): filter_height
    # t0(not for extension instructions): filter_width
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
    # s2(not for extension instructions): filter_y_offset
    # s3(not for extension instructions): filter_n_offset
    # s4(not for extension instructions): tmp_value
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s2, s3, s4

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6
    lw  s2, 60(a2)      // filter_y_offset
    lw  s3, 144(a2)     // filter_n_offset

    esp32p4_s16_conv2d_hwcn_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_hwc8  q0, q1, q2, t5, a3, t1, t2, a6, a7, t0, a2, s2, s3, s4
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_hwcn_loop

    esp32p4_pop_12_stacks_3r  s2, s3, s4
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_conv2d_hwcn_relu
    .type   dl_esp32p4_s16_conv2d_hwcn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_conv2d_hwcn_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: int16_t *filter_ptr
    # a4: mac_shift
    # a5:
    # t3: activation_alpha/_address
    # t4: activation_shift
    # t5: moving_input_ptr
    # t6: n_rs3

    # a6(not for extension instructions): input dilation y offset
    # a7(not for extension instructions): filter_height
    # t0(not for extension instructions): filter_width
    # t1(not for extension instructions): c_div_x_1
    # t2(not for extension instructions): input dilation x offset
    # s2(not for extension instructions): filter_y_offset
    # s3(not for extension instructions): filter_n_offset
    # s4(not for extension instructions): tmp_value
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s2, s3, s4

    esp32p4_s16_conv2d_hwcn_load_args  a2, a3, t1, t6, a4, t2, a6
    lw  s2, 60(a2)      // filter_y_offset
    lw  s3, 144(a2)     // filter_n_offset
    lw  t3, 76(a2)      // activation_alpha
    lw  t4, 84(a2)      // activation_shift

    esp32p4_s16_conv2d_hwcn_relu_loop:
        mv  t5, a1      // reload input_ptr
        esp.zero.qacc

        esp32p4_s16_conv2d_hwc8  q0, q1, q2, t5, a3, t1, t2, a6, a7, t0, a2, s2, s3, s4
        esp32p4_s16_128b_vector_shift_result  q0, a4
        esp32p4_s16_128b_vector_relu  q0, t3, t4
        esp32p4_s16_128b_aligned_vector_store  q0, a0

        addi  t6, t6, -1
        bnez  t6, esp32p4_s16_conv2d_hwcn_relu_loop

    esp32p4_pop_12_stacks_3r  s2, s3, s4
    ret
