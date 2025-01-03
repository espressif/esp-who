    .align 4
    .text
    .global dl_xtensa_bzero_32b
    .type	dl_xtensa_bzero_32b, @function
    .section .iram1
dl_xtensa_bzero_32b:
    .align      4
    entry       sp,     32

    # a2: ptr
    # a3: n

    srli a3, a3, 2  # a3: n // 4
    movi a4, 0      # a4: zero

    loopgtz a3, 1f
    s32i a4, a2, 0
    addi a2, a2, 4
1:
    retw
