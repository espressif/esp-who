#pragma once

#include "image_util.h"

dl_matrix3du_t *bgr2gray(dl_matrix3du_t *image_matrix);
float tenengrad_score(dl_matrix3du_t *gray_image);
float laplace_score(dl_matrix3du_t *gray_image);