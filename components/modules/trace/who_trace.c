#include "who_trace.h"
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"

static void task_trace(void *arg)
{
    char *pbuffer = (char *)malloc(2048);

    while (true)
    {
        printf("\n-------------------------------------------------------------------------\n");
        vTaskList(pbuffer);
        printf("%s", pbuffer);
        printf("-------------------------------------------------------------------------\n\n");
        vTaskDelay(3000 / portTICK_RATE_MS);
    }
    free(pbuffer);
}

void register_trace()
{
    xTaskCreate(task_trace, "trace", 2*1024, NULL, 5, NULL);
}