
############################################################################################################################################################
# tie728_32b_aligned_vector series
############################################################################################################################################################
.macro tie728_32b_aligned_vector_store output_v, output_ptr, temp
    EE.MOVI.32.A \output_v, \temp, 0
    s32i \temp, \output_ptr, 0
    EE.MOVI.32.A \output_v, \temp, 1
    s32i \temp, \output_ptr, 4
    EE.MOVI.32.A \output_v, \temp, 2
    s32i \temp, \output_ptr, 8
    EE.MOVI.32.A \output_v, \temp, 3
    s32i \temp, \output_ptr, 12
    addi \output_ptr, \output_ptr, 16
.endm

.macro tie728_s16_32b_aligned_vector_shift_bias_relu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_alpha, activation_shift, temp
    EE.VLD.128.IP                   \bias_v,   \bias_ptr,         16                # load bias
    EE.SRCMB.S16.QACC               \output_v, \mac_shift,         0                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                    \output_v, \output_v,         \bias_v           # bias
    EE.VRELU.S16                    \output_v, \activation_alpha, \activation_shift # LeakyReLU
    tie728_32b_aligned_vector_store \output_v, \output_ptr,       \temp             # store
.endm

.macro tie728_s16_32b_aligned_vector_shift_bias_prelu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_v, activation_alpha_ptr, activation_shift, temp
    EE.VLD.128.IP                   \bias_v,       \bias_ptr,             16                                # load bias
    EE.VLD.128.IP                   \activation_v, \activation_alpha_ptr, 16                                # load PReLU alph
    EE.SRCMB.S16.QACC               \output_v,     \mac_shift,             0                                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                    \output_v,     \output_v,             \bias_v                           # bias
    EE.VPRELU.S16                   \output_v,     \output_v,             \activation_v, \activation_shift  # PReLU
    tie728_32b_aligned_vector_store \output_v,     \output_ptr,           \temp                             # store
.endm

.macro tie728_s16_32b_aligned_vector_shift_bias_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, temp
    EE.VLD.128.IP                   \bias_v,   \bias_ptr,   16      # load bias
    EE.SRCMB.S16.QACC               \output_v, \mac_shift,   0      # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                    \output_v, \output_v,   \bias_v # bias
    tie728_32b_aligned_vector_store \output_v, \output_ptr, \temp   # store
.endm

.macro tie728_s16_32b_aligned_vector_shift_relu_store output_v, output_ptr, mac_shift, activation_alpha, activation_shift, temp
    EE.SRCMB.S16.QACC               \output_v, \mac_shift,        0                 # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VRELU.S16                    \output_v, \activation_alpha, \activation_shift # LeakyReLU
    tie728_32b_aligned_vector_store \output_v, \output_ptr,       \temp             # store
.endm

.macro tie728_s16_32b_aligned_vector_shift_prelu_store output_v, output_ptr, mac_shift, activation_v, activation_alpha_ptr, activation_shift, temp
    EE.VLD.128.IP                   \activation_v, \activation_alpha_ptr, 16                               # load PReLU alph
    EE.SRCMB.S16.QACC               \output_v,     \mac_shift,             0                               # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VPRELU.S16                   \output_v,     \output_v,             \activation_v, \activation_shift # PReLU
    tie728_32b_aligned_vector_store \output_v,     \output_ptr,           \temp                            # store
.endm

.macro tie728_s16_32b_aligned_vector_shift_store output_v, output_ptr, mac_shift, temp
    EE.SRCMB.S16.QACC               \output_v, \mac_shift,      0 # QACC -> QR without round, roud operation is wrong in this instruction
    tie728_32b_aligned_vector_store \output_v, \output_ptr, \temp # store
.endm






############################################################################################################################################################
# tie728_64b_aligned_vector series
############################################################################################################################################################
.macro tie728_64b_aligned_vector_store output_v, output_ptr
    EE.VST.L.64.IP \output_v, \output_ptr, 8
    EE.VST.H.64.IP \output_v, \output_ptr, 8
.endm

