#include "dl_esp32p4_s8.S"
#include "dl_esp32p4_common.S"



############################################################################################################################################################
####
#### esp32p4_s8_unaligned_conv2d_11cn series
####
############################################################################################################################################################
.macro esp32p4_s8_unaligned_conv2d_11c16  input_v0, input_front_aligned, input_back_aligned, input_ptr, filter_v0, filter_v1, filter_ptr, c_div_x_1, remainder_c, tmp
    # scalar * vecter and accumulate into qacc
    # input_ptr += (c_div_x_1 + 1) * 16 in the end
    # filter_ptr point to the next 16 bytes in the end

    # input_v0:     16 input elements
    # filter_v0:    16 filter elements
    # filter_v1:    16 filter elements
    # input_ptr:    input_ptr
    # filter_ptr:   filter_ptr
    # c_div_x_1:    input_channel // 16 - 1

    bltz \c_div_x_1, 17f    # input_channel < 16

    esp.ld.128.usar.ip \input_front_aligned, \input_ptr, 16


    esp.vld.128.ip \filter_v0, \filter_ptr, 16
    esp.vld.128.ip \filter_v1, \filter_ptr, 16
    esp.ld.128.usar.ip \input_back_aligned, \input_ptr, 16

    beqz  \c_div_x_1, 19f
    # lp.setup  0, \c_div_x_1, 18f
    esp.lp.setup  0, \c_div_x_1, 18f
        esp.src.q.qup  \input_v0, \input_front_aligned, \input_back_aligned
        esp.ld.128.usar.ip  \input_back_aligned, \input_ptr, 16

        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
        esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
        esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 14
    18: esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 15

    19:
    esp.src.q.qup \input_v0, \input_front_aligned, \input_back_aligned
    addi \input_ptr, \input_ptr, -16
    add \input_ptr, \input_ptr, \remainder_c    #input_ptr and the end of c


    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 13
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 14
    esp.vsmulas.s8.qacc \filter_v1, \input_v0, 15


    beqz \remainder_c, 16f      #no c_remainder

    esp.vld.128.ip \filter_v0, \filter_ptr, 16

    esp.vld.128.ip \input_back_aligned, \input_ptr, 0
    esp.src.q \input_v0, \input_front_aligned, \input_back_aligned
    j 15f


17: # input_channel < 16
    esp.vld.128.ip \filter_v0, \filter_ptr,  16

    esp.ld.128.usar.xp \input_front_aligned, \input_ptr, \remainder_c
    esp.vld.128.ip \input_back_aligned, \input_ptr, 0
    esp.src.q \input_v0, \input_front_aligned, \input_back_aligned


15: # remainder_c == 15, 0x1111
    andi \tmp, \remainder_c, 8
    beqz \tmp, 7f

    esp.vld.128.ip \filter_v1, \filter_ptr,  16

    andi \tmp, \remainder_c, 4
    beqz \tmp, 11f
    andi \tmp, \remainder_c, 2
    beqz \tmp, 13f
    andi \tmp, \remainder_c, 1
    beqz \tmp, 14f
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 12
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 13
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 14
    j 16f   # jump to 16f

14:  # remainder_c == 14, 0x1110
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 11
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 12
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 13
    j 16f   # jump to 16f

13:  # remainder_c == 13, 0x1101
    andi \tmp, \remainder_c, 1
    beqz \tmp, 12f
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 10
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 11
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 12
    j 16f   # jump to 16f

12:  # remainder_c == 12, 0x1100
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 9
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 10
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 11
    j 16f   # jump to 16f

11:  # remainder_c == 11, 0x1011
    andi \tmp, \remainder_c, 2
    beqz \tmp, 9f
    andi \tmp, \remainder_c, 1
    beqz \tmp, 10f
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 8
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 9
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 10
    j 16f   # jump to 16f
10:  # remainder_c == 10, 0x1010
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 8
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 9
    j 16f   # jump to 16f
9:  # remainder_c == 9, 0x1001
    andi \tmp, \remainder_c, 1
    beqz \tmp, 8f
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 7
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 8
    j 16f   # jump to 16f
8:  # remainder_c == 8, 0x1000
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc  \filter_v0, \input_v0, 6
    esp.vsmulas.s8.qacc  \filter_v1, \input_v0, 7
    j 16f   # jump to 16f

