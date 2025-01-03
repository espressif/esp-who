
.macro dl_tie728_s8_unaligned_store0 output_v, output_ptr, tmp32
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

.macro dl_tie728_s8_unaligned_store1 output_v, output_ptr
    EE.VST.L.64.IP \output_v, \output_ptr, 8
    EE.VST.H.64.IP \output_v, \output_ptr, 8
.endm


.macro dl_tie728_s8_last_store_data tmp_q, output_v, tmp_a, c_remainder_bytes
    movi \tmp_a, 15
    sub  \tmp_a, \tmp_a, \c_remainder_bytes
    movi \c_remainder_bytes, 0
    EE.SLCXXP.2Q \tmp_q, \output_v, \tmp_a, \c_remainder_bytes #left shift to make the rest part 0
    EE.SRCXXP.2Q \output_v, \tmp_q, \tmp_a, \c_remainder_bytes #right shift to lower bits
.endm


.macro dl_tie728_s8_store_remainder output_v, tmp_a0, tmp_a1, tmp_a2, tmp_a3, output_ptr, remainder_c
    EE.MOVI.32.A \output_v, \tmp_a0, 0
615: # remainder_c == 15, 0x1111
    bbci \remainder_c, 3, 607f
    EE.MOVI.32.A \output_v, \tmp_a1, 1
    bbci \remainder_c, 2, 611f
    EE.MOVI.32.A \output_v, \tmp_a2, 2
    bbci \remainder_c, 1, 613f
    EE.MOVI.32.A \output_v, \tmp_a3, 3
    bbci \remainder_c, 0, 614f

    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    s32i \tmp_a2, \output_ptr, 8
    s16i \tmp_a3, \output_ptr, 12
    srai \tmp_a3, \tmp_a3, 16
    s8i \tmp_a3, \output_ptr, 14
    j 616f

614:  # remainder_c == 14, 0x1110
    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    s32i \tmp_a2, \output_ptr, 8
    s16i \tmp_a3, \output_ptr, 12
    j 616f

613:  # remainder_c == 13, 0x1101
    bbci \remainder_c, 0, 612f
    EE.MOVI.32.A \output_v, \tmp_a3, 3
    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    s32i \tmp_a2, \output_ptr, 8
    s8i \tmp_a3, \output_ptr, 12
    j 616f

612:  # remainder_c == 12, 0x1100
    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    s32i \tmp_a2, \output_ptr, 8
    j 616f

611:  # remainder_c == 11, 0x1011
    bbci \remainder_c, 1, 609f
    EE.MOVI.32.A \output_v, \tmp_a2, 2
    bbci \remainder_c, 0, 610f
    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    s16i \tmp_a2, \output_ptr, 8
    srai \tmp_a2, \tmp_a2, 16
    s8i \tmp_a2, \output_ptr, 10
    j 616f
610:  # remainder_c == 10, 0x1010
    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    s16i \tmp_a2, \output_ptr, 8
    j 616f
609:  # remainder_c == 9, 0x1001
    bbci \remainder_c, 0, 608f
    EE.MOVI.32.A \output_v, \tmp_a2, 2
    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    s8i \tmp_a2, \output_ptr, 8
    j 616f
608:  # remainder_c == 8, 0x1000
    s32i \tmp_a0, \output_ptr, 0
    s32i \tmp_a1, \output_ptr, 4
    j 616f

607: # remainder == 7, 0x111
    bbci \remainder_c, 2, 603f
    bbci \remainder_c, 1, 605f
    EE.MOVI.32.A \output_v, \tmp_a1, 1
    bbci \remainder_c, 0, 606f
    s32i \tmp_a0, \output_ptr, 0
    s16i \tmp_a1, \output_ptr, 4
    srai \tmp_a1, \tmp_a1, 16
    s8i \tmp_a1, \output_ptr, 6
    j 616f

606:  # remainder == 6, 0x110
    s32i \tmp_a0, \output_ptr, 0
    s16i \tmp_a1, \output_ptr, 4
    j 616f

605:  # remainder == 4, 5
    bbci \remainder_c, 0, 604f
    # remainder == 5, 0x101
    EE.MOVI.32.A \output_v, \tmp_a1, 1
    s32i \tmp_a0, \output_ptr, 0
    s8i \tmp_a1, \output_ptr, 4
    j 616f

