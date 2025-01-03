

############################################################################################################################################################
# result process for conv2d / depthwise_conv2d
############################################################################################################################################################
.macro esp32p4_s8_conv2d_128b_vector_bias  bias_ptr
    esp.ld.qacc.l.l.128.ip  \bias_ptr, 16
    esp.ld.qacc.l.h.128.ip  \bias_ptr, 16
    esp.ld.qacc.h.l.128.ip  \bias_ptr, 16
    esp.ld.qacc.h.h.128.ip  \bias_ptr, 16
.endm



.macro esp32p4_s8_conv2d_element_bias  bias_ptr, tmp
    lw  \tmp, 0(\bias_ptr)
    addi  \bias_ptr, \bias_ptr, 4
    esp.movx.w.xacc.l  \tmp
    slti  \tmp, \tmp, 0     // if tmp < 0, tmp = 1, otherwise tmp = 0
    slli  \tmp, \tmp, 31    // shift left to the sign bit.
    srai  \tmp, \tmp, 31    // extend the sign bit to all bits.
    esp.movx.w.xacc.h  \tmp
.endm



############################################################################################################################################################
# esp32p4_s8_32b_unaligned_vector series
############################################################################################################################################################
.macro esp32p4_s8_32b_unaligned_vector_store  output_v, output_ptr, tmp
    esp.movi.32.a  \output_v, \tmp, 0
    sw  \tmp, 0(\output_ptr)
    esp.movi.32.a  \output_v, \tmp, 1
    sw  \tmp, 4(\output_ptr)
    esp.movi.32.a  \output_v, \tmp, 2
    sw  \tmp, 8(\output_ptr)
    esp.movi.32.a  \output_v, \tmp, 3
    sw  \tmp, 12(\output_ptr)
    addi  \output_ptr, \output_ptr, 16
.endm



############################################################################################################################################################
# esp32p4_s8_64b_unaligned_vector series
############################################################################################################################################################
.macro esp32p4_s8_64b_unaligned_vector_store  output_v, output_ptr
    esp.vst.l.64.ip \output_v, \output_ptr, 8
    esp.vst.h.64.ip \output_v, \output_ptr, 8
.endm



############################################################################################################################################################
# esp32p4_s8_128b_vector series
############################################################################################################################################################
.macro esp32p4_s8_128b_vector_shift_result  output_v, mac_shift
    esp.srcmb.s8.qacc  \output_v, \mac_shift, 1
.endm



.macro esp32p4_s8_128b_aligned_vector_store  output_v, output_ptr
    esp.vst.128.ip  \output_v, \output_ptr, 16
.endm



.macro esp32p4_s8_128b_vector_relu  output_v, activation_alpha, activation_shift
    # LeakyReLU
    esp.vrelu.s8  \output_v, \activation_alpha, \activation_shift
.endm

.macro esp32p4_s8_128b_vector_prelu  output_v, activation_alpha_v, activation_alpha_ptr, activation_shift
    esp.vld.128.ip \activation_alpha_v, \activation_alpha_ptr, 16
    esp.vprelu.s8  \output_v, \output_v, \activation_alpha_v, \activation_shift
.endm



.macro dl_esp32p4_s8_last_store_data tmp_q, output_v, tmp_a, c_remainder_bytes
    movi \tmp_a, 15
    sub  \tmp_a, \tmp_a, \c_remainder_bytes
    movi \c_remainder_bytes, 0
    esp.slcxxp.2q \tmp_q, \output_v, \tmp_a, \c_remainder_bytes #left shift to make the rest part 0
    esp.srcxxp.2q \output_v, \tmp_q, \tmp_a, \c_remainder_bytes #right shift to lower bits
.endm



.macro dl_esp32p4_s8_store_remainder output_v, tmp_a0, tmp_a1, tmp_a2, tmp_a3, tmp_a4, output_ptr, remainder_c
    esp.movi.32.a \output_v, \tmp_a0, 0
615: # remainder_c == 15, 0x1111
    andi \tmp_a4, \remainder_c, 8
    beqz \tmp_a4, 607f
    esp.movi.32.a \output_v, \tmp_a1, 1
    andi \tmp_a4, \remainder_c, 4
    beqz \tmp_a4, 611f
    esp.movi.32.a \output_v, \tmp_a2, 2
    andi \tmp_a4, \remainder_c, 2
    beqz \tmp_a4, 613f
    esp.movi.32.a \output_v, \tmp_a3, 3
    andi \tmp_a4, \remainder_c, 1
    beqz \tmp_a4, 614f

    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    sw \tmp_a2, 8(\output_ptr)
    sh \tmp_a3, 12(\output_ptr)
    srai \tmp_a3, \tmp_a3, 16
    sb \tmp_a3, 14(\output_ptr)
    j 616f

