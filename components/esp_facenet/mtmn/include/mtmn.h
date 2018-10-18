#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "dl_lib.h"


typedef enum
{
    PNET = 0,
    RNET = 1,
    ONET = 2,
} net_type_en;

typedef struct
{
    net_type_en net_type;
    char *file_name;
    int w;
    int h;
    float score_threshold;
    float nms_threshold;
} net_config_t; 

typedef struct
{
    dl_matrix3d_t *category;
    dl_matrix3d_t *offset;
    dl_matrix3d_t *landmark;
} mtmn_net_t;


/**
 * @brief Forward the pnet process, coarse detection
 *
 * @param in        Image matrix, rgb888 format, size is 320x240
 * @return          Scores for every pixel, and box offset with respect.
 */
mtmn_net_t *pnet(dl_matrix3du_t *in);

/**
 * @brief Forward the rnet process, fine determine the boxes from pnet
 *
 * @param in        Image matrix, rgb888 format
 * @param threshold Score threshold to detect human face
 * @return          Scores for every box, and box offset with respect.
 */
mtmn_net_t *rnet_with_score_verify(dl_matrix3du_t *in, float threshold);

/**
 * @brief Forward the onet process, fine determine the boxes from rnet
 *
 * @param in        Image matrix, rgb888 format
 * @param threshold Score threshold to detect human face
 * @return          Scores for every box, box offset, and landmark with respect.
 */
mtmn_net_t *onet_with_score_verify(dl_matrix3du_t *in, float threshold);
#ifdef __cplusplus
}
#endif