604:  # remainder == 4, 0x100
    s32i \tmp_a0, \output_ptr, 0
    j 616f

603:  # remainder == 1, 2, 3
    bbci \remainder_c, 1, 601f
    bbci \remainder_c, 0, 602f
    # remainder == 3, 0x011
    s16i \tmp_a0, \output_ptr, 0
    srai \tmp_a0, \tmp_a0, 16
    s8i \tmp_a0, \output_ptr, 2
    j 616f

602:  # remainder == 2, 0x010
    s16i \tmp_a0, \output_ptr, 0
    j 616f

601:  # remainder == 1, 0x001
    s8i \tmp_a0, \output_ptr, 0

616:
.endm


############################################################################################################################################################
# result process for Conv2D / Depthwise_Conv2D
############################################################################################################################################################


.macro tie728_s8_conv2d_per_layer_result output_v mac_shift
    EE.SRCMB.S8.QACC \output_v, \mac_shift, 0
.endm


.macro tie728_s8_vector_round_result output_v mac_shift tmp tmp_q1
    beqz  \mac_shift, 500f

    MOVI  \tmp, 257                             // 0000 0000 0000 0000 0000 0001 0000 0001
    EE.MOVI.32.Q  \output_v, \tmp, 0
    EE.MOVI.32.Q  \output_v, \tmp, 1
    EE.MOVI.32.Q  \output_v, \tmp, 2
    EE.MOVI.32.Q  \output_v, \tmp, 3
    movi.n  \tmp, 16
    wsr.sar  \tmp
    EE.VSL.32  \tmp_q1, \output_v
    EE.ORQ  \tmp_q1, \tmp_q1, \output_v         // 0000 0001 0000 0001 0000 0001 0000 0001

    addi  \tmp, \mac_shift, -1
    EE.SRCMB.S8.QACC  \output_v, \tmp, 0

    movi.n  \tmp, 1
    EE.MOVI.32.Q  \output_v, \tmp, 0
    EE.VSMULAS.S8.QACC  \tmp_q1, \output_v, 0   // qacc[0:16] += round
    EE.SRCMB.S8.QACC  \output_v, \tmp, 0
    j  501f
500:
    EE.SRCMB.S8.QACC  \output_v, \mac_shift, 0
501:
.endm


.macro tie728_s8_element_round_result  output mac_shift tmp tmp_q1
    beqz  \mac_shift, 505f

    addi  \tmp, \mac_shift, -1
    EE.SRS.ACCX  \output, \tmp, 0

    movi.n  \tmp, 1
    EE.ZERO.Q  \tmp_q1
    EE.MOVI.32.Q  \tmp_q1, \tmp, 0
    EE.VMULAS.S8.ACCX  \tmp_q1, \tmp_q1
    EE.SRS.ACCX  \output, \tmp, 0
    j  506f
505:
    EE.SRS.ACCX  \output, \mac_shift, 0
506:
.endm


# what if a1 not 16 byte aligned?
.macro tie728_s8_conv2d_per_channel_result output_v scale_q scale_factor tmp tmp_q1
    # entry need to be 128
    movi \tmp, 4
    EE.SRCMB.S16.QACC \output_v, \tmp, 0 # get the lower 16 bit in QACC

    mov \tmp, a1
    EE.ST.QACC_L.L.128.IP \tmp, 16
    EE.ST.QACC_L.H.32.IP \tmp, 16
    EE.ST.QACC_H.L.128.IP \tmp, 16
    EE.ST.QACC_H.H.32.IP \tmp, 4

    movi \tmp, 20
    EE.SRCMB.S16.QACC \tmp_q1, \tmp, 0 # \tmp_q1: even 16 bit

    l16si \tmp, a1, 5 #re-arrange qacc odd 16 bit
    s16i \tmp, a1, 2
    l16si \tmp, a1, 10
    s16i \tmp, a1, 4
    l16si \tmp, a1, 15
    s16i \tmp, a1, 6
    l16si \tmp, a1, 32
    s16i \tmp, a1, 8
    l16si \tmp, a1, 37
    s16i \tmp, a1, 10
    l16si \tmp, a1, 42
    s16i \tmp, a1, 12
    l16si \tmp, a1, 47
    s16i \tmp, a1, 14

    EE.VLD.128.IP \output_v, a1, 0 # \output_v: odd 16 bit

    EE.VZIP.16 \output_v, \tmp_q1

    movi \tmp, 11
    ssr \tmp

    EE.VLD.128.IP \scale_q, \scale_factor, 16 # scale factor
    movi \tmp, 0
    EE.VMUL.S16 \output_v, \output_v, \scale_q

    EE.VLD.128.IP \scale_q, \scale_factor, 16
    EE.MOV.S16.QACC \output_v
    EE.VMUL.S16 \tmp_q1, \tmp_q1, \scale_q

    EE.SRCMB.S8.QACC \output_v, \tmp, 0
    EE.MOV.S16.QACC \tmp_q1
    EE.SRCMB.S8.QACC \tmp_q1, \tmp, 0
    EE.VUNZIP.8 \output_v, \tmp_q1
