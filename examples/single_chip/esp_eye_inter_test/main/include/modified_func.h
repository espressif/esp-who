#pragma once

dl_matrix3d_t *dl_matrix3d_heap_alloc(int n, int w, int h, int c, int mode);
int8_t enroll_face_heap(face_id_list *l, 
                dl_matrix3du_t *aligned_face);
fptp_t cos_sim(dl_matrix3d_t *id_1,
                    dl_matrix3d_t *id_2);