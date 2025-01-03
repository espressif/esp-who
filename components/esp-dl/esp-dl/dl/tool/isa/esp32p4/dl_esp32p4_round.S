    .text
    .global dl_esp32p4_cfg_round
    .type   dl_esp32p4_cfg_round, @function
    .balign 4
    .option norvc
dl_esp32p4_cfg_round:

    # a0: int8_t round_mode
    # a1:
    # a2:

    # a3:
    # a4:
    # a5:
    # t3:
    # t4:
    # t5:
    # t6:

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    slli a0, a0, 4
    esp.movx.r.cfg a1
    or a1, a1, a0
    esp.movx.w.cfg a1
    ret



    .text
    .global dl_esp32p4_round_half_even
    .type   dl_esp32p4_round_half_even, @function
    .balign 4
    .option norvc
dl_esp32p4_round_half_even:

    # fa0: float value

    # a0: int32_t ret
    # a1:
    # a2:

    # a3:
    # a4:
    # a5:
    # t3:
    # t4:
    # t5:
    # t6:

    # s0:
    # s1:
    # s8:
    # s9:
    # s10:
    # s11:

    fcvt.w.s a0, fa0, rne
    ret
