#include "dl_tie728_s16_unaligned.S"
#include "dl_tie728_s16.S"

############################################################################################################################################################
####
#### tie728_s16_unaligned_depthwise_conv2d_33c1 series
####
############################################################################################################################################################
.macro tie728_s16_unaligned_depthwise_conv2d_3381 input_v0, input_v1, input_v2, input_back, filter_v0, filter_v1, filter_v2, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, next_3381_16
    # EE.ZERO.QACC

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_y_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    EE.SRC.Q.LD.IP           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v2,  \filter_ptr, 16,                \input_v1, \filter_v1
    EE.SRC.Q.LD.IP           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v2
    EE.SRC.Q.LD.IP           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_y_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    EE.SRC.Q.LD.IP           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v2,  \filter_ptr, 16,                \input_v1, \filter_v1
    EE.SRC.Q.LD.IP           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v2
    EE.SRC.Q.LD.IP           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \next_3381_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    EE.SRC.Q.LD.IP           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v2,  \filter_ptr, 16,                \input_v1, \filter_v1
    EE.SRC.Q.LD.IP           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v2
    EE.SRC.Q.LD.IP           \input_v2,   \input_ptr,  16,                \input_v1, \input_back
.endm



.macro tie728_s16_unaligned_depthwise_conv2d_3381_last input_v0, input_v1, input_v2, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, next_3381_16
    # EE.ZERO.QACC

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_y_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    EE.SRC.Q.LD.IP           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v0,  \filter_ptr, 16,                \input_v1, \filter_v1
    EE.SRC.Q.LD.IP           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v1,  \filter_ptr, 16,                \input_v2, \filter_v0
    EE.SRC.Q.LD.IP           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_y_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v0,  \filter_ptr, 16,                \input_v0, \filter_v1
    EE.SRC.Q.LD.IP           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v1,  \filter_ptr, 16,                \input_v1, \filter_v0
    EE.SRC.Q.LD.IP           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \dilation_x_offset_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v1
    EE.SRC.Q.LD.IP           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    EE.LD.128.USAR.XP        \input_back, \input_ptr,  \next_3381_16
    EE.VMULAS.S16.QACC.LD.IP \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    EE.SRC.Q                 \input_v2,   \input_v2,   \input_back

    EE.VMULAS.S16.QACC.LD.IP \filter_v0,  \filter_ptr, 16,                \input_v1, \filter_v1
    EE.VMULAS.S16.QACC       \input_v2,   \filter_v0
.endm



.macro tie728_s16_unaligned_depthwise_conv2d_11r1 input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_remainder, forward
    EE.LD.128.USAR.IP  \input_v0,   \input_ptr, 16
    EE.VLD.128.XP      \input_back, \input_ptr, \forward
    EE.SRC.Q           \input_v0,   \input_v0,  \input_back

    EE.LD.128.USAR.XP  \filter_v0,   \filter_ptr, \c_remainder
    EE.VLD.128.IP      \filter_back, \filter_ptr, 0
    EE.SRC.Q           \filter_v0,   \filter_v0,  \filter_back

    EE.VMULAS.S16.QACC \input_v0,    \filter_v0
.endm



.macro tie728_s16_unaligned_depthwise_conv2d_33r1 input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, c_remainder
    # EE.ZERO.QACC

    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_16

    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_16

    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
    tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_16
