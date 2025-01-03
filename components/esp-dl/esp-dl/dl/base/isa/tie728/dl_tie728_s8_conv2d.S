#include "dl_tie728_s8.S"

############################################################################################################################################################
####
#### tie728_s8_conv2d_11cn series
####
############################################################################################################################################################
.macro tie728_s8_conv2d_11c16 input_v0 input_ptr filter_v0 filter_v1 filter_ptr c_div_x_1
    # scalar * vecter and accumulate into QACC
    # input_ptr += (c_div_x_1 + 1) * 16 in the end
    # filter_ptr point to the next 16 bytes in the end

    # input_v0:     16 input elements
    # filter_v0:    16 filter elements
    # filter_v1:    16 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:        input_channel // 16 - 1

    EE.VLD.128.IP \input_v0,  \input_ptr,  16
    EE.VLD.128.IP \filter_v0, \filter_ptr, 16
    EE.VLD.128.IP \filter_v1, \filter_ptr, 16

    loopgtz \c_div_x_1, 0f
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 14
    EE.VSMULAS.S8.QACC.LD.INCP \input_v0,  \input_ptr,  \filter_v1, \input_v0, 15
    EE.VLD.128.IP               \filter_v1, \filter_ptr, 16
0:
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
    EE.VSMULAS.S8.QACC                                  \filter_v0, \input_v0, 14
    EE.VSMULAS.S8.QACC                                  \filter_v1, \input_v0, 15
.endm


############################################################################################################################################################
####
#### tie728_s8_conv2d_11cn
####
############################################################################################################################################################
.macro tie728_s8_conv2d_11cn_load_args args filter_ptr c_div_x_1 n_rs3
    l32i \n_rs3,          \args,  96  // output_channel_div_8
    l32i \filter_ptr,     \args,  48  // filter
    l32i \c_div_x_1,      \args, 100  // input_channel / x - 1
.endm



    .align 4
    .text
    .global dl_tie728_s8_conv2d_11cn
    .type   dl_tie728_s8_conv2d_11cn, @function
    # .section .iram1
dl_tie728_s8_conv2d_11cn:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12:
    # a13:
    # a14:
    # a15: moving_input_ptr
    tie728_s8_conv2d_11cn_load_args a4, a5, a6, a7

    l32i a11, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_11cn


dl_tie728_s8_conv2d_per_layer_11cn:

    beqz a11, tie728_s8_conv2d_per_layer_11cn_no_bias_loop

    tie728_s8_conv2d_per_layer_11cn_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_vector_round_result q0, a8, a15, q3
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_11cn_bias_loop
    retw


    tie728_s8_conv2d_per_layer_11cn_no_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_vector_round_result q0, a8, a15, q3
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_11cn_no_bias_loop
    retw


dl_tie728_s8_conv2d_per_channel_11cn:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_conv2d_per_channel_11cn_no_bias_loop

    tie728_s8_conv2d_per_channel_11cn_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # tie728_s8_conv2d_bias q0, q1, a11
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a11, a15, q2
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_11cn_bias_loop
    retw


    tie728_s8_conv2d_per_channel_11cn_no_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_11cn_no_bias_loop
    retw



    .align 4
    .text
    .global dl_tie728_s8_conv2d_11cn_relu
    .type   dl_tie728_s8_conv2d_11cn_relu, @function
    # .section .iram1
dl_tie728_s8_conv2d_11cn_relu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s8_conv2d_11cn_load_args a4, a5, a6, a7

    l32i a12, a4, 76  # activation_alpha
    l32i a13, a4, 84  # activation_shift

    l32i a11, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_11cn_relu


dl_tie728_s8_conv2d_per_layer_11cn_relu:

    beqz a11, tie728_s8_conv2d_per_layer_11cn_no_bias_relu_loop

    tie728_s8_conv2d_per_layer_11cn_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_11cn_bias_relu_loop
    retw


    tie728_s8_conv2d_per_layer_11cn_no_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_11cn_no_bias_relu_loop
    retw


dl_tie728_s8_conv2d_per_channel_11cn_relu:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_conv2d_per_channel_11cn_no_bias_relu_loop

    tie728_s8_conv2d_per_channel_11cn_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # tie728_s8_conv2d_bias_relu q0, q1, a11, a12, a13
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a11, a15, q2
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_11cn_bias_relu_loop
    retw


    tie728_s8_conv2d_per_channel_11cn_no_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_11cn_no_bias_relu_loop
    retw



    .align 4
    .text
    .global dl_tie728_s8_conv2d_11cn_prelu
    .type   dl_tie728_s8_conv2d_11cn_prelu, @function
    # .section .iram1
dl_tie728_s8_conv2d_11cn_prelu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9:
    # a10:
    # a11: bias_ptr
    # a12: activation_alpha
    # a13: activation_shift
    # a14:
    # a15: moving_input_ptr
    tie728_s8_conv2d_11cn_load_args a4, a5, a6, a7

    l32i a12, a4, 80  # activation_alpha_ptr
    l32i a13, a4, 84  # activation_shift

    l32i a11, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_11cn_prelu


dl_tie728_s8_conv2d_per_layer_11cn_prelu:

    beqz a11, tie728_s8_conv2d_per_layer_11cn_no_bias_prelu_loop

    tie728_s8_conv2d_per_layer_11cn_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_11cn_bias_prelu_loop
    retw


    tie728_s8_conv2d_per_layer_11cn_no_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_11cn_no_bias_prelu_loop
    retw


dl_tie728_s8_conv2d_per_channel_11cn_prelu:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_conv2d_per_channel_11cn_no_bias_prelu_loop

    tie728_s8_conv2d_per_channel_11cn_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # tie728_s8_conv2d_bias_prelu q0, q1, a11, q2, a12, a13
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a11, a15, q2
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_11cn_bias_prelu_loop
    retw


    tie728_s8_conv2d_per_channel_11cn_no_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_11c16 q0, a15, q1, q2, a5, a6
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_11cn_no_bias_prelu_loop
    retw







############################################################################################################################################################
####
#### tie728_s8_unaligned_conv2d_11cn
####
############################################################################################################################################################

.macro tie728_s8_conv2d_1_1_n_remainder_result operation_type output output_ptr mac_shift bias_ptr activation_alpha activation_shift tmp tmp_q1
    #l16si a7, a1, 0
    EE.MOVI.32.A \operation_type, \tmp, 0 # operation type in q7[0]

    bbci \tmp, 3, 7f
    bbci \tmp, 2, 11f
11:  # per_channel bias + prelu, 0x1011
    bbci \tmp, 1, 9f
    bbci \tmp, 0, 10f

    # l16si \tmp, \mac_shift, 0
    # addi \mac_shift, \mac_shift, 2
    # EE.SRS.ACCX \output, \tmp, 0

    # # add bias
    # l8ui \tmp, \bias_ptr, 0 # load bias
    # addi \bias_ptr, \bias_ptr, 1
    # sext \tmp, \tmp, 7
    # add \output, \output, \tmp

    movi \tmp, 4
    EE.SRS.ACCX \output, \tmp, 0

    l16si \tmp, \bias_ptr, 0 # load bias
    addi \bias_ptr, \bias_ptr, 2

    add \output, \output, \tmp # add bias

    l16si \tmp, \mac_shift, 0
    addi \mac_shift, \mac_shift, 2
    addi \tmp, \tmp, -4

    ssr \tmp  # mac_shift-4
    sra \output, \output

    # prelu
    bgez \output, 17f

    l8ui \tmp, \activation_alpha, 0  # load PReLU alpha
    sext \tmp, \tmp, 7
    mull \output, \output, \tmp
    ssr \activation_shift  # activation_shift
    sra \output, \output

    j 17f  # jump to 17f
