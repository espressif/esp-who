#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "jpeg.h"


const char *TAG="jpeg";

typedef struct {	
    uint8_t *in;   //Pointer to jpeg data
    int in_pos;    //Current position in jpeg data
    uint8_t *out;
    int out_pos;
} jpeg_decode_obj_t;

//Input function for jpeg decoder. Just returns bytes from the inData field of the JpegDev structure.
static UINT jpeg_decode_in_callback(JDEC *decoder, BYTE *buf, UINT len) 
{
    //Read bytes from input file
    jpeg_decode_obj_t *jpeg_decode_obj = (jpeg_decode_obj_t *)decoder->device;

    if (buf != NULL) {
        memcpy(buf, &jpeg_decode_obj->in[jpeg_decode_obj->in_pos], len);
    }
    jpeg_decode_obj->in_pos += len;
    return len;
}

//Output function. Re-encodes the RGB888 data from the decoder as big-endian RGB565 and
//stores it in the outData array of the JpegDev structure.
static UINT jpeg_decode_out_callback(JDEC *decoder, void *bitmap, JRECT *rect) 
{
    jpeg_decode_obj_t *jpeg_decode_obj = (jpeg_decode_obj_t *)decoder->device;
    uint8_t *in = (uint8_t*)bitmap;

    for (int y = rect->top; y <= rect->bottom; y++) {
        for (int x = rect->left; x <= rect->right; x++) {
            //The LCD wants the 16-bit value in big-endian, so swap bytes
            jpeg_decode_obj->out[2 * (y * decoder->width + x)] = in[1];
            jpeg_decode_obj->out[2 * (y * decoder->width + x) + 1] = in[0];
            jpeg_decode_obj->out_pos += 2;
            in += 2;
        }
    }
    return 1;
}

uint8_t *jpeg_decode(uint8_t *jpeg, int *w, int* h)
{
    jpeg_decode_obj_t jpeg_decode_obj = {0};
    JDEC decoder = {0};
    int ret = -1;

    jpeg_decode_obj.in = jpeg;
    jpeg_decode_obj.in_pos = 0;
    jpeg_decode_obj.out_pos = 0;
    char *work_buf = (char *)heap_caps_calloc(JPEG_WORK_BUF_SIZE, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    //Prepare and decode the jpeg.
    ret = jd_prepare(&decoder, jpeg_decode_in_callback, work_buf, JPEG_WORK_BUF_SIZE, (void*)&jpeg_decode_obj);
    if (ret != JDR_OK) {
        ESP_LOGE(TAG, "Image decoder: jd_prepare failed (%d)", ret);
        free(work_buf);
        return NULL;
    }
    *w = decoder.width;
    *h = decoder.height;
    jpeg_decode_obj.out = (uint8_t *)heap_caps_calloc(decoder.width * decoder.height, sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    ret = jd_decomp(&decoder, jpeg_decode_out_callback, 0);
    if (ret != JDR_OK) {
        ESP_LOGE(TAG, "Image decoder: jd_decode failed (%d)", ret);
        free(jpeg_decode_obj.out);
        free(work_buf);
        return NULL;
    }

    free(work_buf);
    return jpeg_decode_obj.out;
}