7: # remainder == 7, 0x111
    andi \tmp, \remainder_c, 4
    beqz \tmp, 3f

    esp.vld.128.ip \filter_v1, \filter_ptr,  16
    andi \tmp, \remainder_c, 2
    beqz \tmp, 5f
    andi \tmp, \remainder_c, 1
    beqz \tmp, 6f
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc \filter_v1, \input_v0, 5
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 6
    j 16f    # jump to 16f

6:  # remainder == 6, 0x110
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 4
    esp.vsmulas.s8.qacc \filter_v1, \input_v0, 5
    j 16f   # jump to 16f

5:  # remainder == 4, 5
    andi \tmp, \remainder_c, 1
    beqz \tmp, 4f
    # remainder == 5, 0x101
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc \filter_v1, \input_v0, 3
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 4
    j 16f   # jump to 16f

4:  # remainder == 4, 0x100
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc.ld.incp \filter_v1, \filter_ptr, \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 2
    esp.vsmulas.s8.qacc \filter_v1, \input_v0, 3
    j 16f   # jump to 16f

3:  # remainder == 1, 2, 3
    andi \tmp, \remainder_c, 2
    beqz \tmp, 1f

    esp.vld.128.ip \filter_v1, \filter_ptr,  16

    andi \tmp, \remainder_c, 1
    beqz \tmp, 2f
    # remainder == 3, 0x011
    esp.vsmulas.s8.qacc.ld.incp \filter_v0, \filter_ptr, \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc \filter_v1, \input_v0, 1
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 2
    j 16f   # jump to 16f

2:  # remainder == 2, 0x010
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 0
    esp.vsmulas.s8.qacc \filter_v1, \input_v0, 1
    j 16f   # jump to 16f

1:  # remainder == 1, 0x001
    esp.vsmulas.s8.qacc \filter_v0, \input_v0, 0

16:
.endm



.macro esp32p4_s8_unaligned_conv2d_11c1  input_v, input_front, input_back, filter_v, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, c_remainder_shift, zero
    # input_v:              16 input elements
    # filter_v:             16 filter elements
    # input_ptr:            input_ptr
    # filter_ptr:           filter_ptr
    # c_div_x_1:            input_channel // 16 - 1
    # c_remainder:          input_channel % 16
    # c_remainder_shift:    15 - c_remainder

    esp.ld.128.usar.ip  \input_front, \input_ptr, 16
    esp.ld.128.usar.ip  \filter_front, \filter_ptr, 16

    bltz  \c_div_x_1, 11f     // input_channel < 16
        esp.ld.128.usar.ip  \input_back, \input_ptr, 16

        beqz  \c_div_x_1, 10f
        # esp.lp.setup  0, \c_div_x_1, 9f
        # Use the zero register as a loop counter, and the value remains zero after the loop is complete.
        mv  \zero, \c_div_x_1
        9:
            esp.src.q.qup  \input_v, \input_front, \input_back

            esp.ld.128.usar.ip  \filter_back, \filter_ptr, 16
            esp.src.q.qup  \filter_v, \filter_front, \filter_back

            esp.ld.128.usar.ip  \input_back, \input_ptr, 16
            esp.vmulas.s8.xacc  \filter_v, \input_v
            addi  \zero, \zero, -1
            bgtz  \zero, 9b

        10:
        // last entire-128b
        esp.src.q.qup  \input_v, \input_front, \input_back

        esp.ld.128.usar.ip  \filter_back, \filter_ptr, 16
        esp.src.q.qup  \filter_v, \filter_front, \filter_back

        esp.vmulas.s8.xacc  \filter_v, \input_v

        beqz  \c_remainder, 12f

    11:
        // c_remainder > 0
        esp.ld.128.usar.xp  \input_back, \input_ptr, \c_remainder
        esp.src.q.qup \input_v, \input_front, \input_back

        esp.ld.128.usar.xp  \filter_back, \filter_ptr, \c_remainder
        esp.src.q.qup  \filter_v, \filter_front, \filter_back

        esp.slcxxp.2q  \input_back,  \input_v, \c_remainder_shift, \zero
        esp.slcxxp.2q  \filter_back, \filter_v, \c_remainder_shift, \zero

        esp.vmulas.s8.xacc  \filter_v, \input_v

    12:
        addi  \input_ptr,  \input_ptr, -16
        addi  \filter_ptr, \filter_ptr, -16
