#include "dl_esp32p4_s16.S"
#include "dl_esp32p4_common.S"


############################################################################################################################################################
####
#### esp32p4_s16_unaligned_conv2d_11cn series
####
############################################################################################################################################################
.macro esp32p4_s16_unaligned_conv2d_11c8  input_v, input_front, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, c_remainder, temp
    # input_v:     8 input elements
    # filter_v0:    8 filter elements
    # filter_v1:    8 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 8 - 1

    esp.ld.128.usar.ip  \input_front, \input_ptr, 16

    bltz  \c_div_x_1, 7f
        esp.ld.128.usar.ip  \input_back, \input_ptr, 16
        esp.vld.128.ip  \filter_v0, \filter_ptr, 16
        esp.vld.128.ip  \filter_v1, \filter_ptr, 16

        beqz  \c_div_x_1, 8f
        mv  \temp, \c_div_x_1
        // esp.lp.setup  0, \c_div_x_1, 9f
        9:
            esp.src.q.qup  \input_v, \input_front, \input_back
            esp.ld.128.usar.ip  \input_back, \input_ptr, 16

            esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
            esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
            esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
            esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
            esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 4
            esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 5
            esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 6
            esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 7
            addi  \temp, \temp, -1
            bgtz  \temp, 9b

        8:
        # last entire-128b
        esp.src.q.qup  \input_v, \input_front, \input_back
        esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
        esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
        esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
        esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
        esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 4
        esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 5
        esp.vsmulas.s16.qacc  \filter_v0, \input_v, 6
        esp.vsmulas.s16.qacc  \filter_v1, \input_v, 7
        beqz  \c_remainder, 0f # jump to c_remainder == 0

    # c_remainder
    7:
        esp.ld.128.usar.xp  \input_back, \input_ptr, \c_remainder
        esp.src.q.qup  \input_v, \input_front, \input_back
        esp.vld.128.ip  \filter_v0, \filter_ptr,  16

        li  \temp, 8
        blt  \c_remainder, \temp, 3f
            # remainder == 0x1__0
            esp.vld.128.ip  \filter_v1, \filter_ptr,  16

            li  \temp, 12
            blt  \c_remainder, \temp, 5f
                # remainder == 0x11_0
                li  \temp, 14
                blt  \c_remainder, \temp, 6f
                    # remainder == 0x1110, 7
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 4
                    esp.vsmulas.s16.qacc  \filter_v1, \input_v, 5
                    esp.vsmulas.s16.qacc  \filter_v0, \input_v, 6
                    j 0f

                6:  # remainder == 0x1100, 6
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 3
                    esp.vsmulas.s16.qacc  \filter_v0, \input_v, 4
                    esp.vsmulas.s16.qacc  \filter_v1, \input_v, 5
                    j 0f

            5:
                li  \temp, 10
                blt  \c_remainder, \temp, 4f
                    # remainder == 0x1010, 5
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 2
                    esp.vsmulas.s16.qacc  \filter_v1, \input_v, 3
                    esp.vsmulas.s16.qacc  \filter_v0, \input_v, 4
                    j 0f

                4:  # remainder == 0x1000, 4
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v1, \filter_ptr, \filter_v1, \input_v, 1
                    esp.vsmulas.s16.qacc  \filter_v0, \input_v, 2
                    esp.vsmulas.s16.qacc  \filter_v1, \input_v, 3
                    j 0f

        3:  # remainder == 0x0__0
            li  \temp, 4
            blt  \c_remainder, \temp, 1f
                # remainder == 0x01_0
                esp.vld.128.ip  \filter_v1, \filter_ptr,  16

                li  \temp, 6
                blt  \c_remainder, \temp, 2f
                    # remainder == 0x0110, 3
                    esp.vsmulas.s16.qacc.ld.incp  \filter_v0, \filter_ptr, \filter_v0, \input_v, 0
                    esp.vsmulas.s16.qacc  \filter_v1, \input_v, 1
                    esp.vsmulas.s16.qacc  \filter_v0, \input_v, 2
                    j 0f

                2:  # remainder == 0x0100, 2
                    esp.vsmulas.s16.qacc  \filter_v0, \input_v, 0
                    esp.vsmulas.s16.qacc  \filter_v1, \input_v, 1
                    j 0f

            1:  # remainder == 0x0010, 1
                esp.vsmulas.s16.qacc  \filter_v0, \input_v, 0

    0:
        addi  \input_ptr, \input_ptr, -16

