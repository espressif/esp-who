############################################################################################################################################################
# xtensa general tools
############################################################################################################################################################
.macro xtensa_clear_accumulator zero
    # zero: any a-register equals 0

    wsr \zero, acchi
    wsr \zero, acclo
.endm


.macro xtensa_fetch_accumulator output temp
    # output: output value
    # temp: temporary variable

    rsr \output, acchi
    rsr \temp, acclo

    src \output, \output, \temp
.endm


.macro xtensa_saturation x
    # x: any a-register

    clamps \x, \x, 15
.endm



.macro xtensa_bias x bias_address temp
    # x: a variable to be added
    # bias_address:
    # temp: a temporary variable keep bias element value

    l16si \temp,         \bias_address, 0
    addi  \bias_address, \bias_address, 2
    add   \x,            \x,            \temp
.endm


.macro xtensa_store output_address x
    # output_address:
    # x: a variable to be stored

    s16i \x,              \output_address, 0
    addi \output_address, \output_address, 2
.endm


.macro xtensa_relu x zero
    # x: varaible
    # zero: any a-register equals to 0

    max \x, \x, \zero
.endm



############################################################################################################################################################
# xtensa_s16_conv2d_11c1
############################################################################################################################################################
.macro xtensa_s16_conv2d_11c1 input__v0 input__v1 filter_v0 filter_v1 input__ptr filter_ptr c c_rs2_1
    # input__v0: 2 input elements
    # input__v1: 2 input elements
    # filter_v0: 2 filter elements
    # filter_v1: 2 filter elements
    # input__ptr:
    # filter_ptr:
    # c: input_channel
    # c_rs2_1: input_channel >> 2 - 1

    bgei \c, 4, 5f
    beqi \c, 3, 3f
    beqi \c, 2, 2f
    # c == 1
    ldinc \input__v0, \input__ptr
    ldinc \filter_v0, \filter_ptr
    mula.dd.ll \input__v0, \filter_v0
    addi \input__ptr, \input__ptr, -2
    addi \filter_ptr, \filter_ptr, -2
    j 1f
2:
    # c == 2
    ldinc \input__v0, \input__ptr
    ldinc \filter_v0, \filter_ptr
    mula.dd.ll \input__v0, \filter_v0
    mula.dd.hh \input__v0, \filter_v0
    j 1f
3:
    # c == 3
    ldinc \input__v0, \input__ptr
    ldinc \filter_v0, \filter_ptr
    ldinc \input__v1, \input__ptr
    mula.dd.ll.ldinc \filter_v1, \filter_ptr, \input__v0, \filter_v0
    mula.dd.hh                                \input__v0, \filter_v0
    mula.dd.ll                                \input__v1, \filter_v1
    addi \input__ptr, \input__ptr, -2
    addi \filter_ptr, \filter_ptr, -2
    j 1f

5:
    # c >= 4
    ldinc \input__v0, \input__ptr
    ldinc \filter_v0, \filter_ptr
    ldinc \input__v1, \input__ptr
    mula.dd.ll.ldinc \filter_v1, \filter_ptr, \input__v0, \filter_v0

    loopgtz \c_rs2_1, 4f
    mula.dd.hh.ldinc \input__v0, \input__ptr, \input__v0, \filter_v0
    mula.dd.ll.ldinc \filter_v0, \filter_ptr, \input__v1, \filter_v1
    mula.dd.hh.ldinc \input__v1, \input__ptr, \input__v1, \filter_v1
    mula.dd.ll.ldinc \filter_v1, \filter_ptr, \input__v0, \filter_v0
4:
    mula.dd.hh \input__v0, \filter_v0
    mula.dd.ll \input__v1, \filter_v1
    mula.dd.hh \input__v1, \filter_v1

    bbci  \c, 1, 6f
    # c % 4 == 2 or 3
    ldinc \input__v0, \input__ptr
    ldinc \filter_v0, \filter_ptr
    mula.dd.hh \input__v0, \filter_v0
    mula.dd.ll \input__v0, \filter_v0