.endm


.macro tie728_s8_conv2d_per_channel_with_bias_result output_v scale_q scale_factor bias_ptr tmp tmp_q1
    # entry need to be 128
    movi \tmp, 4
    EE.SRCMB.S16.QACC \output_v, \tmp, 0 # get the lower 16 bit in QACC

    mov \tmp, a1
    EE.ST.QACC_L.L.128.IP \tmp, 16
    EE.ST.QACC_L.H.32.IP \tmp, 16
    EE.ST.QACC_H.L.128.IP \tmp, 16
    EE.ST.QACC_H.H.32.IP \tmp, 4

    movi \tmp, 20
    EE.SRCMB.S16.QACC \tmp_q1, \tmp, 0 # \tmp_q1: even 16 bit

    l16si \tmp, a1, 5 #re-arrange qacc odd 16 bit
    s16i \tmp, a1, 2
    l16si \tmp, a1, 10
    s16i \tmp, a1, 4
    l16si \tmp, a1, 15
    s16i \tmp, a1, 6
    l16si \tmp, a1, 32
    s16i \tmp, a1, 8
    l16si \tmp, a1, 37
    s16i \tmp, a1, 10
    l16si \tmp, a1, 42
    s16i \tmp, a1, 12
    l16si \tmp, a1, 47
    s16i \tmp, a1, 14

    EE.VLD.128.IP \output_v, a1, 0 # \output_v: odd 16 bit

    EE.VZIP.16 \output_v, \tmp_q1


    EE.VLD.128.IP \scale_q, \bias_ptr, 16 # load bias
    movi \tmp, 11
    EE.VADDS.S16 \output_v, \output_v, \scale_q # add int16 bias with exponent(input+filter-4)


    EE.VLD.128.IP \scale_q, \bias_ptr, 16 # load bias
    ssr \tmp
    EE.VADDS.S16 \tmp_q1, \tmp_q1, \scale_q


    EE.VLD.128.IP \scale_q, \scale_factor, 16 # scale factor
    movi \tmp, 0
    EE.VMUL.S16 \output_v, \output_v, \scale_q

    EE.VLD.128.IP \scale_q, \scale_factor, 16
    EE.MOV.S16.QACC \output_v
    EE.VMUL.S16 \tmp_q1, \tmp_q1, \scale_q

    EE.SRCMB.S8.QACC \output_v, \tmp, 0
    EE.MOV.S16.QACC \tmp_q1
    EE.SRCMB.S8.QACC \tmp_q1, \tmp, 0
    EE.VUNZIP.8 \output_v, \tmp_q1

.endm


.macro tie728_s8_conv2d_128b_vector_bias  bias_ptr
    EE.LD.QACC_L.L.128.IP  \bias_ptr, 16
    EE.LD.QACC_L.H.32.IP  \bias_ptr, 16
    EE.LD.QACC_H.L.128.IP  \bias_ptr, 16
    EE.LD.QACC_H.H.32.IP  \bias_ptr, 16
.endm


.macro tie728_s8_conv2d_element_bias  bias_ptr
    EE.LD.ACCX.IP  \bias_ptr, 8
.endm