10:  # per_channel bias + relu, 0x1010
    # l16si \tmp, \mac_shift, 0
    # addi \mac_shift, \mac_shift, 2
    # EE.SRS.ACCX \output, \tmp, 0

    # # add bias
    # l8ui \tmp, \bias_ptr, 0 # load bias
    # addi \bias_ptr, \bias_ptr, 1
    # sext \tmp, \tmp, 7
    # add \output, \output, \tmp

    movi \tmp, 4
    EE.SRS.ACCX \output, \tmp, 0

    l16si \tmp, \bias_ptr, 0 # load bias
    addi \bias_ptr, \bias_ptr, 2

    add \output, \output, \tmp # add bias

    l16si \tmp, \mac_shift, 0
    addi \mac_shift, \mac_shift, 2
    addi \tmp, \tmp, -4

    ssr \tmp  # mac_shift-4
    sra \output, \output

    # Relu or LeakyRelu
    bgez \output, 16f
    mull \output, \output, \activation_alpha
    ssr \activation_shift  # activation_shift
    sra \output, \output

    j 16f  # jump to 16f

9:  # per_channel bias, 0x1001
    bbci \tmp, 0, 8f
    # l16si \tmp, \mac_shift, 0
    # addi \mac_shift, \mac_shift, 2
    # EE.SRS.ACCX \output, \tmp, 0

    # # add bias
    # l8ui \tmp, \bias_ptr, 0 # load bias
    # addi \bias_ptr, \bias_ptr, 1
    # sext \tmp, \tmp, 7
    # add \output, \output, \tmp
    movi \tmp, 4
    EE.SRS.ACCX \output, \tmp, 0

    l16si \tmp, \bias_ptr, 0 # load bias
    addi \bias_ptr, \bias_ptr, 2

    add \output, \output, \tmp # add bias

    l16si \tmp, \mac_shift, 0
    addi \mac_shift, \mac_shift, 2
    addi \tmp, \tmp, -4

    ssr \tmp  # mac_shift-4
    sra \output, \output

    j 16f  # jump to 16f
8:  # per_channel no_bias + prelu, 0x1000
    l16si \tmp, \mac_shift, 0
    addi \mac_shift, \mac_shift, 2
    EE.SRS.ACCX \output, \tmp, 0

    # prelu
    bgez \output, 17f

    l8ui \tmp, \activation_alpha, 0  # load PReLU alpha
    sext \tmp, \tmp, 7
    mull \output, \output, \tmp
    ssr \activation_shift  # activation_shift
    sra \output, \output

    j 17f  # jump to 16f

7: # per_channel no_bias + relu, 0x111
    bbci \tmp, 2, 3f
    bbci \tmp, 1, 5f
    bbci \tmp, 0, 6f

    l16si \tmp, \mac_shift, 0

    addi \mac_shift, \mac_shift, 2
    EE.SRS.ACCX \output, \tmp, 0

    # Relu or LeakyRelu
    bgez \output, 16f
    mull \output, \output, \activation_alpha
    ssr \activation_shift  # activation_shift
    sra \output, \output

    j 16f  # jump to 16f

6:  # per_channel no_bias, 0x110
    l16si \tmp, \mac_shift, 0
    addi \mac_shift, \mac_shift, 2
    EE.SRS.ACCX \output, \tmp, 0


    j 16f  # jump to 16f

5:  # remainder == 4, 5
    bbci \tmp, 0, 4f
    # per_layer bias + prelu, 0x101
    # EE.SRS.ACCX \output, \mac_shift, 0
    tie728_s8_element_round_result  \output, \mac_shift, \tmp, \tmp_q1

    # bias will be preload
    # # add bias
    # l8ui \tmp, \bias_ptr, 0 # load bias
    # addi \bias_ptr, \bias_ptr, 1
    # sext \tmp, \tmp, 7
    # add \output, \output, \tmp

    # prelu
    bgez \output, 17f

    l8ui \tmp, \activation_alpha, 0  # load PReLU alpha
    sext \tmp, \tmp, 7
    mull \output, \output, \tmp
    ssr \activation_shift  # activation_shift
    sra \output, \output

    j 17f  # jump to 17f

4:  # per_layer bias + relu, 0x100
    # EE.SRS.ACCX \output, \mac_shift, 0
    tie728_s8_element_round_result  \output, \mac_shift, \tmp, \tmp_q1

    # bias will be preload
    # # add bias
    # l8ui \tmp, \bias_ptr, 0 # load bias
    # addi \bias_ptr, \bias_ptr, 1
    # sext \tmp, \tmp, 7
    # add \output, \output, \tmp

    # Relu or LeakyRelu
    bgez \output, 16f
    mull \output, \output, \activation_alpha
    ssr \activation_shift  # activation_shift
    sra \output, \output


    j 16f  # jump to 16f

3:  # remainder == 1, 2, 3
    bbci \tmp, 1, 1f
    bbci \tmp, 0, 2f
    # per_layer bias, 0x011
    # EE.SRS.ACCX \output, \mac_shift, 0
    tie728_s8_element_round_result  \output, \mac_shift, \tmp, \tmp_q1

    # bias will be preload
    # # add bias
    # l8ui \tmp, \bias_ptr, 0 # load bias
    # addi \bias_ptr, \bias_ptr, 1
    # sext \tmp, \tmp, 7
    # add \output, \output, \tmp

    j 16f  # jump to 16f

2:  # per_layer no_bias + prelu, 0x010
    # EE.SRS.ACCX \output, \mac_shift, 0
    tie728_s8_element_round_result  \output, \mac_shift, \tmp, \tmp_q1

    # prelu
    bgez \output, 17f

    l8ui \tmp, \activation_alpha, 0  # load PReLU alpha
    sext \tmp, \tmp, 7
    mull \output, \output, \tmp
    ssr \activation_shift  # activation_shift
    sra \output, \output

    j 17f  # jump to 17f

1:  # per_layer no_bias + relu, 0x001
    bbci \tmp, 0, 0f

    # EE.SRS.ACCX \output, \mac_shift, 0
    tie728_s8_element_round_result  \output, \mac_shift, \tmp, \tmp_q1

    # Relu or LeakyRelu
    bgez \output, 16f
    mull \output, \output, \activation_alpha
    ssr \activation_shift  # activation_shift
    sra \output, \output

    j 16f  # jump to 16f

0:  # per_layer no_bias
    # EE.SRS.ACCX \output, \mac_shift, 0
    tie728_s8_element_round_result  \output, \mac_shift, \tmp, \tmp_q1
    j 16f  # jump to 16f

17: # update prelu ptr
    addi \activation_alpha, \activation_alpha, 1
16:
    clamps \output, \output, 7
    s8i \output, \output_ptr, 0
    addi \output_ptr, \output_ptr, 1
.endm



.macro tie728_s8_conv2d_1_1_unaligned_c input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_v1 filter_ptr c_div_x_1 remainder_c input_sar
    # scalar * vecter and accumulate into QACC
    # input_ptr += (c_div_x_1 + 1) * 16 in the end
    # filter_ptr point to the next 16 bytes in the end

    # input_v0:     16 input elements
    # filter_v0:    16 filter elements
    # filter_v1:    16 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 16 - 1

    blti \c_div_x_1, 0, 17f # input_channel < 16

    EE.LD.128.USAR.IP \input_front_aligned, \input_ptr, 16


    EE.VLD.128.IP \filter_v0, \filter_ptr, 16
    EE.VLD.128.IP \filter_v1, \filter_ptr, 16
    EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 16

    loopgtz \c_div_x_1, 0f
        # EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 16
        EE.SRC.Q.QUP \input_v0, \input_front_aligned, \input_back_aligned
        EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 16

        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 14
        EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 15

    0:


    # EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \remainder_c
    EE.SRC.Q.QUP \input_v0, \input_front_aligned, \input_back_aligned
    addi \input_ptr, \input_ptr, -16
    add \input_ptr, \input_ptr, \remainder_c #input_ptr and the end of c


    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 14
    EE.VSMULAS.S8.QACC \filter_v1, \input_v0, 15


    beqz \remainder_c, 16f #no c_remainder

    EE.VLD.128.IP \filter_v0, \filter_ptr, 16

    # rur.sar_byte \input_sar #input sar
    # EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 0
    # wur.sar_byte \input_sar #input sar
    EE.VLD.128.IP \input_back_aligned, \input_ptr, 0
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned
    j 15f


