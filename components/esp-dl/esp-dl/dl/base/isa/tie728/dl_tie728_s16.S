.macro dl_tie728_128b_unaligned_store0 output_v, output_ptr, tmp32
    EE.MOVI.32.A \output_v, \tmp32, 0
    s32i \tmp32, \output_ptr, 0
    EE.MOVI.32.A \output_v, \tmp32, 1
    s32i \tmp32, \output_ptr, 4
    EE.MOVI.32.A \output_v, \tmp32, 2
    s32i \tmp32, \output_ptr, 8
    EE.MOVI.32.A \output_v, \tmp32, 3
    s32i \tmp32, \output_ptr, 12
    addi \output_ptr, \output_ptr, 16
.endm



.macro dl_tie728_128b_unaligned_store1 output_v, output_ptr
    EE.VST.L.64.IP \output_v, \output_ptr, 8
    EE.VST.H.64.IP \output_v, \output_ptr, 8
.endm



.macro dl_tie728_128b_last_store_data tmp_q, output_v, tmp_a, c_remainder_bytes
    beqi \c_remainder_bytes, 0, 600f
    movi \tmp_a, 15
    sub  \tmp_a, \tmp_a, \c_remainder_bytes
    movi \c_remainder_bytes, 0
    EE.SLCXXP.2Q \tmp_q, \output_v, \tmp_a, \c_remainder_bytes #left shift to make the rest part 0
    EE.SRCXXP.2Q \output_v, \tmp_q, \tmp_a, \c_remainder_bytes #right shift to lower bits
600:
.endm



.macro dl_tie728_s16_store_remainder remainder_data, c_remainder, tmp_a, output_ptr
607:    # remainder == 1, 0x111
    bbci \c_remainder, 2, 603f
    bbci \c_remainder, 1, 605f
    bbci \c_remainder, 0, 606f
    EE.MOVI.32.A \remainder_data, \tmp_a, 0
    s32i \tmp_a, \output_ptr, 0
    EE.MOVI.32.A \remainder_data, \tmp_a, 1
    s32i \tmp_a, \output_ptr, 4
    EE.MOVI.32.A \remainder_data, \tmp_a, 2
    s32i \tmp_a, \output_ptr, 8
    EE.MOVI.32.A \remainder_data, \tmp_a, 3
    s16i \tmp_a, \output_ptr, 12
    j 600f
606:    # remainder == 1, 0x110
    EE.MOVI.32.A \remainder_data, \tmp_a, 0
    s32i \tmp_a, \output_ptr, 0
    EE.MOVI.32.A \remainder_data, \tmp_a, 1
    s32i \tmp_a, \output_ptr, 4
    EE.MOVI.32.A \remainder_data, \tmp_a, 2
    s32i \tmp_a, \output_ptr, 8
    j 600f
605:    # remainder == 1, 0x101
    bbci \c_remainder, 0, 604f
    EE.MOVI.32.A \remainder_data, \tmp_a, 0
    s32i \tmp_a, \output_ptr, 0
    EE.MOVI.32.A \remainder_data, \tmp_a, 1
    s32i \tmp_a, \output_ptr, 4
    EE.MOVI.32.A \remainder_data, \tmp_a, 2
    s16i \tmp_a, \output_ptr, 8
    j 600f
604:    # remainder == 1, 0x100
    EE.MOVI.32.A \remainder_data, \tmp_a, 0
    s32i \tmp_a, \output_ptr, 0
    EE.MOVI.32.A \remainder_data, \tmp_a, 1
    s32i \tmp_a, \output_ptr, 4
    j 600f
603:    # remainder == 1, 0x011
    bbci \c_remainder, 1, 601f
    bbci \c_remainder, 0, 602f
    EE.MOVI.32.A \remainder_data, \tmp_a, 0
    s32i \tmp_a, \output_ptr, 0
    EE.MOVI.32.A \remainder_data, \tmp_a, 1
    s16i \tmp_a, \output_ptr, 4
    j 600f
602:    # remainder == 1, 0x010
    EE.MOVI.32.A \remainder_data, \tmp_a, 0
    s32i \tmp_a, \output_ptr, 0
    j 600f
601:    # remainder == 1, 0x001
    bbci \c_remainder, 0, 600f
    EE.MOVI.32.A \remainder_data, \tmp_a, 0
    s16i \tmp_a, \output_ptr, 0
600:
.endm



############################################################################################################################################################
# result process for Conv2D / Depthwise_Conv2D
############################################################################################################################################################
.macro tie728_s16_conv2d_per_layer_result  output_v, mac_shift
    EE.SRCMB.S16.QACC  \output_v, \mac_shift, 0
.endm



.macro tie728_s16_vector_round_result output_v mac_shift tmp tmp_q1
    beqz  \mac_shift, 500f

    movi  \tmp, 1                             // 0000 0000 0000 0000 0000 0000 0000 0001
    EE.MOVI.32.Q  \output_v, \tmp, 0
    EE.MOVI.32.Q  \output_v, \tmp, 1
    EE.MOVI.32.Q  \output_v, \tmp, 2
    EE.MOVI.32.Q  \output_v, \tmp, 3
    movi.n  \tmp, 16
    wsr.sar  \tmp
    EE.VSL.32  \tmp_q1, \output_v
    EE.ORQ  \tmp_q1, \tmp_q1, \output_v         // 0000 0000 0000 0001 0000 0000 0000 0001

    addi  \tmp, \mac_shift, -1
    EE.SRCMB.S16.QACC  \output_v, \tmp, 0

    movi.n  \tmp, 1
    EE.MOVI.32.Q  \output_v, \tmp, 0
    EE.VSMULAS.S16.QACC  \tmp_q1, \output_v, 0   // qacc[0:16] += round
    EE.SRCMB.S16.QACC  \output_v, \tmp, 0
    j  501f
500:
    EE.SRCMB.S16.QACC  \output_v, \mac_shift, 0
501:
.endm



.macro tie728_s16_element_round_result  output mac_shift tmp tmp_q1
    beqz  \mac_shift, 505f

    addi  \tmp, \mac_shift, -1
    EE.SRS.ACCX  \output, \tmp, 0

    movi.n  \tmp, 1
    EE.ZERO.Q  \tmp_q1
    EE.MOVI.32.Q  \tmp_q1, \tmp, 0
    EE.VMULAS.S16.ACCX  \tmp_q1, \tmp_q1
    EE.SRS.ACCX  \output, \tmp, 0
    j  506f
505:
    EE.SRS.ACCX  \output, \mac_shift, 0
506:
.endm



.macro tie728_s16_conv2d_128b_vector_bias  bias_ptr
    EE.LD.QACC_L.L.128.IP  \bias_ptr, 16
    EE.LD.QACC_L.H.32.IP  \bias_ptr, 16
    EE.LD.QACC_H.L.128.IP  \bias_ptr, 16
    EE.LD.QACC_H.H.32.IP  \bias_ptr, 16
.endm



.macro tie728_s16_conv2d_element_bias  bias_ptr
    EE.LD.ACCX.IP  \bias_ptr, 8
.endm



.macro tie728_s16_conv2d_relu  output_v, activation_alpha, activation_shift
    # LeakyReLU
    EE.VRELU.S16  \output_v, \activation_alpha, \activation_shift
.endm



.macro tie728_s16_conv2d_prelu  output_v, activation_v, activation_alpha_ptr activation_shift
    EE.VLD.128.IP   \activation_v,    \activation_alpha_ptr,   16 # load PReLU alph

    # PReLU
    EE.VPRELU.S16 \output_v, \output_v, \activation_v, \activation_shift
.endm