.endm




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_33c1
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_33c1, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_33c1:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
    l32i  a8, a4, 132
    addi  a8, a8, -16                   # a8: next_3381 - 16
    l32i  a9, a4, 100                   # a9: c_div_x_1
    l32i a10, a4,  64                   # a10: mac_shift
    # l32i a11, a4,  68                 # a11: bias_ptr
    l32i a12, a4,  76                   # a12: activation_alpha
    l32i a13, a4,  84                   # a13: activation_shift

    blti a9, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_c_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                    # a15: output_sar_byte

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q4, a5, 16, q0, q3   # q4: filter_v0; q0: input_v0

    EE.LD.128.USAR.IP q1, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q2, a3, 16, q1, q3   # q2: input_v2; q1: input_v1

    tie728_s16_unaligned_depthwise_conv2d_33c1_c_div_x:
        beqi a15, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_128b
        beqi a15, 8, tie728_s16_unaligned_depthwise_conv2d_33c1_64b
        tie728_s16_unaligned_depthwise_conv2d_33c1_32b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_32b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_32b_aligned_vector_store  q3, a2, a15

            tie728_s16_unaligned_depthwise_conv2d_33c1_32b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_32b_aligned_vector_store  q3, a2, a15

            j tie728_s16_unaligned_depthwise_conv2d_33c1_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_64b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_64b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_64b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_64b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_64b_aligned_vector_store  q3, a2

            j tie728_s16_unaligned_depthwise_conv2d_33c1_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_128b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_128b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_128b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_128b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_128b_aligned_vector_store  q3, a2


    tie728_s16_unaligned_depthwise_conv2d_33c1_c_remainder:
        l32i a9, a4, 136                # a9: c_remainder
        beqz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_c_remainder_end
            EE.ZERO.QACC
            srli a14, a9, 1
            tie728_s16_unaligned_depthwise_conv2d_33r1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9
            tie728_s16_vector_round_result  q0, a10, a15, q5
            tie728_s16_variable_vector_store  a2, q0, a14, a15

        tie728_s16_unaligned_depthwise_conv2d_33c1_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_33c1_relu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_33c1_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_33c1_relu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
    l32i  a8, a4, 132
    addi  a8, a8, -16                   # a8: next_3381 - 16
    l32i  a9, a4, 100                   # a9: c_div_x_1
    l32i a10, a4,  64                   # a10: mac_shift
    # l32i a11, a4,  68                 # a11: bias_ptr
    l32i a12, a4,  76                   # a12: activation_alpha
    l32i a13, a4,  84                   # a13: activation_shift

    blti a9, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                    # a15: output_sar_byte

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q4, a5, 16, q0, q3   # q4: filter_v0; q0: input_v0

    EE.LD.128.USAR.IP q1, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q2, a3, 16, q1, q3   # q2: input_v2; q1: input_v1

    tie728_s16_unaligned_depthwise_conv2d_33c1_relu_c_div_x:
        beqi a15, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_relu_128b
        beqi a15, 8, tie728_s16_unaligned_depthwise_conv2d_33c1_relu_64b
        tie728_s16_unaligned_depthwise_conv2d_33c1_relu_32b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_relu_32b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_relu  q3, a12, a13
                tie728_32b_aligned_vector_store  q3, a2, a15

            tie728_s16_unaligned_depthwise_conv2d_33c1_relu_32b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_relu  q3, a12, a13
            tie728_32b_aligned_vector_store  q3, a2, a15

            j tie728_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_relu_64b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_relu_64b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_relu  q3, a12, a13
                tie728_64b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_relu_64b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_relu  q3, a12, a13
            tie728_64b_aligned_vector_store  q3, a2

            j tie728_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_relu_128b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_relu_128b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_relu  q3, a12, a13
                tie728_128b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_relu_128b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_relu  q3, a12, a13
            tie728_128b_aligned_vector_store  q3, a2


    tie728_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder:
        l32i a9, a4, 136                # a9: c_remainder
        beqz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder_end
            EE.ZERO.QACC
            srli a14, a9, 1
            tie728_s16_unaligned_depthwise_conv2d_33r1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9
            tie728_s16_vector_round_result  q0, a10, a15, q5
            tie728_s16_conv2d_relu  q0, a12, a13
            tie728_s16_variable_vector_store  a2, q0, a14, a15

        tie728_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_33c1_prelu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_33c1_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_33c1_prelu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
    l32i  a8, a4, 132
    addi  a8, a8, -16                   # a8: next_3381 - 16
    l32i  a9, a4, 100                   # a9: c_div_x_1
    l32i a10, a4,  64                   # a10: mac_shift
    # l32i a11, a4,  68                 # a11: bias_ptr
    l32i a12, a4,  80                   # a12: activation_alpha_ptr
    l32i a13, a4,  84                   # a13: activation_shift

    blti a9, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_c_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                    # a15: output_sar_byte

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q4, a5, 16, q0, q3   # q4: filter_v0; q0: input_v0

    EE.LD.128.USAR.IP q1, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q2, a3, 16, q1, q3   # q2: input_v2; q1: input_v1

    tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_c_div_x:
        beqi a15, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_64b
        tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_32b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_32b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_prelu  q3, q5, a12, a13
                tie728_32b_aligned_vector_store  q3, a2, a15

            tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_32b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_prelu  q3, q5, a12, a13
            tie728_32b_aligned_vector_store  q3, a2, a15

            j tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_64b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_64b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_prelu  q3, q5, a12, a13
                tie728_64b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_64b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_prelu  q3, q5, a12, a13
            tie728_64b_aligned_vector_store  q3, a2

            j tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_128b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_128b_last
                EE.ZERO.QACC
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_prelu  q3, q5, a12, a13
                tie728_128b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_128b_last:
            EE.ZERO.QACC
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_prelu  q3, q5, a12, a13
            tie728_128b_aligned_vector_store  q3, a2


    tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_c_remainder:
        l32i a9, a4, 136                # a9: c_remainder
        beqz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_c_remainder_end
            EE.ZERO.QACC
            srli a14, a9, 1
            tie728_s16_unaligned_depthwise_conv2d_33r1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9
            tie728_s16_vector_round_result  q0, a10, a15, q5
            tie728_s16_conv2d_prelu  q0, q5, a12, a13
            tie728_s16_variable_vector_store  a2, q0, a14, a15

        tie728_s16_unaligned_depthwise_conv2d_33c1_prelu_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
    l32i  a8, a4, 132
    addi  a8, a8, -16                   # a8: next_3381 - 16
    l32i  a9, a4, 100                   # a9: c_div_x_1
    l32i a10, a4,  64                   # a10: mac_shift
    l32i a11, a4,  68                   # a11: bias_ptr
    # l32i a12, a4,  76                 # a12: activation_alpha
    # l32i a13, a4,  84                 # a13: activation_shift

    blti a9, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                    # a15: output_sar_byte

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q4, a5, 16, q0, q3   # q4: filter_v0; q0: input_v0

    EE.LD.128.USAR.IP q1, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q2, a3, 16, q1, q3   # q2: input_v2; q1: input_v1

    tie728_s16_unaligned_depthwise_conv2d_33c1_bias_c_div_x:
        beqi a15, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_128b
        beqi a15, 8, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_64b
        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_32b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_32b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_32b_aligned_vector_store  q3, a2, a15

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_32b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_32b_aligned_vector_store  q3, a2, a15

            j tie728_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_64b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_64b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_64b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_64b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_64b_aligned_vector_store  q3, a2

            j tie728_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_128b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_128b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_128b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_128b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_128b_aligned_vector_store  q3, a2


    tie728_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder:
        l32i a9, a4, 136                # a9: c_remainder
        beqz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder_end
            EE.ZERO.QACC
            srli a14, a9, 1
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_33r1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9
            tie728_s16_vector_round_result  q0, a10, a15, q5
            tie728_s16_variable_vector_store  a2, q0, a14, a15

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder_end:

    retw





    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
    l32i  a8, a4, 132
    addi  a8, a8, -16                   # a8: next_3381 - 16
    l32i  a9, a4, 100                   # a9: c_div_x_1
    l32i a10, a4,  64                   # a10: mac_shift
    l32i a11, a4,  68                   # a11: bias_ptr
    l32i a12, a4,  76                   # a12: activation_alpha
    l32i a13, a4,  84                   # a13: activation_shift

    blti a9, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                    # a15: output_sar_byte

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q4, a5, 16, q0, q3   # q4: filter_v0; q0: input_v0

    EE.LD.128.USAR.IP q1, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q2, a3, 16, q1, q3   # q2: input_v2; q1: input_v1

    tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_div_x:
        beqi a15, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b
        beqi a15, 8, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b
        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_relu  q3, a12, a13
                tie728_32b_aligned_vector_store  q3, a2, a15

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_relu  q3, a12, a13
            tie728_32b_aligned_vector_store  q3, a2, a15

            j tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_relu  q3, a12, a13
                tie728_64b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_relu  q3, a12, a13
            tie728_64b_aligned_vector_store  q3, a2

            j tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_relu  q3, a12, a13
                tie728_128b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_relu  q3, a12, a13
            tie728_128b_aligned_vector_store  q3, a2


    tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder:
        l32i a9, a4, 136                # a9: c_remainder
        beqz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder_end
            EE.ZERO.QACC
            srli a14, a9, 1
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_33r1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9
            tie728_s16_vector_round_result  q0, a10, a15, q5
            tie728_s16_conv2d_relu  q0, a12, a13
            tie728_s16_variable_vector_store  a2, q0, a14, a15

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
    l32i  a8, a4, 132
    addi  a8, a8, -16                   # a8: next_3381 - 16
    l32i  a9, a4, 100                   # a9: c_div_x_1
    l32i a10, a4,  64                   # a10: mac_shift
    l32i a11, a4,  68                   # a11: bias_ptr
    l32i a12, a4,  80                   # a12: activation_alpha_ptr
    l32i a13, a4,  84                   # a13: activation_shift

    blti a9, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_c_remainder

    EE.LD.128.USAR.IP q0, a2, 0
    rur.sar_byte a15                    # a15: output_sar_byte

    EE.LD.128.USAR.IP q0, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q4, a5, 16, q0, q3   # q4: filter_v0; q0: input_v0

    EE.LD.128.USAR.IP q1, a3, 16
    EE.LD.128.USAR.XP q3, a3, a6
    EE.SRC.Q.LD.IP q2, a3, 16, q1, q3   # q2: input_v2; q1: input_v1

    tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_c_div_x:
        beqi a15, 0, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_128b
        beqi a15, 8, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_64b
        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_32b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_32b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_prelu  q3, q5, a12, a13
                tie728_32b_aligned_vector_store  q3, a2, a15

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_32b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_prelu  q3, q5, a12, a13
            tie728_32b_aligned_vector_store  q3, a2, a15

            j tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_64b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_64b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_prelu  q3, q5, a12, a13
                tie728_64b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_64b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_prelu  q3, q5, a12, a13
            tie728_64b_aligned_vector_store  q3, a2

            j tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_c_remainder

        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_128b:
            loopgtz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_128b_last
                EE.ZERO.QACC
                tie728_s16_conv2d_128b_vector_bias  a11
                tie728_s16_unaligned_depthwise_conv2d_3381 q0, q1, q2, q3, q4, q5, q6, a3, a5, a6, a7, a8
                tie728_s16_vector_round_result  q3, a10, a15, q5
                tie728_s16_conv2d_prelu  q3, q5, a12, a13
                tie728_128b_aligned_vector_store  q3, a2

            tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_128b_last:
            EE.ZERO.QACC
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_3381_last q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a8
            tie728_s16_vector_round_result  q3, a10, a15, q5
            tie728_s16_conv2d_prelu  q3, q5, a12, a13
            tie728_128b_aligned_vector_store  q3, a2


    tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_c_remainder:
        l32i a9, a4, 136                # a9: c_remainder
        beqz a9, tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_c_remainder_end
            EE.ZERO.QACC
            srli a14, a9, 1
            tie728_s16_conv2d_128b_vector_bias  a11
            tie728_s16_unaligned_depthwise_conv2d_33r1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9
            tie728_s16_vector_round_result  q0, a10, a15, q5
            tie728_s16_conv2d_prelu  q0, q5, a12, a13
            tie728_s16_variable_vector_store  a2, q0, a14, a15
        tie728_s16_unaligned_depthwise_conv2d_33c1_bias_prelu_c_remainder_end:

    retw









