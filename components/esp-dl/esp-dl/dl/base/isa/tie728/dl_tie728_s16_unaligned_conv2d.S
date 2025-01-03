#include "dl_tie728_s16_unaligned.S"
#include "dl_tie728_s16.S"

############################################################################################################################################################
####
#### tie728_s16_unaligned_conv2d_11cn series
####
############################################################################################################################################################
.macro tie728_s16_unaligned_conv2d_11c8 input_v, input_front, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, c_remainder
    # input_v:     8 input elements
    # filter_v0:    8 filter elements
    # filter_v1:    8 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 8 - 1

    EE.LD.128.USAR.IP \input_front, \input_ptr, 16

    blti \c_div_x_1, 0, 7f
    EE.LD.128.USAR.IP \input_back, \input_ptr, 16
    EE.VLD.128.IP \filter_v0, \filter_ptr, 16
    EE.VLD.128.IP \filter_v1, \filter_ptr, 16
    loopgtz \c_div_x_1, 8f
        EE.SRC.Q.QUP \input_v, \input_front, \input_back
        EE.LD.128.USAR.IP \input_back, \input_ptr, 16

        EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
        EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
        EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
        EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
        EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 4
        EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 5
        EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 6
        EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 7
8:
    # last entire-128b
    EE.SRC.Q.QUP \input_v, \input_front, \input_back
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 4
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 5
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 6
    EE.VSMULAS.S16.QACC \filter_v1, \input_v, 7
    beqz \c_remainder, 0f # jump to c_remainder == 0
7:
    # c_remainder
    EE.LD.128.USAR.XP \input_back, \input_ptr, \c_remainder
    EE.SRC.Q.QUP \input_v, \input_front, \input_back
    EE.VLD.128.IP \filter_v0, \filter_ptr,  16

    bbci \c_remainder, 3, 3f
    # remainder == 0x1__0
    EE.VLD.128.IP \filter_v1, \filter_ptr,  16
    bbci \c_remainder, 2, 5f
    # remainder == 0x11_0
    bbci \c_remainder, 1, 6f
    # remainder == 0x1110, 7
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 4
    EE.VSMULAS.S16.QACC \filter_v1, \input_v, 5
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 6
    j 0f

6:  # remainder == 0x1100, 6
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 4
    EE.VSMULAS.S16.QACC \filter_v1, \input_v, 5
    j 0f

5:  # remainder == 0x10_0
    bbci \c_remainder, 1, 4f
    # remainder == 0x1010, 5
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
    EE.VSMULAS.S16.QACC \filter_v1, \input_v, 3
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 4
    j 0f

4:  # remainder == 0x1000, 4
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 2
    EE.VSMULAS.S16.QACC \filter_v1, \input_v, 3
    j 0f

3:  # remainder == 0x0__0
    bbci \c_remainder, 2, 1f
    # remainder == 0x01_0
    EE.VLD.128.IP \filter_v1, \filter_ptr,  16

    bbci \c_remainder, 1, 2f
    # remainder == 0x0110, 3
    EE.VSMULAS.S16.QACC.LD.INCP \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
    EE.VSMULAS.S16.QACC \filter_v1, \input_v, 1
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 2
    j 0f

2:  # remainder == 0x0100, 2
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 0
    EE.VSMULAS.S16.QACC \filter_v1, \input_v, 1
    j 0f

1:  # remainder == 0x0010, 1
    EE.VSMULAS.S16.QACC \filter_v0, \input_v, 0

0:
    addi \input_ptr, \input_ptr, -16

.endm



.macro tie728_s16_unaligned_conv2d_11c1 input_v, input_front, input_back, filter_v, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, temp, zero
    # input_v:     8 input elements
    # filter_v:    8 filter elements
    # filter_v1:    8 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 8 - 1

    EE.LD.128.USAR.IP \input_front, \input_ptr, 16
    EE.LD.128.USAR.IP \filter_front, \filter_ptr, 16

    blti \c_div_x_1, 0, 7f # input_channel < 8

    EE.LD.128.USAR.IP \input_back, \input_ptr, 16
    loopgtz \c_div_x_1, 8f
        EE.SRC.Q.QUP \input_v, \input_front, \input_back

        EE.LD.128.USAR.IP \filter_back, \filter_ptr, 16
        EE.SRC.Q.QUP \filter_v, \filter_front, \filter_back

        EE.LD.128.USAR.IP \input_back, \input_ptr, 16
        EE.VMULAS.S16.ACCX \filter_v, \input_v
    8:
    # last entire-128b
    EE.SRC.Q.QUP \input_v, \input_front, \input_back

    EE.LD.128.USAR.IP \filter_back, \filter_ptr, 16
    EE.SRC.Q.QUP \filter_v, \filter_front, \filter_back

    EE.VMULAS.S16.ACCX \filter_v, \input_v

    beqz \c_remainder, 0f