17: # input_channel < 16
    EE.VLD.128.IP \filter_v0, \filter_ptr,  16

    EE.LD.128.USAR.XP \input_front_aligned, \input_ptr, \remainder_c
    # rur.sar_byte \input_sar #input sar
    # EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 0
    # wur.sar_byte \input_sar #input sar
    EE.VLD.128.IP \input_back_aligned, \input_ptr, 0
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned


15: # remainder_c == 15, 0x1111
    bbci \remainder_c, 3, 7f

    EE.VLD.128.IP \filter_v1, \filter_ptr,  16

    bbci \remainder_c, 2, 11f
    bbci \remainder_c, 1, 13f
    bbci \remainder_c, 0, 14f
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 13
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 14
    j 16f  # jump to 16f

14:  # remainder_c == 14, 0x1110
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 12
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 13
    j 16f  # jump to 16f

13:  # remainder_c == 13, 0x1101
    bbci \remainder_c, 0, 12f
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 11
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 12
    j 16f  # jump to 16f

12:  # remainder_c == 12, 0x1100
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 10
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 11
    j 16f  # jump to 16f

11:  # remainder_c == 11, 0x1011
    bbci \remainder_c, 1, 9f
    bbci \remainder_c, 0, 10f
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 9
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 10
    j 16f  # jump to 16f
10:  # remainder_c == 10, 0x1010
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 8
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 9
    j 16f  # jump to 16f
9:  # remainder_c == 9, 0x1001
    bbci \remainder_c, 0, 8f
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 7
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 8
    j 16f  # jump to 16f
8:  # remainder_c == 8, 0x1000
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC  \filter_v0, \input_v0, 6
    EE.VSMULAS.S8.QACC  \filter_v1, \input_v0, 7
    j 16f  # jump to 16f

7: # remainder == 7, 0x111
    bbci \remainder_c, 2, 3f

    EE.VLD.128.IP \filter_v1, \filter_ptr,  16
    bbci \remainder_c, 1, 5f
    bbci \remainder_c, 0, 6f
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC \filter_v1, \input_v0, 5
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 6
    j 16f  # jump to 16f

6:  # remainder == 6, 0x110
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 4
    EE.VSMULAS.S8.QACC \filter_v1, \input_v0, 5
    j 16f  # jump to 16f

5:  # remainder == 4, 5
    bbci \remainder_c, 0, 4f
    # remainder == 5, 0x101
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC \filter_v1, \input_v0, 3
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 4
    j 16f  # jump to 16f

4:  # remainder == 4, 0x100
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 2
    EE.VSMULAS.S8.QACC \filter_v1, \input_v0, 3
    j 16f  # jump to 16f

3:  # remainder == 1, 2, 3
    bbci \remainder_c, 1, 1f

    EE.VLD.128.IP \filter_v1, \filter_ptr,  16

    bbci \remainder_c, 0, 2f
    # remainder == 3, 0x011
    EE.VSMULAS.S8.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC \filter_v1, \input_v0, 1
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 2
    j 16f  # jump to 16f

2:  # remainder == 2, 0x010
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 0
    EE.VSMULAS.S8.QACC \filter_v1, \input_v0, 1
    j 16f  # jump to 16f

1:  # remainder == 1, 0x001
    EE.VSMULAS.S8.QACC \filter_v0, \input_v0, 0

16:
.endm



.macro tie728_s8_conv2d_1_1_c_n_remainder input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_front_aligned filter_back_aligned filter_ptr c_div_x_1 remainder_c input_sar filter_sar
    # scalar * vecter and accumulate into QACC
    # input_ptr += (c_div_x_1 + 1) * 16 in the end
    # filter_ptr point to the next 16 bytes in the end

    # input_v0:     16 input elements
    # filter_v0:    16 filter elements
    # filter_v1:    16 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:        input_channel // 16 - 1

    blti \c_div_x_1, 0, 17f # input_channel < 16

    EE.LD.128.USAR.IP \input_front_aligned, \input_ptr, 16
    EE.LD.128.USAR.IP \filter_front_aligned, \filter_ptr, 16
    EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 16

    loopgtz \c_div_x_1, 0f
        EE.SRC.Q.QUP \input_v0, \input_front_aligned, \input_back_aligned

        EE.LD.128.USAR.IP \filter_back_aligned, \filter_ptr, 16
        EE.SRC.Q.QUP \filter_v0, \filter_front_aligned, \filter_back_aligned

        EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 16
        EE.VMULAS.S8.ACCX \filter_v0, \input_v0
    0:


    # EE.LD.128.USAR.XP \input_back_aligned, \input_ptr, \remainder_c
    EE.SRC.Q.QUP \input_v0, \input_front_aligned, \input_back_aligned
    rur.sar_byte \input_sar #input sar
    addi \input_ptr, \input_ptr, -16

    EE.LD.128.USAR.XP \filter_back_aligned, \filter_ptr, \remainder_c
    add \input_ptr, \input_ptr, \remainder_c #input_ptr and the end of c
    EE.SRC.Q.QUP \filter_v0, \filter_front_aligned, \filter_back_aligned

    EE.VMULAS.S8.ACCX \filter_v0, \input_v0

    beqz \remainder_c, 16f #no remainder_c


    # filter remainder is in one 128bit then filter_back_aligned = filter_front_aligned
    # rur.sar_byte \filter_sar #filter sar
    # EE.LD.128.USAR.IP \filter_back_aligned, \filter_ptr, 0
    # wur.sar_byte \filter_sar #filter sar
    EE.VLD.128.IP \filter_back_aligned, \filter_ptr, 0
    EE.SRC.Q \filter_v0, \filter_front_aligned, \filter_back_aligned


    # input remainder is in one 128bit then input_back_aligned = input_front_aligned
    EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 0
    wur.sar_byte \input_sar #input sar
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned #right shift delete the low sar_byte


    #left shift to make the rest part(15 - remainder_c) 0 EE.SLXCCP left shift a0+1
    movi \filter_sar, 15
    sub  \filter_sar, \filter_sar, \remainder_c
    movi \input_sar, 0
    EE.SLCXXP.2Q \input_back_aligned, \input_v0, \filter_sar, \input_sar #left shift to make the rest part 0
    EE.SLCXXP.2Q \filter_back_aligned, \filter_v0, \filter_sar, \input_sar

    EE.VMULAS.S8.ACCX \filter_v0, \input_v0

    j 16f

17: # input_channel < 16
    # filter remainder is in one 128bit then filter_back_aligned = filter_front_aligned
    EE.LD.128.USAR.XP \filter_front_aligned, \filter_ptr, \remainder_c
    # rur.sar_byte \filter_sar #filter sar
    # EE.LD.128.USAR.IP \filter_back_aligned, \filter_ptr, 0
    # wur.sar_byte \filter_sar #filter sar
    EE.VLD.128.IP \filter_back_aligned, \filter_ptr, 0
    EE.SRC.Q \filter_v0, \filter_front_aligned, \filter_back_aligned


    # input remainder is in one 128bit then input_back_aligned = input_front_aligned
    EE.LD.128.USAR.XP \input_front_aligned, \input_ptr, \remainder_c
    # rur.sar_byte \input_sar #input sar
    # EE.LD.128.USAR.IP \input_back_aligned, \input_ptr, 0
    # wur.sar_byte \input_sar #input sar
    EE.VLD.128.IP \input_back_aligned, \input_ptr, 0
    EE.SRC.Q \input_v0, \input_front_aligned, \input_back_aligned


    #left shift to make the rest part(15 - remainder_c) 0 EE.SLXCCP left shift a0+1
    movi \filter_sar, 15
    sub  \filter_sar, \filter_sar, \remainder_c
    movi \input_sar, 0
    EE.SLCXXP.2Q \input_back_aligned, \input_v0, \filter_sar, \input_sar #left shift to make the rest part 0
    EE.SLCXXP.2Q \filter_back_aligned, \filter_v0, \filter_sar, \input_sar

    EE.VMULAS.S8.ACCX \filter_v0, \input_v0