############################################################################################################################################################
####
#### tie728_s16_unaligned_depthwise_conv2d_hwc1 series
####
############################################################################################################################################################
.macro tie728_s16_unaligned_depthwise_conv2d_1w81 input_v0, input_v1, input_back, input_ptr, filter_v0, filter_ptr, dilation_x_offset_16, dilation_y_offset, filter_w, filter_w_rs1_1 filter_y_offset
    loopgtz \filter_w_rs1_1, 1f
        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back

        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back
    1:

    bbci \filter_w, 0, 2f
        # three 8-input-element left
        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back

        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_y_offset
        EE.VMULAS.S16.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v0
        EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back

        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        EE.SRC.Q \input_v0, \input_v1, \input_back
        EE.LD.128.USAR.IP \input_v1, \input_ptr, 16
        j 3f

    2:  # two 8-input-element left
        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_y_offset
        EE.VMULAS.S16.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back

        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back
    3:
.endm



.macro tie728_s16_unaligned_depthwise_conv2d_1w81_last input_v0 input_v1 input_back input_ptr filter_v0 filter_ptr dilation_x_offset_16 filter_w filter_w_rs1_1 next_hws1 filter_y_offset
    loopgtz \filter_w_rs1_1, 4f
        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back

        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        EE.SRC.Q.LD.IP \input_v1, \input_ptr, 16, \input_v0, \input_back
    4:

    bbci \filter_w, 0, 5f
        # three 8-input-element left
        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VMULAS.S16.QACC.LD.IP \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        EE.SRC.Q.LD.IP \input_v0, \input_ptr, 16, \input_v1, \input_back

        EE.LD.128.USAR.XP \input_back, \input_ptr, \next_hws1
        EE.VMULAS.S16.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v0
        EE.SRC.Q \input_v0, \input_v0, \input_back

        EE.VMULAS.S16.QACC \input_v0, \filter_v0
        j 6f

    5:  # two 8-input-element left
        EE.LD.128.USAR.XP \input_back, \input_ptr, \next_hws1
        EE.VMULAS.S16.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        EE.SRC.Q \input_v1, \input_v1, \input_back

        EE.VMULAS.S16.QACC \input_v1, \filter_v0
    6:
.endm



.macro tie728_s16_unaligned_depthwise_conv2d_hw81 input_v0, input_v1, input_back, filter_v0, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, next_hws1, filter_h, filter_w, filter_w_rs1_1, args, filter_offset_q, filter_y_offset
    l32i \filter_h, \args, 52  # filter_height
    l32i \filter_w, \args, 56  # filter_width

    # EE.ZERO.QACC

    blti \filter_w, 2, 9f
        # filter_w >= 2
        EE.LD.128.USAR.IP \input_v1, \input_ptr, 16
        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_x_offset_16
        EE.VLD.128.IP \filter_v0, \filter_ptr, 16 # filter_v0
        EE.SRC.Q \input_v0, \input_v1, \input_back # input_v0
        EE.LD.128.USAR.IP \input_v1, \input_ptr, 16

        blti \filter_h, 2, 8f
            7:
                tie728_s16_unaligned_depthwise_conv2d_1w81 \input_v0, \input_v1, \input_back, \input_ptr, \filter_v0, \filter_ptr, \dilation_x_offset_16, \dilation_y_offset_16, \filter_w, \filter_w_rs1_1, \filter_y_offset
                addi \filter_h, \filter_h, -1
                bgei \filter_h, 2, 7b
        8:  # last y
        tie728_s16_unaligned_depthwise_conv2d_1w81_last \input_v0, \input_v1, \input_back, \input_ptr, \filter_v0, \filter_ptr, \dilation_x_offset_16, \filter_w, \filter_w_rs1_1, \next_hws1, \filter_y_offset
        j 12f

    9:  # filter_w == 1
        EE.LD.128.USAR.IP \input_v1, \input_ptr, 16
        EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_y_offset_16
        EE.VLD.128.XP \filter_v0, \filter_ptr, \filter_y_offset # filter_v0
        EE.SRC.Q \input_v0, \input_v1, \input_back # input_v0

        blti \filter_h, 2, 11f
            addi \filter_h, \filter_h, -1
            loopgtz \filter_h, 10f
                EE.LD.128.USAR.IP \input_v1, \input_ptr, 16
                EE.LD.128.USAR.XP \input_back, \input_ptr, \dilation_y_offset_16
                EE.VMULAS.S16.QACC.LD.XP \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
                EE.SRC.Q \input_v0, \input_v1, \input_back
            10:
        11: # last y
        EE.VMULAS.S16.QACC                                    \input_v0, \filter_v0
        sub \input_ptr, \input_ptr, \dilation_y_offset_16
        add \input_ptr, \input_ptr, \next_hws1

    12:
    EE.MOVI.32.A \filter_offset_q, \filter_h, 2
    add \filter_ptr, \filter_ptr, \filter_h