.macro tie728_s16_64b_aligned_vector_shift_bias_relu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_alpha, activation_shift
    EE.VLD.128.IP                   \bias_v,   \bias_ptr,         16                # load bias
    EE.SRCMB.S16.QACC               \output_v, \mac_shift,         0                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                    \output_v, \output_v,         \bias_v           # bias
    EE.VRELU.S16                    \output_v, \activation_alpha, \activation_shift # LeakyReLU
    tie728_64b_aligned_vector_store \output_v, \output_ptr                          # store
.endm

.macro tie728_s16_64b_aligned_vector_shift_bias_prelu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_v, activation_alpha_ptr, activation_shift
    EE.VLD.128.IP                   \bias_v,       \bias_ptr,             16                                # load bias
    EE.VLD.128.IP                   \activation_v, \activation_alpha_ptr, 16                                # load PReLU alph
    EE.SRCMB.S16.QACC               \output_v,     \mac_shift,             0                                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                    \output_v,     \output_v,             \bias_v                           # bias
    EE.VPRELU.S16                   \output_v,     \output_v,             \activation_v, \activation_shift  # PReLU
    tie728_64b_aligned_vector_store \output_v,     \output_ptr                                              # store
.endm

.macro tie728_s16_64b_aligned_vector_shift_bias_store output_v, output_ptr, mac_shift, bias_v, bias_ptr
    EE.VLD.128.IP                   \bias_v,   \bias_ptr,  16      # load bias
    EE.SRCMB.S16.QACC               \output_v, \mac_shift,  0      # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                    \output_v, \output_v,  \bias_v # bias
    tie728_64b_aligned_vector_store \output_v, \output_ptr         # store
.endm

.macro tie728_s16_64b_aligned_vector_shift_relu_store output_v, output_ptr, mac_shift, activation_alpha, activation_shift
    EE.SRCMB.S16.QACC               \output_v, \mac_shift,        0                 # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VRELU.S16                    \output_v, \activation_alpha, \activation_shift # LeakyReLU
    tie728_64b_aligned_vector_store \output_v, \output_ptr                          # store
.endm

.macro tie728_s16_64b_aligned_vector_shift_prelu_store output_v, output_ptr, mac_shift, activation_v, activation_alpha_ptr, activation_shift
    EE.VLD.128.IP                   \activation_v, \activation_alpha_ptr, 16                               # load PReLU alph
    EE.SRCMB.S16.QACC               \output_v,     \mac_shift,             0                               # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VPRELU.S16                   \output_v,     \output_v,             \activation_v, \activation_shift # PReLU
    tie728_64b_aligned_vector_store \output_v,     \output_ptr                                             # store
.endm

.macro tie728_s16_64b_aligned_vector_shift_store output_v, output_ptr, mac_shift
    EE.SRCMB.S16.QACC               \output_v, \mac_shift, 0 # QACC -> QR without round, roud operation is wrong in this instruction
    tie728_64b_aligned_vector_store \output_v, \output_ptr   # store
.endm




############################################################################################################################################################
# tie728_128b_aligned_vector series
############################################################################################################################################################
.macro tie728_128b_aligned_vector_store output_v, output_ptr
    EE.VST.128.IP \output_v, \output_ptr, 16
.endm

.macro tie728_s16_128b_aligned_vector_shift_bias_relu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_alpha, activation_shift
    EE.VLD.128.IP                    \bias_v,   \bias_ptr,         16                # load bias
    EE.SRCMB.S16.QACC                \output_v, \mac_shift,         0                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                     \output_v, \output_v,         \bias_v           # bias
    EE.VRELU.S16                     \output_v, \activation_alpha, \activation_shift # LeakyReLU
    tie728_128b_aligned_vector_store \output_v, \output_ptr                          # store
.endm

.macro tie728_s16_128b_aligned_vector_shift_bias_prelu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_v, activation_alpha_ptr, activation_shift
    EE.VLD.128.IP                    \bias_v,       \bias_ptr,             16                                # load bias
    EE.VLD.128.IP                    \activation_v, \activation_alpha_ptr, 16                                # load PReLU alph
    EE.SRCMB.S16.QACC                \output_v,     \mac_shift,             0                                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                     \output_v,     \output_v,             \bias_v                           # bias
    EE.VPRELU.S16                    \output_v,     \output_v,             \activation_v, \activation_shift  # PReLU
    tie728_128b_aligned_vector_store \output_v,     \output_ptr                                              # store