.macro tie728_s8_conv2d_bias output_v bias_v bias_ptr
    EE.VLD.128.IP \bias_v, \bias_ptr, 16 # load bias

    # bias
    EE.VADDS.S8 \output_v, \output_v, \bias_v
.endm



.macro tie728_s8_conv2d_bias_relu output_v bias_v bias_ptr activation_alpha activation_shift
    EE.VLD.128.IP \bias_v, \bias_ptr, 16 # load bias

    # bias
    EE.VADDS.S8 \output_v, \output_v, \bias_v

    # LeakyReLU
    EE.VRELU.S8 \output_v, \activation_alpha, \activation_shift
.endm



.macro tie728_s8_conv2d_bias_prelu output_v bias_v bias_ptr activation_v activation_alpha_ptr activation_shift
    EE.VLD.128.IP   \bias_v,     \bias_ptr,    16 # load bias
    EE.VLD.128.IP   \activation_v,    \activation_alpha_ptr,   16 # load PReLU alph

    # bias
    EE.VADDS.S8 \output_v, \output_v, \bias_v

    # PReLU
    EE.VPRELU.S8 \output_v, \output_v, \activation_v, \activation_shift
.endm



.macro tie728_s8_conv2d_relu output_v activation_alpha activation_shift

    # LeakyReLU
    EE.VRELU.S8 \output_v, \activation_alpha, \activation_shift

.endm



.macro tie728_s8_conv2d_prelu output_v activation_v activation_alpha_ptr activation_shift
    EE.VLD.128.IP   \activation_v,    \activation_alpha_ptr,   16 # load PReLU alph

    # PReLU
    EE.VPRELU.S8 \output_v, \output_v, \activation_v, \activation_shift
.endm




############################################################################################################################################################
####
#### tie728_s8_unaligned_conv2d / depthwise_conv2d
####
############################################################################################################################################################



.macro tie728_s8_unaligned_conv2d_operation_type operation_type mac_shift bias_ptr activation_shift activation_alpha args
    movi \operation_type, 0
0: // per-layer or per-channel
    bltz \mac_shift, 1f
    j 2f
    1: //per_channel
        l32i \mac_shift, \args, 104 // filter_channel_factor address
        addi \operation_type, \operation_type, 6

2: // bias
    beqz \bias_ptr, 3f // no bias
        addi \operation_type, \operation_type, 3

3: // activation     dl_tie728_s8_unaligned_conv2d_activation
    bltz \activation_shift, 5f // no activation

        beqz \activation_alpha, 4f
            addi \operation_type, \operation_type, 2
            j 5f

        4:
            l32i \activation_alpha, \args, 76  // load activation_alpha
            addi \operation_type, \operation_type, 1 // Relu or LeakyReLU

5: //put operation type in \operation_type

.endm



.macro tie728_s8_conv2d_1_1_unaligned_c_result operation_type output_v mac_shift bias_ptr activation_alpha activation_shift tmp tmp_q1 tmp_q2

    bbci \operation_type, 3, 7f
    bbci \operation_type, 2, 11f
11:  # per_channel bias + prelu, 0x1011
    bbci \operation_type, 1, 9f
    bbci \operation_type, 0, 10f

    # tie728_s8_conv2d_per_channel_result \output_v, \tmp_q2, \mac_shift, \tmp, \tmp_q1
    # tie728_s8_conv2d_bias_prelu \output_v, \tmp_q1, \bias_ptr, \tmp_q2, \activation_alpha, \activation_shift

    tie728_s8_conv2d_per_channel_with_bias_result \output_v, \tmp_q2, \mac_shift, \bias_ptr, \tmp, \tmp_q1
    tie728_s8_conv2d_prelu \output_v, \tmp_q2, \activation_alpha, \activation_shift

    j 16f  # jump to 16f
10:  # per_channel bias + relu, 0x1010
    # tie728_s8_conv2d_per_channel_result \output_v, \tmp_q2, \mac_shift, \tmp, \tmp_q1
    # tie728_s8_conv2d_bias_relu \output_v, \tmp_q1, \bias_ptr, \activation_alpha, \activation_shift

    tie728_s8_conv2d_per_channel_with_bias_result \output_v, \tmp_q2, \mac_shift, \bias_ptr, \tmp, \tmp_q1
    tie728_s8_conv2d_relu \output_v, \activation_alpha, \activation_shift
    j 16f  # jump to 16f

