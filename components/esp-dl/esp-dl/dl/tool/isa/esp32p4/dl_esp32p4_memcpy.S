
    .text
    .align 2
    .global dl_esp32p4_memcpy
    .type   dl_esp32p4_memcpy, @function
    .balign 4
    .option norvc
dl_esp32p4_memcpy:

    # a0: void *store_ptr
    # a1: const void *load_ptr
    # a2: const int length(bytes)

    # a3: length // 16
    # a4: remainder
    # a5: length // 32
    # t3: odd_flag
    # t4: store_ptr sar_bytes / remainder_4b
    # t5: remainder_1b
    # t6: load_ptr sar_bytes

    # a6(not for extension instructions): head unaligned bytes 2
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

    esp.ld.128.usar.ip q0, a1, 0
    esp.movx.r.sar.bytes t6

    esp.ld.128.usar.ip q1, a0, 0
    esp.movx.r.sar.bytes t4
    li a6, 16
    sub a6, a6, t4      // head unaligned bytes 2

    li t0, 16
    beq a6, t0, 13f
    blt a6, a2, dl_esp32p4_memcpy_done_min
    mv a6, a2
dl_esp32p4_memcpy_done_min:
    srli t6, a6, 2
    slli a7, t6, 2
    sub a7, a6, a7

    mv t0, t6
    blez t0, 10f
9:
    lw a3, 0(a1)
    addi a1, a1, 4
    sw a3, 0(a0)
    addi a0, a0, 4
    addi t0, t0, -1
    bgtz t0, 9b
10:
    mv t0, a7
    blez t0, 12f
11:
    lbu a3, 0(a1)
    addi a1, a1, 1
    sb a3, 0(a0)
    addi a0, a0, 1
    addi t0, t0, -1
    bgtz t0, 11b
12:
    sub a2, a2, a6
    esp.ld.128.usar.ip q0, a1, 0
    esp.movx.r.sar.bytes t6
13:
    beqz t6, 1f
    srli a3, a2, 4      // len // 16
    slli a4, a3, 4
    sub a4, a2, a4      // remainder

    srli a5, a3, 1      // len // 32
    slli t3, a5, 1
    sub t3, a3, t3      // odd_flag

    srli t4, a4, 2      //remainder_4b
    slli t5, t4, 2
    sub t5, a4, t5      //remainder_1b

    mv t0, a5
    blez t0, 15f
14:
    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q1, a1, 16
    esp.ld.128.usar.ip q2, a1, 0
    esp.src.q q0, q0, q1
    esp.src.q q1, q1, q2
    esp.vst.128.ip q0, a0, 16
    esp.vst.128.ip q1, a0, 16
    addi t0, t0, -1
    bgtz t0, 14b

15:
    beqz t3, 4f
    esp.ld.128.usar.ip q0, a1, 16
    esp.ld.128.usar.ip q1, a1, 0
    esp.src.q q0, q0, q1
    esp.vst.128.ip q0, a0, 16
    bnez t3, 4f

1:
    srli a3, a2, 4      // len // 16
    slli a4, a3, 4
    sub a4, a2, a4      // remainder

    srli a5, a3, 1      // len // 32
    slli t3, a5, 1
    sub t3, a3, t3      // odd_flag

    srli t4, a4, 2      //remainder_4b
    slli t5, t4, 2
    sub t5, a4, t5      //remainder_1b

    mv t0, a5
    blez t0, 3f
2:
    esp.vld.128.ip q0, a1, 16
    esp.vld.128.ip q1, a1, 16
    esp.vst.128.ip q0, a0, 16
    esp.vst.128.ip q1, a0, 16
    addi t0, t0, -1
    bgtz t0, 2b
3:
    beqz t3, 4f
    esp.vld.128.ip q0, a1, 16
    esp.vst.128.ip q0, a0, 16
4:
    mv t0, t4
    blez t0, 6f
5:
    lw a3, 0(a1)
    addi a1, a1, 4
    sw a3, 0(a0)
    addi a0, a0, 4
    addi t0, t0, -1
    bgtz t0, 5b
6:

    mv t0, t5
    blez t0, 8f
7:
    lbu a3, 0(a1)
    addi a1, a1, 1
    sb a3, 0(a0)
    addi a0, a0, 1
    addi t0, t0, -1
    bgtz t0, 7b
8:
    ret