16:
.endm



    .align 4
    .text
    .global dl_tie728_s8_unaligned_conv2d_11cn
    .type   dl_tie728_s8_unaligned_conv2d_11cn, @function
    # .section .iram1
dl_tie728_s8_unaligned_conv2d_11cn:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs4 / input_sar
    # a8: channel_factor
    # a9: c_remainder
    # a10: operation_type / n_remainder
    # a11: bias_ptr
    # a12:
    # a13:
    # a14: input_sar
    # a15: moving_input_ptr

    tie728_s8_conv2d_11cn_load_args a4, a5, a6, a7
    # l32i a7, a4,  96  // output_channel_div_8
    # l32i a5, a4,  48  // filter
    # l32i a6, a4, 100  // input_channel / x - 1
    l32i a9, a4, 136  # c_remainder: c % 16



    l32i a8,  a4, 64  // mac_shift
    l32i a11, a4, 68  // bias
    #l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift
    l32i a12, a4, 80  // activation_alpha_ptr


    tie728_s8_unaligned_conv2d_operation_type a10, a8, a11, a13, a12, a4


    beqz a7, tie728_s8_conv2d_1_1_unaligned_c_n_loop_end # output_channel < 16

    EE.LD.128.USAR.IP q1, a2, 0
    rur.sar_byte a14

    beqi a14, 0, tie728_s8_conv2d_1_1_unaligned_c_n_loop0
    beqi a14, 8, tie728_s8_unaligned_conv2d_11cn_loop8

# output sar_byte != 0 && != 8
    tie728_s8_unaligned_conv2d_11cn_loop:
        mov a15, a3    # reload input_ptr
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a11, tie728_s8_unaligned_conv2d_11cn_loop_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_unaligned_conv2d_11cn_loop_no_preload_bias:
        tie728_s8_conv2d_1_1_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a14
        tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a11, a12, a13, a15, q1, q2

        # store to unaligned address
        dl_tie728_s8_unaligned_store0 q0, a2, a14

        addi a7, a7, -1
    bnez a7, tie728_s8_unaligned_conv2d_11cn_loop
    j tie728_s8_conv2d_1_1_unaligned_c_n_loop_end

# output sar_byte == 0
    tie728_s8_conv2d_1_1_unaligned_c_n_loop0:
        mov a15, a3    # reload input_ptr
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a11, tie728_s8_conv2d_1_1_unaligned_c_n_loop0_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_conv2d_1_1_unaligned_c_n_loop0_no_preload_bias:
        tie728_s8_conv2d_1_1_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a14
        tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a11, a12, a13, a15, q1, q2

        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_1_1_unaligned_c_n_loop0
    j tie728_s8_conv2d_1_1_unaligned_c_n_loop_end

# output sar_byte == 8
    tie728_s8_unaligned_conv2d_11cn_loop8:
        mov a15, a3    # reload input_ptr
        EE.ZERO.QACC

        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a11, tie728_s8_unaligned_conv2d_11cn_loop8_no_preload_bias
            tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_unaligned_conv2d_11cn_loop8_no_preload_bias:
        tie728_s8_conv2d_1_1_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a14
        tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a11, a12, a13, a15, q1, q2

        #store to unaligned address
        dl_tie728_s8_unaligned_store1 q0, a2

        addi a7, a7, -1
    bnez a7, tie728_s8_unaligned_conv2d_11cn_loop8
    j tie728_s8_conv2d_1_1_unaligned_c_n_loop_end


tie728_s8_conv2d_1_1_unaligned_c_n_loop_end:


    # handle the n remainder
    # s16i a10, a1, 0
    EE.MOVI.32.Q q7, a10, 0 # store operation type in q7[0]
    l32i a10, a4, 140  # n % 16 remainder_n
    beqz a10, dl_tie728_s8_unaligned_conv2d_11cn_end


    # n_remainder
    tie728_s8_conv2d_1_1_c_unaligned_n_loop:
        mov a15, a3    # reload input_ptr
        EE.ZERO.ACCX
        # complete one n in ACCX
        # Without modifications specifically for per-channel, there may be issues with per-channel
        beqz a11, tie728_s8_conv2d_1_1_c_unaligned_n_loop_no_preload_bias
            tie728_s8_conv2d_element_bias a11
        tie728_s8_conv2d_1_1_c_unaligned_n_loop_no_preload_bias:
        tie728_s8_conv2d_1_1_c_n_remainder q0, q1, q2, a15, q3, q4, q5, a5, a6, a9, a14, a7
        tie728_s8_conv2d_1_1_n_remainder_result q7, a7, a2, a8, a11, a12, a13, a14, q0
        # l16si a7, a1, 0
        # mov a14, a7

        # EE.SRS.ACCX a14, a8, 0
        # clamps a14, a14, 7
        # s8i a14, a2, 0
        # addi a2, a2, 1

        addi a10, a10, -1
    bnez a10, tie728_s8_conv2d_1_1_c_unaligned_n_loop

dl_tie728_s8_unaligned_conv2d_11cn_end:

    retw














############################################################################################################################################################
####
#### tie728_s16_conv2d_33cn series
####
############################################################################################################################################################
.macro tie728_s8_conv2d_33c16 input_v0 filter_v0 filter_v1 input_ptr filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro tie728_s8_conv2d_hwcn_load_args args filter_ptr c_div_x_1  n_rs3 dilation_x_offset dilation_y_offset
    l32i \n_rs3,          \args,  96  // output_channel_div_8
    l32i \filter_ptr,     \args,  48  // filter
    l32i \c_div_x_1,      \args, 100  // input_channel / x - 1
    l32i \dilation_x_offset, \args, 108  // input dilation x offset
    l32i \dilation_y_offset, \args, 112  // input dilation y offset
.endm



    .align 4
    .text
    .global dl_tie728_s8_conv2d_33cn
    .type   dl_tie728_s8_conv2d_33cn, @function
    # .section .iram1
dl_tie728_s8_conv2d_33cn:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: bias_ptr
    # a12:
    # a13:
    # a14:
    # a15: moving_input_ptr
    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10

    l32i a11, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_33cn


dl_tie728_s8_conv2d_per_layer_33cn:

    beqz a11, tie728_s8_conv2d_per_layer_33cn_no_bias_loop

    tie728_s8_conv2d_per_layer_33cn_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_vector_round_result q0, a8, a15, q3
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_33cn_bias_loop
    retw


    tie728_s8_conv2d_per_layer_33cn_no_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_vector_round_result q0, a8, a15, q3
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_33cn_no_bias_loop
    retw


dl_tie728_s8_conv2d_per_channel_33cn:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_conv2d_per_channel_33cn_no_bias_loop

    tie728_s8_conv2d_per_channel_33cn_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # tie728_s8_conv2d_bias q0, q1, a11
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a11, a15, q2
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_33cn_bias_loop
    retw


    tie728_s8_conv2d_per_channel_33cn_no_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_33cn_no_bias_loop
    retw



    .align 4
    .text
    .global dl_tie728_s8_conv2d_33cn_relu
    .type   dl_tie728_s8_conv2d_33cn_relu, @function
    # .section .iram1
dl_tie728_s8_conv2d_33cn_relu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
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
    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10

    l32i a12, a4, 76  # activation_alpha
    l32i a13, a4, 84  # activation_shift

    l32i a11, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_33cn_relu