7:
    # c_remainder > 0
    EE.LD.128.USAR.XP \input_back, \input_ptr, \c_remainder
    EE.SRC.Q.QUP \input_v, \input_front, \input_back

    EE.LD.128.USAR.XP \filter_back, \filter_ptr, \c_remainder
    EE.SRC.Q.QUP \filter_v, \filter_front, \filter_back

    EE.SLCXXP.2Q  \input_back,  \input_v, \temp, \zero
    EE.SLCXXP.2Q \filter_back, \filter_v, \temp, \zero

    EE.VMULAS.S16.ACCX \filter_v, \input_v
0:
    addi  \input_ptr,  \input_ptr, -16
    addi \filter_ptr, \filter_ptr, -16
.endm




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn
    .type   dl_tie728_s16_unaligned_conv2d_11cn, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i a5, a4,  48           # a5: filter_ptr
    l32i a6, a4, 100           # a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i a7, a4, 136           # a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i a8, a4,  64           # a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a9, a4,  96           # a9: n_div_x           = output_channel / (vector_width / element_width)

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_64b
        tie728_s16_unaligned_conv2d_11cn_32b:
            tie728_s16_unaligned_conv2d_11cn_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_n_remainder

        tie728_s16_unaligned_conv2d_11cn_64b:
            tie728_s16_unaligned_conv2d_11cn_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_n_remainder

        tie728_s16_unaligned_conv2d_11cn_128b:
            tie728_s16_unaligned_conv2d_11cn_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_n_remainder_end

        movi a10, 15
        sub  a10, a10, a7       # a10: 15 - c_remainder
        movi a11,  0            # a11: activation_shift = zero

        tie728_s16_unaligned_conv2d_11cn_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a10, a11
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_store  a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn_relu
    .type   dl_tie728_s16_unaligned_conv2d_11cn_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn_relu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4,  64           # a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i  a9, a4,  96           # a9: n_div_x           = output_channel / (vector_width / element_width)
    movi a10,  0                # a10: activation_alpha = zero
    movi a11,  0                # a11: activation_shift = zero

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_relu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_relu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_relu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_relu_64b
        tie728_s16_unaligned_conv2d_11cn_relu_32b:
            tie728_s16_unaligned_conv2d_11cn_relu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a10, a11
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_relu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_relu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_relu_64b:
            tie728_s16_unaligned_conv2d_11cn_relu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a10, a11
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_relu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_relu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_relu_128b:
            tie728_s16_unaligned_conv2d_11cn_relu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a10, a11
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_relu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_relu_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_relu_n_remainder_end

        movi a10, 15
        sub  a10, a10, a7       # a10: 15 - c_remainder

        tie728_s16_unaligned_conv2d_11cn_relu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a10, a11
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_relu  a14
            tie728_s16_element_store  a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_relu_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_relu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn_leakyrelu
    .type   dl_tie728_s16_unaligned_conv2d_11cn_leakyrelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn_leakyrelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           # a5: filter_ptr
    l32i  a6, a4, 100           # a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           # a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4,  64           # a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i  a9, a4,  96           # a9: n_div_x           = output_channel / (vector_width / element_width)
    l32i a10, a4,  76           # a10: activation_alpha
    l32i a11, a4,  84           # a11: activation_shift

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_leakyrelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_leakyrelu_64b
        tie728_s16_unaligned_conv2d_11cn_leakyrelu_32b:
            tie728_s16_unaligned_conv2d_11cn_leakyrelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a10, a11
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_leakyrelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_leakyrelu_64b:
            tie728_s16_unaligned_conv2d_11cn_leakyrelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a10, a11
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_leakyrelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_leakyrelu_128b:
            tie728_s16_unaligned_conv2d_11cn_leakyrelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a10, a11
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_leakyrelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder_end

        ssr  a11                # ssr: activation_shift
        movi a11, 15
        sub  a11, a11, a7       # a11: 15 - c_remainder
        movi a12,  0            # a12: zero

        tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a11, a12
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_leakyrelu  a14, a10
            tie728_s16_element_store   a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_leakyrelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn_prelu
    .type   dl_tie728_s16_unaligned_conv2d_11cn_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn_prelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           # a5: filter_ptr
    l32i  a6, a4, 100           # a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           # a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4,  64           # a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i  a9, a4,  96           # a9: n_div_x           = output_channel / (vector_width / element_width)
    l32i a10, a4,  80           # a10: activation_alpha_ptr
    l32i a11, a4,  84           # a11: activation_shift

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_prelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_prelu_64b
        tie728_s16_unaligned_conv2d_11cn_prelu_32b:
            tie728_s16_unaligned_conv2d_11cn_prelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a10, a11
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_prelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_prelu_64b:
            tie728_s16_unaligned_conv2d_11cn_prelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a10, a11
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_prelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_prelu_128b:
            tie728_s16_unaligned_conv2d_11cn_prelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a10, a11
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_prelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder_end

        ssr  a11                # ssr: activation_shift
        movi a11, 15
        sub  a11, a11, a7       # a11: 15 - c_remainder
        movi a12,  0            # a12: zero

        tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a11, a12
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_prelu  a14, a10, a15
            tie728_s16_element_store   a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_prelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn_bias
    .type   dl_tie728_s16_unaligned_conv2d_11cn_bias, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn_bias:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           # a5: filter_ptr
    l32i  a6, a4, 100           # a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           # a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4,  64           # a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i  a9, a4,  96           # a9: n_div_x           = output_channel / (vector_width / element_width)
    l32i a10, a4,  68           # a10: bias_ptr

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_bias_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_bias_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_bias_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_bias_64b
        tie728_s16_unaligned_conv2d_11cn_bias_32b:
            tie728_s16_unaligned_conv2d_11cn_bias_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_64b:
            tie728_s16_unaligned_conv2d_11cn_bias_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_128b:
            tie728_s16_unaligned_conv2d_11cn_bias_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_bias_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_bias_n_remainder_end

        movi a11, 15
        sub  a11, a11, a7       # a11: 15 - c_remainder
        movi a12,  0            # a12: zero

        tie728_s16_unaligned_conv2d_11cn_bias_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a10
            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a11, a12
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_store  a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_bias_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn_bias_relu
    .type   dl_tie728_s16_unaligned_conv2d_11cn_bias_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn_bias_relu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           # a5: filter_ptr
    l32i  a6, a4, 100           # a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           # a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4,  64           # a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i  a9, a4,  96           # a9: n_div_x           = output_channel / (vector_width / element_width)
    l32i a10, a4,  68           # a10: bias_ptr
    movi a11,  0                # a11: activation_alpha = zero
    movi a12,  0                # a12: activation_shift = zero

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_bias_relu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_bias_relu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_bias_relu_64b
        tie728_s16_unaligned_conv2d_11cn_bias_relu_32b:
            tie728_s16_unaligned_conv2d_11cn_bias_relu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a11, a12
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_relu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_relu_64b:
            tie728_s16_unaligned_conv2d_11cn_bias_relu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a11, a12
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_relu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_relu_128b:
            tie728_s16_unaligned_conv2d_11cn_bias_relu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a11, a12
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_relu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_end

        movi a11, 15
        sub  a11, a11, a7       # a11: 15 - c_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a10
            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a11, a12
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_relu  a14
            tie728_s16_element_store  a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu
    .type   dl_tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           # a5: filter_ptr
    l32i  a6, a4, 100           # a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           # a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4,  64           # a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i  a9, a4,  96           # a9: n_div_x           = output_channel / (vector_width / element_width)
    l32i a10, a4,  68           # a10: bias_ptr
    l32i a11, a4,  76           # a11: activation_alpha
    l32i a12, a4,  84           # a12: activation_shift

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_64b
        tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_32b:
            tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a11, a12
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_64b:
            tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a11, a12
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_128b:
            tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_relu  q0, a11, a12
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_end

        ssr  a12                # ssr: activation_shift
        movi a12, 15
        sub  a12, a12, a7       # a12: 15 - c_remainder
        movi a13,  0            # a13: zero

        tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a10
            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a12, a13
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_leakyrelu  a14, a11
            tie728_s16_element_store   a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_11cn_bias_prelu
    .type   dl_tie728_s16_unaligned_conv2d_11cn_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_11cn_bias_prelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4,  64           #  a8: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i  a9, a4,  96           #  a9: n_div_x           = output_channel / (vector_width / element_width)
    l32i a10, a4,  68           # a10: bias_ptr
    l32i a11, a4,  80           # a11: activation_alpha_ptr
    l32i a12, a4,  84           # a12: activation_shift

    blti a9, 1, tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_11cn_bias_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_11cn_bias_prelu_64b
        tie728_s16_unaligned_conv2d_11cn_bias_prelu_32b:
            tie728_s16_unaligned_conv2d_11cn_bias_prelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a11, a12
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_prelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_prelu_64b:
            tie728_s16_unaligned_conv2d_11cn_bias_prelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a11, a12
                tie728_64b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_prelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder

        tie728_s16_unaligned_conv2d_11cn_bias_prelu_128b:
            tie728_s16_unaligned_conv2d_11cn_bias_prelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a10
                tie728_s16_unaligned_conv2d_11c8 q0, q1, q2, q3, q4, a15, a5, a6, a7
                tie728_s16_vector_round_result  q0, a8, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a11, a12
                tie728_128b_aligned_vector_store  q0, a2

                addi a9, a9, -1
                bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_prelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder:
        l32i a9, a4, 140       # a9: n_remainder
        beqz a9, tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder_end

        ssr  a12                # ssr: activation_shift
        movi a12, 15
        sub  a12, a12, a7       # a12: 15 - c_remainder
        movi a13,  0            # a13: zero

        tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a10
            tie728_s16_unaligned_conv2d_11c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a12, a13
            tie728_s16_element_round_result  a14, a8, a15, q0
            tie728_s16_element_prelu  a14, a11, a15
            tie728_s16_element_store   a2, a14

            addi a9, a9, -1
            bnez a9, tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_11cn_bias_prelu_n_remainder_end:

    retw






