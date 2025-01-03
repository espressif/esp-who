#include "dl_esp32p4_s16.S"
#include "dl_esp32p4_common.S"


############################################################################################################################################################
####
#### esp32p4_s16_unaligned_depthwise_conv2d_33c1 series
####
############################################################################################################################################################
.macro esp32p4_s16_unaligned_depthwise_conv2d_3381  input_v0, input_v1, input_v2, input_back, filter_v0, filter_v1, filter_v2, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, next_3381_16
    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_y_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    esp.src.q.ld.ip           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v2,  \filter_ptr, 16,                \input_v1, \filter_v1
    esp.src.q.ld.ip           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v2
    esp.src.q.ld.ip           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_y_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    esp.src.q.ld.ip           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v2,  \filter_ptr, 16,                \input_v1, \filter_v1
    esp.src.q.ld.ip           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v2
    esp.src.q.ld.ip           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \next_3381_16
    esp.vmulas.s16.qacc.ld.ip \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    esp.src.q.ld.ip           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v2,  \filter_ptr, 16,                \input_v1, \filter_v1
    esp.src.q.ld.ip           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v2
    esp.src.q.ld.ip           \input_v2,   \input_ptr,  16,                \input_v1, \input_back
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_3381_last  input_v0, input_v1, input_v2, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, next_3381_16
    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_y_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    esp.src.q.ld.ip           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v0,  \filter_ptr, 16,                \input_v1, \filter_v1
    esp.src.q.ld.ip           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v1,  \filter_ptr, 16,                \input_v2, \filter_v0
    esp.src.q.ld.ip           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_y_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v0,  \filter_ptr, 16,                \input_v0, \filter_v1
    esp.src.q.ld.ip           \input_v0,   \input_ptr,  16,                \input_v2, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v1,  \filter_ptr, 16,                \input_v1, \filter_v0
    esp.src.q.ld.ip           \input_v1,   \input_ptr,  16,                \input_v0, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \dilation_x_offset_16
    esp.vmulas.s16.qacc.ld.ip \filter_v0,  \filter_ptr, 16,                \input_v2, \filter_v1
    esp.src.q.ld.ip           \input_v2,   \input_ptr,  16,                \input_v1, \input_back

    esp.ld.128.usar.xp        \input_back, \input_ptr,  \next_3381_16
    esp.vmulas.s16.qacc.ld.ip \filter_v1,  \filter_ptr, 16,                \input_v0, \filter_v0
    esp.src.q                 \input_v2,   \input_v2,   \input_back

    esp.vmulas.s16.qacc.ld.ip \filter_v0,  \filter_ptr, 16,                \input_v1, \filter_v1
    esp.vmulas.s16.qacc       \input_v2,   \filter_v0
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_11r1  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_remainder, forward
    esp.ld.128.usar.xp  \input_v0,    \input_ptr, \c_remainder
    esp.vld.128.xp      \input_back,  \input_ptr, \forward
    esp.src.q           \input_v0,    \input_v0,  \input_back

    esp.ld.128.usar.xp  \filter_v0,   \filter_ptr, \c_remainder
    esp.vld.128.ip      \filter_back, \filter_ptr, 0
    esp.src.q           \filter_v0,   \filter_v0,  \filter_back

    esp.vmulas.s16.qacc \input_v0,    \filter_v0
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_33r1  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, dilation_x_offset_c_remainder, dilation_y_offset_c_remainder, c_remainder
    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_c_remainder

    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_c_remainder

    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
    // esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_c_remainder

    esp.ld.128.usar.xp  \input_v0,    \input_ptr, \c_remainder
    esp.vld.128.ip      \input_back,  \input_ptr, 0
    esp.src.q           \input_v0,    \input_v0,  \input_back

    esp.ld.128.usar.xp  \filter_v0,   \filter_ptr, \c_remainder
    esp.vld.128.ip      \filter_back, \filter_ptr, 0
    esp.src.q           \filter_v0,   \filter_v0,  \filter_back

    esp.vmulas.s16.qacc \input_v0,    \filter_v0
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_33c1_load_args  args, filter_ptr, dilation_x_offset, dilation_y_offset, next_hwx1, c_div_x_1, mac_shift
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
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_3381 - 16
    # t4: mac_shift
    # t5: c_div_x_1 / c_remainder
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): tmp value / moving c_div_x_1
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp value
    # t1(not for extension instructions):
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8: bias_ptr
    # s9:
    # s10:
    # s11:

    esp32p4_push_4_stacks_1r  s8

    esp32p4_s16_unaligned_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4
    lw  s8, 68(a2)       // bias

    addi  a4, a4, -16                           // a4: dilation_x_offset - 16
    addi  a5, a5, -16                           // a5: dilation_y_offset - 16
    addi  t3, t3, -16                           // t3: next_3381 - 16

    bltz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t6                // t6: output_sar_byte

        esp.ld.128.usar.ip  q0, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q4, a3, 16, q0, q3     // q4: filter_v0; q0: input_v0

        esp.ld.128.usar.ip  q1, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q2, a1, 16, q1, q3     // q2: input_v2; q1: input_v1

        beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_128b
            li  a6, 8
            beq  t6, a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_64b
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_32b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_32b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_32b_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_32b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_32b_last:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_64b:
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_64b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_64b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_64b_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_64b_unaligned_vector_store  q3, a0
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_64b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_64b_last:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_64b_unaligned_vector_store  q3, a0

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_128b:
            beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_128b_last
                // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_128b_loop_end
                mv  a6, t5
                esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_128b_loop:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_aligned_vector_store  q3, a0
                    addi  a6, a6, -1
                    bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_128b_loop

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_128b_last:
                esp.zero.qacc
                esp32p4_s16_conv2d_128b_vector_bias  s8
                esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                esp32p4_s16_128b_vector_shift_result  q3, t4
                esp32p4_s16_128b_aligned_vector_store  q3, a0


    esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder:
        lw  t5, 136(a2)                         // t5: c_remainder
        beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder_end
            esp.zero.qacc

            addi  a4, a4, 16
            addi  a5, a5, 16

            sub  a4, a4, t5     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t5     // a5: dilation_y_offset - c_remainder

            srli  t0, t5, 1
            esp32p4_s16_conv2d_128b_vector_bias  s8
            esp32p4_s16_unaligned_depthwise_conv2d_33r1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t5
            esp32p4_s16_128b_vector_shift_result  q0, t4
            dl_esp32p4_s16_store_remainder  q0, t0, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_c_remainder_end:

    esp32p4_pop_4_stacks_1r  s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_3381 - 16
    # t4: mac_shift
    # t5: c_div_x_1 / c_remainder
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): tmp value / moving c_div_x_1
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp value
    # t1(not for extension instructions):
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8: bias_ptr
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8

    esp32p4_s16_unaligned_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4
    lw  s8, 68(a2)       // bias
    lw  s0, 76(a2)       // activation_alpha
    lw  s1, 84(a2)       // activation_shift

    addi  a4, a4, -16                           // a4: dilation_x_offset - 16
    addi  a5, a5, -16                           // a5: dilation_y_offset - 16
    addi  t3, t3, -16                           // t3: next_3381 - 16

    bltz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t6                // t6: output_sar_byte

        esp.ld.128.usar.ip  q0, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q4, a3, 16, q0, q3     // q4: filter_v0; q0: input_v0

        esp.ld.128.usar.ip  q1, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q2, a1, 16, q1, q3     // q2: input_v2; q1: input_v1

        beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b
            li  a6, 8
            beq  t6, a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_128b_vector_relu  q3, s0, s1
                        esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_32b_last:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_vector_relu  q3, s0, s1
                    esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b:
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_128b_vector_relu  q3, s0, s1
                        esp32p4_s16_64b_unaligned_vector_store  q3, a0
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_64b_last:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_vector_relu  q3, s0, s1
                    esp32p4_s16_64b_unaligned_vector_store  q3, a0

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b:
            beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b_last
                // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b_loop_end
                mv  a6, t5
                esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b_loop:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_vector_relu  q3, s0, s1
                    esp32p4_s16_128b_aligned_vector_store  q3, a0
                    addi  a6, a6, -1
                    bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b_loop

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_128b_last:
                esp.zero.qacc
                esp32p4_s16_conv2d_128b_vector_bias  s8
                esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                esp32p4_s16_128b_vector_shift_result  q3, t4
                esp32p4_s16_128b_vector_relu  q3, s0, s1
                esp32p4_s16_128b_aligned_vector_store  q3, a0


    esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder:
        lw  t5, 136(a2)                         // t5: c_remainder
        beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder_end
            esp.zero.qacc

            addi  a4, a4, 16
            addi  a5, a5, 16

            sub  a4, a4, t5     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t5     // a5: dilation_y_offset - c_remainder

            srli  t0, t5, 1
            esp32p4_s16_conv2d_128b_vector_bias  s8
            esp32p4_s16_unaligned_depthwise_conv2d_33r1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t5
            esp32p4_s16_128b_vector_shift_result  q0, t4
            esp32p4_s16_128b_vector_relu  q0, s0, s1
            dl_esp32p4_s16_store_remainder  q0, t0, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_bias_relu_c_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_3381 - 16
    # t4: mac_shift
    # t5: c_div_x_1 / c_remainder
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): tmp value / moving c_div_x_1
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp value
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

    esp32p4_s16_unaligned_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4

    addi  a4, a4, -16                           // a4: dilation_x_offset - 16
    addi  a5, a5, -16                           // a5: dilation_y_offset - 16
    addi  t3, t3, -16                           // t3: next_3381 - 16

    bltz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t6                // t6: output_sar_byte

        esp.ld.128.usar.ip  q0, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q4, a3, 16, q0, q3     // q4: filter_v0; q0: input_v0

        esp.ld.128.usar.ip  q1, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q2, a1, 16, q1, q3     // q2: input_v2; q1: input_v1

        beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_128b
            li  a6, 8
            beq  t6, a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_64b
                # esp32p4_s16_unaligned_depthwise_conv2d_33c1_32b:
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_32b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_32b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_32b_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_32b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_32b_last:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_64b:
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_64b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_64b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_64b_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_64b_unaligned_vector_store  q3, a0
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_64b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_64b_last:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_64b_unaligned_vector_store  q3, a0

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_c_remainder

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_128b:
            beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_128b_last
                // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_128b_loop_end
                mv  a6, t5
                esp32p4_s16_unaligned_depthwise_conv2d_33c1_128b_loop:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_aligned_vector_store  q3, a0
                    addi  a6, a6, -1
                    bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_128b_loop

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_128b_last:
                esp.zero.qacc
                esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                esp32p4_s16_128b_vector_shift_result  q3, t4
                esp32p4_s16_128b_aligned_vector_store  q3, a0


    esp32p4_s16_unaligned_depthwise_conv2d_33c1_c_remainder:
        lw  t5, 136(a2)                         // t5: c_remainder
        beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_c_remainder_end
            esp.zero.qacc

            addi  a4, a4, 16
            addi  a5, a5, 16

            sub  a4, a4, t5     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t5     // a5: dilation_y_offset - c_remainder

            srli  t0, t5, 1
            esp32p4_s16_unaligned_depthwise_conv2d_33r1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t5
            esp32p4_s16_128b_vector_shift_result  q0, t4
            dl_esp32p4_s16_store_remainder  q0, t0, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_c_remainder_end:

    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_3381 - 16
    # t4: mac_shift
    # t5: c_div_x_1 / c_remainder
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): tmp value / moving c_div_x_1
    # a7(not for extension instructions):
    # t0(not for extension instructions): tmp value
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

    esp32p4_s16_unaligned_depthwise_conv2d_33c1_load_args  a2, a3, a4, a5, t3, t5, t4
    lw  s0, 76(a2)       // activation_alpha
    lw  s1, 84(a2)       // activation_shift

    addi  a4, a4, -16                           // a4: dilation_x_offset - 16
    addi  a5, a5, -16                           // a5: dilation_y_offset - 16
    addi  t3, t3, -16                           // t3: next_3381 - 16

    bltz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t6                // t6: output_sar_byte

        esp.ld.128.usar.ip  q0, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q4, a3, 16, q0, q3     // q4: filter_v0; q0: input_v0

        esp.ld.128.usar.ip  q1, a1, 16
        esp.ld.128.usar.xp  q3, a1, a4
        esp.src.q.ld.ip  q2, a1, 16, q1, q3     // q2: input_v2; q1: input_v1

        beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_128b
            li  a6, 8
            beq  t6, a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_64b
                # esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_32b:
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_32b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_32b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_32b_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_128b_vector_relu  q3, s0, s1
                        esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_32b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_32b_last:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_vector_relu  q3, s0, s1
                    esp32p4_s16_32b_unaligned_vector_store  q3, a0, t6

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_64b:
                beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_64b_last
                    // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_64b_loop_end
                    mv  a6, t5
                    esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_64b_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                        esp32p4_s16_128b_vector_shift_result  q3, t4
                        esp32p4_s16_128b_vector_relu  q3, s0, s1
                        esp32p4_s16_64b_unaligned_vector_store  q3, a0
                        addi  a6, a6, -1
                        bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_64b_loop

                esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_64b_last:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_vector_relu  q3, s0, s1
                    esp32p4_s16_64b_unaligned_vector_store  q3, a0

                j  esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_128b:
            beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_128b_last
                // esp.lp.setup  0, t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_128b_loop_end
                mv  a6, t5
                esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_128b_loop:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_3381  q0, q1, q2, q3, q4, q5, q6, a1, a3, a4, a5, t3
                    esp32p4_s16_128b_vector_shift_result  q3, t4
                    esp32p4_s16_128b_vector_relu  q3, s0, s1
                    esp32p4_s16_128b_aligned_vector_store  q3, a0
                    addi  a6, a6, -1
                    bgtz  a6, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_128b_loop

            esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_128b_last:
                esp.zero.qacc
                esp32p4_s16_unaligned_depthwise_conv2d_3381_last  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t3
                esp32p4_s16_128b_vector_shift_result  q3, t4
                esp32p4_s16_128b_vector_relu  q3, s0, s1
                esp32p4_s16_128b_aligned_vector_store  q3, a0


    esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder:
        lw  t5, 136(a2)                         // t5: c_remainder
        beqz  t5, esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder_end
            esp.zero.qacc

            addi  a4, a4, 16
            addi  a5, a5, 16

            sub  a4, a4, t5     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t5     // a5: dilation_y_offset - c_remainder

            srli  t0, t5, 1
            esp32p4_s16_unaligned_depthwise_conv2d_33r1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t5
            esp32p4_s16_128b_vector_shift_result  q0, t4
            esp32p4_s16_128b_vector_relu  q0, s0, s1
            dl_esp32p4_s16_store_remainder  q0, t0, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_33c1_relu_c_remainder_end:

    esp32p4_pop_8_stacks_2r  s0, s1
    ret






