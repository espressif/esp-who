############################################################################################################################################################
# esp32p4_common series
############################################################################################################################################################
.macro esp32p4_push_4_stacks_1r  value_0
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -4
    sw  \value_0, 0(sp)
.endm



.macro esp32p4_pop_4_stacks_1r  value_0
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 0(sp)
    addi  sp, sp, 4
.endm



.macro esp32p4_push_8_stacks_2r  value_0, value_1
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -8
    sw  \value_0, 4(sp)
    sw  \value_1, 0(sp)
.endm



.macro esp32p4_pop_8_stacks_2r  value_0, value_1
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 4(sp)
    lw  \value_1, 0(sp)
    addi  sp, sp, 8
.endm



.macro esp32p4_push_12_stacks_3r  value_0, value_1, value_2
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -12
    sw  \value_0, 8(sp)
    sw  \value_1, 4(sp)
    sw  \value_2, 0(sp)
.endm



.macro esp32p4_pop_12_stacks_3r  value_0, value_1, value_2
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 8(sp)
    lw  \value_1, 4(sp)
    lw  \value_2, 0(sp)
    addi  sp, sp, 12
.endm



.macro esp32p4_push_16_stacks_4r  value_0, value_1, value_2, value_3
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -16
    sw  \value_0, 12(sp)
    sw  \value_1, 8(sp)
    sw  \value_2, 4(sp)
    sw  \value_3, 0(sp)
.endm



.macro esp32p4_pop_16_stacks_4r  value_0, value_1, value_2, value_3
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 12(sp)
    lw  \value_1, 8(sp)
    lw  \value_2, 4(sp)
    lw  \value_3, 0(sp)
    addi  sp, sp, 16
.endm



.macro esp32p4_push_20_stacks_5r  value_0, value_1, value_2, value_3, value_4
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -20
    sw  \value_0, 16(sp)
    sw  \value_1, 12(sp)
    sw  \value_2, 8(sp)
    sw  \value_3, 4(sp)
    sw  \value_4, 0(sp)
.endm



.macro esp32p4_pop_20_stacks_5r  value_0, value_1, value_2, value_3, value_4
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 16(sp)
    lw  \value_1, 12(sp)
    lw  \value_2, 8(sp)
    lw  \value_3, 4(sp)
    lw  \value_4, 0(sp)
    addi  sp, sp, 20
.endm



.macro esp32p4_push_28_stacks_7r  value_0, value_1, value_2, value_3, value_4, value_5, value_6
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -28
    sw  \value_0, 24(sp)
    sw  \value_1, 20(sp)
    sw  \value_2, 16(sp)
    sw  \value_3, 12(sp)
    sw  \value_4, 8(sp)
    sw  \value_5, 4(sp)
    sw  \value_6, 0(sp)
.endm



.macro esp32p4_pop_28_stacks_7r  value_0, value_1, value_2, value_3, value_4, value_5, value_6
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 24(sp)
    lw  \value_1, 20(sp)
    lw  \value_2, 16(sp)
    lw  \value_3, 12(sp)
    lw  \value_4, 8(sp)
    lw  \value_5, 4(sp)
    lw  \value_6, 0(sp)
    addi  sp, sp, 28
.endm



.macro esp32p4_push_36_stacks_9r  value_0, value_1, value_2, value_3, value_4, value_5, value_6, value_7, value_8
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -36
    sw  \value_0, 32(sp)
    sw  \value_1, 28(sp)
    sw  \value_2, 24(sp)
    sw  \value_3, 20(sp)
    sw  \value_4, 16(sp)
    sw  \value_5, 12(sp)
    sw  \value_6, 8(sp)
    sw  \value_7, 4(sp)
    sw  \value_8, 0(sp)
.endm



.macro esp32p4_pop_36_stacks_9r  value_0, value_1, value_2, value_3, value_4, value_5, value_6, value_7, value_8
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 32(sp)
    lw  \value_1, 28(sp)
    lw  \value_2, 24(sp)
    lw  \value_3, 20(sp)
    lw  \value_4, 16(sp)
    lw  \value_5, 12(sp)
    lw  \value_6, 8(sp)
    lw  \value_7, 4(sp)
    lw  \value_8, 0(sp)
    addi  sp, sp, 36
.endm



.macro esp32p4_push_128_stacks_4r  value_0, value_1, value_2, value_3
    # The order of register arguments for push and pop must be consistent.
    addi  sp, sp, -128
    sw  \value_0, 124(sp)
    sw  \value_1, 120(sp)
    sw  \value_2, 116(sp)
    sw  \value_3, 112(sp)
.endm



.macro esp32p4_pop_128_stacks_4r  value_0, value_1, value_2, value_3
    # The order of register arguments for push and pop must be consistent.
    lw  \value_0, 124(sp)
    lw  \value_1, 120(sp)
    lw  \value_2, 116(sp)
    lw  \value_3, 112(sp)
    addi  sp, sp, 128
.endm



.macro esp32p4_clamp  input, min, max
    // check input and min
    blt  \input, \min, 0f      // if input < min

    // check input and max
    blt  \max, \input, 1f      // if max < input

    // If the input value is already within the range, there is no need for clamping; proceed directly to the end.
    j  2f

    0:
        // If the input value is less than the minimum value, assign the minimum value to the result register.
        mv  \input, \min
        j  2f

    1:
        // If the input value exceeds the maximum value, assign the maximum value to the result register.
        mv  \input, \max

    2:
.endm