############################################################################################################################################################
####
#### tie728_s16_unaligned_conv2d_33cn series
####
############################################################################################################################################################
.macro tie728_s16_unaligned_conv2d_33c8 input_v0, input_front, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset
    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro tie728_s16_unaligned_conv2d_33c1 input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, temp, zero
    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_y_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn
    .type   dl_tie728_s16_unaligned_conv2d_33cn, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_64b
        tie728_s16_unaligned_conv2d_33cn_32b:
            tie728_s16_unaligned_conv2d_33cn_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_n_remainder

        tie728_s16_unaligned_conv2d_33cn_64b:
            tie728_s16_unaligned_conv2d_33cn_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_n_remainder

        tie728_s16_unaligned_conv2d_33cn_128b:
            tie728_s16_unaligned_conv2d_33cn_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_n_remainder_end

        movi a12, 15
        sub  a12, a12, a7       # a12: 15 - c_remainder
        movi a13,  0            # a13: activation_shift = zero

        tie728_s16_unaligned_conv2d_33cn_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a12, a13
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_store  a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn_relu
    .type   dl_tie728_s16_unaligned_conv2d_33cn_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn_relu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)
    movi a12,  0                # a12: activation_alpha = zero
    movi a13,  0                # a13: activation_shift = zero

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_relu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_relu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_relu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_relu_64b
        tie728_s16_unaligned_conv2d_33cn_relu_32b:
            tie728_s16_unaligned_conv2d_33cn_relu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a12, a13
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_relu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_relu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_relu_64b:
            tie728_s16_unaligned_conv2d_33cn_relu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a12, a13
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_relu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_relu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_relu_128b:
            tie728_s16_unaligned_conv2d_33cn_relu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a12, a13
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_relu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_relu_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_relu_n_remainder_end

        movi a12, 15
        sub  a12, a12, a7       # a12: 15 - c_remainder

        tie728_s16_unaligned_conv2d_33cn_relu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a12, a13
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_relu  a14
            tie728_s16_element_store  a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_relu_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_relu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn_leakyrelu
    .type   dl_tie728_s16_unaligned_conv2d_33cn_leakyrelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn_leakyrelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)
    l32i a12, a4,  76           # a12: activation_alpha
    l32i a13, a4,  84           # a13: activation_shift

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_leakyrelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_leakyrelu_64b
        tie728_s16_unaligned_conv2d_33cn_leakyrelu_32b:
            tie728_s16_unaligned_conv2d_33cn_leakyrelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a12, a13
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_leakyrelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_leakyrelu_64b:
            tie728_s16_unaligned_conv2d_33cn_leakyrelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a12, a13
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_leakyrelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_leakyrelu_128b:
            tie728_s16_unaligned_conv2d_33cn_leakyrelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a12, a13
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_leakyrelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder_end

        ssr  a13                # ssr: activation_shift
        movi a13, 15
        sub  a13, a13, a7       # a13: 15 - c_remainder
        # movi a14,  0            # a14: zero

        tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            movi a14,  0            # a14: zero
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a13, a14
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_leakyrelu  a14, a12
            tie728_s16_element_store   a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_leakyrelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn_prelu
    .type   dl_tie728_s16_unaligned_conv2d_33cn_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn_prelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)
    l32i a12, a4,  80           # a12: activation_alpha_ptr
    l32i a13, a4,  84           # a13: activation_shift

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_prelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_prelu_64b
        tie728_s16_unaligned_conv2d_33cn_prelu_32b:
            tie728_s16_unaligned_conv2d_33cn_prelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a12, a13
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_prelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_prelu_64b:
            tie728_s16_unaligned_conv2d_33cn_prelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a12, a13
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_prelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_prelu_128b:
            tie728_s16_unaligned_conv2d_33cn_prelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a12, a13
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_prelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder_end

        ssr  a13                # ssr: activation_shift
        movi a13, 15
        sub  a13, a13, a7       # a13: 15 - c_remainder

        tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            movi a14,  0        # a14: zero
            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a13, a14
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_prelu  a14, a12, a15
            tie728_s16_element_store   a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_prelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn_bias
    .type   dl_tie728_s16_unaligned_conv2d_33cn_bias, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn_bias:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)
    l32i a12, a4,  68           # a12: bias_ptr

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_bias_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_bias_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_bias_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_bias_64b
        tie728_s16_unaligned_conv2d_33cn_bias_32b:
            tie728_s16_unaligned_conv2d_33cn_bias_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_64b:
            tie728_s16_unaligned_conv2d_33cn_bias_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_128b:
            tie728_s16_unaligned_conv2d_33cn_bias_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_bias_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_bias_n_remainder_end

        movi a13, 15
        sub  a13, a13, a7       # a13: 15 - c_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a12
            movi a14,  0        # a14: zero
            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a13, a14
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_store  a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_bias_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn_bias_relu
    .type   dl_tie728_s16_unaligned_conv2d_33cn_bias_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn_bias_relu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)
    l32i a12, a4,  68           # a12: bias_ptr
    movi a13,  0                # a13: activation_alpha = zero
    movi a14,  0                # a14: activation_shift = zero

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_bias_relu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_bias_relu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_bias_relu_64b
        tie728_s16_unaligned_conv2d_33cn_bias_relu_32b:
            tie728_s16_unaligned_conv2d_33cn_bias_relu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_relu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_relu_64b:
            tie728_s16_unaligned_conv2d_33cn_bias_relu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_relu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_relu_128b:
            tie728_s16_unaligned_conv2d_33cn_bias_relu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_relu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_end

        movi a13, 15
        sub  a13, a13, a7       # a13: 15 - c_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a12
            movi a14,  0        # a14: zero
            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a13, a14
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_relu  a14
            tie728_s16_element_store  a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu
    .type   dl_tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)
    l32i a12, a4,  68           # a12: bias_ptr
    l32i a13, a4,  76           # a13: activation_alpha
    l32i a14, a4,  84           # a14: activation_shift

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_64b
        tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_32b:
            tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_64b:
            tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_128b:
            tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_end

        movi a10, 15
        sub  a10, a10, a7       # a10: 15 - c_remainder
        ssr  a14                # ssr: activation_shift

        tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a12
            l32i  a9, a4, 112   #  a9: dilation_y_offset
            movi a14,  0        # a14: zero
            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a14

            l32i  a9, a4,  64   #  a9: mac_shift         = output.exponent - filter.exponent - input.exponent
            tie728_s16_element_round_result  a14, a9, a15, q0
            tie728_s16_element_leakyrelu  a14, a13
            tie728_s16_element_store   a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_33cn_bias_prelu
    .type   dl_tie728_s16_unaligned_conv2d_33cn_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_33cn_bias_prelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args, 15 - input_channel % (vector_width / element_width) * sizeof(feature_t)

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a10, a4,  64           # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
    l32i a11, a4,  96           # a11: n_div_x           = output_channel / (vector_width / element_width)
    l32i a12, a4,  68           # a12: bias_ptr
    l32i a13, a4,  80           # a13: activation_alpha_ptr
    l32i a14, a4,  84           # a14: activation_shift

    blti a11, 1, tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            #  a15: output_sar_byte

    tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_33cn_bias_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_33cn_bias_prelu_64b
        tie728_s16_unaligned_conv2d_33cn_bias_prelu_32b:
            tie728_s16_unaligned_conv2d_33cn_bias_prelu_32b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a13, a14
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_prelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_prelu_64b:
            tie728_s16_unaligned_conv2d_33cn_bias_prelu_64b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a13, a14
                tie728_64b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_prelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder

        tie728_s16_unaligned_conv2d_33cn_bias_prelu_128b:
            tie728_s16_unaligned_conv2d_33cn_bias_prelu_128b_multiple_loop:
                mov a15, a3     # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a12
                tie728_s16_unaligned_conv2d_33c8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a13, a14
                tie728_128b_aligned_vector_store  q0, a2

                addi a11, a11, -1
                bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_prelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder:
        l32i a11, a4, 140       # a11: n_remainder
        beqz a11, tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder_end

        movi a10, 15
        sub  a10, a10, a7       # a10: 15 - c_remainder
        ssr  a14                # ssr: activation_shift

        tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder_loop:
            mov a15, a3         # a15: input_ptr
            EE.ZERO.ACCX

            tie728_s16_conv2d_element_bias  a12
            l32i  a9, a4, 112   #  a9: dilation_y_offset
            movi a14,  0        # a14: zero
            tie728_s16_unaligned_conv2d_33c1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a14

            l32i  a9, a4,  64   # a9: mac_shift         = output.exponent - filter.exponent - input.exponent
            tie728_s16_element_round_result  a14, a9, a15, q0
            tie728_s16_element_prelu  a14, a13, a15
            tie728_s16_element_store   a2, a14

            addi a11, a11, -1
            bnez a11, tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_33cn_bias_prelu_n_remainder_end:

    retw