.endm



.macro esp32p4_s16_unaligned_conv2d_11c1  input_v, input_front, input_back, filter_v, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, temp, zero
    # input_v:     8 input elements
    # filter_v:    8 filter elements
    # filter_v1:    8 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 8 - 1

    esp.ld.128.usar.ip  \input_front, \input_ptr, 16
    esp.ld.128.usar.ip  \filter_front, \filter_ptr, 16

    bltz  \c_div_x_1, 7f     // input_channel < 8
        esp.ld.128.usar.ip  \input_back, \input_ptr, 16

        beqz  \c_div_x_1, 8f
        // esp.lp.setup  0, \c_div_x_1, (8f - 4)
        # Use the zero register as a loop counter, and the value remains zero after the loop is complete.
        mv  \zero, \c_div_x_1
        9:
            esp.src.q.qup  \input_v, \input_front, \input_back

            esp.ld.128.usar.ip  \filter_back, \filter_ptr, 16
            esp.src.q.qup  \filter_v, \filter_front, \filter_back

            esp.ld.128.usar.ip  \input_back, \input_ptr, 16
            esp.vmulas.s16.xacc  \filter_v, \input_v
            addi  \zero, \zero, -1
            bgtz  \zero, 9b
        8:
        # last entire-128b
        esp.src.q.qup  \input_v, \input_front, \input_back

        esp.ld.128.usar.ip  \filter_back, \filter_ptr, 16
        esp.src.q.qup  \filter_v, \filter_front, \filter_back

        esp.vmulas.s16.xacc  \filter_v, \input_v

        beqz  \c_remainder, 0f

    7:
        # c_remainder > 0
        esp.ld.128.usar.xp  \input_back, \input_ptr, \c_remainder
        esp.src.q.qup  \input_v, \input_front, \input_back

        esp.ld.128.usar.xp  \filter_back, \filter_ptr, \c_remainder
        esp.src.q.qup  \filter_v, \filter_front, \filter_back

        esp.slcxxp.2q  \input_back,  \input_v, \temp, \zero
        esp.slcxxp.2q  \filter_back, \filter_v, \temp, \zero

        esp.vmulas.s16.xacc  \filter_v, \input_v

    0:
        addi  \input_ptr,  \input_ptr, -16
        addi  \filter_ptr, \filter_ptr, -16
.endm



