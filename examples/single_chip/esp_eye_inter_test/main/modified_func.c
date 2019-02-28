#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "esp_log.h"
#include "fr_forward.h"
#include "freertos/FreeRTOS.h"
#include "rom/ets_sys.h"
#include "esp_partition.h"
#include "modified_func.h"


fptp_t cos_sim(dl_matrix3d_t *id_1,
                    dl_matrix3d_t *id_2)
{
    assert(id_1->c == id_2->c);
    uint16_t c = id_1->c;
    fptp_t l2_norm_1 = 0;
    fptp_t l2_norm_2 = 0;
    fptp_t dist = 0;
    for (int i = 0; i < c; i++)
    {
        l2_norm_1 += ((id_1->item[i]) * (id_1->item[i]));
        l2_norm_2 += ((id_2->item[i]) * (id_2->item[i]));
    }
    l2_norm_1 = sqrt(l2_norm_1);
    l2_norm_2 = sqrt(l2_norm_2);
    for (uint16_t i = 0; i < c; i++)
    {
        dist += ((id_1->item[i]) * (id_2->item[i]) / (l2_norm_1 * l2_norm_2));
    }
    return dist;
}


dl_matrix3d_t *dl_matrix3d_heap_alloc(int n, int w, int h, int c, int mode)
{
    dl_matrix3d_t *r = calloc(1, sizeof(dl_matrix3d_t));
    fptp_t *items = NULL;
    if(mode == 0){
        items = heap_caps_calloc(n * w * h * c, sizeof(fptp_t), MALLOC_CAP_SPIRAM);
    }
    else{
        items = heap_caps_calloc(n * w * h * c, sizeof(fptp_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    }
    if (NULL == items)
    {
        //printf("dram fail: %d, %d, %d, %d, size: %d\n", n, w, h, c, n * w * h * c * sizeof(fptp_t));
        items = calloc(n * w * h * c, sizeof(fptp_t));
    }
    if (NULL == items)
    {
        printf("matrix3d item alloc failed.\n");
        free(r);
        return NULL;
    }

    r->w = w;
    r->h = h;
    r->c = c;
    r->n = n;
    r->stride = w * c;
    r->item = items;

    return r;
}

static void devide_face_id(dl_matrix3d_t *id, uint8_t num)
{
    fptp_t *in1 = id->item;
    for (int i = 0; i < id->c; i++)
    {
        (*in1++) /= num;
    }
}


static dl_matrix3dq_t *transform_frmn_input(dl_matrix3du_t *image)
{
    dl_matrix3d_t *image_3d = dl_matrix3d_alloc(image->n,
                                                image->w,
                                                image->h,
                                                image->c);

    fptp_t r = 0;
    fptp_t *a = image_3d->item;
    uc_t *b = image->item;
    uint32_t count = (image->n) * (image->w) * (image->h) * (image->c);
    for (uint32_t i = 0; i < count; i++)
    {
        r = *b++;
        r = (r - 127.5) * (0.0078125);
        *a++ = r;
    }
    dl_matrix3dq_t *image_3dq = dl_matrixq_from_matrix3d_qmf(image_3d, -10);
    dl_matrix3d_free(image_3d);
    return image_3dq;
}


static dl_matrix3d_t *get_face_id(dl_matrix3du_t *aligned_face)
{
    dl_matrix3d_t *face_id = NULL;
    dl_matrix3dq_t *mobileface_in = transform_frmn_input(aligned_face);
#if CONFIG_XTENSA_IMPL
    dl_matrix3dq_t *face_id_q = frmn_q(mobileface_in, DL_XTENSA_IMPL);
#else
    dl_matrix3dq_t *face_id_q = frmn_q(mobileface_in, DL_C_IMPL);
#endif
    face_id = dl_matrix3d_from_matrixq(face_id_q);
    dl_matrix3dq_free(face_id_q);
    return face_id;
}


int8_t enroll_face_heap(face_id_list *l, 
                dl_matrix3du_t *aligned_face)
{
    static int8_t confirm_counter = 0;

    // add new_id to dest_id
    dl_matrix3d_t *new_id = get_face_id(aligned_face);

    if ((l->count < l->size)&&(confirm_counter == 0))
        l->id_list[l->tail] = dl_matrix3d_heap_alloc(1, 1, 1, FACE_ID_SIZE,0);

    add_face_id(l->id_list[l->tail], new_id);
    dl_matrix3d_free(new_id);

    confirm_counter++;

    if (confirm_counter == l->confirm_times)
    {
        devide_face_id(l->id_list[l->tail], l->confirm_times);
        confirm_counter = 0;

        l->tail = (l->tail + 1) % l->size;
        l->count++;
        // Overlap head
        if (l->count > l->size)
        {
            l->head = (l->head + 1) % l->size;
            l->count = l->size;
        }

        return 0;
    }

    return l->confirm_times - confirm_counter;
}