.endm



.macro esp32p4_s8_unaligned_conv2d_11cn_load_args  args, filter_ptr, c_div_x_1, n_div_x, mac_shift, c_remainder
    lw  \filter_ptr,    48(\args)       // filter
    lw  \c_div_x_1,     100(\args)      // input_channel / x - 1
    lw  \n_div_x,       96(\args)       // output_channel / x
    lw  \mac_shift,     64(\args)       // mac_shift
    lw  \c_remainder,   136(\args)      // input_channel % (vector_width / element_width) * sizeof(feature_t)
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_11cn_bias
    .type   dl_esp32p4_s8_unaligned_conv2d_11cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_11cn_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr / n_remainder bias tmp value
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): temp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -128    // clamp min value
    li  s8, 127    // clamp max value
    esp32p4_s8_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4
    lw  t6, 68(a2)       // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3            //  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_11cn_bias_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s8_unaligned_conv2d_11cn_bias_64b
                esp32p4_s8_unaligned_conv2d_11cn_bias_32b_multiple_loop:     // esp32p4_s8_unaligned_conv2d_11cn_bias_32b
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder

            esp32p4_s8_unaligned_conv2d_11cn_bias_64b:
                esp32p4_s8_unaligned_conv2d_11cn_bias_64b_multiple_loop:
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder

        esp32p4_s8_unaligned_conv2d_11cn_bias_128b:
            esp32p4_s8_unaligned_conv2d_11cn_bias_128b_multiple_loop:
                mv  t3, a1     // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder:
        lw t1, 140(a2)       // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       // t4: 15 - c_remainder
            li  t5,  0            // t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder_loop:
                // mv t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1         // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_11cn_bias_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu
    .type   dl_esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr / n_remainder bias tmp value
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): temp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10

    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3            //  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_64b
                esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_32b_multiple_loop:     // esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_32b
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder

            esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_64b:
                esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_64b_multiple_loop:
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder

        esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_128b:
            esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_128b_multiple_loop:
                mv  t3, a1     // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_relu  q0, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder:
        lw t1, 140(a2)       // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       // t4: 15 - c_remainder
            li  t5,  0            // t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_loop:
                // mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1         // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_leakyrelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_11cn_bias_leakyrelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_11cn_bias_prelu
    .type   dl_esp32p4_s8_unaligned_conv2d_11cn_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_11cn_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr / n_remainder bias tmp value
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): temp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10

    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3            //  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_64b
                esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_32b_multiple_loop:     // esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_32b
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder

            esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_64b:
                esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_64b_multiple_loop:
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder

        esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_128b:
            esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_128b_multiple_loop:
                mv  t3, a1     // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder:
        lw t1, 140(a2)       // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       // t4: 15 - c_remainder
            li  t5,  0            // t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder_loop:
                // mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1         // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_prelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_11cn_bias_prelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_11cn
    .type   dl_esp32p4_s8_unaligned_conv2d_11cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_11cn:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): temp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -128    // clamp min value
    li  s8, 127    // clamp max value
    esp32p4_s8_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4

    beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3            //  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_11cn_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s8_unaligned_conv2d_11cn_64b
                esp32p4_s8_unaligned_conv2d_11cn_32b_multiple_loop:     // esp32p4_s8_unaligned_conv2d_11cn_32b
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_n_remainder

            esp32p4_s8_unaligned_conv2d_11cn_64b:
                esp32p4_s8_unaligned_conv2d_11cn_64b_multiple_loop:
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_n_remainder

        esp32p4_s8_unaligned_conv2d_11cn_128b:
            esp32p4_s8_unaligned_conv2d_11cn_128b_multiple_loop:
                mv  t3, a1     // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_11cn_n_remainder:
        lw t1, 140(a2)       // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       // t4: 15 - c_remainder
            li  t5,  0            // t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_11cn_n_remainder_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_11cn_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_11cn_leakyrelu
    .type   dl_esp32p4_s8_unaligned_conv2d_11cn_leakyrelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_11cn_leakyrelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): temp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10

    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4

    beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3            //  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_64b
                esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_32b_multiple_loop:     // esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_32b
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder

            esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_64b:
                esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_64b_multiple_loop:
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder

        esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_128b:
            esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_128b_multiple_loop:
                mv  t3, a1     // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_relu  q0, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder:
        lw t1, 140(a2)       // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       // t4: 15 - c_remainder
            li  t5,  0            // t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_leakyrelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_11cn_leakyrelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_11cn_prelu
    .type   dl_esp32p4_s8_unaligned_conv2d_11cn_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_11cn_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): temp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10

    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_11cn_load_args  a2, a3, t0, t1, a5, a4

    beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3            //  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_11cn_prelu_128b
            li  t2, 8
            beq  t3, t2, esp32p4_s8_unaligned_conv2d_11cn_prelu_64b
                esp32p4_s8_unaligned_conv2d_11cn_prelu_32b_multiple_loop:     // esp32p4_s8_unaligned_conv2d_11cn_prelu_32b
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_prelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder

            esp32p4_s8_unaligned_conv2d_11cn_prelu_64b:
                esp32p4_s8_unaligned_conv2d_11cn_prelu_64b_multiple_loop:
                    mv t3, a1     // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_prelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder

        esp32p4_s8_unaligned_conv2d_11cn_prelu_128b:
            esp32p4_s8_unaligned_conv2d_11cn_prelu_128b_multiple_loop:
                mv  t3, a1     // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_11c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_prelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder:
        lw t1, 140(a2)       // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       // t4: 15 - c_remainder
            li  t5,  0            // t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder_loop:
                mv  t3, a1         // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_11c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_prelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_11cn_prelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret




############################################################################################################################################################
####
#### esp32p4_s8_unaligned_conv2d_33cn series
####
############################################################################################################################################################
.macro esp32p4_s8_unaligned_conv2d_33c16  input_v0, input_front, input_back, input_ptr, filter_v0, filter_v1, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, temp
    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    add  \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
    # add  \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro esp32p4_s8_unaligned_conv2d_33c1  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, c_remainder_shift, zero
    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_y_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    add \input_ptr, \input_ptr, \dilation_x_offset

    esp32p4_s8_unaligned_conv2d_11c1 \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
    # add \input_ptr, \input_ptr, \dilation_y_offset
.endm



.macro esp32p4_s8_unaligned_conv2d_hwcn_load_args  args, filter_ptr, c_div_x_1, n_div_x, mac_shift, c_remainder, dilation_x_offset, dilation_y_offset
    esp32p4_s8_unaligned_conv2d_11cn_load_args  \args, \filter_ptr, \c_div_x_1, \n_div_x, \mac_shift, \c_remainder
    lw  \dilation_x_offset, 108(\args)  // input dilation x offset
    lw  \dilation_y_offset, 112(\args)  // input dilation y offset
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_33cn_bias
    .type   dl_esp32p4_s8_unaligned_conv2d_33cn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_33cn_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr / n_remainder bias tmp value
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)       // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               #  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_33cn_bias_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s8_unaligned_conv2d_33cn_bias_64b
            esp32p4_s8_unaligned_conv2d_33cn_bias_32b:
                esp32p4_s8_unaligned_conv2d_33cn_bias_32b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder

            esp32p4_s8_unaligned_conv2d_33cn_bias_64b:
                esp32p4_s8_unaligned_conv2d_33cn_bias_64b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder

        esp32p4_s8_unaligned_conv2d_33cn_bias_128b:
            esp32p4_s8_unaligned_conv2d_33cn_bias_128b_multiple_loop:
                mv  t3, a1     # t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder_loop:
                // mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1         // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_33cn_bias_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu
    .type   dl_esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10
    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               #  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_64b
            esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_32b:
                esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_32b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder

            esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_64b:
                esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_64b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder

        esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_128b:
            esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_128b_multiple_loop:
                mv  t3, a1     # t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_relu  q0, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_loop:
                // mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1         // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_leakyrelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_33cn_bias_leakyrelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_33cn_bias_prelu
    .type   dl_esp32p4_s8_unaligned_conv2d_33cn_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_33cn_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10
    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)      // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               #  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_64b
            esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_32b:
                esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_32b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder

            esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_64b:
                esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_64b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder

        esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_128b:
            esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_128b_multiple_loop:
                mv  t3, a1     # t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder_loop:
                // mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1         // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_prelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_33cn_bias_prelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_33cn
    .type   dl_esp32p4_s8_unaligned_conv2d_33cn, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_33cn:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_12_stacks_3r  s0, s1, s8    // push stacks
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               #  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_33cn_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s8_unaligned_conv2d_33cn_64b
            esp32p4_s8_unaligned_conv2d_33cn_32b:
                esp32p4_s8_unaligned_conv2d_33cn_32b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_n_remainder

            esp32p4_s8_unaligned_conv2d_33cn_64b:
                esp32p4_s8_unaligned_conv2d_33cn_64b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_n_remainder

        esp32p4_s8_unaligned_conv2d_33cn_128b:
            esp32p4_s8_unaligned_conv2d_33cn_128b_multiple_loop:
                mv  t3, a1     # t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_33cn_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_33cn_n_remainder_loop:
                mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_33cn_n_remainder_end:

    esp32p4_pop_12_stacks_3r  s0, s1, s8     // restore registers
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_33cn_leakyrelu
    .type   dl_esp32p4_s8_unaligned_conv2d_33cn_leakyrelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_33cn_leakyrelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10
    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               #  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_64b
            esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_32b:
                esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_32b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder

            esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_64b:
                esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_64b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder

        esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_128b:
            esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_128b_multiple_loop:
                mv  t3, a1     # t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_relu  q0, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder_loop:
                mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_leakyrelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_33cn_leakyrelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_33cn_prelu
    .type   dl_esp32p4_s8_unaligned_conv2d_33cn_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_33cn_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_20_stacks_5r  s0, s1, s8, s9, s10
    li  s1, -128        // clamp min value
    li  s8, 127         // clamp max value
    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        esp.movx.r.sar.bytes  t3               #  t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_33cn_prelu_128b
            li  a7, 8
            beq  t3, a7, esp32p4_s8_unaligned_conv2d_33cn_prelu_64b
            esp32p4_s8_unaligned_conv2d_33cn_prelu_32b:
                esp32p4_s8_unaligned_conv2d_33cn_prelu_32b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_prelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder

            esp32p4_s8_unaligned_conv2d_33cn_prelu_64b:
                esp32p4_s8_unaligned_conv2d_33cn_prelu_64b_multiple_loop:
                    mv  t3, a1     # t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_prelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder

        esp32p4_s8_unaligned_conv2d_33cn_prelu_128b:
            esp32p4_s8_unaligned_conv2d_33cn_prelu_128b_multiple_loop:
                mv  t3, a1     # t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_33c16  q0, q1, q2, t3, q3, q4, a3, t0, a4, t2, a6, a7
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_prelu_128b_multiple_loop


    esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder:
        lw  t1, 140(a2)       # t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder_end
            li  t4, 15
            sub  t4, t4, a4       # t4: 15 - c_remainder
            li  t5,  0            # t5: activation_shift = zero

            esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder_loop:
                mv  t3, a1         # t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_33c1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, t4, t5
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_prelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_33cn_prelu_n_remainder_end:

    esp32p4_pop_20_stacks_5r  s0, s1, s8, s9, s10
    ret