.macro esp32p4_s16_unaligned_conv2d_11cn_load_args  args, filter_ptr, c_div_x_1, n_div_x, mac_shift, c_remainder
    lw  \filter_ptr,    48(\args)       // filter
    lw  \c_div_x_1,     100(\args)      // input_channel / x - 1
    lw  \n_div_x,       96(\args)       // output_channel / x
    lw  \mac_shift,     64(\args)       // mac_shift
    lw  \c_remainder,   136(\args)      // input_channel % (vector_width / element_width) * sizeof(feature_t)
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_11cn_bias
    .type   dl_esp32p4_s16_unaligned_conv2d_11cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_11cn_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value
    esp32p4_s16_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3     //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_11cn_bias_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s16_unaligned_conv2d_11cn_bias_64b
                esp32p4_s16_unaligned_conv2d_11cn_bias_32b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder

            esp32p4_s16_unaligned_conv2d_11cn_bias_64b:
                esp32p4_s16_unaligned_conv2d_11cn_bias_64b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder

        esp32p4_s16_unaligned_conv2d_11cn_bias_128b:
            esp32p4_s16_unaligned_conv2d_11cn_bias_128b_multiple_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_conv2d_128b_vector_bias  t6
                esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder:
        lw  t1, 140(a2)              // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_conv2d_element_bias  t6
                esp32p4_s16_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_11cn_bias_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_11cn_bias_relu
    .type   dl_esp32p4_s16_unaligned_conv2d_11cn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_11cn_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s16_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3     //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_64b
                esp32p4_s16_unaligned_conv2d_11cn_bias_relu_32b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder

            esp32p4_s16_unaligned_conv2d_11cn_bias_relu_64b:
                esp32p4_s16_unaligned_conv2d_11cn_bias_relu_64b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder

        esp32p4_s16_unaligned_conv2d_11cn_bias_relu_128b:
            esp32p4_s16_unaligned_conv2d_11cn_bias_relu_128b_multiple_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_conv2d_128b_vector_bias  t6
                esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_vector_relu  q0, s9, s10
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder:
        lw  t1, 140(a2)              // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_conv2d_element_bias  t6
                esp32p4_s16_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_leakyrelu  s0, s9, s10
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_11cn_bias_relu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_11cn
    .type   dl_esp32p4_s16_unaligned_conv2d_11cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_11cn:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6:

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value
    esp32p4_s16_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4

    beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3     //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_11cn_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s16_unaligned_conv2d_11cn_64b
                esp32p4_s16_unaligned_conv2d_11cn_32b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_n_remainder

            esp32p4_s16_unaligned_conv2d_11cn_64b:
                esp32p4_s16_unaligned_conv2d_11cn_64b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_n_remainder

        esp32p4_s16_unaligned_conv2d_11cn_128b:
            esp32p4_s16_unaligned_conv2d_11cn_128b_multiple_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_11cn_n_remainder:
        lw  t1, 140(a2)              // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_11cn_n_remainder_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_11cn_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_11cn_relu
    .type   dl_esp32p4_s16_unaligned_conv2d_11cn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_11cn_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6:

    # a6(not for extension instructions):
    # a7(not for extension instructions):
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions):
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s16_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4

    beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3     //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_11cn_relu_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s16_unaligned_conv2d_11cn_relu_64b
                esp32p4_s16_unaligned_conv2d_11cn_relu_32b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_relu_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder

            esp32p4_s16_unaligned_conv2d_11cn_relu_64b:
                esp32p4_s16_unaligned_conv2d_11cn_relu_64b_multiple_loop:
                    mv  t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_relu_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder

        esp32p4_s16_unaligned_conv2d_11cn_relu_128b:
            esp32p4_s16_unaligned_conv2d_11cn_relu_128b_multiple_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_unaligned_conv2d_11c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_vector_relu  q0, s9, s10
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_relu_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder:
        lw  t1, 140(a2)              // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_leakyrelu  s0, s9, s10
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_11cn_relu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10     // restore registers
    ret