############################################################################################################################################################
####
#### esp32p4_s16_unaligned_depthwise_conv2d_hwc1 series
####
############################################################################################################################################################
.macro esp32p4_s16_unaligned_depthwise_conv2d_1w81  input_v0, input_v1, input_back, input_ptr, filter_v0, filter_ptr, dilation_x_offset_16, dilation_y_offset, filter_w, filter_w_rs1_1, filter_y_offset, temp
    blez  \filter_w_rs1_1, 1f
    // esp.lp.setup  0, \filter_w_rs1_1, 0f
    mv  \temp, \filter_w_rs1_1
    0:
        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        esp.src.q.ld.ip  \input_v0, \input_ptr, 16, \input_v1, \input_back

        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        esp.src.q.ld.ip  \input_v1, \input_ptr, 16, \input_v0, \input_back
        addi  \temp, \temp, -1
        bgtz  \temp, 0b

    1:
    andi  \temp, \filter_w, 0xfffffffe
    beq  \filter_w, \temp, 2f
        # three 8-input-element left
        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        esp.src.q.ld.ip  \input_v0, \input_ptr, 16, \input_v1, \input_back

        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_y_offset
        esp.vmulas.s16.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v0
        esp.src.q.ld.ip  \input_v1, \input_ptr, 16, \input_v0, \input_back

        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        esp.src.q  \input_v0, \input_v1, \input_back
        esp.ld.128.usar.ip  \input_v1, \input_ptr, 16
        j  3f

    2:  # two 8-input-element left
        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_y_offset
        esp.vmulas.s16.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        esp.src.q.ld.ip  \input_v0, \input_ptr, 16, \input_v1, \input_back

        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        esp.src.q.ld.ip  \input_v1, \input_ptr, 16, \input_v0, \input_back
    3:
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_1w81_last  input_v0, input_v1, input_back, input_ptr, filter_v0, filter_ptr, dilation_x_offset_16, filter_w, filter_w_rs1_1, next_hws1, filter_y_offset, temp
    blez  \filter_w_rs1_1, 5f
    // esp.lp.setup  0, \filter_w_rs1_1, 4f
    mv  \temp, \filter_w_rs1_1
    4:
        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        esp.src.q.ld.ip  \input_v0, \input_ptr, 16, \input_v1, \input_back

        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v1, \filter_v0
        esp.src.q.ld.ip  \input_v1, \input_ptr, 16, \input_v0, \input_back
        addi  \temp, \temp, -1
        bgtz  \temp, 4b

    5:
    andi  \temp, \filter_w, 0xfffffffe
    beq  \filter_w, \temp, 6f
        # three 8-input-element left
        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vmulas.s16.qacc.ld.ip  \filter_v0, \filter_ptr, 16, \input_v0, \filter_v0
        esp.src.q.ld.ip  \input_v0, \input_ptr, 16, \input_v1, \input_back

        esp.ld.128.usar.xp  \input_back, \input_ptr, \next_hws1
        esp.vmulas.s16.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v1, \filter_v0
        esp.src.q  \input_v0, \input_v0, \input_back

        esp.vmulas.s16.qacc  \input_v0, \filter_v0
        j  7f

    6:  # two 8-input-element left
        esp.ld.128.usar.xp  \input_back, \input_ptr, \next_hws1
        esp.vmulas.s16.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
        esp.src.q  \input_v1, \input_v1, \input_back

        esp.vmulas.s16.qacc  \input_v1, \filter_v0
    7:
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_hw81  input_v0, input_v1, input_back, filter_v0, input_ptr, filter_ptr, dilation_x_offset_16, dilation_y_offset_16, next_hws1, filter_h, filter_w, filter_w_rs1_1, args, filter_y_offset, filter_n_offset, temp
    lw  \filter_h, 52(\args)  # filter_height
    lw  \filter_w, 56(\args)  # filter_width

    addi  \temp, \filter_w, -1
    beqz  \temp, 10f
        # filter_w >= 2
        esp.ld.128.usar.ip  \input_v1, \input_ptr, 16
        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_x_offset_16
        esp.vld.128.ip  \filter_v0, \filter_ptr, 16  # filter_v0
        esp.src.q  \input_v0, \input_v1, \input_back # input_v0
        esp.ld.128.usar.ip  \input_v1, \input_ptr, 16

        addi  \filter_h, \filter_h, -1
        beqz  \filter_h, 9f
            8:
                esp32p4_s16_unaligned_depthwise_conv2d_1w81  \input_v0, \input_v1, \input_back, \input_ptr, \filter_v0, \filter_ptr, \dilation_x_offset_16, \dilation_y_offset_16, \filter_w, \filter_w_rs1_1, \filter_y_offset, \temp
                addi  \filter_h, \filter_h, -1
                bgtz  \filter_h, 8b
        9:  # last y
        esp32p4_s16_unaligned_depthwise_conv2d_1w81_last  \input_v0, \input_v1, \input_back, \input_ptr, \filter_v0, \filter_ptr, \dilation_x_offset_16, \filter_w, \filter_w_rs1_1, \next_hws1, \filter_y_offset, \temp
        j  13f

    10:  # filter_w == 1
        esp.ld.128.usar.ip  \input_v1, \input_ptr, 16
        esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_y_offset_16
        esp.vld.128.xp  \filter_v0, \filter_ptr, \filter_y_offset # filter_v0
        esp.src.q  \input_v0, \input_v1, \input_back # input_v0

        addi  \filter_h, \filter_h, -1
        beqz  \filter_h, 12f
            // esp.lp.setup  0, \filter_h, 11f
            11:
                esp.ld.128.usar.ip  \input_v1, \input_ptr, 16
                esp.ld.128.usar.xp  \input_back, \input_ptr, \dilation_y_offset_16
                esp.vmulas.s16.qacc.ld.xp  \filter_v0, \filter_ptr, \filter_y_offset, \input_v0, \filter_v0
                esp.src.q  \input_v0, \input_v1, \input_back
                addi  \filter_h, \filter_h, -1
                bgtz  \filter_h, 11b

        12: # last y
        esp.vmulas.s16.qacc  \input_v0, \filter_v0
        sub  \input_ptr, \input_ptr, \dilation_y_offset_16
        add  \input_ptr, \input_ptr, \next_hws1

    13:

    add  \filter_ptr, \filter_ptr, \filter_n_offset
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_11r1_padding  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_remainder, forward, filter_y_offset
    esp.ld.128.usar.xp  \input_v0,   \input_ptr, \c_remainder
    esp.vld.128.xp      \input_back, \input_ptr, \forward
    esp.src.q           \input_v0,   \input_v0,  \input_back

    esp.ld.128.usar.xp  \filter_v0,   \filter_ptr, \c_remainder
    esp.vld.128.xp      \filter_back, \filter_ptr, \filter_y_offset
    esp.src.q           \filter_v0,   \filter_v0,  \filter_back

    esp.vmulas.s16.qacc  \input_v0,    \filter_v0
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_hwr1  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, dilation_x_offset_c_remainder, dilation_y_offset_c_remainder, filter_h, filter_w, filter_w_rs1_1, c_remainder, args, filter_y_offset, temp
    lw  \filter_h, 52(\args)   # filter_height
    lw  \filter_w, 56(\args)   # filter_width

    addi  \temp, \filter_w, -1
    beqz  \temp, 5f
        4:
            beqz  \filter_w_rs1_1, 1f
                // esp.lp.setup  0, \filter_w_rs1_1, 1f
                mv  \temp, \filter_w_rs1_1
                0:
                    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
                    esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
                    addi  \temp, \temp, -1
                    bgtz  \temp, 0b
            1:
            andi  \temp, \filter_w, 0xfffffffe
            beq  \temp, \filter_w, 2f
                # 3 left
                esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
                esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
                esp32p4_s16_unaligned_depthwise_conv2d_11r1_padding  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_c_remainder, \filter_y_offset
                j  3f
            2:
                # 2 left
                esp32p4_s16_unaligned_depthwise_conv2d_11r1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_x_offset_c_remainder
                esp32p4_s16_unaligned_depthwise_conv2d_11r1_padding  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_c_remainder, \filter_y_offset
            3:
            addi  \filter_h, \filter_h, -1
            // blt  x0, \filter_h, 4b
            bgtz  \filter_h, 4b

        j 7f

    5:
        # filter_w == 1
        beqz  \filter_h, 7f
        // esp.lp.setup  0, \filter_h, 6f
        mv  \temp, \filter_h
        6:
            esp32p4_s16_unaligned_depthwise_conv2d_11r1_padding  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_remainder, \dilation_y_offset_c_remainder, \filter_y_offset
            addi  \temp, \temp, -1
            bgtz  \temp, 6b
    7:
.endm



.macro esp32p4_s16_unaligned_depthwise_conv2d_hwc1_load_args  args, filter_ptr, dilation_x_offset, dilation_y_offset, next_hwx1, c_div_x_1, mac_shift, filter_w, filter_w_rs1_1
    esp32p4_s16_unaligned_depthwise_conv2d_33c1_load_args  \args, \filter_ptr, \dilation_x_offset, \dilation_y_offset, \next_hwx1, \c_div_x_1, \mac_shift
    lw  \filter_w,         56(\args)
    lw  \filter_w_rs1_1,   148(\args)
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr / unaligned_filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_hw81 - 16 / c_remainder
    # t4: filter_y_offset / unaligned_filter_y_offset
    # t5: mac_shift
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): filter_n_offset / c_remainder number
    # a7(not for extension instructions): c_div_x_1
    # t0(not for extension instructions): filter_h
    # t1(not for extension instructions): filter_w
    # t2(not for extension instructions): filter_w_rs1_1
    # s2(not for extension instructions): tmp value
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0:
    # s1:
    # s8: bias_ptr
    # s9:
    # s10:
    # s11:

    esp32p4_push_8_stacks_2r  s2, s8

    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, a7, t5, t1, t2
    lw  s8, 68(a2)                  // bias

    addi  a4, a4, -16               // a4: dilation_x_offset - 16
    addi  a5, a5, -16               // a5: dilation_y_offset - 16
    lw  a6, 144(a2)                 // a6: filter_n_offset

    bltz  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        addi  t3, t3, -16           // t3: next_hw81 - 16
        lw  t4, 60(a2)              // t4: filter_y_offset
        esp.movx.r.sar.bytes  t6    // t6: output_sar_byte

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_c_div_x:
            beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_128b
                li  s2, 8
                beq  t6, s2, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_64b
                    # esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_32b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_32b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_32b_unaligned_vector_store  q0, a0, t6

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_32b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder

                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_64b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_64b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_64b_unaligned_vector_store  q0, a0

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_64b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_128b:
                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_128b_multiple_loop:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                    esp32p4_s16_128b_vector_shift_result  q0, t5
                    esp32p4_s16_128b_aligned_vector_store  q0, a0

                    addi  a7, a7, -1
                    bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_128b_multiple_loop


    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder:
        lw  t3, 136(a2)         // t3: c_remainder
        beqz  t3, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder_end
            esp.zero.qacc

            srli  a6, t3, 1
            lw  a3, 168(a2)     // a3: unaligned_filter_ptr
            lw  t4, 160(a2)     // t4: unaligned_filter_y_offset
            addi  a4, a4, 16
            addi  a5, a5, 16
            sub  a4, a4, t3     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t3     // a5: dilation_y_offset - c_remainder

            esp32p4_s16_conv2d_128b_vector_bias  s8
            esp32p4_s16_unaligned_depthwise_conv2d_hwr1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t0, t1, t2, t3, a2, t4, t6
            esp32p4_s16_128b_vector_shift_result  q0, t5
            dl_esp32p4_s16_store_remainder  q0, a6, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_c_remainder_end:

    esp32p4_pop_8_stacks_2r  s2, s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr / unaligned_filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_hw81 - 16 / c_remainder
    # t4: filter_y_offset / unaligned_filter_y_offset
    # t5: mac_shift
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): filter_n_offset / c_remainder number
    # a7(not for extension instructions): c_div_x_1
    # t0(not for extension instructions): filter_h
    # t1(not for extension instructions): filter_w
    # t2(not for extension instructions): filter_w_rs1_1
    # s2(not for extension instructions): tmp value
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8: bias_ptr
    # s9:
    # s10:
    # s11:

    esp32p4_push_16_stacks_4r  s0, s1, s2, s8

    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, a7, t5, t1, t2
    lw  s8, 68(a2)                  // bias
    lw  s0, 76(a2)                  // activation_alpha
    lw  s1, 84(a2)                  // activation_shift

    addi  a4, a4, -16               // a4: dilation_x_offset - 16
    addi  a5, a5, -16               // a5: dilation_y_offset - 16
    lw  a6, 144(a2)                 // a6: filter_n_offset

    bltz  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        addi  t3, t3, -16           // t3: next_hw81 - 16
        lw  t4, 60(a2)              // t4: filter_y_offset
        esp.movx.r.sar.bytes  t6    // t6: output_sar_byte

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_div_x:
            beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b
                li  s2, 8
                beq  t6, s2, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b
                    # esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_32b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_32b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_128b_vector_relu  q0, s0, s1
                        esp32p4_s16_32b_unaligned_vector_store  q0, a0, t6

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_32b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder

                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_conv2d_128b_vector_bias  s8
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_128b_vector_relu  q0, s0, s1
                        esp32p4_s16_64b_unaligned_vector_store  q0, a0

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_64b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b:
                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b_multiple_loop:
                    esp.zero.qacc
                    esp32p4_s16_conv2d_128b_vector_bias  s8
                    esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                    esp32p4_s16_128b_vector_shift_result  q0, t5
                    esp32p4_s16_128b_vector_relu  q0, s0, s1
                    esp32p4_s16_128b_aligned_vector_store  q0, a0

                    addi  a7, a7, -1
                    bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_128b_multiple_loop


    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder:
        lw  t3, 136(a2)         // t3: c_remainder
        beqz  t3, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder_end
            esp.zero.qacc

            srli  a6, t3, 1
            lw  a3, 168(a2)     // a3: unaligned_filter_ptr
            lw  t4, 160(a2)     // t4: unaligned_filter_y_offset
            addi  a4, a4, 16
            addi  a5, a5, 16
            sub  a4, a4, t3     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t3     // a5: dilation_y_offset - c_remainder

            esp32p4_s16_conv2d_128b_vector_bias  s8
            esp32p4_s16_unaligned_depthwise_conv2d_hwr1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t0, t1, t2, t3, a2, t4, t6
            esp32p4_s16_128b_vector_shift_result  q0, t5
            esp32p4_s16_128b_vector_relu  q0, s0, s1
            dl_esp32p4_s16_store_remainder  q0, a6, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_bias_relu_c_remainder_end:

    esp32p4_pop_16_stacks_4r  s0, s1, s2, s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr / unaligned_filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_hw81 - 16 / c_remainder
    # t4: filter_y_offset / unaligned_filter_y_offset
    # t5: mac_shift
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): filter_n_offset / c_remainder number
    # a7(not for extension instructions): c_div_x_1
    # t0(not for extension instructions): filter_h
    # t1(not for extension instructions): filter_w
    # t2(not for extension instructions): filter_w_rs1_1
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

    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, a7, t5, t1, t2

    addi  a4, a4, -16               // a4: dilation_x_offset - 16
    addi  a5, a5, -16               // a5: dilation_y_offset - 16
    lw  a6, 144(a2)                 // a6: filter_n_offset

    bltz  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        addi  t3, t3, -16           // t3: next_hw81 - 16
        lw  t4, 60(a2)              // t4: filter_y_offset
        esp.movx.r.sar.bytes  t6    // t6: output_sar_byte

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_c_div_x:
            beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_128b
                li  s2, 8
                beq  t6, s2, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_64b
                    # esp32p4_s16_unaligned_depthwise_conv2d_hwc1_32b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_32b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_32b_unaligned_vector_store  q0, a0, t6

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_32b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_c_remainder

                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_64b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_64b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_64b_unaligned_vector_store  q0, a0

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_64b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_hwc1_128b:
                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_128b_multiple_loop:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                    esp32p4_s16_128b_vector_shift_result  q0, t5
                    esp32p4_s16_128b_aligned_vector_store  q0, a0

                    addi  a7, a7, -1
                    bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_128b_multiple_loop


    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_c_remainder:
        lw  t3, 136(a2)         // t3: c_remainder
        beqz  t3, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_c_remainder_end
            esp.zero.qacc

            srli  a6, t3, 1
            lw  a3, 168(a2)     // a3: unaligned_filter_ptr
            lw  t4, 160(a2)     // t4: unaligned_filter_y_offset
            addi  a4, a4, 16
            addi  a5, a5, 16
            sub  a4, a4, t3     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t3     // a5: dilation_y_offset - c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_hwr1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t0, t1, t2, t3, a2, t4, t6
            esp32p4_s16_128b_vector_shift_result  q0, t5
            dl_esp32p4_s16_store_remainder  q0, a6, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_c_remainder_end:

    esp32p4_pop_4_stacks_1r  s2
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu
    .type   dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr / unaligned_filter_ptr
    # a4: dilation_x_offset - 16 / dilation_x_offset - c_remainder
    # a5: dilation_y_offset - 16 / dilation_y_offset - c_remainder
    # t3: next_hw81 - 16 / c_remainder
    # t4: filter_y_offset / unaligned_filter_y_offset
    # t5: mac_shift
    # t6: output_sar_byte / tmp value

    # a6(not for extension instructions): filter_n_offset / c_remainder number
    # a7(not for extension instructions): c_div_x_1
    # t0(not for extension instructions): filter_h
    # t1(not for extension instructions): filter_w
    # t2(not for extension instructions): filter_w_rs1_1
    # s2(not for extension instructions): tmp value
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: activation_alpha/_address
    # s1: activation_shift
    # s8:
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s2

    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_load_args  a2, a3, a4, a5, t3, a7, t5, t1, t2
    lw  s0, 76(a2)                  // activation_alpha
    lw  s1, 84(a2)                  // activation_shift

    addi  a4, a4, -16               // a4: dilation_x_offset - 16
    addi  a5, a5, -16               // a5: dilation_y_offset - 16
    lw  a6, 144(a2)                 // a6: filter_n_offset

    bltz  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        addi  t3, t3, -16           // t3: next_hw81 - 16
        lw  t4, 60(a2)              // t4: filter_y_offset
        esp.movx.r.sar.bytes  t6    // t6: output_sar_byte

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_c_div_x:
            beqz  t6, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_128b
                li  s2, 8
                beq  t6, s2, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_64b
                    # esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_32b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_32b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_128b_vector_relu  q0, s0, s1
                        esp32p4_s16_32b_unaligned_vector_store  q0, a0, t6

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_32b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder

                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_64b:
                    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_64b_multiple_loop:
                        esp.zero.qacc
                        esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                        esp32p4_s16_128b_vector_shift_result  q0, t5
                        esp32p4_s16_128b_vector_relu  q0, s0, s1
                        esp32p4_s16_64b_unaligned_vector_store  q0, a0

                        addi  a7, a7, -1
                        bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_64b_multiple_loop
                    j  esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_128b:
                esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_128b_multiple_loop:
                    esp.zero.qacc
                    esp32p4_s16_unaligned_depthwise_conv2d_hw81  q0, q1, q2, q3, a1, a3, a4, a5, t3, t0, t1, t2, a2, t4, a6, t6
                    esp32p4_s16_128b_vector_shift_result  q0, t5
                    esp32p4_s16_128b_vector_relu  q0, s0, s1
                    esp32p4_s16_128b_aligned_vector_store  q0, a0

                    addi  a7, a7, -1
                    bgez  a7, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_128b_multiple_loop


    esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder:
        lw  t3, 136(a2)         // t3: c_remainder
        beqz  t3, esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder_end
            esp.zero.qacc

            srli  a6, t3, 1
            lw  a3, 168(a2)     // a3: unaligned_filter_ptr
            lw  t4, 160(a2)     // t4: unaligned_filter_y_offset
            addi  a4, a4, 16
            addi  a5, a5, 16
            sub  a4, a4, t3     // a4: dilation_x_offset - c_remainder
            sub  a5, a5, t3     // a5: dilation_y_offset - c_remainder

            esp32p4_s16_unaligned_depthwise_conv2d_hwr1  q0, q1, q2, q3, q4, q5, a1, a3, a4, a5, t0, t1, t2, t3, a2, t4, t6
            esp32p4_s16_128b_vector_shift_result  q0, t5
            esp32p4_s16_128b_vector_relu  q0, s0, s1
            dl_esp32p4_s16_store_remainder  q0, a6, t6, a0

        esp32p4_s16_unaligned_depthwise_conv2d_hwc1_relu_c_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s2
    ret