6:
    bbci \c, 0, 1f
    # c % 2 == 1
    ldinc \input__v0, \input__ptr
    ldinc \filter_v0, \filter_ptr
    mula.dd.ll \input__v0, \filter_v0
    addi \input__ptr, \input__ptr, -2
    addi \filter_ptr, \filter_ptr, -2
1:
.endm



.macro xtensa_load_args input__ptr args filter_ptr c n c_rs2_1 mac_shift
    l32i \c,          \args,         4  // input_channel
    l32i \n,          \args,        36  // output_channel
    l32i \filter_ptr, \args,        48  // filter
    l32i \mac_shift,  \args,        64  // mac_shift
    l32i \c_rs2_1,    \args,        92  // input_channel >> 2 - 1
    addi \filter_ptr, \filter_ptr,  -4  // ldinc will bump up pointer first then load
    addi \input__ptr, \input__ptr,  -4  // ldinc will bump up pointer first then load
    ssr  \mac_shift
    movi \mac_shift,      0
.endm




    .align 4
    .text
    .global dl_xtensa_s16_conv2d_11cn_bias
    .type	dl_xtensa_s16_conv2d_11cn_bias, @function
    .section .iram1
dl_xtensa_s16_conv2d_11cn_bias:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    xtensa_load_args a3, a4, a5, a6, a7, a8, a9
    # a10: bias_address
    # a11:
    # a12:
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_11cn_bias_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_11c1 m0, m1, m2, m3, a15, a5, a6, a8

    xtensa_fetch_accumulator a13, a14
    xtensa_bias a13, a10, a14
    # xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_11cn_bias_loop
    retw




    .align 4
    .text
    .global dl_xtensa_s16_conv2d_11cn_bias_relu
    .type	dl_xtensa_s16_conv2d_11cn_bias_relu, @function
    .section .iram1
dl_xtensa_s16_conv2d_11cn_bias_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    xtensa_load_args a3, a4, a5, a6, a7, a8, a9
    # a10: bias_address
    # a11:
    # a12:
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_11cn_bias_relu_loop:
    mov a15, a3          # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_11c1 m0, m1, m2, m3, a15, a5, a6, a8

    xtensa_fetch_accumulator a13, a14
    xtensa_bias a13, a10, a14
    xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_11cn_bias_relu_loop
    retw




    .align 4
    .text
    .global dl_xtensa_s16_conv2d_11cn
    .type	dl_xtensa_s16_conv2d_11cn, @function
    .section .iram1
dl_xtensa_s16_conv2d_11cn:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    xtensa_load_args a3, a4, a5, a6, a7, a8, a9
    # a10: bias_address
    # a11:
    # a12:
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    # l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_11cn_loop:
    mov a15, a3          # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_11c1 m0, m1, m2, m3, a15, a5, a6, a8

    xtensa_fetch_accumulator a13, a14
    # xtensa_bias a13, a10, a14
    # xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_11cn_loop
    retw




    .align 4
    .text
    .global dl_xtensa_s16_conv2d_11cn_relu
    .type	dl_xtensa_s16_conv2d_11cn_relu, @function
    .section .iram1
dl_xtensa_s16_conv2d_11cn_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    xtensa_load_args a3, a4, a5, a6, a7, a8, a9
    # a10: bias_address
    # a11:
    # a12:
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    # l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_11cn_relu_loop:
    mov a15, a3          # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_11c1 m0, m1, m2, m3, a15, a5, a6, a8

    xtensa_fetch_accumulator a13, a14
    # xtensa_bias a13, a10, a14
    xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_11cn_relu_loop
    retw