.endm

.macro tie728_s16_128b_aligned_vector_shift_bias_store output_v, output_ptr, mac_shift, bias_v, bias_ptr
    EE.VLD.128.IP                    \bias_v,   \bias_ptr,  16      # load bias
    EE.SRCMB.S16.QACC                \output_v, \mac_shift,  0      # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                     \output_v, \output_v,  \bias_v # bias
    tie728_128b_aligned_vector_store \output_v, \output_ptr         # store
.endm

.macro tie728_s16_128b_aligned_vector_shift_relu_store output_v, output_ptr, mac_shift, activation_alpha, activation_shift
    EE.SRCMB.S16.QACC                \output_v, \mac_shift,        0                 # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VRELU.S16                     \output_v, \activation_alpha, \activation_shift # LeakyReLU
    tie728_128b_aligned_vector_store \output_v, \output_ptr                          # store
.endm

.macro tie728_s16_128b_aligned_vector_shift_prelu_store output_v, output_ptr, mac_shift, activation_v, activation_alpha_ptr, activation_shift
    EE.VLD.128.IP                    \activation_v, \activation_alpha_ptr, 16                               # load PReLU alph
    EE.SRCMB.S16.QACC                \output_v,     \mac_shift,             0                               # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VPRELU.S16                    \output_v,     \output_v,             \activation_v, \activation_shift # PReLU
    tie728_128b_aligned_vector_store \output_v,     \output_ptr                                             # store
.endm

.macro tie728_s16_128b_aligned_vector_shift_store output_v, output_ptr, mac_shift
    EE.SRCMB.S16.QACC                \output_v, \mac_shift, 0 # QACC -> QR without round, roud operation is wrong in this instruction
    tie728_128b_aligned_vector_store \output_v, \output_ptr   # store
.endm

############################################################################################################################################################
# tie728_element series
############################################################################################################################################################
.macro tie728_s16_element_store output_ptr, output
    clamps \output,     \output,     15
    s16i   \output,     \output_ptr,  0
    addi   \output_ptr, \output_ptr,  2
.endm

.macro tie728_s16_element_bias output, bias_ptr, bias
    l16si \bias,     \bias_ptr,     0
    addi  \bias_ptr, \bias_ptr,     2
    add   \output,   \output,   \bias
.endm

.macro tie728_s16_element_relu output
    bgez \output, 0f
    movi \output, 0
0:
.endm

.macro tie728_s16_element_leakyrelu output, alpha
    bgez \output, 0f
    mull \output, \output, \alpha
    sra  \output, \output
0:
.endm

.macro tie728_s16_element_prelu output, alpha_ptr, alpha
    l16si \alpha,     \alpha_ptr, 0
    addi  \alpha_ptr, \alpha_ptr, 2

    bgez  \output, 0f
    mull  \output, \output, \alpha
    sra   \output, \output
0:
.endm

.macro tie728_s16_element_shift_bias_relu_store output_ptr, mac_shift, bias_ptr, temp1, temp2
    EE.SRS.ACCX              \temp1,      \mac_shift, 0      # shift
    tie728_s16_element_bias  \temp1,      \bias_ptr,  \temp2 # bias
    tie728_s16_element_relu  \temp1                          # relu
    tie728_s16_element_store \output_ptr, \temp1             # store
.endm

# DONNOT forget to set ssr before call this macro
.macro tie728_s16_element_shift_bias_leakyrelu_store output_ptr, mac_shift, bias_ptr, activation_alpha, temp1, temp2
    EE.SRS.ACCX                  \temp1,      \mac_shift,        0                 # shift
    tie728_s16_element_bias      \temp1,      \bias_ptr,         \temp2            # bias
    tie728_s16_element_leakyrelu \temp1,      \activation_alpha                    # leakyrelu
    tie728_s16_element_store     \output_ptr, \temp1                               # store
