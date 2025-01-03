    .align 4
    .text
    .global dl_tie728_bzero_128b
    .type	dl_tie728_bzero_128b, @function
    .section .iram1
dl_tie728_bzero_128b:
    .align      4
    entry       sp,     32

    # a2: ptr
    # a3: n

    srli a3, a3, 4  # a3: n // 16

    EE.ZERO.Q q0
    loopgtz a3, 1f
    EE.VST.128.IP q0, a2, 16
1:
    retw


    .align 4
    .text
    .global dl_tie728_bzero
    .type	dl_tie728_bzero, @function
    .section .iram1
dl_tie728_bzero:
    .align      4
    entry       sp,     32

    # a2: ptr
    # a3: n(bytes)

    movi a10, 0
    EE.LD.128.USAR.IP q1, a2, 0
    RUR.SAR_BYTE a8
    beqz a8, 1f
    movi a9, 16
    sub a9, a9, a8  # head unaligned bytes

    min a9, a9, a3
    loopgtz a9, 0f
    s8i a10, a2, 0
    addi a2, a2, 1

0:
    sub a3, a3, a9
    blti a3, 1, 4f
1:
    srli a4, a3, 4  # n // 16
    slli a5, a4, 4
    sub a5, a3, a5  # remainder

    srli a6, a5, 2  #remainder_4b
    slli a7, a6, 2
    sub a7, a5, a7 #remainder_1b


    EE.ZERO.Q q0
    loopgtz a4, 2f
    EE.VST.128.IP q0, a2, 16
2:
    loopgtz a6, 3f
    s32i a10, a2, 0
    addi a2, a2, 4
3:
    loopgtz a7, 4f
    s8i a10, a2, 0
    addi a2, a2, 1
4:
    retw
