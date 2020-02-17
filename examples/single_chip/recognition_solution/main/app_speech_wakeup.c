#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "xtensa/core-macros.h"
#include "esp_partition.h"
#include "app_speech_srcif.h"
#include "sdkconfig.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "dl_lib_coefgetter_if.h"
#include "app_main.h"

static const esp_wn_iface_t *wakenet = &WAKENET_MODEL;
static const model_coeff_getter_t *model_coeff_getter = &WAKENET_COEFF;

static src_cfg_t srcif;
static model_iface_data_t *model_data;

QueueHandle_t sndQueue;

static void event_wakeup_detected(int r)
{
    assert(g_state == WAIT_FOR_WAKEUP);
    printf("%s DETECTED.\n", wakenet->get_word_name(model_data, r));
    g_state = WAIT_FOR_CONNECT;
}

void nnTask(void *arg)
{
    int audio_chunksize = wakenet->get_samp_chunksize(model_data);
    int16_t *buffer=malloc(audio_chunksize*sizeof(int16_t));
    assert(buffer);

    while(1) {
        xQueueReceive(sndQueue, buffer, portMAX_DELAY);

        int r=wakenet->detect(model_data, buffer);
        if (r) 
        {
            event_wakeup_detected(r);
        }
    }

    free(buffer);
    vTaskDelete(NULL);
}

void app_speech_wakeup_init()
{
    //Initialize NN model
    model_data=wakenet->create(model_coeff_getter,DET_MODE_95);

    int wake_word_num = wakenet->get_word_num(model_data);
    char *wake_word_list = wakenet->get_word_name(model_data, 1);
    if (wake_word_num) printf("wake word number = %d, word1 name = %s\n", 
                               wake_word_num, wake_word_list);  

    int audio_chunksize=wakenet->get_samp_chunksize(model_data);

    //Initialize sound source
    sndQueue=xQueueCreate(2, (audio_chunksize*sizeof(int16_t)));
    srcif.queue=&sndQueue;
    srcif.item_size=audio_chunksize*sizeof(int16_t);

    xTaskCreatePinnedToCore(&recsrcTask, "rec", 3*1024, (void*)&srcif, 5, NULL, 1);

    xTaskCreatePinnedToCore(&nnTask, "nn", 2*1024, NULL, 5, NULL, 1);
}