.endm



.macro tie728_s16_unaligned_depthwise_conv2d_11r1_padding input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_remainder, forward, filter_y_offset
    EE.LD.128.USAR.IP  \input_v0,   \input_ptr, 16
    EE.VLD.128.XP      \input_back, \input_ptr, \forward
    EE.SRC.Q           \input_v0,   \input_v0,  \input_back

    EE.LD.128.USAR.XP  \filter_v0,   \filter_ptr, \c_remainder
    EE.VLD.128.XP      \filter_back, \filter_ptr, \filter_y_offset
    EE.SRC.Q           \filter_v0,   \filter_v0,  \filter_back

    EE.VMULAS.S16.QACC \input_v0,    \filter_v0
.endm



.macro tie728_s16_unaligned_depthwise_conv2d_hwr1 input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, filter_h, filter_w, filter_w_rs1_1, c_remainder, args, filter_y_offset
    l32i \filter_h, \args, 52   # filter_height
    l32i \filter_w, \args, 56   # filter_width

    # EE.ZERO.QACC

    blti \filter_w, 2, 5f
        4:
            loopgtz \filter_w_rs1_1, 1f
                tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
                tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
            1:
            bbci \filter_w, 0, 2f
                # 3 left
                tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
                tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
                tie728_s16_unaligned_depthwise_conv2d_11r1_padding \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_16, \filter_y_offset
                j 3f
            2:
                # 2 left
                tie728_s16_unaligned_depthwise_conv2d_11r1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_16
                tie728_s16_unaligned_depthwise_conv2d_11r1_padding \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_16, \filter_y_offset
            3:
            addi \filter_h, \filter_h, -1
            bgei \filter_h, 1, 4b

        j 7f

    5:
        # filter_w == 1
        loopgtz \filter_h, 6f
            tie728_s16_unaligned_depthwise_conv2d_11r1_padding \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_16, \filter_y_offset
        6:
    7:
.endm




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_hwc1
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_hwc1, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_hwc1:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i a11, a4, 144
    l32i a15, a4, 60
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
                                        # a9
                                        # a10
    l32i a11, a4, 148                   # a11: filter_w_rs1_1
    l32i a12, a4, 100                   # a12: c_div_x_1
    l32i a13, a4,  64                   # a13: mac_shift
    # l32i a14, a4,  76                   # a14: activation_alpha
    # l32i a15, a4,  84                   # a15: activation_shift


    blti a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_c_remainder
        l32i  a8, a4, 132
        addi  a8, a8, -16               # a8: next_hw81 - 16

        EE.LD.128.USAR.IP q0, a2, 0
        rur.sar_byte a9                 # a9: output_sar_byte

        tie728_s16_unaligned_depthwise_conv2d_hwc1_c_div_x:
            beqi a9, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_128b
            beqi a9, 8, tie728_s16_unaligned_depthwise_conv2d_hwc1_64b
            tie728_s16_unaligned_depthwise_conv2d_hwc1_32b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_32b_multiple_loop:
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a15
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_32b_aligned_vector_store  q0, a2, a9

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_32b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_64b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_64b_multiple_loop:
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a15
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_64b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_64b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_128b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_128b_multiple_loop:
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a15
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_128b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_128b_multiple_loop


    tie728_s16_unaligned_depthwise_conv2d_hwc1_c_remainder:
        l32i a12, a4, 136                # a12: c_remainder
        beqz a12, tie728_s16_unaligned_depthwise_conv2d_hwc1_c_remainder_end
            EE.ZERO.QACC
            srli a8, a12, 1
            l32i a5, a4,  168 # filter_ptr unaligned
            l32i a15, a4, 160
            tie728_s16_unaligned_depthwise_conv2d_hwr1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9, a10, a11, a12, a4, a15
            tie728_s16_vector_round_result  q0, a13, a9, q5
            tie728_s16_variable_vector_store  a2, q0, a8, a9

        tie728_s16_unaligned_depthwise_conv2d_hwc1_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_relu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
                                        # a9
                                        # a10
    l32i a11, a4, 148                   # a11: filter_w_rs1_1
    l32i a12, a4, 100                   # a12: c_div_x_1
    l32i a13, a4,  64                   # a13: mac_shift
    l32i a14, a4,  76                   # a14: activation_alpha
    l32i a15, a4,  84                   # a15: activation_shift

    EE.MOVI.32.Q q7, a13, 3


    blti a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder
        l32i  a8, a4, 132
        addi  a8, a8, -16               # a8: next_hw81 - 16

        EE.LD.128.USAR.IP q0, a2, 0
        rur.sar_byte a9                 # a9: output_sar_byte

        tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_c_div_x:
            beqi a9, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_128b
            beqi a9, 8, tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_64b
            tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_32b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_32b_multiple_loop:
                    EE.MOVI.32.A q7, a13, 1
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_s16_conv2d_relu  q0, a14, a15
                    tie728_32b_aligned_vector_store  q0, a2, a9

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_32b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_64b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_64b_multiple_loop:
                    EE.MOVI.32.A q7, a13, 1
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_s16_conv2d_relu  q0, a14, a15
                    tie728_64b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_64b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_128b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_128b_multiple_loop:
                    EE.MOVI.32.A q7, a13, 1
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_s16_conv2d_relu  q0, a14, a15
                    tie728_128b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_128b_multiple_loop


    tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder:
        l32i a12, a4, 136                # a12: c_remainder
        beqz a12, tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder_end
            EE.ZERO.QACC
            srli a8, a12, 1
            l32i a5, a4,  168 # filter_ptr unaligned
            l32i a13, a4, 160
            tie728_s16_unaligned_depthwise_conv2d_hwr1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9, a10, a11, a12, a4, a13
            EE.MOVI.32.A q7, a13, 3
            tie728_s16_vector_round_result  q0, a13, a9, q5
            tie728_s16_conv2d_relu  q0, a14, a15
            tie728_s16_variable_vector_store  a2, q0, a8, a9

        tie728_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
                                        # a9
                                        # a10
    l32i a11, a4, 148                   # a11: filter_w_rs1_1
    l32i a12, a4, 100                   # a12: c_div_x_1
    l32i a13, a4,  64                   # a13: mac_shift
    l32i a14, a4,  80                   # a14: activation_alpha_ptr
    l32i a15, a4,  84                   # a15: activation_shift

    EE.MOVI.32.Q q7, a13, 3


    blti a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_c_remainder
        l32i  a8, a4, 132
        addi  a8, a8, -16               # a8: next_hw81 - 16

        EE.LD.128.USAR.IP q0, a2, 0
        rur.sar_byte a9                 # a9: output_sar_byte

        tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_c_div_x:
            beqi a9, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_128b
            beqi a9, 8, tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_64b
            tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_32b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_32b_multiple_loop:
                    EE.MOVI.32.A q7, a13, 1
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_s16_conv2d_prelu  q0, q5, a14, a15
                    tie728_32b_aligned_vector_store  q0, a2, a9

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_32b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_64b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_64b_multiple_loop:
                    EE.MOVI.32.A q7, a13, 1
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_s16_conv2d_prelu  q0, q5, a14, a15
                    tie728_64b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_64b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_128b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_128b_multiple_loop:
                    EE.MOVI.32.A q7, a13, 1
                    EE.ZERO.QACC
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_s16_conv2d_prelu  q0, q5, a14, a15
                    tie728_128b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_128b_multiple_loop


    tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_c_remainder:
        l32i a12, a4, 136                # a12: c_remainder
        beqz a12, tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_c_remainder_end
            EE.ZERO.QACC
            srli a8, a12, 1
            l32i a5, a4,  168 # filter_ptr unaligned
            l32i a13, a4, 160
            tie728_s16_unaligned_depthwise_conv2d_hwr1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9, a10, a11, a12, a4, a13
            EE.MOVI.32.A q7, a13, 3
            tie728_s16_vector_round_result  q0, a13, a9, q5
            tie728_s16_conv2d_prelu  q0, q5, a14, a15
            tie728_s16_variable_vector_store  a2, q0, a8, a9

        tie728_s16_unaligned_depthwise_conv2d_hwc1_prelu_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i a11, a4, 144
    l32i a15, a4, 60
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
                                        # a9
                                        # a10
    l32i a11, a4, 148                   # a11: filter_w_rs1_1
    l32i a12, a4, 100                   # a12: c_div_x_1
    l32i a13, a4,  64                   # a13: mac_shift
    l32i a14, a4,  68                   # a14: bias_ptr
                                        # a15


    blti a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder
        l32i  a8, a4, 132
        addi  a8, a8, -16               # a8: next_hw81 - 16

        EE.LD.128.USAR.IP q0, a2, 0
        rur.sar_byte a9                 # a9: output_sar_byte

        tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_c_div_x:
            beqi a9, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_128b
            beqi a9, 8, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_64b
            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_32b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_32b_multiple_loop:
                    EE.ZERO.QACC
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a15
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_32b_aligned_vector_store  q0, a2, a9

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_32b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_64b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_64b_multiple_loop:
                    EE.ZERO.QACC
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a15
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_64b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_64b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_128b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_128b_multiple_loop:
                    EE.ZERO.QACC
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a15
                    tie728_s16_vector_round_result  q0, a13, a9, q5
                    tie728_128b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_128b_multiple_loop


    tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder:
        l32i a12, a4, 136               # a12: c_remainder
        beqz a12, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder_end
            EE.ZERO.QACC
            srli a8, a12, 1
            l32i a5, a4,  168 # filter_ptr unaligned
            l32i a15, a4, 160
            tie728_s16_conv2d_128b_vector_bias  a14
            tie728_s16_unaligned_depthwise_conv2d_hwr1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9, a10, a11, a12, a4, a15
            tie728_s16_vector_round_result  q0, a13, a9, q5
            tie728_s16_variable_vector_store  a2, q0, a8, a9

        tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
                                        # a9
                                        # a10
    l32i a11, a4, 148                   # a11: filter_w_rs1_1
    l32i a12, a4, 100                   # a12: c_div_x_1
    l32i a13, a4,  64                   # a13: mac_shift
    l32i a14, a4,  68                   # a14: bias_ptr
    l32i a15, a4,  76                   # a15: activation_alpha

    EE.MOVI.32.Q q7, a13, 3


    blti a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder
        l32i  a8, a4, 132
        addi  a8, a8, -16               # a8: next_hw81 - 16

        EE.LD.128.USAR.IP q0, a2, 0
        rur.sar_byte a9                 # a9: output_sar_byte

        tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_div_x:
            beqi a9, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b
            beqi a9, 8, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b
            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_32b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_32b_multiple_loop:
                    EE.ZERO.QACC
                    EE.MOVI.32.A q7, a13, 1
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    l32i a9, a4, 84     # a9: activation_shift
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a10, q5
                    tie728_s16_conv2d_relu  q0, a15, a9
                    tie728_32b_aligned_vector_store  q0, a2, a10

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_32b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b_multiple_loop:
                    EE.ZERO.QACC
                    EE.MOVI.32.A q7, a13, 1
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    l32i a9, a4, 84     # a9: activation_shift
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a10, q5
                    tie728_s16_conv2d_relu  q0, a15, a9
                    tie728_64b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b_multiple_loop:
                    EE.ZERO.QACC
                    EE.MOVI.32.A q7, a13, 1
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    l32i a9, a4, 84     # a9: activation_shift
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a10, q5
                    tie728_s16_conv2d_relu  q0, a15, a9
                    tie728_128b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b_multiple_loop


    tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder:
        l32i a12, a4, 136               # a12: c_remainder
        beqz a12, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder_end
            EE.ZERO.QACC
            srli a8, a12, 1
            l32i a5, a4,  168 # filter_ptr unaligned
            l32i a13, a4, 160
            tie728_s16_conv2d_128b_vector_bias  a14
            tie728_s16_unaligned_depthwise_conv2d_hwr1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9, a10, a11, a12, a4, a13
            l32i a9, a4, 84             # a9: activation_shift
            EE.MOVI.32.A q7, a13, 3
            tie728_s16_vector_round_result  q0, a13, a10, q5
            tie728_s16_conv2d_relu  q0, a15, a9
            tie728_s16_variable_vector_store  a2, q0, a8, a10

        tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder_end:

    retw




    .align 4
    .text
    .global dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu
    .type   dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu, @function
    # .section .iram1