############################################################################################################################################################
####
#### esp32p4_s16_unaligned_conv2d_33cn series
####
############################################################################################################################################################
.macro esp32p4_s16_unaligned_conv2d_33c8  input_v0, input_front, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, temp
    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    # add  \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro esp32p4_s16_unaligned_conv2d_33c1  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, temp, zero
    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s16_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro esp32p4_s16_unaligned_conv2d_hwcn_load_args  args, filter_ptr, c_div_x_1, n_div_x, mac_shift, c_remainder, dilation_x_offset, dilation_y_offset
    esp32p4_s16_unaligned_conv2d_11cn_load_args  \args, \filter_ptr, \c_div_x_1, \n_div_x, \mac_shift, \c_remainder
    lw  \dilation_x_offset, 108(\args)  // input dilation x offset
    lw  \dilation_y_offset, 112(\args)  // input dilation y offset
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_33cn_bias
    .type   dl_esp32p4_s16_unaligned_conv2d_33cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_33cn_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): tmp value
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_33cn_bias_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s16_unaligned_conv2d_33cn_bias_64b
            esp32p4_s16_unaligned_conv2d_33cn_bias_32b:
                esp32p4_s16_unaligned_conv2d_33cn_bias_32b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder

            esp32p4_s16_unaligned_conv2d_33cn_bias_64b:
                esp32p4_s16_unaligned_conv2d_33cn_bias_64b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder

        esp32p4_s16_unaligned_conv2d_33cn_bias_128b:
            esp32p4_s16_unaligned_conv2d_33cn_bias_128b_multiple_loop:
                mv  t3, a1                      // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_conv2d_128b_vector_bias  t6
                esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder_loop:
                mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_conv2d_element_bias  t6
                esp32p4_s16_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_33cn_bias_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_33cn_bias_relu
    .type   dl_esp32p4_s16_unaligned_conv2d_33cn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_33cn_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): tmp value
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_64b
            esp32p4_s16_unaligned_conv2d_33cn_bias_relu_32b:
                esp32p4_s16_unaligned_conv2d_33cn_bias_relu_32b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder

            esp32p4_s16_unaligned_conv2d_33cn_bias_relu_64b:
                esp32p4_s16_unaligned_conv2d_33cn_bias_relu_64b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder

        esp32p4_s16_unaligned_conv2d_33cn_bias_relu_128b:
            esp32p4_s16_unaligned_conv2d_33cn_bias_relu_128b_multiple_loop:
                mv  t3, a1                      // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_conv2d_128b_vector_bias  t6
                esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_vector_relu  q0, s9, s10
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_loop:
                mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_conv2d_element_bias  t6
                esp32p4_s16_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_leakyrelu  s0, s9, s10
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_33cn_bias_relu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_33cn
    .type   dl_esp32p4_s16_unaligned_conv2d_33cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_33cn:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): tmp value
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_33cn_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s16_unaligned_conv2d_33cn_64b
            esp32p4_s16_unaligned_conv2d_33cn_32b:
                esp32p4_s16_unaligned_conv2d_33cn_32b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_n_remainder

            esp32p4_s16_unaligned_conv2d_33cn_64b:
                esp32p4_s16_unaligned_conv2d_33cn_64b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_n_remainder

        esp32p4_s16_unaligned_conv2d_33cn_128b:
            esp32p4_s16_unaligned_conv2d_33cn_128b_multiple_loop:
                mv  t3, a1                      // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_33cn_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_33cn_n_remainder_loop:
                mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_33cn_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_33cn_relu
    .type   dl_esp32p4_s16_unaligned_conv2d_33cn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_33cn_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value
    # t4: 15 - c_remainder
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): tmp value
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions):
    # s3(not for extension instructions):
    # s4(not for extension instructions):
    # s5(not for extension instructions):

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10    // push stacks
    li  s1, -32768      // clamp min value
    li  s8, 32767       // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               //  t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_33cn_relu_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s16_unaligned_conv2d_33cn_relu_64b
            esp32p4_s16_unaligned_conv2d_33cn_relu_32b:
                esp32p4_s16_unaligned_conv2d_33cn_relu_32b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_relu_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder

            esp32p4_s16_unaligned_conv2d_33cn_relu_64b:
                esp32p4_s16_unaligned_conv2d_33cn_relu_64b_multiple_loop:
                    mv  t3, a1                  // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_relu_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder

        esp32p4_s16_unaligned_conv2d_33cn_relu_128b:
            esp32p4_s16_unaligned_conv2d_33cn_relu_128b_multiple_loop:
                mv  t3, a1                      // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_unaligned_conv2d_33c8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_vector_relu  q0, s9, s10
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_relu_128b_multiple_loop


    esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder_loop:
                mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s16_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s16_element_leakyrelu  s0, s9, s10
                esp32p4_s16_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_33cn_relu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10     // restore registers
    ret






############################################################################################################################################################
####
#### esp32p4_s16_unaligned_conv2d_hwcn series
####
############################################################################################################################################################
.macro esp32p4_s16_unaligned_conv2d_hwc8  input_v0, input_front, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, filter_h, filter_w, args, filter_y_offset, filter_n_offset, temp
    lw  \filter_h, 52(\args)  # filter_height
    11:
        lw  \filter_w, 56(\args)  # filter_width
        addi  \filter_w, \filter_w, -1
        beqz  \filter_w, 12f
            10:
                esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp
                add  \input_ptr, \input_ptr, \dilation_x_offset

                addi  \filter_w, \filter_w, -1
                bnez  \filter_w, 10b
        12:
        esp32p4_s16_unaligned_conv2d_11c8  \input_v0, \input_front, \input_back, \filter_v0, \filter_v1, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp

        add  \filter_ptr, \filter_ptr, \filter_y_offset
        add  \input_ptr, \input_ptr, \dilation_y_offset

        addi  \filter_h, \filter_h, -1
        bnez  \filter_h, 11b

    add  \filter_ptr, \filter_ptr, \filter_n_offset
