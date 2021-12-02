#include <stdio.h>
#include "event_logic.hpp"
#include "who_button.h"
#include "who_ai_image_task.hpp"

typedef enum
{
    MENU = 1,
    PLAY,
    UP,
    DOWN
} key_name_t;

static QueueHandle_t xQueueKeyStateI = NULL;
static QueueHandle_t xQueueSRResultI = NULL;
static QueueHandle_t xQueueEventO = NULL;
static SemaphoreHandle_t xMutexEvent = NULL;
static int key_state;
static key_name_t adc_button_name;
static int recognizer_state;

static void event_generate(void *arg)
{
    while (1)
    {
        xQueueReceive(xQueueKeyStateI, &key_state, portMAX_DELAY);
        switch (key_state)
        {
        case KEY_SHORT_PRESS:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_RECOGNIZE;
            xSemaphoreGive(xMutexEvent);
            break;

        case KEY_LONG_PRESS:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_ENROLL;
            xSemaphoreGive(xMutexEvent);
            break;

        case KEY_DOUBLE_CLICK:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_DELETE;
            xSemaphoreGive(xMutexEvent);
            break;

        default:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_DETECT;
            xSemaphoreGive(xMutexEvent);
            break;
        }
        xQueueSend(xQueueEventO, &recognizer_state, portMAX_DELAY);
    }
}

static void event_generate_from_adc_button(void *arg)
{
    while (1)
    {
        xQueueReceive(xQueueKeyStateI, &adc_button_name, portMAX_DELAY);
        switch (adc_button_name)
        {
        case MENU:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_ENROLL;
            xSemaphoreGive(xMutexEvent);     
            break;

        case PLAY:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_DELETE;
            xSemaphoreGive(xMutexEvent);
            break;

        case UP:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_RECOGNIZE;
            xSemaphoreGive(xMutexEvent);
            break;

        case DOWN:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_RECOGNIZE;
            xSemaphoreGive(xMutexEvent);
            break;

        default:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_DETECT;
            xSemaphoreGive(xMutexEvent);
            break;
        }
        xQueueSend(xQueueEventO, &recognizer_state, portMAX_DELAY);
    }
}

static void event_generate_from_sr(void *arg)
{
    int task_id_receive = 0;
    int task_name_send = 0;
    while (1)
    {
        xQueueReceive(xQueueSRResultI, &task_id_receive, portMAX_DELAY);
        switch (task_id_receive)
        {
        case 0:
            task_name_send = TASK_IDLE;
            xQueueSend(xQueueEventO, &task_name_send, portMAX_DELAY);
            break;

        case 1:
            task_name_send = TASK_CAMERA_LCD;
            xQueueSend(xQueueEventO, &task_name_send, portMAX_DELAY);
            break;

        // case 2:
        //     task_name_send = TASK_CAT_FACE_DETECTION;
        //     xQueueSend(xQueueEventO, &task_name_send, portMAX_DELAY);
        //     break;

        case 2:
            task_name_send = TASK_HUMAN_FACE_RECOGNITION;
            xQueueSend(xQueueEventO, &task_name_send, portMAX_DELAY);
            break;

        case 3:
            task_name_send = TASK_MOTION_DETECTION;
            xQueueSend(xQueueEventO, &task_name_send, portMAX_DELAY);
            break;
        
        // case 5:
        //     task_name_send = TASK_COLOR_DETECTION;
        //     xQueueSend(xQueueEventO, &task_name_send, portMAX_DELAY);
        //     break;

        // case 6:
        //     task_name_send = TASK_HAND_DETECTION;
        //     xQueueSend(xQueueEventO, &task_name_send, portMAX_DELAY);
        //     break;

        case 20:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_ENROLL;
            xQueueSend(xQueueEventO, &recognizer_state, portMAX_DELAY);
            xSemaphoreGive(xMutexEvent);
            break;
        
        case 21:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_DELETE;
            xQueueSend(xQueueEventO, &recognizer_state, portMAX_DELAY);
            xSemaphoreGive(xMutexEvent);
            break;
        
        case 22:
            xSemaphoreTake(xMutexEvent, portMAX_DELAY);
            recognizer_state = FACE_RECOGNITION_RECOGNIZE;
            xQueueSend(xQueueEventO, &recognizer_state, portMAX_DELAY);
            xSemaphoreGive(xMutexEvent);
            break;

        default:
            break;
        }
    }
}

void register_event(const QueueHandle_t sr_result_i, const QueueHandle_t key_state_i, const QueueHandle_t event_o)
{
    xQueueKeyStateI = key_state_i;
    xQueueSRResultI = sr_result_i;
    xQueueEventO = event_o;
    xMutexEvent = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(event_generate_from_adc_button, "event_key_task", 1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(event_generate_from_sr, "event_sr_task", 1024, NULL, 5, NULL, 0);
}