.endm

# DONNOT forget to set ssr before call this macro
.macro tie728_s16_element_shift_bias_prelu_store output_ptr, mac_shift, bias_ptr, activation_alpha_ptr, temp1, temp2
    EE.SRS.ACCX              \temp1,      \mac_shift,            0                         # shift
    tie728_s16_element_bias  \temp1,      \bias_ptr,             \temp2                    # bias
    tie728_s16_element_prelu \temp1,      \activation_alpha_ptr, \temp2                    # prelu
    tie728_s16_element_store \output_ptr, \temp1                                           # store
.endm

.macro tie728_s16_element_shift_bias_store output_ptr, mac_shift, bias_ptr, temp1, temp2
    EE.SRS.ACCX              \temp1,      \mac_shift,            0                         # shift
    tie728_s16_element_bias  \temp1,      \bias_ptr,             \temp2                    # bias
    tie728_s16_element_store \output_ptr, \temp1                                           # store
.endm

.macro tie728_s16_element_shift_relu_store output_ptr, mac_shift, temp1
    EE.SRS.ACCX              \temp1,      \mac_shift, 0 # shift
    tie728_s16_element_relu  \temp1                     # relu
    tie728_s16_element_store \output_ptr, \temp1        # store
.endm

# DONNOT forget to set ssr before call this macro
.macro tie728_s16_element_shift_leakyrelu_store output_ptr, mac_shift, activation_alpha, temp1
    EE.SRS.ACCX                  \temp1,     \mac_shift,        0                 # shift
    tie728_s16_element_leakyrelu \temp1,     \activation_alpha                    # leakyrelu
    tie728_s16_element_store     \output_ptr, \temp1                              # store
.endm

# DONNOT forget to set ssr before call this macro
.macro tie728_s16_element_shift_prelu_store output_ptr, mac_shift, activation_alpha_ptr, temp1, temp2
    EE.SRS.ACCX              \temp1,      \mac_shift,            0                         # shift
    tie728_s16_element_prelu \temp1,      \activation_alpha_ptr, \temp2                    # prelu
    tie728_s16_element_store \output_ptr, \temp1                                           # store
.endm

.macro tie728_s16_element_shift_store output_ptr, mac_shift, temp1
    EE.SRS.ACCX              \temp1,      \mac_shift, 0 # shift
    tie728_s16_element_store \output_ptr, \temp1        # store
.endm


############################################################################################################################################################
# tie728_s16_variable_vector series
############################################################################################################################################################
.macro tie728_s16_variable_vector_store output_ptr, vector, number, temp
    7:
        bbci \number, 2, 3f
        bbci \number, 1, 5f
        bbci \number, 0, 6f

        # number == 0x111
        EE.MOVI.32.A \vector, \temp, 0
        s32i \temp, \output_ptr, 0
        EE.MOVI.32.A \vector, \temp, 1
        s32i \temp, \output_ptr, 4
        EE.MOVI.32.A \vector, \temp, 2
        s32i \temp, \output_ptr, 8
        EE.MOVI.32.A \vector, \temp, 3
        s16i \temp, \output_ptr, 12
        j 0f
    6:
        # number == 0x110
        EE.MOVI.32.A \vector, \temp, 0
        s32i \temp, \output_ptr, 0
        EE.MOVI.32.A \vector, \temp, 1
        s32i \temp, \output_ptr, 4
        EE.MOVI.32.A \vector, \temp, 2
        s32i \temp, \output_ptr, 8
        j 0f
    5:
        # number == 0x10_
        bbci \number, 0, 4f

        # number == 0x101
        EE.MOVI.32.A \vector, \temp, 0
        s32i \temp, \output_ptr, 0
        EE.MOVI.32.A \vector, \temp, 1
        s32i \temp, \output_ptr, 4
        EE.MOVI.32.A \vector, \temp, 2
        s16i \temp, \output_ptr, 8
        j 0f
    4:
        # number == 0x100
        EE.MOVI.32.A \vector, \temp, 0
        s32i \temp, \output_ptr, 0
        EE.MOVI.32.A \vector, \temp, 1
        s32i \temp, \output_ptr, 4
        j 0f
    3:
        # number == 0x0__
        bbci \number, 1, 1f
        bbci \number, 0, 2f

        # number == 0x011
        EE.MOVI.32.A \vector, \temp, 0
        s32i \temp, \output_ptr, 0
        EE.MOVI.32.A \vector, \temp, 1
        s16i \temp, \output_ptr, 4
        j 0f
    2:
        # number == 0x010
        EE.MOVI.32.A \vector, \temp, 0
        s32i \temp, \output_ptr, 0
        j 0f
    1:
        # number == 0x001
        EE.MOVI.32.A \vector, \temp, 0
        s16i \temp, \output_ptr, 0
    0:
.endm

.macro tie728_s16_variable_vector_shift_bias_relu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_alpha, activation_shift, number, temp
    EE.VLD.128.IP                    \bias_v,     \bias_ptr,         0                          # load bias
    EE.SRCMB.S16.QACC                \output_v,   \mac_shift,        0                          # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                     \output_v,   \output_v,         \bias_v                    # bias
    EE.VRELU.S16                     \output_v,   \activation_alpha, \activation_shift          # LeakyReLU
    tie728_s16_variable_vector_store \output_ptr, \output_v,         \number,          \temp    # store

.endm

.macro tie728_s16_variable_vector_shift_bias_prelu_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, activation_v, activation_alpha_ptr, activation_shift, number, temp
    EE.VLD.128.IP                    \bias_v,       \bias_ptr,             0                                # load bias
    EE.VLD.128.IP                    \activation_v, \activation_alpha_ptr, 0                                # load PReLU alph
    EE.SRCMB.S16.QACC                \output_v,     \mac_shift,            0                                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                     \output_v,     \output_v,             \bias_v                          # bias
    EE.VPRELU.S16                    \output_v,     \output_v,             \activation_v, \activation_shift # PReLU
    tie728_s16_variable_vector_store \output_ptr,   \output_v,             \number,       \temp             # store
.endm

.macro tie728_s16_variable_vector_shift_bias_store output_v, output_ptr, mac_shift, bias_v, bias_ptr, number, temp
    EE.VLD.128.IP                    \bias_v,     \bias_ptr,  0                 # load bias
    EE.SRCMB.S16.QACC                \output_v,   \mac_shift, 0                 # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VADDS.S16                     \output_v,   \output_v,  \bias_v           # bias
    tie728_s16_variable_vector_store \output_ptr, \output_v,  \number, \temp    # store
.endm

.macro tie728_s16_variable_vector_shift_relu_store output_v, output_ptr, mac_shift, activation_alpha, activation_shift, number, temp
    EE.SRCMB.S16.QACC                \output_v,   \mac_shift,        0                          # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VRELU.S16                     \output_v,   \activation_alpha, \activation_shift          # LeakyReLU
    tie728_s16_variable_vector_store \output_ptr, \output_v,         \number,          \temp    # store
.endm

.macro tie728_s16_variable_vector_shift_prelu_store output_v, output_ptr, mac_shift, activation_v, activation_alpha_ptr, activation_shift, number, temp
    EE.VLD.128.IP                    \activation_v, \activation_alpha_ptr, 0                                # load PReLU alph
    EE.SRCMB.S16.QACC                \output_v,     \mac_shift,            0                                # QACC -> QR without round, roud operation is wrong in this instruction
    EE.VPRELU.S16                    \output_v,     \output_v,             \activation_v, \activation_shift # PReLU
    tie728_s16_variable_vector_store \output_ptr,   \output_v,             \number,       \temp             # store
.endm

.macro tie728_s16_variable_vector_shift_store output_v, output_ptr, mac_shift, number, temp
    EE.SRCMB.S16.QACC                \output_v,   \mac_shift, 0                 # QACC -> QR without round, roud operation is wrong in this instruction
    tie728_s16_variable_vector_store \output_ptr, \output_v,  \number, \temp    # store
.endm
