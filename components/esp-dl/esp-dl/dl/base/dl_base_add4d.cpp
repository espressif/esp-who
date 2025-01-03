// #include "dl_base_add4d.hpp"

// #include "dl_base_activate_output.hpp"
// #include "dl_base_isa.hpp"

// namespace dl {
// namespace base {

// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_w1_16_w2_16(feature_t *output_ptr,
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

//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s8_add4d_bchw_w1_16_w2_16_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #endif
//                 output_ptr_base += args.output_w;
//                 input0_ptr_base += input0_w;
//                 input1_ptr_base += input1_w;
//             }
//             input0_ptr_base += ichan_stride0;
//             input1_ptr_base += ichan_stride1;
//         }
//         input0_ptr_base += ibacth_stride0;
//         input1_ptr_base += ibacth_stride1;
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_w1_16_w2_1(feature_t *output_ptr,
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

//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s8_add4d_bchw_w1_16_w2_1_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #endif
//                 output_ptr_base += args.output_w;

//                 input0_ptr_base += input0_w;
//                 input1_ptr_base += input1_w;
//             }
//             input0_ptr_base += ichan_stride0;
//             input1_ptr_base += ichan_stride1;
//         }
//         input0_ptr_base += ibacth_stride0;
//         input1_ptr_base += ibacth_stride1;
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_w1_1_w2_16(feature_t *output_ptr,
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

//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd(
//                     output_ptr_base, input1_ptr_base, input0_ptr_base, args.output_w);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s8_add4d_bchw_w1_16_w2_1_simdadd(
//                     output_ptr_base, input1_ptr_base, input0_ptr_base, args.output_w);
// #endif
//                 output_ptr_base += args.output_w;
//                 input0_ptr_base += input0_w;
//                 input1_ptr_base += input1_w;
//             }
//             input0_ptr_base += ichan_stride0;
//             input1_ptr_base += ichan_stride1;
//         }
//         input0_ptr_base += ibacth_stride0;
//         input1_ptr_base += ibacth_stride1;
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_w1_8_w2_8(feature_t *output_ptr,
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

//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s16_add4d_bchw_w1_8_w2_8_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s16_add4d_bchw_w1_8_w2_8_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #endif
//                 output_ptr_base += args.output_w;
//                 input0_ptr_base += input0_w;
//                 input1_ptr_base += input1_w;
//             }
//             input0_ptr_base += ichan_stride0;
//             input1_ptr_base += ichan_stride1;
//         }
//         input0_ptr_base += ibacth_stride0;
//         input1_ptr_base += ibacth_stride1;
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_w1_8_w2_1(feature_t *output_ptr,
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

//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s16_add4d_bchw_w1_8_w2_1_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s16_add4d_bchw_w1_8_w2_1_simdadd(
//                     output_ptr_base, input0_ptr_base, input1_ptr_base, args.output_w);
// #endif
//                 output_ptr_base += args.output_w;
//                 input0_ptr_base += input0_w;
//                 input1_ptr_base += input1_w;
//             }
//             input0_ptr_base += ichan_stride0;
//             input1_ptr_base += ichan_stride1;
//         }
//         input0_ptr_base += ibacth_stride0;
//         input1_ptr_base += ibacth_stride1;
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_w1_1_w2_8(feature_t *output_ptr,
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

//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);
//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                 dl_esp32p4_s16_add4d_bchw_w1_8_w2_1_simdadd(
//                     output_ptr_base, input1_ptr_base, input0_ptr_base, args.output_w);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                 dl_esp32s3_s16_add4d_bchw_w1_8_w2_1_simdadd(
//                     output_ptr_base, input1_ptr_base, input0_ptr_base, args.output_w);
// #endif
//                 output_ptr_base += args.output_w;
//                 input0_ptr_base += input0_w;
//                 input1_ptr_base += input1_w;
//             }
//             input0_ptr_base += ichan_stride0;
//             input1_ptr_base += ichan_stride1;
//         }
//         input0_ptr_base += ibacth_stride0;
//         input1_ptr_base += ibacth_stride1;
//     }
// }

// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_rescale(feature_t *output_ptr,
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

//     feature_t *input0_ptr_base = input0_ptr;
//     feature_t *input1_ptr_base = input1_ptr;
//     feature_t *output_ptr_base = output_ptr;
//     int ichan_stride0 = input0_hw - (args.output_h * input0_w);
//     int ichan_stride1 = input1_hw - (args.output_h * input1_w);
//     int ibacth_stride0 = input0_chw - (args.output_c * input0_hw);
//     int ibacth_stride1 = input1_chw - (args.output_c * input1_hw);

//     for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//         for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//             for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
//                 feature_t *input0_row_ptr = input0_ptr_base;
//                 feature_t *input1_row_ptr = input1_ptr_base;

//                 for (int lLoop = 0; lLoop < args.output_w; lLoop++) {
//                     buffer_t buffer = (buffer_t)(*input0_row_ptr) + (buffer_t)(*input1_row_ptr);
//                     tool::truncate(*output_ptr_base, buffer);

//                     input0_row_ptr += args.input0_w_same;
//                     input1_row_ptr += args.input1_w_same;
//                     output_ptr_base++;
//                 }

//                 input0_ptr_base += input0_w;
//                 input1_ptr_base += input1_w;
//             }

//             input0_ptr_base += ichan_stride0;
//             input1_ptr_base += ichan_stride1;
//         }

//         input0_ptr_base += ibacth_stride0;
//         input1_ptr_base += ibacth_stride1;
//     }
// }
// // int8
// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_rescale_int8(feature_t *output_ptr,
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
//                         buffer_t buffer = (buffer_t)(*input0_row_ptr) + (buffer_t)(*input1_row_ptr);
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
//                         dl_esp32p4_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                         dl_esp32s3_s8_add4d_bchw_w1_16_w2_16_simdadd_unaligned(
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
//                         dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned(
//                             output_ptr_base, input1_ptr_base, input0_ptr_base, args_temp);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                         dl_esp32s3_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned(
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
//                         dl_esp32p4_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                         dl_esp32s3_s8_add4d_bchw_w1_16_w2_1_simdadd_unaligned(
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

// // int16
// // int8
// template <typename feature_t, typename buffer_t>
// inline void add4d_bchw_rescale_int16(feature_t *output_ptr,
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
//     int input1_w = args.input1_w * args.input1_h_same;                                   // s9
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
//                         buffer_t buffer = (buffer_t)(*input0_row_ptr) + (buffer_t)(*input1_row_ptr);
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
//                         dl_esp32p4_s16_add4d_bchw_w1_8_w2_8_simdadd_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                         dl_esp32s3_s16_add4d_bchw_w1_8_w2_8_simdadd_unaligned(
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
//                         dl_esp32p4_s16_add4d_bchw_w1_8_w2_1_simdadd_unaligned(
//                             output_ptr_base, input1_ptr_base, input0_ptr_base, args_temp);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                         dl_esp32s3_s16_add4d_bchw_w1_8_w2_1_simdadd_unaligned(
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
//         } else { //
//             for (int iLoop = 0; iLoop < args.output_b; iLoop++) {
//                 for (int jLoop = 0; jLoop < args.output_c; jLoop++) {
//                     for (int kLoop = 0; kLoop < args.output_h; kLoop++) {
// #ifdef CONFIG_IDF_TARGET_ESP32P4
//                         dl_esp32p4_s16_add4d_bchw_w1_8_w2_1_simdadd_unaligned(
//                             output_ptr_base, input0_ptr_base, input1_ptr_base, args_temp);
// #elif defined CONFIG_IDF_TARGET_ESP32S3
//                         dl_esp32s3_s16_add4d_bchw_w1_8_w2_1_simdadd_unaligned(
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
// inline void add4d_11c_rescale(feature_t *output_ptr,
//                               feature_t *input0_ptr,
//                               feature_t *input1_ptr,
//                               const arithArgsType<feature_t> &args)
// {
//     buffer_t buffer;
//     for (size_t output_c = 0; output_c < args.channel; output_c++) // C
//     {
//         buffer = (buffer_t)input0_ptr[output_c] + (buffer_t)(DL_RIGHT_SHIFT(input1_ptr[output_c], args.input_shift));
//         buffer = DL_RIGHT_SHIFT(buffer * args.output_scale, args.output_shift);
//         tool::truncate(output_ptr[output_c], buffer);
//     }
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // specialize add4d<int16_t>
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inline void load_add4d_11c_s16(arith_i_impl_func_s16_t &i_impl_func,
//                                arith_c_impl_func_s16_t &c_impl_func,
//                                arith_n_wise_tail_s16_t &n_wise_tail,
//                                const arithArgsType<int16_t> &args)
// {
// #if defined(CONFIG_IDF_TARGET_ESP32P4) || defined(CONFIG_IDF_TARGET_ESP32S3)
//     if (args.input0_w % 8 == 0 && args.input1_w % 8 == 0 && args.input0_w_same == 1 && args.input1_w_same == 1) {
//         c_impl_func = add4d_bchw_w1_8_w2_8<int16_t, int32_t>;
//     } else if (args.input0_w % 8 == 0 && args.input1_w == 1) {
//         c_impl_func = add4d_bchw_w1_8_w2_1<int16_t, int32_t>;
//     } else if (args.input0_w == 1 && args.input1_w % 8 == 0) {
//         c_impl_func = add4d_bchw_w1_1_w2_8<int16_t, int32_t>;
//     } else {
//         c_impl_func = add4d_bchw_rescale_int16<int16_t, int32_t>;
//     }
// #else
//     c_impl_func = add4d_bchw_rescale_int16<int16_t, int32_t>;
// #endif
// }

// template <>
// void add4d<int16_t>(void *const args_ptr)
// {
//     const arithArgsType<int16_t> &args = *((arithArgsType<int16_t> *)args_ptr);

//     arith_i_impl_func_s16_t i_impl_func = NULL;
//     arith_c_impl_func_s16_t c_impl_func = NULL;
//     arith_n_wise_tail_s16_t n_wise_tail = NULL;

// #if CONFIG_ESP32P4_BOOST
//     dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
// #endif

//     load_add4d_11c_s16(i_impl_func, c_impl_func, n_wise_tail, args);

//     arith_operation_shell_<int16_t>(args, i_impl_func, c_impl_func, n_wise_tail);
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // specialize add4d<int8_t>
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inline void load_add4d_11c_s8(arith_i_impl_func_s8_t &i_impl_func,
//                               arith_c_impl_func_s8_t &c_impl_func,
//                               arith_n_wise_tail_s8_t &n_wise_tail,
//                               const arithArgsType<int8_t> &args)
// {
// #if defined(CONFIG_IDF_TARGET_ESP32P4) || defined(CONFIG_IDF_TARGET_ESP32S3)
//     if (args.input0_w % 16 == 0 && args.input1_w % 16 == 0 && args.input0_w_same == 1 && args.input1_w_same == 1) {
//         c_impl_func = add4d_bchw_w1_16_w2_16<int8_t, int16_t>;
//     } else if (args.input0_w % 16 == 0 && args.input1_w == 1) {
//         c_impl_func = add4d_bchw_w1_16_w2_1<int8_t, int16_t>;
//     } else if (args.input0_w == 1 && args.input1_w % 16 == 0) {
//         c_impl_func = add4d_bchw_w1_1_w2_16<int8_t, int16_t>;
//     } else {
//         c_impl_func = add4d_bchw_rescale_int8<int8_t, int16_t>;
//     }
// #else
//     c_impl_func = add4d_bchw_rescale_int8<int8_t, int16_t>;
// #endif
// }

// template <>
// void add4d<int8_t>(void *const args_ptr)
// {
//     const arithArgsType<int8_t> &args = *((arithArgsType<int8_t> *)args_ptr);

//     arith_i_impl_func_s8_t i_impl_func = NULL;
//     arith_c_impl_func_s8_t c_impl_func = NULL;
//     arith_n_wise_tail_s8_t n_wise_tail = NULL;

// #if CONFIG_ESP32P4_BOOST
//     dl_esp32p4_cfg_round(ROUND_MODE_HALF_EVEN);
// #endif
//     load_add4d_11c_s8(i_impl_func, c_impl_func, n_wise_tail, args);

//     arith_operation_shell_<int8_t>(args, i_impl_func, c_impl_func, n_wise_tail);
// }
// } // namespace base
// } // namespace dl