dl_tie728_s8_conv2d_per_layer_33cn_relu:

    beqz a11, tie728_s8_conv2d_per_layer_33cn_no_bias_relu_loop

    tie728_s8_conv2d_per_layer_33cn_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_33cn_bias_relu_loop
    retw


    tie728_s8_conv2d_per_layer_33cn_no_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_33cn_no_bias_relu_loop
    retw


dl_tie728_s8_conv2d_per_channel_33cn_relu:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_conv2d_per_channel_33cn_no_bias_relu_loop

    tie728_s8_conv2d_per_channel_33cn_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # tie728_s8_conv2d_bias_relu q0, q1, a11, a12, a13
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a11, a15, q2
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_33cn_bias_relu_loop
    retw


    tie728_s8_conv2d_per_channel_33cn_no_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        tie728_s8_conv2d_relu q0, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_33cn_no_bias_relu_loop
    retw



    .align 4
    .text
    .global dl_tie728_s8_conv2d_33cn_prelu
    .type   dl_tie728_s8_conv2d_33cn_prelu, @function
    # .section .iram1
dl_tie728_s8_conv2d_33cn_prelu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
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
    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10

    l32i a12, a4, 80  # activation_alpha_ptr
    l32i a13, a4, 84  # activation_shift

    l32i a11, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_33cn_prelu


dl_tie728_s8_conv2d_per_layer_33cn_prelu:

    beqz a11, tie728_s8_conv2d_per_layer_33cn_no_bias_prelu_loop

    tie728_s8_conv2d_per_layer_33cn_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a11
        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_33cn_bias_prelu_loop
    retw


    tie728_s8_conv2d_per_layer_33cn_no_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_33cn_no_bias_prelu_loop
    retw


dl_tie728_s8_conv2d_per_channel_33cn_prelu:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a11, tie728_s8_conv2d_per_channel_33cn_no_bias_prelu_loop

    tie728_s8_conv2d_per_channel_33cn_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # tie728_s8_conv2d_bias_prelu q0, q1, a11, q2, a12, a13
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a11, a15, q2
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_33cn_bias_prelu_loop
    retw


    tie728_s8_conv2d_per_channel_33cn_no_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_33c16 q0, q1, q2, a15, a5, a6, a9, a10
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        tie728_s8_conv2d_prelu q0, q2, a12, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_33cn_no_bias_prelu_loop
    retw




############################################################################################################################################################
####
#### tie728_s8_unaligned_conv2d_33cn
####
############################################################################################################################################################


.macro tie728_s8_conv2d_3_3_unaligned_c input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_v1 filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset remainder_c input_sar
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm




.macro tie728_s8_conv2d_3_3_c_n_remainder input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_front_aligned filter_back_aligned filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset remainder_c input_sar filter_sar

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \input_sar, \filter_sar
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



    .align 4
    .text
    .global dl_tie728_s8_unaligned_conv2d_33cn
    .type   dl_tie728_s8_unaligned_conv2d_33cn, @function
    # .section .iram1
dl_tie728_s8_unaligned_conv2d_33cn:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs4 / input_sar
    # a8: channel_factor
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a10:  / c_remainder
    # a11: bias_ptr
    # a12: activation_alpha
    # a13: activation_shift / n_remainder
    # a14: tmp variable: input_sar / operation_type q7[0]
    # a15: moving_input_ptr

    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10
    # l32i a7, a4,  96  // output_channel_div_8
    # l32i a5, a4,  48  // filter
    # l32i a6, a4, 100  // input_channel / x - 1

    l32i a8,  a4, 64  // mac_shift
    l32i a11, a4, 68  // bias
    #l32i a12, a4, 76  // activation_alpha
    l32i a13, a4, 84  // activation_shift
    l32i a12, a4, 80  // activation_alpha_ptr

    tie728_s8_unaligned_conv2d_operation_type a14, a8, a11, a13, a12, a4
    EE.MOVI.32.Q q7, a14, 0  # operation_type q7[0]

    beqz a7, tie728_s8_conv2d_3_3_unaligned_c_n_loop_end # output_channel < 16

    EE.LD.128.USAR.IP q1, a2, 0  # output sar_byte
    rur.sar_byte a14

    bgez a13, tie728_s8_conv2d_3_3_unaligned_c_n_activation
        l32i a13, a4, 136  # c_remainder: c % 16, replace activation_shift
        EE.MOVI.32.A q7, a12, 0  # operation_type, replace activation_alpha

        beqi a14, 0, tie728_s8_conv2d_3_3_unaligned_c_n_loop0
        beqi a14, 8, tie728_s8_conv2d_3_3_unaligned_c_n_loop8

    # output sar_byte != 0 && != 8
        tie728_s8_conv2d_3_3_unaligned_c_n_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_unaligned_c_n_loop_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a11
            tie728_s8_conv2d_3_3_unaligned_c_n_loop_no_preload_bias:
            tie728_s8_conv2d_3_3_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a10, a13, a14
            tie728_s8_conv2d_1_1_unaligned_c_result a12, q0, a8, a11, a12, a13, a15, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store0 q0, a2, a14

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_3_3_unaligned_c_n_loop
        j tie728_s8_conv2d_3_3_unaligned_c_n_loop_end

    # output sar_byte == 0
        tie728_s8_conv2d_3_3_unaligned_c_n_loop0:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_unaligned_c_n_loop0_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a11
            tie728_s8_conv2d_3_3_unaligned_c_n_loop0_no_preload_bias:
            tie728_s8_conv2d_3_3_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a10, a13, a14
            tie728_s8_conv2d_1_1_unaligned_c_result a12, q0, a8, a11, a12, a13, a15, q1, q2

            EE.VST.128.IP q0, a2, 16

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_3_3_unaligned_c_n_loop0
        j tie728_s8_conv2d_3_3_unaligned_c_n_loop_end

    # output sar_byte == 8
        tie728_s8_conv2d_3_3_unaligned_c_n_loop8:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_unaligned_c_n_loop8_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a11
            tie728_s8_conv2d_3_3_unaligned_c_n_loop8_no_preload_bias:
            tie728_s8_conv2d_3_3_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a10, a13, a14
            tie728_s8_conv2d_1_1_unaligned_c_result a12, q0, a8, a11, a12, a13, a15, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store1 q0, a2

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_3_3_unaligned_c_n_loop8
        j tie728_s8_conv2d_3_3_unaligned_c_n_loop_end

    tie728_s8_conv2d_3_3_unaligned_c_n_activation:
        beqi a14, 0, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop0
        beqi a14, 8, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop8

    # output sar_byte != 0 && != 8
        tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a11
            tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop_no_preload_bias:
            l32i a13, a4, 136  # c_remainder: c % 16, replace activation_shift
            tie728_s8_conv2d_3_3_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a10, a13, a14

            EE.MOVI.32.A q7, a14, 0 # operation_type
            l32i a13, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_unaligned_c_result a14, q0, a8, a11, a12, a13, a15, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store0 q0, a2, a14

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop
        j tie728_s8_conv2d_3_3_unaligned_c_n_loop_end

    # output sar_byte == 0
        tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop0:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop0_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a11
            tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop0_no_preload_bias:
            l32i a13, a4, 136  # c_remainder: c % 16, replace activation_shift
            tie728_s8_conv2d_3_3_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a10, a13, a14

            EE.MOVI.32.A q7, a14, 0 # operation_type
            l32i a13, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_unaligned_c_result a14, q0, a8, a11, a12, a13, a15, q1, q2

            EE.VST.128.IP q0, a2, 16

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop0
        j tie728_s8_conv2d_3_3_unaligned_c_n_loop_end

    # output sar_byte == 8
        tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop8:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop8_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a11
            tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop8_no_preload_bias:
            l32i a13, a4, 136  # c_remainder: c % 16, replace activation_shift
            tie728_s8_conv2d_3_3_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a9, a10, a13, a14

            EE.MOVI.32.A q7, a14, 0 # operation_type
            l32i a13, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_unaligned_c_result a14, q0, a8, a11, a12, a13, a15, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store1 q0, a2

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_3_3_unaligned_c_n_activation_loop8
        j tie728_s8_conv2d_3_3_unaligned_c_n_loop_end

