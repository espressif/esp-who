#pragma once

#if __cplusplus
extern "C"
{
#endif

#include "dl_lib_matrix3d.h"
#include "dl_lib_matrix3dq.h"

    typedef struct
    {
        int num;
        dl_matrix3d_t *cls;
        dl_matrix3d_t *score;
        dl_matrix3d_t *boxes;
    } detection_result_t;

    detection_result_t **hd_nano1_q(dl_matrix3dq_t *in, int mode);
    detection_result_t **hd_lite1_q(dl_matrix3dq_t *in, int mode);
    detection_result_t **hd_lite1_q_preload(dl_matrix3dq_t *in, int mode);
    void detection_result_free(detection_result_t *m);
    void detection_results_free(detection_result_t **m, int length);
    void hd_test();

#if __cplusplus
}
#endif