############################################################################################################################################################
####
#### esp32p4_s8_unaligned_conv2d_hwcn series
####
############################################################################################################################################################
.macro esp32p4_s8_unaligned_conv2d_hwc16  input_v0, input_front, input_back, filter_v0, filter_v1, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, filter_h, filter_w, args, filter_y_offset, filter_n_offset, temp
    lw  \filter_h, 52(\args)        // filter_height
    21:
        lw  \filter_w, 56(\args)    // filter_width
        addi  \filter_w, \filter_w, -1
        beqz  \filter_w, 22f
            20:
                esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp
                add  \input_ptr, \input_ptr, \dilation_x_offset

                addi  \filter_w, \filter_w, -1
                bnez  \filter_w, 20b
        22:
        esp32p4_s8_unaligned_conv2d_11c16  \input_v0, \input_front, \input_back, \input_ptr, \filter_v0, \filter_v1, \filter_ptr, \c_div_x_1, \c_remainder, \temp

        add  \filter_ptr, \filter_ptr, \filter_y_offset
        add  \input_ptr, \input_ptr, \dilation_y_offset

        addi  \filter_h, \filter_h, -1
        bnez  \filter_h, 21b

    add  \filter_ptr, \filter_ptr, \filter_n_offset
.endm



.macro esp32p4_s8_unaligned_conv2d_hwc1  input_v0, input_front, input_back, filter_v0, filter_front, filter_back, input_ptr, filter_ptr, c_div_x_1, c_remainder, dilation_x_offset, dilation_y_offset, filter_h, filter_w, args, c_remainder_shift, zero, filter_y_offset, filter_n_offset
    lw  \filter_h, 52(\args)        // filter_height
    21:
        lw  \filter_w, 56(\args)    // filter_width
        addi  \filter_w, \filter_w, -1
        beqz  \filter_w, 22f
            20:
                esp32p4_s8_unaligned_conv2d_11c1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero
                add  \input_ptr, \input_ptr, \dilation_x_offset

                addi  \filter_w, \filter_w, -1
                bnez  \filter_w, 20b
        22:
        esp32p4_s8_unaligned_conv2d_11c1  \input_v0, \input_front, \input_back, \filter_v0, \filter_front, \filter_back, \input_ptr, \filter_ptr, \c_div_x_1, \c_remainder, \c_remainder_shift, \zero

        add  \filter_ptr, \filter_ptr, \filter_y_offset
        add  \input_ptr, \input_ptr, \dilation_y_offset

        addi  \filter_h, \filter_h, -1
        bnez  \filter_h, 21b

    add  \filter_ptr, \filter_ptr, \filter_n_offset