tie728_s8_conv2d_3_3_unaligned_c_n_loop_end:


    # handle the n remainder
    l32i a13, a4, 140  # n % 16 remainder_n
    beqz a13, dl_tie728_s8_unaligned_conv2d_33cn_end

    l32i a14, a4, 84  # activation_shift

    bgez a14, tie728_s8_conv2d_3_3_c_unaligned_n_activation
        l32i a12, a4, 136  # c_remainder: c % 16, replace activation_shift
        # n_remainder
        tie728_s8_conv2d_3_3_c_unaligned_n_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.ACCX

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_c_unaligned_n_loop_no_preload_bias
                tie728_s8_conv2d_element_bias a11
            tie728_s8_conv2d_3_3_c_unaligned_n_loop_no_preload_bias:

            # complete one n in ACCX
            tie728_s8_conv2d_3_3_c_n_remainder q0, q1, q2, a15, q3, q4, q5, a5, a6, a9, a10, a12, a14, a7
            tie728_s8_conv2d_1_1_n_remainder_result q7, a7, a2, a8, a11, a12, a13, a14, q0

            addi a13, a13, -1
        bnez a13, tie728_s8_conv2d_3_3_c_unaligned_n_loop
        j dl_tie728_s8_unaligned_conv2d_33cn_end

    tie728_s8_conv2d_3_3_c_unaligned_n_activation:
        EE.MOVI.32.Q q6, a13, 0 # store remainder_n in q6[0]

        # n_remainder
        tie728_s8_conv2d_3_3_c_unaligned_n_activation_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.ACCX

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a11, tie728_s8_conv2d_3_3_c_unaligned_n_activation_loop_no_preload_bias
                tie728_s8_conv2d_element_bias a11
            tie728_s8_conv2d_3_3_c_unaligned_n_activation_loop_no_preload_bias:

            # complete one n in ACCX
            l32i a13, a4, 136  # c_remainder: c % 16, replace activation_shift
            tie728_s8_conv2d_3_3_c_n_remainder q0, q1, q2, a15, q3, q4, q5, a5, a6, a9, a10, a13, a14, a7

            l32i a13, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_n_remainder_result q7, a7, a2, a8, a11, a12, a13, a14, q0

            EE.MOVI.32.A q6, a13, 0 # remainder_n in q6[0]
            addi a13, a13, -1
            EE.MOVI.32.Q q6, a13, 0
        bnez a13, tie728_s8_conv2d_3_3_c_unaligned_n_activation_loop


dl_tie728_s8_unaligned_conv2d_33cn_end:

    retw








############################################################################################################################################################
####
#### tie728_s8_conv2d_hwcn series
####
############################################################################################################################################################

.macro tie728_s8_conv2d_hwc16 input_v0 input_ptr filter_v0 filter_v1 filter_ptr c_div_x_1 dilation_x_offset dilation_y_offset filter_h filter_w filter_offset_q, args
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)
    # filter_h
    # filter_w

    l32i \filter_w,          \args,  56  # filter_width
    blti \filter_w, 2, 3f

    l32i \filter_h,          \args,  52  # filter_height
    1:
        l32i \filter_w,          \args,  56  # filter_width
        2:
            tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
            add \input_ptr, \input_ptr, \dilation_x_offset

            addi \filter_w, \filter_w, -1
        bgei \filter_w, 2, 2b

        tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1

        EE.MOVI.32.A \filter_offset_q, \filter_w, 1
        add \filter_ptr, \filter_ptr, \filter_w
        add \input_ptr, \input_ptr, \dilation_y_offset

        addi \filter_h, \filter_h, -1
    bnez \filter_h, 1b
    j 5f

    # filter_w == 1
    3:
    l32i \filter_h,          \args,  52  # filter_height
    EE.MOVI.32.A \filter_offset_q, \filter_w, 1
    4:
        tie728_s8_conv2d_11c16 \input_v0, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1
        add \filter_ptr, \filter_ptr, \filter_w
        add \input_ptr, \input_ptr, \dilation_y_offset

        addi \filter_h, \filter_h, -1
    bnez \filter_h, 4b

    5:
    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm



    .align 4
    .text
    .global dl_tie728_s8_conv2d_hwcn
    .type   dl_tie728_s8_conv2d_hwcn, @function
    # .section .iram1
dl_tie728_s8_conv2d_hwcn:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height
    # a12: filter_width
    # a13: bias_ptr
    # a14: filter_y_offset
    # a15: moving_input_ptr
    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10

    l32i a13, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1 #filter_y_offset
    EE.MOVI.32.Q q6, a11, 2 #filter_n_offset

dl_tie728_s8_conv2d_per_layer_hwcn:

    beqz a13, tie728_s8_conv2d_per_layer_hwcn_no_bias_loop

    tie728_s8_conv2d_per_layer_hwcn_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a13
        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_vector_round_result q0, a8, a15, q3
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_hwcn_bias_loop
    retw


    tie728_s8_conv2d_per_layer_hwcn_no_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_vector_round_result q0, a8, a15, q3
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_hwcn_no_bias_loop
    retw


dl_tie728_s8_conv2d_per_channel_hwcn:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a13, tie728_s8_conv2d_per_channel_hwcn_no_bias_loop

    tie728_s8_conv2d_per_channel_hwcn_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # tie728_s8_conv2d_bias q0, q1, a13
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a13, a15, q2
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_hwcn_bias_loop
    retw


    tie728_s8_conv2d_per_channel_hwcn_no_bias_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_hwcn_no_bias_loop
    retw



    .align 4
    .text
    .global dl_tie728_s8_conv2d_hwcn_relu
    .type   dl_tie728_s8_conv2d_hwcn_relu, @function
    # .section .iram1
dl_tie728_s8_conv2d_hwcn_relu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs3
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height / activation_shift
    # a12: filter_width
    # a13: bias_ptr
    # a14: activation_alpha
    # a15: moving_input_ptr
    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10

    l32i a14, a4, 76  # activation_alpha
    # l32i a13, a4, 84  # activation_shift

    l32i a13, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    l32i a12, a4, 60    # filter_y_offset
    l32i a11, a4, 144   # filter_n_offset
    EE.MOVI.32.Q q6, a12, 1
    EE.MOVI.32.Q q6, a11, 2
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_hwcn_relu


dl_tie728_s8_conv2d_per_layer_hwcn_relu:

    beqz a13, tie728_s8_conv2d_per_layer_hwcn_no_bias_relu

    tie728_s8_conv2d_per_layer_hwcn_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a13
        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_vector_round_result q0, a8, a15, q3
        l32i a11, a4, 84  # activation_shift
        tie728_s8_conv2d_relu q0, a14, a11
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_hwcn_bias_relu_loop
    retw


    tie728_s8_conv2d_per_layer_hwcn_no_bias_relu:
    l32i a13, a4, 84  # activation_shift

    tie728_s8_conv2d_per_layer_hwcn_no_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_relu q0, a14, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_hwcn_no_bias_relu_loop
    retw


dl_tie728_s8_conv2d_per_channel_hwcn_relu:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a13, tie728_s8_conv2d_per_channel_hwcn_no_bias_relu

    tie728_s8_conv2d_per_channel_hwcn_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # l32i a11, a4, 84  # activation_shift
        # tie728_s8_conv2d_bias_relu q0, q1, a13, a14, a11
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a13, a15, q2

        l32i a11, a4, 84  # activation_shift
        tie728_s8_conv2d_relu q0, a14, a11

        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_hwcn_bias_relu_loop
    retw


    tie728_s8_conv2d_per_channel_hwcn_no_bias_relu:
    l32i a13, a4, 84  # activation_shift

    tie728_s8_conv2d_per_channel_hwcn_no_bias_relu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        tie728_s8_conv2d_relu q0, a14, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_hwcn_no_bias_relu_loop
    retw



    .align 4
    .text
    .global dl_tie728_s8_conv2d_hwcn_prelu
    .type   dl_tie728_s8_conv2d_hwcn_prelu, @function
    # .section .iram1