614:  # remainder_c == 14, 0x1110
    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    sw \tmp_a2, 8(\output_ptr)
    sh \tmp_a3, 12(\output_ptr)
    j 616f

613:  # remainder_c == 13, 0x1101
    andi \tmp_a4, \remainder_c, 1
    beqz \tmp_a4, 612f
    esp.movi.32.a \output_v, \tmp_a3, 3
    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    sw \tmp_a2, 8(\output_ptr)
    sb \tmp_a3, 12(\output_ptr)
    j 616f

612:  # remainder_c == 12, 0x1100
    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    sw \tmp_a2, 8(\output_ptr)
    j 616f

611:  # remainder_c == 11, 0x1011
    andi \tmp_a4, \remainder_c, 2
    beqz \tmp_a4, 609f
    esp.movi.32.a \output_v, \tmp_a2, 2
    andi \tmp_a4, \remainder_c, 1
    beqz \tmp_a4, 610f
    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    sh \tmp_a2, 8(\output_ptr)
    srai \tmp_a2, \tmp_a2, 16
    sb \tmp_a2, 10(\output_ptr)
    j 616f
610:  # remainder_c == 10, 0x1010
    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    sh \tmp_a2, 8(\output_ptr)
    j 616f
609:  # remainder_c == 9, 0x1001
    andi \tmp_a4, \remainder_c, 1
    beqz \tmp_a4, 608f
    esp.movi.32.a \output_v, \tmp_a2, 2
    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    sb \tmp_a2, 8(\output_ptr)
    j 616f
608:  # remainder_c == 8, 0x1000
    sw \tmp_a0, 0(\output_ptr)
    sw \tmp_a1, 4(\output_ptr)
    j 616f

607: # remainder == 7, 0x111
    andi \tmp_a4, \remainder_c, 4
    beqz \tmp_a4, 603f
    andi \tmp_a4, \remainder_c, 2
    beqz \tmp_a4, 605f
    esp.movi.32.a \output_v, \tmp_a1, 1
    andi \tmp_a4, \remainder_c, 1
    beqz \tmp_a4, 606f
    sw \tmp_a0, 0(\output_ptr)
    sh \tmp_a1, 4(\output_ptr)
    srai \tmp_a1, \tmp_a1, 16
    sb \tmp_a1, 6(\output_ptr)
    j 616f

606:  # remainder == 6, 0x110
    sw \tmp_a0, 0(\output_ptr)
    sh \tmp_a1, 4(\output_ptr)
    j 616f

605:  # remainder == 4, 5
    andi \tmp_a4, \remainder_c, 1
    beqz \tmp_a4, 604f
    # remainder == 5, 0x101
    esp.movi.32.a \output_v, \tmp_a1, 1
    sw \tmp_a0, 0(\output_ptr)
    sb \tmp_a1, 4(\output_ptr)
    j 616f

604:  # remainder == 4, 0x100
    sw \tmp_a0, 0(\output_ptr)
    j 616f

603:  # remainder == 1, 2, 3
    andi \tmp_a4, \remainder_c, 2
    beqz \tmp_a4, 601f
    andi \tmp_a4, \remainder_c, 1
    beqz \tmp_a4, 602f
    # remainder == 3, 0x011
    sh \tmp_a0, 0(\output_ptr)
    srai \tmp_a0, \tmp_a0, 16
    sb \tmp_a0, 2(\output_ptr)
    j 616f

602:  # remainder == 2, 0x010
    sh \tmp_a0, 0(\output_ptr)
    j 616f

601:  # remainder == 1, 0x001
    sb \tmp_a0, 0(\output_ptr)

616:
.endm



############################################################################################################################################################
# esp32p4_s8_element series
############################################################################################################################################################
.macro esp32p4_s8_element_result  output, mac_shift
    esp.srs.s.xacc  \output, \mac_shift
.endm



.macro esp32p4_s8_element_store  output_ptr, output
    sb  \output, 0(\output_ptr)
    addi  \output_ptr, \output_ptr, 1
.endm



.macro esp32p4_s8_element_leakyrelu  output, alpha, shift
    bgez \output, 0f
        mul  \output, \output, \alpha
        sra  \output, \output, \shift
    0:
.endm

.macro esp32p4_s8_element_prelu  output, alpha_ptr, shift
    bgez \output, 0f
        mul  \output, \output, \alpha_ptr
        sra  \output, \output, \shift
        addi \alpha_ptr, \alpha_ptr, 1
    0:
.endm