dl_tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu:
    .align 4
    entry sp, 128
    # a2: int8_t *output_ptr
    # a3: int8_t *input_ptr
    # a4: void *args

    l32i a12, a4, 60
    l32i a11, a4, 144
    EE.MOVI.32.Q q7, a12, 1
    EE.MOVI.32.Q q7, a11, 2

    l32i  a5, a4,  48                   # a5: filter_ptr
    l32i  a6, a4, 124
    addi  a6, a6, -16                   # a6: dilation_x_offset - 16
    l32i  a7, a4, 128
    addi  a7, a7, -16                   # a7: dilation_y_offset - 16
                                        # a9
                                        # a10
    l32i a11, a4, 148                   # a11: filter_w_rs1_1
    l32i a12, a4, 100                   # a12: c_div_x_1
    l32i a13, a4,  64                   # a13: mac_shift
    l32i a14, a4,  68                   # a14: bias_ptr
    l32i a15, a4,  80                   # a15: activation_alpha_ptr

    EE.MOVI.32.Q q7, a13, 3

    blti a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_c_remainder
        l32i  a8, a4, 132
        addi  a8, a8, -16               # a8: next_hw81 - 16

        EE.LD.128.USAR.IP q0, a2, 0
        rur.sar_byte a9                 # a9: output_sar_byte

        tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_c_div_x:
            beqi a9, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_128b
            beqi a9, 8, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_64b
            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_32b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_32b_multiple_loop:
                    EE.ZERO.QACC
                    EE.MOVI.32.A q7, a13, 1
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    l32i a9, a4, 84     # a9: activation_shift
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a10, q5
                    tie728_s16_conv2d_prelu  q0, q5, a15, a9
                    tie728_32b_aligned_vector_store  q0, a2, a10

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_32b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_64b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_64b_multiple_loop:
                    EE.ZERO.QACC
                    EE.MOVI.32.A q7, a13, 1
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    l32i a9, a4, 84     # a9: activation_shift
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a10, q5
                    tie728_s16_conv2d_prelu  q0, q5, a15, a9
                    tie728_64b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_64b_multiple_loop
                j tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_c_remainder

            tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_128b:
                tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_128b_multiple_loop:
                    EE.ZERO.QACC
                    EE.MOVI.32.A q7, a13, 1
                    tie728_s16_conv2d_128b_vector_bias  a14
                    tie728_s16_unaligned_depthwise_conv2d_hw81 q0, q1, q2, q3, a3, a5, a6, a7, a8, a9, a10, a11, a4, q7, a13
                    l32i a9, a4, 84     # a9: activation_shift
                    EE.MOVI.32.A q7, a13, 3
                    tie728_s16_vector_round_result  q0, a13, a10, q5
                    tie728_s16_conv2d_prelu  q0, q5, a15, a9
                    tie728_128b_aligned_vector_store  q0, a2

                    addi a12, a12, -1
                    bgei a12, 0, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_128b_multiple_loop


    tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_c_remainder:
        l32i a12, a4, 136                # a12: c_remainder
        beqz a12, tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_c_remainder_end
            EE.ZERO.QACC
            srli a8, a12, 1
            l32i a5, a4,  168 # filter_ptr unaligned
            l32i a13, a4, 160
            tie728_s16_conv2d_128b_vector_bias  a14
            tie728_s16_unaligned_depthwise_conv2d_hwr1 q0, q1, q2, q3, q4, q5, a3, a5, a6, a7, a9, a10, a11, a12, a4, a13
            l32i a9, a4, 84             # a9: activation_shift
            EE.MOVI.32.A q7, a13, 3
            tie728_s16_vector_round_result  q0, a13, a10, q5
            tie728_s16_conv2d_prelu  q0, q5, a15, a9
            tie728_s16_variable_vector_store  a2, q0, a8, a10

        tie728_s16_unaligned_depthwise_conv2d_hwc1_bias_prelu_c_remainder_end:

    retw