.endm



.macro esp32p4_s16_unaligned_conv2d_hwc1  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, filter_h, filter_w, args, temp, zero, filter_y_offset, filter_n_offset
    lw  \filter_h, 52(\args)  # filter_height
    11:
        lw  \filter_w, 56(\args)  # filter_width
        addi  \filter_w, \filter_w, -1
        beqz  \filter_w, 12f
            10:
                esp32p4_s16_unaligned_conv2d_11c1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero
                add  \input_ptr, \input_ptr, \dilation_x_offset

                addi  \filter_w, \filter_w, -1
                bnez  \filter_w, 10b
        12:
        esp32p4_s16_unaligned_conv2d_11c1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \temp, \zero

        add  \filter_ptr, \filter_ptr, \filter_y_offset
        add  \input_ptr, \input_ptr, \dilation_y_offset

        addi  \filter_h, \filter_h, -1
        bnez  \filter_h, 11b

    add  \filter_ptr, \filter_ptr, \filter_n_offset
.endm



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_hwcn_bias
    .type   dl_esp32p4_s16_unaligned_conv2d_hwcn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_hwcn_bias:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value / output
    # t4: 15 - c_remainder
    # t5: tmp value / zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions):

    # s0:
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_20_stacks_5r  s1, s2, s3, s4, s8
    li  s1, -32768                  // clamp min value
    li  s8, 32767                   // clamp max value

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_hwcn_bias_128b
            li  t5, 8
            beq t3, t5, esp32p4_s16_unaligned_conv2d_hwcn_bias_64b
                # esp32p4_s16_unaligned_conv2d_hwcn_bias_32b:
                esp32p4_s16_unaligned_conv2d_hwcn_bias_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder

            esp32p4_s16_unaligned_conv2d_hwcn_bias_64b:
                esp32p4_s16_unaligned_conv2d_hwcn_bias_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder

        esp32p4_s16_unaligned_conv2d_hwcn_bias_128b:
            esp32p4_s16_unaligned_conv2d_hwcn_bias_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_conv2d_128b_vector_bias  t6
                esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_128b_multiple_loop

    esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder:
        lw  t1, 140(a2)             // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder_end
            lw  s3, 160(a2)         // s3: filter_y_offset_unaligned
            lw  s4, 164(a2)         // s4: filter_n_offset_unaligned
            lw  a3, 168(a2)         // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: zero

            esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_conv2d_element_bias  t6
                esp32p4_s16_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s16_element_result  t3, a5
                esp32p4_clamp  t3, s1, s8
                esp32p4_s16_element_store  a0, t3

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_hwcn_bias_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s1, s2, s3, s4, s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_relu
    .type   dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_hwcn_bias_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value / output
    # t4: 15 - c_remainder
    # t5: tmp value / zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions):

    # s0:
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_28_stacks_7r  s1, s2, s3, s4, s8, s9, s10
    li  s1, -32768                  // clamp min value
    li  s8, 32767                   // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_128b
            li  t5, 8
            beq t3, t5, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_64b
                # esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_32b:
                esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder

            esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_64b:
                esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_conv2d_128b_vector_bias  t6
                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder

        esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_128b:
            esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_conv2d_128b_vector_bias  t6
                esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_vector_relu  q0, s9, s10
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_128b_multiple_loop

    esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder:
        lw  t1, 140(a2)             // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_end
            lw  s3, 160(a2)         // s3: filter_y_offset_unaligned
            lw  s4, 164(a2)         // s4: filter_n_offset_unaligned
            lw  a3, 168(a2)         // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: zero

            esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_conv2d_element_bias  t6
                esp32p4_s16_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s16_element_result  t3, a5
                esp32p4_clamp  t3, s1, s8
                esp32p4_s16_element_leakyrelu  t3, s9, s10
                esp32p4_s16_element_store  a0, t3

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_hwcn_bias_relu_n_remainder_end:

    esp32p4_pop_28_stacks_7r  s1, s2, s3, s4, s8, s9, s10
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_hwcn
    .type   dl_esp32p4_s16_unaligned_conv2d_hwcn, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_hwcn:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value / output
    # t4: 15 - c_remainder
    # t5: tmp value / zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions):

    # s0:
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_20_stacks_5r  s1, s2, s3, s4, s8
    li  s1, -32768                  // clamp min value
    li  s8, 32767                   // clamp max value

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_hwcn_128b
            li  t5, 8
            beq t3, t5, esp32p4_s16_unaligned_conv2d_hwcn_64b
                # esp32p4_s16_unaligned_conv2d_hwcn_32b:
                esp32p4_s16_unaligned_conv2d_hwcn_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_n_remainder

            esp32p4_s16_unaligned_conv2d_hwcn_64b:
                esp32p4_s16_unaligned_conv2d_hwcn_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_n_remainder

        esp32p4_s16_unaligned_conv2d_hwcn_128b:
            esp32p4_s16_unaligned_conv2d_hwcn_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_128b_multiple_loop

    esp32p4_s16_unaligned_conv2d_hwcn_n_remainder:
        lw  t1, 140(a2)             // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_n_remainder_end
            lw  s3, 160(a2)         // s3: filter_y_offset_unaligned
            lw  s4, 164(a2)         // s4: filter_n_offset_unaligned
            lw  a3, 168(a2)         // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: zero

            esp32p4_s16_unaligned_conv2d_hwcn_n_remainder_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s16_element_result  t3, a5
                esp32p4_clamp  t3, s1, s8
                esp32p4_s16_element_store  a0, t3

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_hwcn_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s1, s2, s3, s4, s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s16_unaligned_conv2d_hwcn_relu
    .type   dl_esp32p4_s16_unaligned_conv2d_hwcn_relu, @function
    .balign 4
    .option norvc