dl_tie728_s8_conv2d_hwcn_prelu:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs4
    # a8: mac_shift
    # a9: input dilation x offset
    # a10: input dilation y offset
    # a11: filter_height / activation_shift
    # a12: filter_width
    # a13: bias_ptr
    # a14: activation_alpha
    # a15: moving_input_ptr
    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10

    l32i a14, a4, 80  # activation_alpha_ptr
    # l32i a13, a4, 84  # activation_shift

    l32i a13, a4, 68  # bias
    l32i a8, a4, 64 # mac shift
    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1
    EE.MOVI.32.Q q6, a11, 2
    blti a8, 0, dl_tie728_s8_conv2d_per_channel_hwcn_prelu


dl_tie728_s8_conv2d_per_layer_hwcn_prelu:

    beqz a13, tie728_s8_conv2d_per_layer_hwcn_no_bias_prelu

    tie728_s8_conv2d_per_layer_hwcn_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_128b_vector_bias a13
        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_vector_round_result q0, a8, a15, q3
        l32i a11, a4, 84  # activation_shift
        tie728_s8_conv2d_prelu q0, q2, a14, a11
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_hwcn_bias_prelu_loop
    retw


    tie728_s8_conv2d_per_layer_hwcn_no_bias_prelu:
    l32i a13, a4, 84  # activation_shift

    tie728_s8_conv2d_per_layer_hwcn_no_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_vector_round_result q0, a8, a15, q3
        tie728_s8_conv2d_prelu q0, q2, a14, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_layer_hwcn_no_bias_prelu_loop
    retw


dl_tie728_s8_conv2d_per_channel_hwcn_prelu:
    l32i a8, a4, 104 # filter_channel_factor address

    beqz a13, tie728_s8_conv2d_per_channel_hwcn_no_bias_prelu

    tie728_s8_conv2d_per_channel_hwcn_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        # tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        # l32i a11, a4, 84  # activation_shift
        # tie728_s8_conv2d_bias_prelu q0, q1, a13, q2, a14, a11
        tie728_s8_conv2d_per_channel_with_bias_result q0, q1, a8, a13, a15, q2
        l32i a11, a4, 84  # activation_shift
        tie728_s8_conv2d_prelu q0, q2, a14, a11
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_hwcn_bias_prelu_loop
    retw


    tie728_s8_conv2d_per_channel_hwcn_no_bias_prelu:
    l32i a13, a4, 84  # activation_shift

    tie728_s8_conv2d_per_channel_hwcn_no_bias_prelu_loop:
        mov a15, a3     # reload input_ptr
        EE.ZERO.QACC

        tie728_s8_conv2d_hwc16 q0, a15, q1, q2, a5, a6, a9, a10, a11, a12, q6, a4
        tie728_s8_conv2d_per_channel_result q0, q1, a8, a15, q2
        tie728_s8_conv2d_prelu q0, q2, a14, a13
        EE.VST.128.IP q0, a2, 16

        addi a7, a7, -1
    bnez a7, tie728_s8_conv2d_per_channel_hwcn_no_bias_prelu_loop
    retw





############################################################################################################################################################
####
#### tie728_s8_unaligned_conv2d_hwcn series
####
############################################################################################################################################################



.macro tie728_s8_conv2d_h_w_unaligned_c input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_v1 filter_ptr c_div_x_1 filter_h filter_w args remainder_c tmp filter_offset_q
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)
    # filter_h
    # filter_w

    l32i \filter_w,          \args,  56  # filter_width
    blti \filter_w, 2, 23f


    l32i \filter_h,          \args,  52  # filter_height
    21:
        l32i \filter_w,          \args,  56  # filter_width
        22:
            tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \tmp
            l32i \tmp, \args, 108  # input dilation x offset
            add \input_ptr, \input_ptr, \tmp


            addi \filter_w, \filter_w, -1
        bgei \filter_w, 2, 22b

        tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \tmp
        l32i \tmp, \args, 112  # input dilation y offset
        add \input_ptr, \input_ptr, \tmp
        EE.MOVI.32.A \filter_offset_q, \filter_w, 1
        add \filter_ptr, \filter_ptr, \filter_w

        addi \filter_h, \filter_h, -1
    bnez \filter_h, 21b
    j 25f

    # filter_w == 1
    23:
    l32i \filter_h,          \args,  52  # filter_height
    EE.MOVI.32.A \filter_offset_q, \filter_w, 1
    24:
        tie728_s8_conv2d_1_1_unaligned_c \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \remainder_c, \tmp
        l32i \tmp, \args, 112  # input dilation y offset
        add \input_ptr, \input_ptr, \tmp
        add \filter_ptr, \filter_ptr, \filter_w

        addi \filter_h, \filter_h, -1
    bnez \filter_h, 24b

25:
    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm




.macro tie728_s8_conv2d_h_w_c_n_remainder input_v0 input_front_aligned input_back_aligned input_ptr filter_v0 filter_front_aligned filter_back_aligned filter_ptr c_div_x_1 args filter_h filter_w remainder_c tmp tmp1 filter_offset_q
    # dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset = (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)
    # filter_h
    # filter_w

    l32i \filter_w,          \args,  56  # filter_width
    blti \filter_w, 2, 22f

    l32i \filter_h,          \args,  52  # filter_height
    21:
        l32i \filter_w,          \args,  56  # filter_width
    20:
        tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \tmp, \tmp1
        l32i \tmp, \args, 108  # input dilation x offset
        add \input_ptr, \input_ptr, \tmp


        addi \filter_w, \filter_w, -1
        bgei \filter_w, 2, 20b

        tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \tmp, \tmp1
        l32i \tmp, \args, 112  # input dilation y offset
        add \input_ptr, \input_ptr, \tmp
        EE.MOVI.32.A \filter_offset_q, \filter_w, 1
        add \filter_ptr, \filter_ptr, \filter_w

        addi \filter_h, \filter_h, -1
    bnez \filter_h, 21b
    j 24f

    # filter_w == 1
    22:
    l32i \filter_h,          \args,  52  # filter_height
    EE.MOVI.32.A \filter_offset_q, \filter_w, 1
    23:
        tie728_s8_conv2d_1_1_c_n_remainder \input_v0, \input_front_aligned, \input_back_aligned, \input_ptr, \filter_v0, \filter_front_aligned, \filter_back_aligned, \filter_ptr, \c_div_x_1, \remainder_c, \tmp, \tmp1
        l32i \tmp, \args, 112  # input dilation y offset
        add \input_ptr, \input_ptr, \tmp
        add \filter_ptr, \filter_ptr, \filter_w

        addi \filter_h, \filter_h, -1
    bnez \filter_h, 23b

24:
    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm



    .align 4
    .text
    .global dl_tie728_s8_unaligned_conv2d_hwcn
    .type   dl_tie728_s8_unaligned_conv2d_hwcn, @function
    # .section .iram1