.endm



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_hwcn_bias
    .type   dl_esp32p4_s8_unaligned_conv2d_hwcn_bias, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_hwcn_bias:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr / n_remainder bias tmp value
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions): tmp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_28_stacks_7r  s2, s3, s4, s5, s0, s1, s8
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)       // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_hwcn_bias_128b
            li  s5, 8
            beq  t3, s5, esp32p4_s8_unaligned_conv2d_hwcn_bias_64b
                // esp32p4_s8_unaligned_conv2d_hwcn_bias_32b:
                esp32p4_s8_unaligned_conv2d_hwcn_bias_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder

            esp32p4_s8_unaligned_conv2d_hwcn_bias_64b:
                esp32p4_s8_unaligned_conv2d_hwcn_bias_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder

        esp32p4_s8_unaligned_conv2d_hwcn_bias_128b:
            esp32p4_s8_unaligned_conv2d_hwcn_bias_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_128b_multiple_loop

    esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder:
        lw  t1, 140(a2)         // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder_end
            lw  s3, 160(a2)     // a3: filter_y_offset_unaligned
            lw  s4, 164(a2)     // t3: filter_n_offset_unaligned
            lw  a3, 168(a2)     // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4     // t4: 15 - c_remainder
            li  t5,  0          // t5: zero

            esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder_loop:
                // mv  t3, a1      // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1      // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_hwcn_bias_n_remainder_end:

    esp32p4_pop_28_stacks_7r  s2, s3, s4, s5, s0, s1, s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu
    .type   dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr / n_remainder bias tmp value
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions): tmp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)       // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_128b
            li  s5, 8
            beq  t3, s5, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_64b
                // esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_32b:
                esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder

            esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_64b:
                esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder

        esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_128b:
            esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_relu  q0, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_128b_multiple_loop

    esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder:
        lw  t1, 140(a2)         // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_end
            lw  s3, 160(a2)     // a3: filter_y_offset_unaligned
            lw  s4, 164(a2)     // t3: filter_n_offset_unaligned
            lw  a3, 168(a2)     // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4     // t4: 15 - c_remainder
            li  t5,  0          // t5: zero

            esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_loop:
                // mv  t3, a1      // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1      // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_leakyrelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_hwcn_bias_leakyrelu_n_remainder_end:

    esp32p4_pop_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu
    .type   dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr / n_remainder bias tmp value
    # t4: c_remainder_shift
    # t5: zero
    # t6: bias_ptr

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions): tmp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6
    lw  t6, 68(a2)       // bias

    beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_128b
            li  s5, 8
            beq  t3, s5, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_64b
                // esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_32b:
                esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder

            esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_64b:
                esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_conv2d_128b_vector_bias  t6
                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder

        esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_128b:
            esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_conv2d_128b_vector_bias  t6
                esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_128b_multiple_loop

    esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder:
        lw  t1, 140(a2)         // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder_end
            lw  s3, 160(a2)     // a3: filter_y_offset_unaligned
            lw  s4, 164(a2)     // t3: filter_n_offset_unaligned
            lw  a3, 168(a2)     // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4     // t4: 15 - c_remainder
            li  t5,  0          // t5: zero

            esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder_loop:
                // mv  t3, a1      // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_conv2d_element_bias  t6, t3      // t3: tmp for bias
                mv  t3, a1      // t3: input_ptr
                esp32p4_s8_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_prelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_hwcn_bias_prelu_n_remainder_end:

    esp32p4_pop_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    ret


    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_hwcn
    .type   dl_esp32p4_s8_unaligned_conv2d_hwcn, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_hwcn:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions): tmp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9:
    # s10:
    # s11:

    esp32p4_push_28_stacks_7r  s2, s3, s4, s5, s0, s1, s8
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_hwcn_128b
            li  s5, 8
            beq  t3, s5, esp32p4_s8_unaligned_conv2d_hwcn_64b
                // esp32p4_s8_unaligned_conv2d_hwcn_32b:
                esp32p4_s8_unaligned_conv2d_hwcn_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_n_remainder

            esp32p4_s8_unaligned_conv2d_hwcn_64b:
                esp32p4_s8_unaligned_conv2d_hwcn_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_n_remainder

        esp32p4_s8_unaligned_conv2d_hwcn_128b:
            esp32p4_s8_unaligned_conv2d_hwcn_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_128b_multiple_loop

    esp32p4_s8_unaligned_conv2d_hwcn_n_remainder:
        lw  t1, 140(a2)         // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_n_remainder_end
            lw  s3, 160(a2)     // a3: filter_y_offset_unaligned
            lw  s4, 164(a2)     // t3: filter_n_offset_unaligned
            lw  a3, 168(a2)     // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4     // t4: 15 - c_remainder
            li  t5,  0          // t5: zero

            esp32p4_s8_unaligned_conv2d_hwcn_n_remainder_loop:
                mv  t3, a1      // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_hwcn_n_remainder_end:

    esp32p4_pop_28_stacks_7r  s2, s3, s4, s5, s0, s1, s8
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu
    .type   dl_esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions): tmp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    lw  s9, 76(a2)      // activation_alpha
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_128b
            li  s5, 8
            beq  t3, s5, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_64b
                // esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_32b:
                esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder

            esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_64b:
                esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_relu  q0, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder

        esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_128b:
            esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_relu  q0, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_128b_multiple_loop

    esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder:
        lw  t1, 140(a2)         // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder_end
            lw  s3, 160(a2)     // a3: filter_y_offset_unaligned
            lw  s4, 164(a2)     // t3: filter_n_offset_unaligned
            lw  a3, 168(a2)     // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4     // t4: 15 - c_remainder
            li  t5,  0          // t5: zero

            esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder_loop:
                mv  t3, a1      // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_leakyrelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_hwcn_leakyrelu_n_remainder_end:

    esp32p4_pop_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    ret



    .text
    .align 2
    .global dl_esp32p4_s8_unaligned_conv2d_hwcn_prelu
    .type   dl_esp32p4_s8_unaligned_conv2d_hwcn_prelu, @function
    .balign 4
    .option norvc