############################################################################################################################################################
####
#### tie728_s16_unaligned_conv2d_hwcn series
####
############################################################################################################################################################
.macro tie728_s16_unaligned_conv2d_hwc8 input_v0, input_front, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, filter_h, filter_w, args, filter_offset_q
    l32i \filter_h, \args,  52  # filter_height
    10:
        l32i \filter_w, \args,  56  # filter_width
        beqi \filter_w, 1, 11f
        9:
            tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder
            add \input_ptr, \input_ptr, \dilation_x_offset

            addi \filter_w, \filter_w, -1
            bgei \filter_w, 2, 9b
        11:
        tie728_s16_unaligned_conv2d_11c8 \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder

        EE.MOVI.32.A \filter_offset_q, \filter_w, 1
        add \filter_ptr, \filter_ptr, \filter_w
        add \input_ptr, \input_ptr, \dilation_y_offset

        addi \filter_h, \filter_h, -1
        bnez \filter_h, 10b

    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm



.macro tie728_s16_unaligned_conv2d_hwc1 input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, filter_h, filter_w, args, temp, zero, filter_offset_q
    l32i \filter_h, \args,  52  # filter_height
    10:
        l32i \filter_w, \args,  56  # filter_width
        beqi \filter_w, 1, 11f
        9:
            tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
            add \input_ptr, \input_ptr, \dilation_x_offset

            addi \filter_w, \filter_w, -1
            bgei \filter_w, 2, 9b
        11:
        tie728_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero

        EE.MOVI.32.A \filter_offset_q, \filter_w, 1
        add \filter_ptr, \filter_ptr, \filter_w
        add \input_ptr, \input_ptr, \dilation_y_offset

        addi \filter_h, \filter_h, -1
        bnez \filter_h, 10b

    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn
    .type   dl_tie728_s16_unaligned_conv2d_hwcn, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48               #  a5: filter_ptr
    l32i  a6, a4, 100               #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136               #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108               #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112               #  a9: dilation_y_offset
    l32i a12, a4,  96               # a12: n_div_x           = output_channel / (vector_width / element_width)
    l32i a13, a4, 64                # a13: mac_shift         = output.exponent - filter.exponent - input.exponent

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_64b
        tie728_s16_unaligned_conv2d_hwcn_32b:
            tie728_s16_unaligned_conv2d_hwcn_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7
                tie728_s16_vector_round_result  q0, a13, a15, q1
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_64b:
            tie728_s16_unaligned_conv2d_hwcn_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7
                tie728_s16_vector_round_result  q0, a13, a15, q1
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_128b:
            tie728_s16_unaligned_conv2d_hwcn_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7
                tie728_s16_vector_round_result  q0, a13, a15, q1
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder
        # movi a14,  0                # a14: zero

        tie728_s16_unaligned_conv2d_hwcn_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            movi a14,  0                # a14: zero
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a10, a4, 64        # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_store  a2, a14

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn_relu
    .type   dl_tie728_s16_unaligned_conv2d_hwcn_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn_relu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48               #  a5: filter_ptr
    l32i  a6, a4, 100               #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136               #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108               #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112               #  a9: dilation_y_offset
    l32i a12, a4,  96               # a12: n_div_x           = output_channel / (vector_width / element_width)
    movi a13, 0                     # a13: zero
    movi a14, 0                     # a14: zero

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_relu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_relu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_relu_64b
        tie728_s16_unaligned_conv2d_hwcn_relu_32b:
            tie728_s16_unaligned_conv2d_hwcn_relu_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_relu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_relu_64b:
            tie728_s16_unaligned_conv2d_hwcn_relu_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_relu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_relu_128b:
            tie728_s16_unaligned_conv2d_hwcn_relu_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_relu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder

        tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            movi a14,  0            # a14: zero
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a10, a4, 64        # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_relu  a14
            tie728_s16_element_store  a2, a14

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_relu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn_leakyrelu
    .type   dl_tie728_s16_unaligned_conv2d_hwcn_leakyrelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn_leakyrelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48               #  a5: filter_ptr
    l32i  a6, a4, 100               #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136               #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108               #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112               #  a9: dilation_y_offset
    l32i a12, a4,  96               # a12: n_div_x           = output_channel / (vector_width / element_width)
    l32i a13, a4,  76               # a13: activation_alpha
    l32i a14, a4,  84               # a14: activation_shift

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_64b
        tie728_s16_unaligned_conv2d_hwcn_leakyrelu_32b:
            tie728_s16_unaligned_conv2d_hwcn_leakyrelu_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_leakyrelu_64b:
            tie728_s16_unaligned_conv2d_hwcn_leakyrelu_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_leakyrelu_128b:
            tie728_s16_unaligned_conv2d_hwcn_leakyrelu_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a13, a14
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        EE.MOVI.32.Q q6, a13, 0     # q6[0]: activation_alpha
        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder
        ssr  a14                    # ssr: activation_shift
        # movi a14, 0                 # a14: zero

        tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            movi a14, 0             # a14: zero
            EE.ZERO.ACCX

            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a10, a4, 64        # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
            EE.MOVI.32.A q6, a11, 0 # a11: activation_alpha
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_leakyrelu  a14, a11
            tie728_s16_element_store   a2, a14

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_leakyrelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn_prelu
    .type   dl_tie728_s16_unaligned_conv2d_hwcn_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn_prelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a12, a4,  96           # a12: n_div_x           = output_channel / (vector_width / element_width)
    l32i a13, a4,  80           # a13: activation_alpha_ptr
    l32i a14, a4,  84           # a14: activation_shift

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_prelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_prelu_64b
        tie728_s16_unaligned_conv2d_hwcn_prelu_32b:
            tie728_s16_unaligned_conv2d_hwcn_prelu_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a13, a14
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_prelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_prelu_64b:
            tie728_s16_unaligned_conv2d_hwcn_prelu_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a13, a14
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_prelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_prelu_128b:
            tie728_s16_unaligned_conv2d_hwcn_prelu_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a13, a14
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_prelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        EE.MOVI.32.Q q6, a13, 0     # q6[0]: activation_alpha_ptr
        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder
        ssr  a14                    # ssr: activation_shift

        tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            EE.ZERO.ACCX

            movi a14, 0             # a14: zero
            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a10, a4, 64        # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
            EE.MOVI.32.A q6, a11, 0 # a11: activation_alpha_ptr
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_prelu  a14, a11, a15
            tie728_s16_element_store   a2, a14
            EE.MOVI.32.Q q6, a11, 0 # q6[0]: activation_alpha_ptr

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_prelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn_bias
    .type   dl_tie728_s16_unaligned_conv2d_hwcn_bias, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn_bias:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48               #  a5: filter_ptr
    l32i  a6, a4, 100               #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136               #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108               #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112               #  a9: dilation_y_offset
    l32i a12, a4,  96               # a12: n_div_x           = output_channel / (vector_width / element_width)
    l32i a13, a4,  68               # a13: bias_ptr
    l32i a14, a4,  64               # a14: mac_shift         = output.exponent - filter.exponent - input.exponent

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_bias_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_bias_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_bias_64b
        tie728_s16_unaligned_conv2d_hwcn_bias_32b:
            tie728_s16_unaligned_conv2d_hwcn_bias_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7
                tie728_s16_vector_round_result  q0, a14, a15, q1
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_64b:
            tie728_s16_unaligned_conv2d_hwcn_bias_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7
                tie728_s16_vector_round_result  q0, a14, a15, q1
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_128b:
            tie728_s16_unaligned_conv2d_hwcn_bias_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7
                tie728_s16_vector_round_result  q0, a14, a15, q1
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        EE.MOVI.32.Q q6, a13, 1     # q6[1]: bias_ptr
        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder
        l32i a15, a4, 84
        ssr  a15                    # ssr: activation_shift

        tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            EE.ZERO.ACCX

            EE.MOVI.32.A q6, a14, 1 # a14: bias_ptr
            tie728_s16_conv2d_element_bias  a14
            EE.MOVI.32.Q q6, a14, 1 # q6[1]: bias_ptr
            movi a14, 0             # a14: zero
            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a10, a4, 64        # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_store  a2, a14

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_bias_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn_bias_relu
    .type   dl_tie728_s16_unaligned_conv2d_hwcn_bias_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn_bias_relu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a12, a4,  96           # a12: n_div_x           = output_channel / (vector_width / element_width)
    l32i a13, a4,  68           # a13: bias_ptr
    l32i a14, a4,  76           # a14: activation_alpha

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_bias_relu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_bias_relu_64b
        tie728_s16_unaligned_conv2d_hwcn_bias_relu_32b:
            tie728_s16_unaligned_conv2d_hwcn_bias_relu_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a14, a11
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_relu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_relu_64b:
            tie728_s16_unaligned_conv2d_hwcn_bias_relu_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a14, a11
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_relu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_relu_128b:
            tie728_s16_unaligned_conv2d_hwcn_bias_relu_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a14, a11
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_relu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        EE.MOVI.32.Q q6, a13, 1     # q6[1]: bias_ptr
        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder
        l32i a15, a4, 84
        ssr  a15                    # ssr: activation_shift

        tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            EE.ZERO.ACCX

            EE.MOVI.32.A q6, a14, 1 # a14: bias_ptr
            tie728_s16_conv2d_element_bias  a14
            EE.MOVI.32.Q q6, a14, 1 # q6[1]: bias_ptr
            movi a14, 0             # a14: zero
            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a10, a4, 64        # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
            tie728_s16_element_round_result  a14, a10, a15, q0
            tie728_s16_element_relu  a14
            tie728_s16_element_store  a2, a14

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu
    .type   dl_tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a12, a4,  96           # a12: n_div_x           = output_channel / (vector_width / element_width)
    l32i a13, a4,  68           # a13: bias_ptr
    l32i a14, a4,  76           # a14: activation_alpha

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_64b
        tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_32b:
            tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a14, a11
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_64b:
            tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a14, a11
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_128b:
            tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_relu  q0, a14, a11
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        EE.MOVI.32.Q q6, a13, 1     # q6[1]: bias_ptr
        EE.MOVI.32.Q q6, a14, 0     # q6[0]: activation_alpha
        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder
        l32i a15, a4, 84
        ssr  a15                    # ssr: activation_shift

        tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            EE.ZERO.ACCX

            EE.MOVI.32.A q6, a14, 1 # a14: bias_ptr
            tie728_s16_conv2d_element_bias  a14
            EE.MOVI.32.Q q6, a14, 1 # q6[1]: bias_ptr
            l32i  a9, a4, 112       #  a9: dilation_y_offset
            movi a14, 0             # a14: zero
            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a9, a4, 64         #  a9: mac_shift         = output.exponent - filter.exponent - input.exponent
            EE.MOVI.32.A q6, a11, 0 # a11: activation_alpha
            tie728_s16_element_round_result  a14, a9, a15, q0
            tie728_s16_element_leakyrelu  a14, a11
            tie728_s16_element_store   a2, a14

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_conv2d_hwcn_bias_prelu
    .type   dl_tie728_s16_unaligned_conv2d_hwcn_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_conv2d_hwcn_bias_prelu:
    .align 4
    entry sp, 128
    # a2: int16_t *output_ptr
    # a3: int16_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48           #  a5: filter_ptr
    l32i  a6, a4, 100           #  a6: c_div_x_1         = input_channel / (vector_width / element_width) - 1
    l32i  a7, a4, 136           #  a7: c_remainder       = input_channel % (vector_width / element_width) * sizeof(feature_t)
    l32i  a8, a4, 108           #  a8: dilation_x_offset = (dilation_x * input_channel_with_padding - input_channel) * sizeof(feature_t)
    l32i  a9, a4, 112           #  a9: dilation_y_offset
    l32i a12, a4,  96           # a12: n_div_x           = output_channel / (vector_width / element_width)
    l32i a13, a4,  68           # a13: bias_ptr
    l32i a14, a4,  80           # a14: activation_alpha_ptr

    blti a12, 1, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder
    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15            # a15: output_sar_byte

    tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_div_x:
        beqi a15, 0, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_64b
        tie728_s16_unaligned_conv2d_hwcn_bias_prelu_32b:
            tie728_s16_unaligned_conv2d_hwcn_bias_prelu_32b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a14, a11
                tie728_32b_aligned_vector_store  q0, a2, a15

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_32b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_prelu_64b:
            tie728_s16_unaligned_conv2d_hwcn_bias_prelu_64b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a14, a11
                tie728_64b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_64b_multiple_loop
            j tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder

        tie728_s16_unaligned_conv2d_hwcn_bias_prelu_128b:
            tie728_s16_unaligned_conv2d_hwcn_bias_prelu_128b_multiple_loop:
                mov a15, a3         # a15: input_ptr
                EE.ZERO.QACC

                tie728_s16_conv2d_128b_vector_bias  a13
                tie728_s16_unaligned_conv2d_hwc8 q0, q1, q2, q3, q4, a15, a5, a6, a7, a8, a9, a10, a11, a4, q7

                l32i a10, a4, 64    # a10: mac_shift         = output.exponent - filter.exponent - input.exponent
                l32i a11, a4, 84    # a11: activation_shift
                tie728_s16_vector_round_result  q0, a10, a15, q1
                tie728_s16_conv2d_prelu  q0, q1, a14, a11
                tie728_128b_aligned_vector_store  q0, a2

                addi a12, a12, -1
                bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_128b_multiple_loop


    tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder:
        l32i a12, a4, 140           # a12: n_remainder
        beqz a12, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder_end

        l32i a5, a4, 160
        l32i a15, a4, 164
        EE.MOVI.32.Q q7, a5, 1
        EE.MOVI.32.Q q7, a15, 2
        l32i a5, a4,  168 # filter_ptr unaligned

        EE.MOVI.32.Q q6, a13, 1     # q6[1]: bias_ptr
        EE.MOVI.32.Q q6, a14, 0     # q6[0]: activation_alpha_ptr
        movi a13, 15
        sub  a13, a13, a7           # a13: 15 - c_remainder
        l32i a15, a4, 84
        ssr  a15                    # ssr: activation_shift

        tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder_loop:
            mov a15, a3             # a15: input_ptr
            EE.ZERO.ACCX

            EE.MOVI.32.A q6, a14, 1 # a14: bias_ptr
            tie728_s16_conv2d_element_bias  a14
            EE.MOVI.32.Q q6, a14, 1 # q6[1]: bias_ptr
            l32i  a9, a4, 112       #  a9: dilation_y_offset
            movi a14, 0             # a14: zero
            tie728_s16_unaligned_conv2d_hwc1 q0, q1, q2, q3, q4, q5, a15, a5, a6, a7, a8, a9, a10, a11, a4, a13, a14, q7

            l32i a9, a4, 64         #  a9: mac_shift         = output.exponent - filter.exponent - input.exponent
            EE.MOVI.32.A q6, a11, 0 # a11: activation_alpha_ptr
            tie728_s16_element_round_result  a14, a9, a15, q0
            tie728_s16_element_prelu  a14, a11, a15
            tie728_s16_element_store   a2, a14
            EE.MOVI.32.Q q6, a11, 0 # q6[0]: activation_alpha_ptr

            addi a12, a12, -1
            bnez a12, tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder_loop

        tie728_s16_unaligned_conv2d_hwcn_bias_prelu_n_remainder_end:

    retw
