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
#include "esp_sr_iface.h"
#include "esp_sr_models.h"
#include "app_main.h"

#define SR_MODEL esp_sr_wakenet3_quantized

static src_cfg_t srcif;
static const esp_sr_iface_t *model = &SR_MODEL;
static model_iface_data_t *model_data;

QueueHandle_t sndQueue;

static void event_wakeup_detected(int r)
{
    assert(g_state == WAIT_FOR_WAKEUP);
    printf("%s DETECTED.\n", model->get_word_name(model_data, r));
    g_state = WAIT_FOR_CONNECT;
}

void nnTask(void *arg)
{
    int audio_chunksize = model->get_samp_chunksize(model_data);
    int16_t *buffer=malloc(audio_chunksize*sizeof(int16_t));
    assert(buffer);

    while(1) {
        xQueueReceive(sndQueue, buffer, portMAX_DELAY);

        int r=model->detect(model_data, buffer);
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
    model_data=model->create(DET_MODE_95);

    wake_word_info_t* word_list = malloc(sizeof(wake_word_info_t));
    esp_err_t ret = model->get_word_list(model_data, word_list);
    if (ret == ESP_OK) printf("wake word number = %d, word1 name = %s\n", 
                               word_list->wake_word_num, word_list->wake_word_list[0]);
    free(word_list);    

    int audio_chunksize=model->get_samp_chunksize(model_data);

    //Initialize sound source
    sndQueue=xQueueCreate(2, (audio_chunksize*sizeof(int16_t)));
    srcif.queue=&sndQueue;
    srcif.item_size=audio_chunksize*sizeof(int16_t);

    xTaskCreatePinnedToCore(&recsrcTask, "rec", 3*1024, (void*)&srcif, 5, NULL, 1);

    xTaskCreatePinnedToCore(&nnTask, "nn", 2*1024, NULL, 5, NULL, 1);
}
