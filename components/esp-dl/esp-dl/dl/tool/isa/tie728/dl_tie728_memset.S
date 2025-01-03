    .align 4
    .text
    .global dl_tie728_memset_8b
    .type	dl_tie728_memset_8b, @function
    .section .iram1
dl_tie728_memset_8b:
    .align      4
    entry       sp,     32
    # a2: input_ptr
    # a3: value
    # a4: length(n)

    s8i a3, a1, 0   // store to stack
    mov a7, a1
    EE.VLDBC.8 q0, a7

    EE.LD.128.USAR.IP q1, a2, 0
    RUR.SAR_BYTE a10
    beqz a10, 2f
    movi a11, 16
    sub a11, a11, a10  # head unaligned bytes

    min a11, a11, a4
    loopgtz a11, 1f
    s8i a3, a2, 0
    addi a2, a2, 1

1:
    sub a4, a4, a11
    blti a4, 1, 4f

2:
    srli a5, a4, 4  # len // 16
    slli a6, a5, 4
    sub a6, a4, a6  # remainder

    loopgtz a5, 3f
    EE.VST.128.IP q0, a2, 16
3:
    loopgtz a6, 4f
    s8i a3, a2, 0
    addi a2, a2, 1
4:
    retw



    .align 4
    .text
    .global dl_tie728_memset_16b
    .type	dl_tie728_memset_16b, @function
    .section .iram1
dl_tie728_memset_16b:
    .align      4
    entry       sp,     32
    # a2: input_ptr
    # a3: value_ptr
    # a4: length(n)

    s16i a3, a1, 0   // store to stack
    mov a7, a1
    EE.VLDBC.16 q0, a7

    EE.LD.128.USAR.IP q1, a2, 0
    RUR.SAR_BYTE a10
    beqz a10, 2f
    movi a11, 16
    sub a11, a11, a10  # head unaligned bytes

    movi a8, 2
    rems a9, a11, a8
    beqz a9, 0f
    loopgtz a4, 5f
    s16i a3, a2, 0
    addi a2, a2, 2
5:
    retw

0:
    srli a11, a11, 1
    min a11, a11, a4
    loopgtz a11, 1f
    s16i a3, a2, 0
    addi a2, a2, 2

1:
    sub a4, a4, a11
    blti a4, 1, 4f

2:
    srli a5, a4, 3  # len // 8
    slli a6, a5, 3
    sub a6, a4, a6  # remainder

    loopgtz a5, 3f
    EE.VST.128.IP q0, a2, 16
3:
    loopgtz a6, 4f
    s16i a3, a2, 0
    addi a2, a2, 2
4:
    retw


    .align 4
    .text
    .global dl_tie728_memset_32b
    .type	dl_tie728_memset_32b, @function
    .section .iram1
dl_tie728_memset_32b:
    .align      4
    entry       sp,     32
    # a2: input_ptr
    # a3: value_ptr
    # a4: length(n)

    s32i a3, a1, 0   // store to stack
    mov a7, a1
    EE.VLDBC.32 q0, a7

    EE.LD.128.USAR.IP q1, a2, 0
    RUR.SAR_BYTE a10
    beqz a10, 2f
    movi a11, 16
    sub a11, a11, a10  # head unaligned bytes

    movi a8, 4
    rems a9, a11, a8
    beqz a9, 0f
    loopgtz a4, 5f
    s32i a3, a2, 0
    addi a2, a2, 4
5:
    retw
0:
    srli a11, a11, 2
    min a11, a11, a4
    loopgtz a11, 1f
    s32i a3, a2, 0
    addi a2, a2, 4

1:
    sub a4, a4, a11
    blti a4, 1, 4f

2:
    srli a5, a4, 2  # len // 4
    slli a6, a5, 2
    sub a6, a4, a6  # remainder

    loopgtz a5, 3f
    EE.VST.128.IP q0, a2, 16
3:
    loopgtz a6, 4f
    s32i a3, a2, 0
    addi a2, a2, 4
4:
    retw