dl_tie728_s8_unaligned_conv2d_hwcn:
    .align 4
    entry sp, 128

    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    # a5: int8_t *filter_ptr
    # a6: c_div_x_1
    # a7: n_rs4
    # a8: mac_shift
    # a9: remainder_c
    # a10: tmp / input dilation x offset / input dilation y offset
    # a11: filter_height / activation_shift
    # a12: filter_width
    # a13: bias_ptr
    # a14: activation_alpha_ptr
    # a15: moving_input_ptr

    tie728_s8_conv2d_hwcn_load_args a4, a5, a6, a7, a9, a10
    # l32i a7, a4,  96  // output_channel_div_8
    # l32i a5, a4,  48  // filter
    # l32i a6, a4, 100  // input_channel / x - 1

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q6, a12, 1
    EE.MOVI.32.Q q6, a11, 2

    l32i a8,  a4, 64  // mac_shift
    l32i a13, a4, 68  // bias
    #l32i a14, a4, 76  // activation_alpha
    l32i a11, a4, 84  // activation_shift
    l32i a14, a4, 80  // activation_alpha_ptr

    tie728_s8_unaligned_conv2d_operation_type a12, a8, a13, a11, a14, a4
    EE.MOVI.32.Q q7, a12, 0  # operation_type q7[0]

    beqz a7, tie728_s8_conv2d_h_w_unaligned_c_n_loop_end # output_channel < 16

    EE.LD.128.USAR.IP q1, a2, 0  # output sar_byte
    rur.sar_byte a12

    bgez a11, tie728_s8_conv2d_h_w_unaligned_c_n_activation
        l32i a9, a4, 136  # c_remainder: c % 16
        EE.MOVI.32.A q7, a10, 0  # operation_type

        beqi a12, 0, tie728_s8_conv2d_h_w_unaligned_c_n_loop0
        beqi a12, 8, tie728_s8_conv2d_h_w_unaligned_c_n_loop8

    # output sar_byte != 0 && != 8
        tie728_s8_conv2d_h_w_unaligned_c_n_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_unaligned_c_n_loop_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a13
            tie728_s8_conv2d_h_w_unaligned_c_n_loop_no_preload_bias:
            tie728_s8_conv2d_h_w_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a11, a12, a4, a9, a14, q6
            tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a13, a11, a11, a11, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store0 q0, a2, a14

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_h_w_unaligned_c_n_loop
        j tie728_s8_conv2d_h_w_unaligned_c_n_loop_end

    # output sar_byte == 0
        tie728_s8_conv2d_h_w_unaligned_c_n_loop0:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_unaligned_c_n_loop0_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a13
            tie728_s8_conv2d_h_w_unaligned_c_n_loop0_no_preload_bias:
            tie728_s8_conv2d_h_w_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a11, a12, a4, a9, a14, q6
            tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a13, a11, a11, a11, q1, q2

            EE.VST.128.IP q0, a2, 16

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_h_w_unaligned_c_n_loop0
        j tie728_s8_conv2d_h_w_unaligned_c_n_loop_end

    # output sar_byte == 8
        tie728_s8_conv2d_h_w_unaligned_c_n_loop8:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_unaligned_c_n_loop8_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a13
            tie728_s8_conv2d_h_w_unaligned_c_n_loop8_no_preload_bias:
            tie728_s8_conv2d_h_w_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a11, a12, a4, a9, a14, q6
            tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a13, a11, a11, a11, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store1 q0, a2

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_h_w_unaligned_c_n_loop8
        j tie728_s8_conv2d_h_w_unaligned_c_n_loop_end


    tie728_s8_conv2d_h_w_unaligned_c_n_activation:
        l32i a9, a4, 136  # c_remainder: c % 16

        EE.LD.128.USAR.IP q1, a2, 0  # output sar_byte
        rur.sar_byte a12
        beqi a12, 0, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop0
        beqi a12, 8, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop8

    # output sar_byte != 0 && != 8
        tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a13
            tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop_no_preload_bias:
            tie728_s8_conv2d_h_w_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a11, a12, a4, a9, a10, q6
            EE.MOVI.32.A q7, a10, 0  # operation_type
            l32i a11, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a13, a14, a11, a15, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store0 q0, a2, a11

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop
        j tie728_s8_conv2d_h_w_unaligned_c_n_loop_end

    # output sar_byte == 0
        tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop0:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop0_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a13
            tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop0_no_preload_bias:
            tie728_s8_conv2d_h_w_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a11, a12, a4, a9, a10, q6
            EE.MOVI.32.A q7, a10, 0  # operation_type
            l32i a11, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a13, a14, a11, a15, q1, q2

            EE.VST.128.IP q0, a2, 16

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop0
        j tie728_s8_conv2d_h_w_unaligned_c_n_loop_end

    # output sar_byte == 8
        tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop8:
            mov a15, a3    # reload input_ptr
            EE.ZERO.QACC

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop8_no_preload_bias
                tie728_s8_conv2d_128b_vector_bias a13
            tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop8_no_preload_bias:
            tie728_s8_conv2d_h_w_unaligned_c q0, q1, q2, a15, q3, q4, a5, a6, a11, a12, a4, a9, a10, q6
            EE.MOVI.32.A q7, a10, 0  # operation_type
            l32i a11, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_unaligned_c_result a10, q0, a8, a13, a14, a11, a15, q1, q2

            #store to unaligned address
            dl_tie728_s8_unaligned_store1 q0, a2

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_h_w_unaligned_c_n_activation_loop8
        j tie728_s8_conv2d_h_w_unaligned_c_n_loop_end

tie728_s8_conv2d_h_w_unaligned_c_n_loop_end:


    # handle the n remainder
    l32i a7, a4, 140  # n % 16 remainder_n
    beqz a7, dl_tie728_s8_unaligned_conv2d_hwcn_end

    l32i a5, a4, 160
    l32i a11, a4, 164
    EE.MOVI.32.Q q6, a5, 1
    EE.MOVI.32.Q q6, a11, 2
    l32i a5, a4,  168 # filter_ptr unaligned

    l32i a11, a4, 84  # activation_shift
    l32i a9, a4, 136  # c_remainder: c % 16


    bgez a11, tie728_s8_conv2d_h_w_c_unaligned_n_activation
        # n_remainder
        tie728_s8_conv2d_h_w_c_unaligned_n_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.ACCX

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_c_unaligned_n_loop_no_preload_bias
                tie728_s8_conv2d_element_bias a13
            tie728_s8_conv2d_h_w_c_unaligned_n_loop_no_preload_bias:

            # complete one n in ACCX
            tie728_s8_conv2d_h_w_c_n_remainder q0, q1, q2, a15, q3, q4, q5, a5, a6, a4, a11, a12, a9, a10, a14, q6
            tie728_s8_conv2d_1_1_n_remainder_result q7, a10, a2, a8, a13, a11, a11, a11, q0

            addi a7, a7, -1
        bnez a7, tie728_s8_conv2d_h_w_c_unaligned_n_loop
        j dl_tie728_s8_unaligned_conv2d_hwcn_end

    tie728_s8_conv2d_h_w_c_unaligned_n_activation:
        EE.MOVI.32.Q q6, a7, 0 # store remainder_n in q6[0]

        # n_remainder
        tie728_s8_conv2d_h_w_c_unaligned_n_activation_loop:
            mov a15, a3    # reload input_ptr
            EE.ZERO.ACCX

            # Without modifications specifically for per-channel, there may be issues with per-channel
            beqz a13, tie728_s8_conv2d_h_w_c_unaligned_n_activation_loop_no_preload_bias
                tie728_s8_conv2d_element_bias a13
            tie728_s8_conv2d_h_w_c_unaligned_n_activation_loop_no_preload_bias:

            # complete one n in ACCX
            tie728_s8_conv2d_h_w_c_n_remainder q0, q1, q2, a15, q3, q4, q5, a5, a6, a4, a11, a12, a9, a10, a7, q6

            l32i a11, a4, 84  # activation_shift
            tie728_s8_conv2d_1_1_n_remainder_result q7, a10, a2, a8, a13, a14, a11, a15, q0

            EE.MOVI.32.A q6, a7, 0
            addi a7, a7, -1
            EE.MOVI.32.Q q6, a7, 0 # store remainder_n in q6[0]
        bnez a7, tie728_s8_conv2d_h_w_c_unaligned_n_activation_loop


dl_tie728_s8_unaligned_conv2d_hwcn_end:

    retw