9:  # per_channel bias, 0x1001
    bbci \operation_type, 0, 8f
    # tie728_s8_conv2d_per_channel_result \output_v, \tmp_q2, \mac_shift, \tmp, \tmp_q1
    # tie728_s8_conv2d_bias \output_v, \tmp_q1, \bias_ptr
    tie728_s8_conv2d_per_channel_with_bias_result \output_v, \tmp_q2, \mac_shift, \bias_ptr, \tmp, \tmp_q1

    j 16f  # jump to 16f
8:  #per_channel no_bias + prelu, 0x1000
    tie728_s8_conv2d_per_channel_result \output_v, \tmp_q2, \mac_shift, \tmp, \tmp_q1
    tie728_s8_conv2d_prelu \output_v, \tmp_q2, \activation_alpha, \activation_shift

    j 16f  # jump to 16f

7: # per_channel no_bias + relu, 0x111
    bbci \operation_type, 2, 3f
    bbci \operation_type, 1, 5f
    bbci \operation_type, 0, 6f

    tie728_s8_conv2d_per_channel_result \output_v, \tmp_q2, \mac_shift, \tmp, \tmp_q1
    tie728_s8_conv2d_relu \output_v, \activation_alpha, \activation_shift

    j 16f  # jump to 16f

6:  # per_channel no_bias, 0x110
    tie728_s8_conv2d_per_channel_result \output_v, \tmp_q2, \mac_shift, \tmp, \tmp_q1

    j 16f  # jump to 16f

5:  # remainder == 4, 5
    bbci \operation_type, 0, 4f
    # per_layer bias + prelu, 0x101
    # tie728_s8_conv2d_per_layer_result \output_v, \mac_shift
    tie728_s8_vector_round_result \output_v, \mac_shift, \tmp, \tmp_q1
    # tie728_s8_conv2d_bias_prelu \output_v, \tmp_q1, \bias_ptr, \tmp_q2, \activation_alpha, \activation_shift
    # bias will be preload
    tie728_s8_conv2d_prelu \output_v, \tmp_q2, \activation_alpha, \activation_shift

    j 16f  # jump to 16f

4:  # per_layer bias + relu, 0x100
    # tie728_s8_conv2d_per_layer_result \output_v, \mac_shift
    tie728_s8_vector_round_result \output_v, \mac_shift, \tmp, \tmp_q1
    # tie728_s8_conv2d_bias_relu \output_v, \tmp_q1, \bias_ptr, \activation_alpha, \activation_shift
    # bias will be preload
    tie728_s8_conv2d_relu \output_v, \activation_alpha, \activation_shift

    j 16f  # jump to 16f

3:  # remainder == 1, 2, 3
    bbci \operation_type, 1, 1f
    bbci \operation_type, 0, 2f
    # per_layer bias, 0x011
    # tie728_s8_conv2d_per_layer_result \output_v, \mac_shift
    tie728_s8_vector_round_result \output_v, \mac_shift, \tmp, \tmp_q1
    # bias will be preload
    # tie728_s8_conv2d_bias \output_v, \tmp_q1, \bias_ptr

    j 16f  # jump to 16f

2:  # per_layer no_bias + prelu, 0x010
    # tie728_s8_conv2d_per_layer_result \output_v, \mac_shift
    tie728_s8_vector_round_result \output_v, \mac_shift, \tmp, \tmp_q1
    tie728_s8_conv2d_prelu \output_v, \tmp_q2, \activation_alpha, \activation_shift

    j 16f  # jump to 16f

1:  # no_bias + relu, 0x001
    bbci \operation_type, 0, 0f
    # tie728_s8_conv2d_per_layer_result \output_v, \mac_shift
    tie728_s8_vector_round_result \output_v, \mac_shift, \tmp, \tmp_q1
    tie728_s8_conv2d_relu \output_v, \activation_alpha, \activation_shift

    j 16f  # jump to 16f

0:  # per_layer no_bias, 0x000
    # tie728_s8_conv2d_per_layer_result \output_v, \mac_shift
    tie728_s8_vector_round_result \output_v, \mac_shift, \tmp, \tmp_q1
16:
.endm