############################################################################################################################################################
# xtensa_s16_conv2d_33c1
############################################################################################################################################################
.macro xtensa_s16_conv2d_33c1 input__v0 input__v1 filter_v0 filter_v1 input__ptr filter_ptr c c_rs2_1 dilation_x_offset dilation_y_offset
    # input__v0: 2 input elements
    # input__v1: 2 input elements
    # filter_v0: 2 filter elements
    # filter_v1: 2 filter elements
    # input__ptr:
    # filter_ptr:
    # c: input_channel
    # c_rs2_1: input_channel >> 2 - 1
    # dilation_x_offset: (dilation_x * input_channel_with_padding - input_channel) * sizeof(output_t)
    # dilation_y_offset: (dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)) * sizeof(output_t)

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_x_offset                                                                  # go to the next input__ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_x_offset                                                                  # go to the next input__ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_y_offset                                                                  # go to the next input__ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_x_offset                                                                  # go to the next input__ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_x_offset                                                                  # go to the next input__ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_y_offset                                                                  # go to the next input__ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_x_offset                                                                  # go to the next input__ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
    add \input__ptr, \input__ptr, \dilation_x_offset                                                                  # go to the next input_ptr

    xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
.endm



.macro xtensa_load_hwcn_args input__ptr args filter_ptr c n c_rs2_1 mac_shift dilation_x_offset dilation_y_offset
    xtensa_load_args \input__ptr, \args, \filter_ptr, \c, \n, \c_rs2_1, \mac_shift
    l32i \dilation_x_offset, \args, 108  // input dilation x offset
    l32i \dilation_y_offset, \args, 112  // input dilation y offset
.endm



    .align 4
    .text
    .global dl_xtensa_s16_conv2d_33cn_bias
    .type	dl_xtensa_s16_conv2d_33cn_bias, @function
    .section .iram1
dl_xtensa_s16_conv2d_33cn_bias:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10: bias_address
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_33cn_bias_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_33c1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12

    xtensa_fetch_accumulator a13, a14
    xtensa_bias a13, a10, a14
    # xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_33cn_bias_loop
    retw



    .align 4
    .text
    .global dl_xtensa_s16_conv2d_33cn_bias_relu
    .type	dl_xtensa_s16_conv2d_33cn_bias_relu, @function
    .section .iram1
dl_xtensa_s16_conv2d_33cn_bias_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10: bias_address
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_33cn_bias_relu_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_33c1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12

    xtensa_fetch_accumulator a13, a14
    xtensa_bias a13, a10, a14
    xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_33cn_bias_relu_loop
    retw



    .align 4
    .text
    .global dl_xtensa_s16_conv2d_33cn
    .type	dl_xtensa_s16_conv2d_33cn, @function
    .section .iram1
dl_xtensa_s16_conv2d_33cn:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10: bias_address
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    # l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_33cn_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_33c1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12

    xtensa_fetch_accumulator a13, a14
    # xtensa_bias a13, a10, a14
    # xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_33cn_loop
    retw



    .align 4
    .text
    .global dl_xtensa_s16_conv2d_33cn_relu
    .type	dl_xtensa_s16_conv2d_33cn_relu, @function
    .section .iram1
dl_xtensa_s16_conv2d_33cn_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10:
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: output variable
    # a14: temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    # l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_33cn_relu_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_33c1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12

    xtensa_fetch_accumulator a13, a14
    # xtensa_bias a13, a10, a14
    xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_33cn_relu_loop
    retw




############################################################################################################################################################
# xtensa_s16_conv2d_hwc1
############################################################################################################################################################
.macro xtensa_s16_conv2d_hwc1 input__v0 input__v1 filter_v0 filter_v1 input__ptr filter_ptr c c_rs2_1 dilation_x_offset dilation_y_offset filter_h filter_w args
    # input__v0: 2 input elements
    # input__v1: 2 input elements
    # filter_v0: 2 filter elements
    # filter_v1: 2 filter elements
    # input__ptr:
    # filter_ptr:
    # c: input_channel
    # c_rs2_1: input_channel >> 2 - 1
    # dilation_x_offset: dilation_x * input_channel_with_padding - input_channel
    # dilation_y_offset: dilation_y * input_width_with_padding * input_channel_with_padding - input_channel - dilation_x * input_channel_with_padding * (filter_width - 1)
    # filter_h: filter height
    # filter_w: filter width

    l32i \filter_h, \args, 52  # filter_height
    7:
        l32i \filter_w, \args, 56  # filter_width
        beqi \filter_w, 1, 9f
        8:
            xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
            add \input__ptr, \input__ptr, \dilation_x_offset

            addi \filter_w, \filter_w, -1
            bgei \filter_w, 2, 8b
        9:
        xtensa_s16_conv2d_11c1 \input__v0 \input__v1 \filter_v0 \filter_v1 \input__ptr \filter_ptr \c \c_rs2_1
        l32i \filter_w, \args, 60 # filter_y_offset
        add \input__ptr, \input__ptr, \dilation_y_offset
        add \filter_ptr, \filter_ptr, \filter_w

        addi \filter_h, \filter_h, -1
        bnez \filter_h, 7b

    l32i \filter_h, \args, 144 # filter_n_offset
    add \filter_ptr, \filter_ptr, \filter_h
