#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cnn.h"
#include "input.h"

void test(void *arg)
{
    dl_matrix3d_t *image = dl_matrix3d_alloc(1, 28, 28, 1);
    for (int i = 0; i < image->h * image->w; i++)
    {
        image->item[i] = input_item_array[i] / 255.0f;
    }

    while(1)
    {
        dl_matrix3d_t *o1_1 = dl_matrix3dff_conv_common(image, &conv2d_kernel, &conv2d_bias, 1, 1, PADDING_SAME);
        dl_matrix3d_t *o1_2 = dl_matrix3d_pooling(o1_1, 2, 2, 2, 2, PADDING_VALID, DL_POOLING_MAX);
        dl_matrix3d_relu(o1_2);
        dl_matrix3d_t *o2_1 = dl_matrix3dff_conv_common(o1_2, &conv2d_1_kernel, &conv2d_1_bias, 1, 1, PADDING_SAME);
        dl_matrix3d_t *o2_2 = dl_matrix3d_pooling(o2_1, 2, 2, 2, 2, PADDING_VALID, DL_POOLING_MAX);
        dl_matrix3d_relu(o2_2);
        dl_matrix3d_t *o3_1 = dl_matrix3dff_conv_common(o2_2, &conv2d_2_kernel, &conv2d_2_bias, 1, 1, PADDING_SAME);
        dl_matrix3d_t *o3_2 = dl_matrix3d_pooling(o3_1, 2, 2, 2, 2, PADDING_VALID, DL_POOLING_MAX);
        dl_matrix3d_relu(o3_2);
        dl_matrix3d_t *o4_1 = dl_matrix3d_alloc(1, 1, 1, dense_kernel.h);
        dl_matrix3dff_fc_with_bias(o4_1, o3_2, &dense_kernel, &dense_bias);
        dl_matrix3d_relu(o4_1);
        dl_matrix3d_t *o4_2 = dl_matrix3d_alloc(1, 1, 1, dense_1_kernel.h);
        dl_matrix3dff_fc_with_bias(o4_2, o4_1, &dense_1_kernel, &dense_1_bias);

        int idx = 0;
        fptp_t max = o4_2->item[0];
        printf("Result:\n");
        for (int i = 0; i < o4_2->c; i++)
        {
            printf("%f\t", o4_2->item[i]);
            if (max < o4_2->item[i])
            {
                max = o4_2->item[i];
                idx = i;
            }
        }
        printf("\nThe number is: %d.\n", idx);


        vTaskDelay(100);
    }
}

void app_main()
{
    xTaskCreatePinnedToCore(&test, "test", 4096, NULL, 5, NULL, 0);
}
