#pragma once

#if __cplusplus
extern "C"
{
#endif

#include "dl_lib_matrix3d.h"
#include "dl_lib_matrix3dq.h"

    dl_matrix3d_t *hp_nano1_ls16_q(dl_matrix3dq_t *in, int mode);
    dl_matrix3d_t *hp_mid0_q(dl_matrix3dq_t *in, int mode);
    dl_matrix3d_t *hp_lite1_q(dl_matrix3dq_t *in, int mode);
    dl_matrix3d_t *hp_mid1_q(dl_matrix3dq_t *in, int mode);
    void hp_test();

#if __cplusplus
}
#endif