dl_esp32p4_s16_unaligned_conv2d_hwcn_relu:

    # a0: int16_t *output_ptr
    # a1: int16_t *input_ptr
    # a2: void *args

    # a3: filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving input_ptr / tmp value / output
    # t4: 15 - c_remainder
    # t5: tmp value / zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions):

    # s0:
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_28_stacks_7r  s1, s2, s3, s4, s8, s9, s10
    li  s1, -32768                  // clamp min value
    li  s8, 32767                   // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift

    esp32p4_s16_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s16_unaligned_conv2d_hwcn_relu_128b
            li  t5, 8
            beq t3, t5, esp32p4_s16_unaligned_conv2d_hwcn_relu_64b
                # esp32p4_s16_unaligned_conv2d_hwcn_relu_32b:
                esp32p4_s16_unaligned_conv2d_hwcn_relu_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_relu_32b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder

            esp32p4_s16_unaligned_conv2d_hwcn_relu_64b:
                esp32p4_s16_unaligned_conv2d_hwcn_relu_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                    esp32p4_s16_128b_vector_shift_result  q0, a5
                    esp32p4_s16_128b_vector_relu  q0, s9, s10
                    esp32p4_s16_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_relu_64b_multiple_loop
                j  esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder

        esp32p4_s16_unaligned_conv2d_hwcn_relu_128b:
            esp32p4_s16_unaligned_conv2d_hwcn_relu_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s16_unaligned_conv2d_hwc8  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, t5
                esp32p4_s16_128b_vector_shift_result  q0, a5
                esp32p4_s16_128b_vector_relu  q0, s9, s10
                esp32p4_s16_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_relu_128b_multiple_loop

    esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder:
        lw  t1, 140(a2)             // t1: n_remainder
        beqz  t1, esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder_end
            lw  s3, 160(a2)         // s3: filter_y_offset_unaligned
            lw  s4, 164(a2)         // s4: filter_n_offset_unaligned
            lw  a3, 168(a2)         // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4         // t4: 15 - c_remainder
            li  t5,  0              // t5: zero

            esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.xacc

                esp32p4_s16_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s16_element_result  t3, a5
                esp32p4_clamp  t3, s1, s8
                esp32p4_s16_element_leakyrelu  t3, s9, s10
                esp32p4_s16_element_store  a0, t3

                addi  t1, t1, -1
                bnez  t1, esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder_loop

        esp32p4_s16_unaligned_conv2d_hwcn_relu_n_remainder_end:

    esp32p4_pop_28_stacks_7r  s1, s2, s3, s4, s8, s9, s10
    ret
