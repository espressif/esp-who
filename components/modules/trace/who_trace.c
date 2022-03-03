#include "who_trace.h"
#include <stdio.h>
#include <stdlib.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"

static void task_trace(void *arg)
{
    while (true)
    {
        if (CONFIG_FREERTOS_USE_TRACE_FACILITY == 0 && CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS == 0)
            break;

        int task_num = uxTaskGetNumberOfTasks();
        char *buffer = (char *)malloc(task_num * 40 * sizeof(char));

#if CONFIG_FREERTOS_USE_TRACE_FACILITY
        printf("TaskList-------------------------------------------------------------------------\n");
        vTaskList(buffer);
        printf("%s", buffer);
#endif

#if CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
        printf("TimeStats------------------------------------------------------------------------\n");
        vTaskGetRunTimeStats(buffer);
        printf("%s", buffer);
#endif
        vTaskDelay(1000 / portTICK_RATE_MS);

        free(buffer);
    }
    vTaskDelete(NULL);
}

void register_trace()
{
    xTaskCreate(task_trace, "trace", 3 * 1024, NULL, 5, NULL);
}