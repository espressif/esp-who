

############################################################################################################################################################
# result process for conv2d / depthwise_conv2d
############################################################################################################################################################
.macro esp32p4_s16_conv2d_128b_vector_bias  bias_ptr
    esp.ld.qacc.l.l.128.ip  \bias_ptr, 16
    esp.ld.qacc.l.h.128.ip  \bias_ptr, 16
    esp.ld.qacc.h.l.128.ip  \bias_ptr, 16
    esp.ld.qacc.h.h.128.ip  \bias_ptr, 16
.endm



.macro esp32p4_s16_conv2d_element_bias  bias_ptr
    esp.ld.xacc.ip  \bias_ptr, 8
.endm



############################################################################################################################################################
# esp32p4_s16_32b_unaligned_vector series
############################################################################################################################################################
.macro esp32p4_s16_32b_unaligned_vector_store  output_v, output_ptr, tmp
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
# esp32p4_s16_64b_unaligned_vector series
############################################################################################################################################################
.macro esp32p4_s16_64b_unaligned_vector_store  output_v, output_ptr
    esp.vst.l.64.ip \output_v, \output_ptr, 8
    esp.vst.h.64.ip \output_v, \output_ptr, 8
.endm



############################################################################################################################################################
# esp32p4_s16_128b_vector series
############################################################################################################################################################
.macro esp32p4_s16_128b_vector_shift_result  output_v, mac_shift
    esp.srcmb.s16.qacc  \output_v, \mac_shift, 1
.endm



.macro esp32p4_s16_128b_aligned_vector_store  output_v, output_ptr
    esp.vst.128.ip  \output_v, \output_ptr, 16
.endm



.macro esp32p4_s16_128b_vector_relu  output_v, activation_alpha, activation_shift
    # LeakyReLU
    esp.vrelu.s16  \output_v, \activation_alpha, \activation_shift
.endm

.macro dl_esp32p4_128b_unaligned_store0 output_v, output_ptr, tmp32
    esp.movi.32.a \output_v, \tmp32, 0
    sw \tmp32, 0(\output_ptr)
    esp.movi.32.a \output_v, \tmp32, 1
    sw \tmp32, 4(\output_ptr)
    esp.movi.32.a \output_v, \tmp32, 2
    sw \tmp32, 8(\output_ptr)
    esp.movi.32.a \output_v, \tmp32, 3
    sw \tmp32, 12(\output_ptr)
    addi \output_ptr, \output_ptr, 16
.endm

.macro dl_esp32p4_128b_unaligned_store1 output_v, output_ptr
    esp.vst.l.64.ip \output_v, \output_ptr, 8
    esp.vst.h.64.ip \output_v, \output_ptr, 8
.endm

.macro dl_esp32p4_128b_last_store_data tmp_q, output_v, tmp_a, c_remainder_bytes
    beqz \c_remainder_bytes, 600f
    li \tmp_a, 15
    sub  \tmp_a, \tmp_a, \c_remainder_bytes
    li \c_remainder_bytes, 0
    esp.slcxxp.2q \tmp_q, \output_v, \tmp_a, \c_remainder_bytes #left shift to make the rest part 0
    esp.srcxxp.2q \output_v, \tmp_q, \tmp_a, \c_remainder_bytes #right shift to lower bits
600:
.endm



.macro dl_esp32p4_s16_store_remainder  vector_data, c_remainder, tmp_a, output_ptr
607:    # remainder == 7, 0x111
    andi \tmp_a, \c_remainder, 4
    beqz \tmp_a, 603f
    andi \tmp_a, \c_remainder, 2
    beqz \tmp_a, 605f
    andi \tmp_a, \c_remainder, 1
    beqz \tmp_a, 606f
    esp.movi.32.a \vector_data, \tmp_a, 0
    sw \tmp_a, 0(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 1
    sw \tmp_a, 4(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 2
    sw \tmp_a, 8(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 3
    sh \tmp_a, 12(\output_ptr)
    j 600f
606:    # remainder == 6, 0x110
    esp.movi.32.a \vector_data, \tmp_a, 0
    sw \tmp_a, 0(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 1
    sw \tmp_a, 4(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 2
    sw \tmp_a, 8(\output_ptr)
    j 600f
605:    # remainder == 5, 0x101
    andi \tmp_a, \c_remainder, 1
    beqz \tmp_a, 604f
    esp.movi.32.a \vector_data, \tmp_a, 0
    sw \tmp_a, 0(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 1
    sw \tmp_a, 4(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 2
    sh \tmp_a, 8(\output_ptr)
    j 600f
604:    # remainder == 4, 0x100
    esp.movi.32.a \vector_data, \tmp_a, 0
    sw \tmp_a, 0(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 1
    sw \tmp_a, 4(\output_ptr)
    j 600f
603:    # remainder == 3, 0x011
    andi \tmp_a, \c_remainder, 2
    beqz \tmp_a, 601f
    andi \tmp_a, \c_remainder, 1
    beqz \tmp_a, 602f
    esp.movi.32.a \vector_data, \tmp_a, 0
    sw \tmp_a, 0(\output_ptr)
    esp.movi.32.a \vector_data, \tmp_a, 1
    sh \tmp_a, 4(\output_ptr)
    j 600f
602:    # remainder == 2, 0x010
    esp.movi.32.a \vector_data, \tmp_a, 0
    sw \tmp_a, 0(\output_ptr)
    j 600f
601:    # remainder == 1, 0x001
    andi \tmp_a, \c_remainder, 1
    beqz \tmp_a, 600f
    esp.movi.32.a \vector_data, \tmp_a, 0
    sh \tmp_a, 0(\output_ptr)
600:
.endm



############################################################################################################################################################
# esp32p4_s16_element series
############################################################################################################################################################
.macro esp32p4_s16_element_result  output, mac_shift
    esp.srs.s.xacc  \output, \mac_shift
.endm



.macro esp32p4_s16_element_store  output_ptr, output
    sh  \output, 0(\output_ptr)
    addi  \output_ptr, \output_ptr, 2
.endm



.macro esp32p4_s16_element_leakyrelu  output, alpha, shift
    bgez \output, 0f
        mul  \output, \output, \alpha
        sra  \output, \output, \shift
    0:
.endm