dl_esp32p4_s8_unaligned_conv2d_hwcn_prelu:

    # a0: int8_t *output_ptr
    # a1: int8_t *input_ptr
    # a2: void *args

    # a3: int8_t *filter_ptr
    # a4: c_remainder
    # a5: mac_shift
    # t3: output_sar_byte / moving_input_ptr
    # t4: c_remainder_shift
    # t5: zero
    # t6:

    # a6(not for extension instructions): dilation_y_offset
    # a7(not for extension instructions): filter_h
    # t0(not for extension instructions): c_div_x_1
    # t1(not for extension instructions): n_div_x / n_remainder
    # t2(not for extension instructions): dilation_x_offset
    # s2(not for extension instructions): filter_w
    # s3(not for extension instructions): filter_y_offset
    # s4(not for extension instructions): filter_n_offset
    # s5(not for extension instructions): tmp value

    # s0: output
    # s1: clamp min value
    # s8: clamp max value
    # s9: activation_alpha/_address
    # s10: activation_shift
    # s11:

    esp32p4_push_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    li  s1, -128    // clamp min value
    li  s8, 127     // clamp max value
    lw  s9, 80(a2)      // activation_alpha_ptr
    lw  s10, 84(a2)     // activation_shift
    esp32p4_s8_unaligned_conv2d_hwcn_load_args  a2, a3, t0, t1, a5, a4, t2, a6

    beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder
        esp.ld.128.usar.ip  q0, a0, 0
        lw  s3, 60(a2)              // s3: filter_y_offset
        lw  s4, 144(a2)             // s4: filter_n_offset
        esp.movx.r.sar.bytes  t3    // t3: output_sar_byte

        beqz  t3, esp32p4_s8_unaligned_conv2d_hwcn_prelu_128b
            li  s5, 8
            beq  t3, s5, esp32p4_s8_unaligned_conv2d_hwcn_prelu_64b
                // esp32p4_s8_unaligned_conv2d_hwcn_prelu_32b:
                esp32p4_s8_unaligned_conv2d_hwcn_prelu_32b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_32b_unaligned_vector_store  q0, a0, t3

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_prelu_32b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder

            esp32p4_s8_unaligned_conv2d_hwcn_prelu_64b:
                esp32p4_s8_unaligned_conv2d_hwcn_prelu_64b_multiple_loop:
                    mv  t3, a1      // t3: input_ptr
                    esp.zero.qacc

                    esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                    esp32p4_s8_128b_vector_shift_result  q0, a5
                    esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                    esp32p4_s8_64b_unaligned_vector_store  q0, a0

                    addi  t1, t1, -1
                    bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_prelu_64b_multiple_loop
                j  esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder

        esp32p4_s8_unaligned_conv2d_hwcn_prelu_128b:
            esp32p4_s8_unaligned_conv2d_hwcn_prelu_128b_multiple_loop:
                mv  t3, a1          // t3: input_ptr
                esp.zero.qacc

                esp32p4_s8_unaligned_conv2d_hwc16  q0, q1, q2, q3, q4, t3, a3, t0, a4, t2, a6, a7, s2, a2, s3, s4, s5
                esp32p4_s8_128b_vector_shift_result  q0, a5
                esp32p4_s8_128b_vector_prelu  q0, q1, s9, s10
                esp32p4_s8_128b_aligned_vector_store  q0, a0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_prelu_128b_multiple_loop

    esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder:
        lw  t1, 140(a2)         // t1: n_remainder
        beqz  t1, esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder_end
            lw  s3, 160(a2)     // a3: filter_y_offset_unaligned
            lw  s4, 164(a2)     // t3: filter_n_offset_unaligned
            lw  a3, 168(a2)     // a3: filter_ptr_unaligned

            li  t4, 15
            sub  t4, t4, a4     // t4: 15 - c_remainder
            li  t5,  0          // t5: zero

            esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder_loop:
                mv  t3, a1      // t3: input_ptr
                esp.zero.xacc

                esp32p4_s8_unaligned_conv2d_hwc1  q0, q1, q2, q3, q4, q5, t3, a3, t0, a4, t2, a6, a7, s2, a2, t4, t5, s3, s4
                esp32p4_s8_element_result  s0, a5
                esp32p4_clamp  s0, s1, s8
                esp32p4_s8_element_prelu  s0, s9, s10
                esp32p4_s8_element_store  a0, s0

                addi  t1, t1, -1
                bnez  t1, esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder_loop

        esp32p4_s8_unaligned_conv2d_hwcn_prelu_n_remainder_end:

    esp32p4_pop_36_stacks_9r  s2, s3, s4, s5, s0, s1, s8, s9, s10
    ret