.endm




    .align 4
    .text
    .global dl_xtensa_s16_conv2d_hwcn_bias
    .type	dl_xtensa_s16_conv2d_hwcn_bias, @function
    .section .iram1
dl_xtensa_s16_conv2d_hwcn_bias:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10: bias_address
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: filter_height, output variable
    # a14: filter_width, temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_hwcn_bias_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_hwc1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12, a13, a14, a4

    xtensa_fetch_accumulator a13, a14
    xtensa_bias a13, a10, a14
    # xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_hwcn_bias_loop
    retw



    .align 4
    .text
    .global dl_xtensa_s16_conv2d_hwcn_bias_relu
    .type	dl_xtensa_s16_conv2d_hwcn_bias_relu, @function
    .section .iram1
dl_xtensa_s16_conv2d_hwcn_bias_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10: bias_address
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: filter_height, output variable
    # a14: filter_width, temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_hwcn_bias_relu_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_hwc1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12, a13, a14, a4

    xtensa_fetch_accumulator a13, a14
    xtensa_bias a13, a10, a14
    xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_hwcn_bias_relu_loop
    retw



    .align 4
    .text
    .global dl_xtensa_s16_conv2d_hwcn
    .type	dl_xtensa_s16_conv2d_hwcn, @function
    .section .iram1
dl_xtensa_s16_conv2d_hwcn:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10:
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: filter_height, output variable
    # a14: filter_width, temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    # l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_hwcn_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_hwc1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12, a13, a14, a4

    xtensa_fetch_accumulator a13, a14
    # xtensa_bias a13, a10, a14
    # xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_hwcn_loop
    retw



    .align 4
    .text
    .global dl_xtensa_s16_conv2d_hwcn_relu
    .type	dl_xtensa_s16_conv2d_hwcn_relu, @function
    .section .iram1
dl_xtensa_s16_conv2d_hwcn_relu:
    .align 4
    entry sp, 16

    # a2: int16_t *output_address
    # a3: int16_t *input__ptr
    # a4: void *args

    # a5: filter_ptr
    # a6: c
    # a7: n
    # a8: c_rs2_1
    # a9: mac_shift (after set srr a9 will be 0)
    # a10: bias_address
    # a11: input dilation x offset
    # a12: input dilation y offset
    # a13: filter_height, output variable
    # a14: filter_width, temporary variable
    # a15: moving_input_address
    xtensa_load_hwcn_args a3, a4, a5, a6, a7, a8, a9, a11, a12

    # l32i a10, a4, 68  # bias_address
xtensa_s16_conv2d_hwcn_relu_loop:
    mov a15, a3       # reload input__ptr

    xtensa_clear_accumulator a9

    xtensa_s16_conv2d_hwc1 m0, m1, m2, m3, a15, a5, a6, a8, a11, a12, a13, a14, a4

    xtensa_fetch_accumulator a13, a14
    # xtensa_bias a13, a10, a14
    xtensa_relu a13, a9
    xtensa_saturation a13
    xtensa_store a2, a13

    addi a7, a7, -1  # bump up pointer
    bnez a7, xtensa_s16_conv2d_hwcn_relu_loop
    retw
