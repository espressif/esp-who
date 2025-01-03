// #include "dl_base_mul4d.hpp"

// #include "dl_base_activate_output.hpp"
// #include "dl_base_isa.hpp"

// namespace dl {
// namespace base {

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_w1_16_w2_16(feature_t *output_ptr,
//                                    feature_t *input0_ptr,
//                                    feature_t *input1_ptr,
//                                    const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         int iLoop_input0_offset = iLoop * input0_chw;
//         int iLoop_input1_offset = iLoop * input1_chw;
//         int iLoop_output_offset = iLoop * output_chw;

//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//             int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//             int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                 int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                 int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                 feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                 feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                 feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s8_mul4d_bchw_w1_16_w2_16_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #endif
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_w1_16_w2_1(feature_t *output_ptr,
//                                   feature_t *input0_ptr,
//                                   feature_t *input1_ptr,
//                                   const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         int iLoop_input0_offset = iLoop * input0_chw;
//         int iLoop_input1_offset = iLoop * input1_chw;
//         int iLoop_output_offset = iLoop * output_chw;

//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//             int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//             int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                 int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                 int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                 feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                 feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                 feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s8_mul4d_bchw_w1_16_w2_1_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #endif
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_w1_1_w2_16(feature_t *output_ptr,
//                                   feature_t *input0_ptr,
//                                   feature_t *input1_ptr,
//                                   const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         int iLoop_input0_offset = iLoop * input0_chw;
//         int iLoop_input1_offset = iLoop * input1_chw;
//         int iLoop_output_offset = iLoop * output_chw;

//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//             int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//             int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                 int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                 int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                 feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                 feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                 feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s8_mul4d_bchw_w1_1_w2_16_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #endif
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_w1_8_w2_8(feature_t *output_ptr,
//                                  feature_t *input0_ptr,
//                                  feature_t *input1_ptr,
//                                  const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         int iLoop_input0_offset = iLoop * input0_chw;
//         int iLoop_input1_offset = iLoop * input1_chw;
//         int iLoop_output_offset = iLoop * output_chw;

//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//             int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//             int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                 int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                 int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                 feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                 feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                 feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s16_mul4d_bchw_w1_8_w2_8_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #endif
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_w1_8_w2_1(feature_t *output_ptr,
//                                  feature_t *input0_ptr,
//                                  feature_t *input1_ptr,
//                                  const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         int iLoop_input0_offset = iLoop * input0_chw;
//         int iLoop_input1_offset = iLoop * input1_chw;
//         int iLoop_output_offset = iLoop * output_chw;

//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//             int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//             int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                 int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                 int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                 feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                 feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                 feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s16_mul4d_bchw_w1_8_w2_1_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #endif
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_w1_1_w2_8(feature_t *output_ptr,
//                                  feature_t *input0_ptr,
//                                  feature_t *input1_ptr,
//                                  const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         int iLoop_input0_offset = iLoop * input0_chw;
//         int iLoop_input1_offset = iLoop * input1_chw;
//         int iLoop_output_offset = iLoop * output_chw;

//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//             int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//             int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                 int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                 int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                 feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                 feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                 feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s16_mul4d_bchw_w1_1_w2_8_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s16_mul4d_bchw_w1_1_w2_8_simdmul(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w, args.mul_shift);
// #endif
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_rescale(feature_t *output_ptr,
//                                feature_t *input0_ptr,
//                                feature_t *input1_ptr,
//                                const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9

//     // printf("test mul4d by xzs29\n");
//     index = 0;
//     if (args.output_max_dims == 4) {
//         for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//             int iLoop_input0_offset = iLoop * input0_chw;
//             int iLoop_input1_offset = iLoop * input1_chw;

//             for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                 int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//                 int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;

//                 for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                     int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                     int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;

//                     feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                     feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;

//                     for (int lLoop = 0; lLoop < args.output_w; lLoop++) {
//                         buffer = (buffer_t)input0_ptr_base[lLoop * args.input0_w_same] *
//                             (buffer_t)input1_ptr_base[lLoop * args.input1_w_same];
//                         buffer = DL_RIGHT_SHIFT(buffer, args.mul_shift);
//                         tool::truncate(*(output_ptr++), buffer);
//                         printf("input0=%d , input1=%d, output=%d, shift=%d\n",
//                                input0_ptr_base[lLoop * args.input0_w_same],
//                                input1_ptr_base[lLoop * args.input1_w_same],
//                                buffer,
//                                args.mul_shift);
//                     }
//                 }
//             }
//         }
//     } else if (args.output_max_dims == 3) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             int base_index0_j = jLoop * input0_hw;
//             int base_index1_j = jLoop * input1_hw;
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 int base_index0_k = base_index0_j + kLoop * input0_w;
//                 int base_index1_k = base_index1_j + kLoop * input1_w;
//                 for (int lLoop = 0; lLoop < args.output_w; lLoop++) {
//                     // Increment the index directly based on the previous calculation
//                     int index0 = base_index0_k + lLoop * args.input0_w_same;
//                     int index1 = base_index1_k + lLoop * args.input1_w_same;
//                     buffer = (buffer_t)input0_ptr[index0] * (buffer_t)input1_ptr[index1];
//                     buffer = DL_RIGHT_SHIFT(buffer, args.mul_shift);
//                     tool::truncate(*(output_ptr++), buffer);
//                 }
//             }
//         }
//     } else if (args.output_max_dims == 2) {
//         for (int jLoop = 0; jLoop < args.output_h; jLoop++) {
//             int base_index0 = jLoop * input0_w;
//             int base_index1 = jLoop * input1_w;

//             for (int kLoop = 0; kLoop < args.output_w; kLoop++) {
//                 // Incremental index calculation
//                 int index0 = base_index0 + kLoop * args.input0_w_same;
//                 int index1 = base_index1 + kLoop * args.input1_w_same;

//                 // Fetch values and compute buffer
//                 buffer = (buffer_t)input0_ptr[index0] * (buffer_t)input1_ptr[index1];
//                 buffer = DL_RIGHT_SHIFT(buffer, args.mul_shift);

//                 // Truncate the buffer and store in output
//                 tool::truncate(*(output_ptr++), buffer);
//             }
//         }
//     } else if (args.output_max_dims == 1) {
//         for (int jLoop = 0; jLoop < args.output_w; jLoop++) {
//             // index = jLoop;
//             index0 = jLoop * args.input0_w_same;
//             index1 = jLoop * args.input1_w_same;
//             buffer = (buffer_t)input0_ptr[index0] * (buffer_t)input1_ptr[index1];
//             buffer = DL_RIGHT_SHIFT(buffer, args.mul_shift);
//             // tool::truncate(output_ptr[index], buffer);
//             tool::truncate(*(output_ptr++), buffer);
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_rescale_test(feature_t *output_ptr,
//                                     feature_t *input0_ptr,
//                                     feature_t *input1_ptr,
//                                     const arithArgsType<feature_t> &args)
// {
//     void *args_temp = (void *)&args;
//     buffer_t buffer;
//     int ilen = 16 / sizeof(feature_t);
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
//     if (args.output_w < ilen) {
//         if (args.rescale_input >= 2) {
//             feature_t *temp = input0_ptr;
//             input0_ptr = input1_ptr;
//             input1_ptr = temp;
//         }
//         for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//             int iLoop_input0_offset = iLoop * input0_chw;
//             int iLoop_input1_offset = iLoop * input1_chw;

//             for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                 int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//                 int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;

//                 for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                     int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                     int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;

//                     feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                     feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;

//                     for (int lLoop = 0; lLoop < args.output_w; lLoop++) {
//                         buffer = (buffer_t)input0_ptr_base[lLoop * args.input0_w_same] *
//                             (buffer_t)input1_ptr_base[lLoop * args.input1_w_same];
//                         buffer = DL_RIGHT_SHIFT(buffer, args.mul_shift);
//                         tool::truncate(*(output_ptr++), buffer);
//                         // printf("input0=%d , input1=%d, output=%d, shift=%d\n",
//                         //         input0_ptr_base[lLoop * args.input0_w_same],
//                         //         input1_ptr_base[lLoop * args.input1_w_same],
//                         //         buffer,
//                         //         args.mul_shift);
//                     }
//                 }
//             }
//         }
//     } else {
//         if (args.input0_w_same == 1 && args.input1_w_same == 1) {
//             if (args.rescale_input >= 2) {
//                 feature_t *temp = input0_ptr;
//                 input0_ptr = input1_ptr;
//                 input1_ptr = temp;
//             }
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 int iLoop_input0_offset = iLoop * input0_chw;
//                 int iLoop_input1_offset = iLoop * input1_chw;
//                 int iLoop_output_offset = iLoop * output_chw;

//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//                     int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//                     int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                         int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                         int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                         int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                         feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                         feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                         feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
//                         // dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned(output_ptr_base, input0_ptr_base,
//                         // input1_ptr_base, args_temp);
//                         //  for (int pLoop = 0; pLoop < args.output_w; pLoop++)
//                         //  {
//                         //      printf("input0_ptr_base[%d] = %d , input1_ptr_base[%d] = %d , output_ptr_base[%d] =
//                         //      %d\n", pLoop, input0_ptr_base[pLoop * args.input0_w_same], pLoop,
//                         input1_ptr_base[pLoop
//                         //      * args.input1_w_same], pLoop, output_ptr_base[pLoop]);
//                         //  }
// #endif
//                     }
//                 }
//             }
//         } else if (args.input0_w_same == 0) {
//             // printf("args = %d %d %d %d %d\n",args.output_max_dims, args.output_w, args.c_div_x_1,
//             args.c_remainder,
//             // args.mul_shift); printf("ptr = %p %p %p\n",input0_ptr, input1_ptr, output_ptr);
//             // printf("args.input0_w_same = %d, args.input1_w_same = %d\n", args.input0_w_same, args.input1_w_same);
//             // printf("test input0 number0 = %d\n", input0_ptr[0]);
//             // printf("test input0 number10 = %d\n", input0_ptr[10]);

//             if (args.rescale_input >= 2) {
//                 feature_t *temp = input0_ptr;
//                 input0_ptr = input1_ptr;
//                 input1_ptr = temp;
//             }

//             // args.c_div_x_1 = 2;
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 int iLoop_input0_offset = iLoop * input0_chw;
//                 int iLoop_input1_offset = iLoop * input1_chw;
//                 int iLoop_output_offset = iLoop * output_chw;

//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//                     int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//                     int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                         int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                         int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                         int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                         feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                         feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                         feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         // printf("test---------------------------------------\n");
//                         // dl_esp32p4_s8_mul4d_bchw_w1_1_w2_16_simdmul_unaligned(output_ptr_base, input0_ptr_base,
//                         // input1_ptr_base, args_temp);
//                         dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned(
//                             output_ptr_base,
//                             input1_ptr_base,
//                             input0_ptr_base,
//                             args_temp); // mul4d_ishap_1_96_5_49_un3_s8.espdl
//                                         // for (int pLoop = 0; pLoop < args.output_w; pLoop++)
//                                         // {
//                         //     printf("input0_ptr_base[%d] = %d , input1_ptr_base[%d] = %d , output_ptr_base[%d] =
//                         //     %d\n", pLoop, input0_ptr_base[pLoop * args.input0_w_same], pLoop,
//                         input1_ptr_base[pLoop *
//                         //     args.input1_w_same], pLoop, output_ptr_base[pLoop]);
//                         // }

// #endif
//                     }
//                 }
//             }
//         } else {
//             // printf("args = %d %d %d %d %d\n",args.output_max_dims, args.output_w, args.c_div_x_1,
//             args.c_remainder,
//             // args.mul_shift); printf("ptr = %p %p %p\n",input0_ptr, input1_ptr, output_ptr);
//             if (args.rescale_input >= 2) {
//                 feature_t *temp = input0_ptr;
//                 input0_ptr = input1_ptr;
//                 input1_ptr = temp;
//             }

//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 int iLoop_input0_offset = iLoop * input0_chw;
//                 int iLoop_input1_offset = iLoop * input1_chw;
//                 int iLoop_output_offset = iLoop * output_chw;

//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     int jLoop_input0_offset = iLoop_input0_offset + jLoop * input0_hw;
//                     int jLoop_input1_offset = iLoop_input1_offset + jLoop * input1_hw;
//                     int jLoop_output_offset = iLoop_output_offset + jLoop * output_hw;

//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                         int kLoop_input0_offset = jLoop_input0_offset + kLoop * input0_w;
//                         int kLoop_input1_offset = jLoop_input1_offset + kLoop * input1_w;
//                         int kLoop_output_offset = jLoop_output_offset + kLoop * args.output_w;

//                         feature_t *input0_ptr_base = input0_ptr + kLoop_input0_offset;
//                         feature_t *input1_ptr_base = input1_ptr + kLoop_input1_offset;
//                         feature_t *output_ptr_base = output_ptr + kLoop_output_offset;
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         // dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned(output_ptr_base, input0_ptr_base,
//                         // input1_ptr_base, args_temp);
//                         dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
//                         // for (int pLoop = 0; pLoop < args.output_w; pLoop++)
//                         // {
//                         //     printf("input0_ptr_base[%d] = %d , input1_ptr_base[%d] = %d , output_ptr_base[%d] =
//                         //     %d\n", pLoop, input0_ptr_base[pLoop * args.input0_w_same], pLoop,
//                         input1_ptr_base[pLoop *
//                         //     args.input1_w_same], pLoop, output_ptr_base[pLoop]);
//                         // }

// #endif
//                     }
//                 }
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_rescale_int8(feature_t *output_ptr,
//                                     feature_t *input0_ptr,
//                                     feature_t *input1_ptr,
//                                     const arithArgsType<feature_t> &args)
// {
//     void *args_temp = (void *)&args;
//     buffer_t buffer;
//     int ilen = 16 / sizeof(feature_t);
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;
//     if (args.rescale_input >= 2) {
//         feature_t *temp = input0_ptr;
//         input0_ptr = input1_ptr;
//         input1_ptr = temp;
//     }
//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     if (args.output_w < ilen) {
//         for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//             for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                 for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                     feature_t *input0_row_ptr = input0_ptr_base;
//                     feature_t *input1_row_ptr = input1_ptr_base;

//                     for (int lLoop = 0; lLoop < args.output_w; lLoop++) {
//                         buffer_t buffer = (buffer_t)(*input0_row_ptr) * (buffer_t)(*input1_row_ptr);
//                         buffer = DL_RIGHT_SHIFT(buffer, args.mul_shift);
//                         tool::truncate(*output_ptr_base, buffer);

//                         input0_row_ptr += args.input0_w_same;
//                         input1_row_ptr += args.input1_w_same;
//                         output_ptr_base++;
//                     }

//                     input0_ptr_base += input0_w;
//                     input1_ptr_base += input1_w;
//                 }

//                 input0_ptr_base += ichan_stride0;
//                 input1_ptr_base += ichan_stride1;
//             }

//             input0_ptr_base += ibacth_stride0;
//             input1_ptr_base += ibacth_stride1;
//         }
//     } else {
//         if (args.input0_w_same == 1 && args.input1_w_same == 1) {
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s8_mul4d_bchw_w1_16_w2_16_simdmul_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #endif
//                         output_ptr_base += args.output_w;
//                         input0_ptr_base += input0_w;
//                         input1_ptr_base += input1_w;
//                     }
//                     input0_ptr_base += ichan_stride0;
//                     input1_ptr_base += ichan_stride1;
//                 }
//                 input0_ptr_base += ibacth_stride0;
//                 input1_ptr_base += ibacth_stride1;
//             }
//         } else if (args.input0_w_same == 0) {
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned(
//                             output_ptr_base, input1_ptr_base, input0_ptr_base, args_temp);
// #endif
//                         output_ptr_base += args.output_w;
//                         input0_ptr_base += input0_w;
//                         input1_ptr_base += input1_w;
//                     }
//                     input0_ptr_base += ichan_stride0;
//                     input1_ptr_base += ichan_stride1;
//                 }
//                 input0_ptr_base += ibacth_stride0;
//                 input1_ptr_base += ibacth_stride1;
//             }
//         } else {
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s8_mul4d_bchw_w1_16_w2_1_simdmul_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #endif
//                         output_ptr_base += args.output_w;
//                         input0_ptr_base += input0_w;
//                         input1_ptr_base += input1_w;
//                     }
//                     input0_ptr_base += ichan_stride0;
//                     input1_ptr_base += ichan_stride1;
//                 }
//                 input0_ptr_base += ibacth_stride0;
//                 input1_ptr_base += ibacth_stride1;
//             }
//         }
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void mul4d_bchw_rescale_int16(feature_t *output_ptr,
//                                      feature_t *input0_ptr,
//                                      feature_t *input1_ptr,
//                                      const arithArgsType<feature_t> &args)
// {
//     void *args_temp = (void *)&args;
//     buffer_t buffer;
//     int ilen = 16 / sizeof(feature_t);
//     int index = 0;
//     int index0 = 0;
//     int index1 = 0;
//     int output_chw = args.output_c * args.output_h * args.output_w;                      // s10
//     int output_hw = args.output_h * args.output_w;                                       // s11
//     int input0_chw = args.input0_c * args.input0_h * args.input0_w * args.input0_b_same; // a4
//     int input0_hw = args.input0_h * args.input0_w * args.input0_c_same;                  // a5
//     int input0_w = args.input0_w * args.input0_h_same;                                   // a6
//     int input1_chw = args.input1_c * args.input1_h * args.input1_w * args.input1_b_same; // a7
//     int input1_hw = args.input1_h * args.input1_w * args.input1_c_same;                  // s8
//     int input1_w = args.input1_w * args.input1_h_same;
//     if (args.rescale_input >= 2) {
//         feature_t *temp = input0_ptr;
//         input0_ptr = input1_ptr;
//         input1_ptr = temp;
//     }
//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     if (args.output_w < ilen) {
//         for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//             for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                 for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                     feature_t *input0_row_ptr = input0_ptr_base;
//                     feature_t *input1_row_ptr = input1_ptr_base;

//                     for (int lLoop = 0; lLoop < args.output_w; lLoop++) {
//                         buffer_t buffer = (buffer_t)(*input0_row_ptr) * (buffer_t)(*input1_row_ptr);
//                         buffer = DL_RIGHT_SHIFT(buffer, args.mul_shift);
//                         tool::truncate(*output_ptr_base, buffer);

//                         input0_row_ptr += args.input0_w_same;
//                         input1_row_ptr += args.input1_w_same;
//                         output_ptr_base++;
//                     }

//                     input0_ptr_base += input0_w;
//                     input1_ptr_base += input1_w;
//                 }

//                 input0_ptr_base += ichan_stride0;
//                 input1_ptr_base += ichan_stride1;
//             }

//             input0_ptr_base += ibacth_stride0;
//             input1_ptr_base += ibacth_stride1;
//         }
//     } else {
//         if (args.input0_w_same == 1 && args.input1_w_same == 1) {
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s16_mul4d_bchw_w1_8_w2_8_simdmul_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #endif
//                         output_ptr_base += args.output_w;
//                         input0_ptr_base += input0_w;
//                         input1_ptr_base += input1_w;
//                     }
//                     input0_ptr_base += ichan_stride0;
//                     input1_ptr_base += ichan_stride1;
//                 }
//                 input0_ptr_base += ibacth_stride0;
//                 input1_ptr_base += ibacth_stride1;
//             }
//         } else if (args.input0_w_same == 0) {
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned(
//                             output_ptr_base, input1_ptr_base, input0_ptr_base, args_temp);
// #endif
//                         output_ptr_base += args.output_w;
//                         input0_ptr_base += input0_w;
//                         input1_ptr_base += input1_w;
//                     }
//                     input0_ptr_base += ichan_stride0;
//                     input1_ptr_base += ichan_stride1;
//                 }
//                 input0_ptr_base += ibacth_stride0;
//                 input1_ptr_base += ibacth_stride1;
//             }
//         } else {
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s16_mul4d_bchw_w1_8_w2_1_simdmul_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #endif
//                         output_ptr_base += args.output_w;
//                         input0_ptr_base += input0_w;
//                         input1_ptr_base += input1_w;
//                     }
//                     input0_ptr_base += ichan_stride0;
//                     input1_ptr_base += ichan_stride1;
//                 }
//                 input0_ptr_base += ibacth_stride0;
//                 input1_ptr_base += ibacth_stride1;
//             }
//         }
//     }
// }
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // specialize mul4d<int16_t>
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inline void load_mul4d_11c_s16(arith_i_impl_func_s16_t &i_impl_func,
//                                arith_c_impl_func_s16_t &c_impl_func,
//                                arith_n_wise_tail_s16_t &n_wise_tail,
//                                const arithArgsType<int16_t> &args)
// {
// #if defined(CONFIG_IDF_TARGET_ESP32P4) || defined(CONFIG_IDF_TARGET_ESP32S3)
//     if (args.input0_w % 8 == 0 && args.input1_w % 8 == 0 && args.input0_w_same == 1 && args.input1_w_same == 1) {
//         c_impl_func = mul4d_bchw_w1_8_w2_8<int16_t, int32_t>;
//     } else if (args.input0_w % 8 == 0 && args.input1_w == 1) {
//         c_impl_func = mul4d_bchw_w1_8_w2_1<int16_t, int32_t>;
//     } else if (args.input0_w == 1 && args.input1_w % 8 == 0) {
//         c_impl_func = mul4d_bchw_w1_1_w2_8<int16_t, int32_t>;
//     } else {
//         c_impl_func = mul4d_bchw_rescale_int16<int16_t, int32_t>;
//     }
// #else
//     c_impl_func = mul4d_bchw_rescale_int16<int16_t, int32_t>;
// #endif
// }

// template <>
// void mul4d<int16_t>(void *const args_ptr)
// {
//     const arithArgsType<int16_t> &args = *((arithArgsType<int16_t> *)args_ptr);

//     arith_i_impl_func_s16_t i_impl_func = NULL;
//     arith_c_impl_func_s16_t c_impl_func = NULL;
//     arith_n_wise_tail_s16_t n_wise_tail = NULL;

// #if CONFIG_ESP32P4_BOOST
//     dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
// #endif

//     load_mul4d_11c_s16(i_impl_func, c_impl_func, n_wise_tail, args);

//     arith_operation_shell_<int16_t>(args, i_impl_func, c_impl_func, n_wise_tail);
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // specialize mul4d<int8_t>
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inline void load_mul4d_11c_s8(arith_i_impl_func_s8_t &i_impl_func,
//                               arith_c_impl_func_s8_t &c_impl_func,
//                               arith_n_wise_tail_s8_t &n_wise_tail,
//                               const arithArgsType<int8_t> &args)
// {
// #if defined(CONFIG_IDF_TARGET_ESP32P4) || defined(CONFIG_IDF_TARGET_ESP32S3)
//     if (args.input0_w % 16 == 0 && args.input1_w % 16 == 0 && args.input0_w_same == 1 && args.input1_w_same == 1) {
//         c_impl_func = mul4d_bchw_w1_16_w2_16<int8_t, int16_t>;
//     } else if (args.input0_w % 16 == 0 && args.input1_w == 1) {
//         c_impl_func = mul4d_bchw_w1_16_w2_1<int8_t, int16_t>;
//     } else if (args.input0_w == 1 && args.input1_w % 16 == 0) {
//         c_impl_func = mul4d_bchw_w1_1_w2_16<int8_t, int16_t>;
//     } else {
//         // c_impl_func = mul4d_bchw_rescale<int8_t, int16_t>;
//         c_impl_func = mul4d_bchw_rescale_int8<int8_t, int16_t>;
//     }
// #else
//     // c_impl_func = mul4d_bchw_rescale<int8_t, int16_t>;
//     c_impl_func = mul4d_bchw_rescale_int8<int8_t, int16_t>;
// #endif
// }

// template <>
// void mul4d<int8_t>(void *const args_ptr)
// {
//     const arithArgsType<int8_t> &args = *((arithArgsType<int8_t> *)args_ptr);

//     arith_i_impl_func_s8_t i_impl_func = NULL;
//     arith_c_impl_func_s8_t c_impl_func = NULL;
//     arith_n_wise_tail_s8_t n_wise_tail = NULL;

// #if CONFIG_ESP32P4_BOOST
//     dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
// #endif
//     load_mul4d_11c_s8(i_impl_func, c_impl_func, n_wise_tail, args);

//     arith_operation_shell_<int8_t>(args, i_impl_func, c_impl_func, n_wise_tail);
// }
// } // namespace base
// } // namespace dl
