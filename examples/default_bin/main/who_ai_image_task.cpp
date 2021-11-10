#include "who_ai_image_task.hpp"
#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"
#include "fb_gfx.h"

#include "cat_face_detect_mn03.hpp"
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "color_detector.hpp"

#include "face_recognition_tool.hpp"
#if CONFIG_MFN_V1
#if CONFIG_S8
#include "face_recognition_112_v1_s8.hpp"
#elif CONFIG_S16
#include "face_recognition_112_v1_s16.hpp"
#endif
#endif

#include "who_ai_utils.hpp"
#include "who_lcd.h"

#define AI_TASK_NUM 7

using namespace std;
using namespace dl;

static const char *TAG = "ai_image_task";

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueEvent = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static QueueHandle_t xQueueResult = NULL;
static QueueHandle_t xQueueTaskState[AI_TASK_NUM] = {NULL};
static bool taskRunFlag[AI_TASK_NUM] = {false};
static bool taskRunFlagSelf[AI_TASK_NUM] = {false};

static int gEvent = FACE_RECOGNITION_DETECT;
static int gTask = TASK_IDLE;
static bool gReturnFB = true;
static face_info_t recognize_result;

static SemaphoreHandle_t xMutex;

typedef enum
{
    SHOW_STATE_HUMAN_FACE_IDLE,
    SHOW_STATE_HUMAN_FACE_DELETE,
    SHOW_STATE_HUMAN_FACE_RECOGNIZE,
    SHOW_STATE_HUMAN_FACE_ENROLL,
} show_state_t;

#define RGB565_MASK_RED 0xF800
#define RGB565_MASK_GREEN 0x07E0
#define RGB565_MASK_BLUE 0x001F
#define FRAME_DELAY_NUM 16

static void rgb_print(camera_fb_t *fb, uint32_t color, const char *str)
{
    fb_gfx_print(fb, (fb->width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(camera_fb_t *fb, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    rgb_print(fb, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}

static void task_idle_handler(void *arg)
{
    int _state = 0;
    bool draw_logo = false;
    // camera_fb_t *frame = NULL;
    while (1)
    {
        xQueueReceive(xQueueTaskState[TASK_IDLE], &_state, portMAX_DELAY);
        taskRunFlagSelf[TASK_IDLE] = true;
        draw_logo = true;
        while (taskRunFlag[TASK_IDLE])
        {
            if (draw_logo)
            {
                // vTaskDelay(100);
                // app_lcd_set_color(0x0000);
                vTaskDelay(50);
                app_lcd_draw_wallpaper();
                vTaskDelay(100);
                draw_logo = false;
            }
            // xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY);
            // esp_camera_fb_return(frame);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        taskRunFlagSelf[TASK_IDLE] = false;
    }
}

static void camera_lcd_handler(void *arg)
{
    camera_fb_t *frame = NULL;
    int _state = 0;
    while (1)
    {
        xQueueReceive(xQueueTaskState[TASK_CAMERA_LCD], &_state, portMAX_DELAY);
        taskRunFlagSelf[TASK_CAMERA_LCD] = true;
        while (taskRunFlag[TASK_CAMERA_LCD])
        {
            xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY);
            xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
        }
        taskRunFlagSelf[TASK_CAMERA_LCD] = false;
    }
}

static void cat_face_detection_handler(void *arg)
{
    CatFaceDetectMN03 detector(0.4F, 0.3F, 10, 0.3F);
    camera_fb_t *frame = NULL;
    int _state = 0;
    while (1)
    {
        xQueueReceive(xQueueTaskState[TASK_CAT_FACE_DETECTION], &_state, portMAX_DELAY);
        taskRunFlagSelf[TASK_CAT_FACE_DETECTION] = true;
        while (taskRunFlag[TASK_CAT_FACE_DETECTION])
        {
            bool is_detected = false;
            if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
            {
                std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                if (detect_results.size() > 0)
                {
                    draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                    print_detection_result(detect_results);
                    is_detected = true;
                }
            }

            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }

            if (xQueueResult)
            {
                xQueueSend(xQueueResult, &is_detected, portMAX_DELAY);
            }
        }
        taskRunFlagSelf[TASK_CAT_FACE_DETECTION] = false;
    }
}

static void human_face_recognition_handler(void *arg)
{
    camera_fb_t *frame = NULL;
    HumanFaceDetectMSR01 *detector = new HumanFaceDetectMSR01(0.3F, 0.3F, 10, 0.3F);
    HumanFaceDetectMNP01 *detector2 = new HumanFaceDetectMNP01(0.4F, 0.3F, 10);

    // print_memory("x1", 0);
#if CONFIG_MFN_V1
#if CONFIG_S8
    FaceRecognition112V1S8 *recognizer = new FaceRecognition112V1S8();
#elif CONFIG_S16
    FaceRecognition112V1S16 *recognizer = new FaceRecognition112V1S16();
#endif
#endif
    // print_memory("x2", 0);
    show_state_t frame_show_state = SHOW_STATE_HUMAN_FACE_IDLE;
    int _gEvent;
    int _state = 0;

    while (1)
    {
        xQueueReceive(xQueueTaskState[TASK_HUMAN_FACE_RECOGNITION], &_state, portMAX_DELAY);
        taskRunFlagSelf[TASK_HUMAN_FACE_RECOGNITION] = true;

        while (taskRunFlag[TASK_HUMAN_FACE_RECOGNITION])
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            _gEvent = gEvent;
            gEvent = FACE_RECOGNITION_DETECT;
            xSemaphoreGive(xMutex);

            if (_gEvent)
            {
                bool is_detected = false;

                if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
                {
                    std::list<dl::detect::result_t> &detect_candidates = detector->infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                    std::list<dl::detect::result_t> &detect_results = detector2->infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

                    if (detect_results.size() == 1)
                        is_detected = true;

                    if (is_detected)
                    {
                        switch (_gEvent)
                        {
                        case FACE_RECOGNITION_ENROLL:
                            recognizer->enroll_id((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
                            ESP_LOGW("ENROLL", "ID %d is enrolled", recognizer->get_enrolled_ids().back().id);
                            frame_show_state = SHOW_STATE_HUMAN_FACE_ENROLL;
                            break;

                        case FACE_RECOGNITION_RECOGNIZE:
                            recognize_result = recognizer->recognize((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
                            // print_detection_result(detect_results);
                            if (recognize_result.id > 0)
                                ESP_LOGI("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
                            else
                                ESP_LOGE("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
                            frame_show_state = SHOW_STATE_HUMAN_FACE_RECOGNIZE;
                            break;

                        case FACE_RECOGNITION_DELETE:
                            recognizer->delete_id();
                            ESP_LOGE("DELETE", "% d IDs left", recognizer->get_enrolled_id_num());
                            frame_show_state = SHOW_STATE_HUMAN_FACE_DELETE;
                            break;

                        default:
                            break;
                        }
                    }

                    if (frame_show_state != SHOW_STATE_HUMAN_FACE_IDLE)
                    {
                        static int frame_count = 0;
                        switch (frame_show_state)
                        {
                        case SHOW_STATE_HUMAN_FACE_DELETE:
                            rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", recognizer->get_enrolled_id_num());
                            break;

                        case SHOW_STATE_HUMAN_FACE_RECOGNIZE:
                            if (recognize_result.id > 0)
                                rgb_printf(frame, RGB565_MASK_GREEN, "ID %d", recognize_result.id);
                            else
                                rgb_print(frame, RGB565_MASK_RED, "who ?");
                            break;

                        case SHOW_STATE_HUMAN_FACE_ENROLL:
                            rgb_printf(frame, RGB565_MASK_BLUE, "Enroll: ID %d", recognizer->get_enrolled_ids().back().id);
                            break;

                        default:
                            break;
                        }

                        if (++frame_count > FRAME_DELAY_NUM)
                        {
                            frame_count = 0;
                            frame_show_state = SHOW_STATE_HUMAN_FACE_IDLE;
                        }
                    }

                    if (detect_results.size())
                    {
                        draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                    }
                }

                if (xQueueFrameO)
                {

                    xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
                }
                else if (gReturnFB)
                {
                    esp_camera_fb_return(frame);
                }
                else
                {
                    free(frame);
                }

                if (xQueueResult && is_detected)
                {
                    xQueueSend(xQueueResult, &recognize_result, portMAX_DELAY);
                }
            }
        }
        taskRunFlagSelf[TASK_HUMAN_FACE_RECOGNITION] = false;
    }
}

static void motion_detection_handler(void *arg)
{
    camera_fb_t *frame1 = NULL;
    camera_fb_t *frame2 = NULL;

    int _state = 0;
    while (1)
    {
        xQueueReceive(xQueueTaskState[TASK_MOTION_DETECTION], &_state, portMAX_DELAY);
        taskRunFlagSelf[TASK_MOTION_DETECTION] = true;

        while (taskRunFlag[TASK_MOTION_DETECTION])
        {
            if (gEvent)
            {
                bool is_moved = false;
                if (xQueueReceive(xQueueFrameI, &(frame1), portMAX_DELAY))
                {
                    if (xQueueReceive(xQueueFrameI, &(frame2), portMAX_DELAY))
                    {
                        uint32_t moving_point_number = dl::image::get_moving_point_number((uint16_t *)frame1->buf, (uint16_t *)frame2->buf, frame1->height, frame1->width, 8, 15);
                        if (moving_point_number > 50)
                        {
                            ESP_LOGI(TAG, "Something moved!");
                            dl::image::draw_filled_rectangle((uint16_t *)frame2->buf, frame2->height, frame2->width, 0, 0, 20, 20);
                            is_moved = true;
                        }
                    }
                }

                if (xQueueFrameO)
                {
                    esp_camera_fb_return(frame1);
                    xQueueSend(xQueueFrameO, &frame2, portMAX_DELAY);
                }
                else
                {
                    esp_camera_fb_return(frame1);
                    esp_camera_fb_return(frame2);
                }

                if (xQueueResult)
                {
                    xQueueSend(xQueueResult, &is_moved, portMAX_DELAY);
                }
            }
        }
        taskRunFlagSelf[TASK_MOTION_DETECTION] = false;
    }
}

static void task_event_handler(void *arg)
{
    int _gEventReceive;
    while (true)
    {
        xQueueReceive(xQueueEvent, &(_gEventReceive), portMAX_DELAY);
        if ((_gEventReceive >= TASK_IDLE) && (_gEventReceive <= TASK_HAND_DETECTION) && (_gEventReceive != gTask))
        {
            ESP_LOGW(TAG, "Task Switching  %d -> %d", gTask, _gEventReceive);
            taskRunFlag[gTask] = false;
            while (!taskRunFlagSelf[gTask])
                vTaskDelay(20 / portTICK_PERIOD_MS);
            gTask = _gEventReceive;
            taskRunFlag[gTask] = true;
            xQueueSend(xQueueTaskState[gTask], &gTask, portMAX_DELAY);
        }
        else if ((FACE_RECOGNITION_IDLE >= TASK_IDLE) && (_gEventReceive <= FACE_RECOGNITION_DELETE) && (gTask == TASK_HUMAN_FACE_RECOGNITION))
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            gEvent = _gEventReceive;
            xSemaphoreGive(xMutex);
        }
    }
}

void register_ai_image_task(const QueueHandle_t frame_i,
                            const QueueHandle_t event,
                            const QueueHandle_t result,
                            const QueueHandle_t frame_o,
                            const bool camera_fb_return)
{
    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    xQueueEvent = event;
    xQueueResult = result;
    gReturnFB = camera_fb_return;
    xMutex = xSemaphoreCreateMutex();

    for (int i = 0; i < AI_TASK_NUM; ++i)
    {
        xQueueTaskState[i] = xQueueCreate(1, sizeof(int));
    }
    gTask = TASK_HUMAN_FACE_RECOGNITION;
    taskRunFlag[TASK_HUMAN_FACE_RECOGNITION] = true;
    xQueueSend(xQueueTaskState[gTask], &gTask, portMAX_DELAY);

    xTaskCreatePinnedToCore(task_idle_handler, "task_idle", 2 * 1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(camera_lcd_handler, "task_camera_lcd", 2 * 1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(cat_face_detection_handler, "task_cat_face_detection", 4 * 1024, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(human_face_recognition_handler, "task_human_face_recognition", 4 * 1024, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(motion_detection_handler, "task_motion_detection", 4 * 1024, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_event_handler, "task_event_control", 4 * 1024, NULL, 5, NULL, 0);
}

static void ai_image_handler(void *arg)
{
    camera_fb_t *frame = NULL;
    HumanFaceDetectMSR01 *human_face_detector = new HumanFaceDetectMSR01(0.3F, 0.3F, 10, 0.3F);
    HumanFaceDetectMNP01 *human_face_detector2 = new HumanFaceDetectMNP01(0.4F, 0.3F, 10);
    CatFaceDetectMN03 *cat_face_detector = new CatFaceDetectMN03(0.4F, 0.3F, 10, 0.3F);

#if CONFIG_MFN_V1
#if CONFIG_S8
    FaceRecognition112V1S8 *recognizer = new FaceRecognition112V1S8();
#elif CONFIG_S16
    FaceRecognition112V1S16 *recognizer = new FaceRecognition112V1S16();
#endif
#endif
    show_state_t frame_show_state = SHOW_STATE_HUMAN_FACE_IDLE;
    int _gEvent;
    int _gTaskLast = TASK_IDLE;
    int _gTaskNow;

    while (1)
    {
        static int frame_count = 0;
        _gTaskNow = gTask;

        if (_gTaskNow == TASK_IDLE)
        {
            if (_gTaskNow != _gTaskLast)
            {
                _gTaskLast = _gTaskNow;
                vTaskDelay(100);
                app_lcd_draw_wallpaper();
                vTaskDelay(100);
                continue;
            }
            else
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
                continue;
            }
        }

        xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY);

        switch (_gTaskNow)
        {
        case TASK_CAT_FACE_DETECTION:
        {
            std::list<dl::detect::result_t> &detect_results = cat_face_detector->infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
            if (detect_results.size() > 0)
            {
                draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                print_detection_result(detect_results);
            }
            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }

            break;
        }

        case TASK_HUMAN_FACE_RECOGNITION:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            _gEvent = gEvent;
            gEvent = FACE_RECOGNITION_DETECT;
            xSemaphoreGive(xMutex);

            std::list<dl::detect::result_t> &detect_candidates = human_face_detector->infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
            std::list<dl::detect::result_t> &detect_results = human_face_detector2->infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

            if (detect_results.size() == 1)
            {
                switch (_gEvent)
                {
                case FACE_RECOGNITION_ENROLL:
                    recognizer->enroll_id((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
                    ESP_LOGW("ENROLL", "ID %d is enrolled", recognizer->get_enrolled_ids().back().id);
                    frame_show_state = SHOW_STATE_HUMAN_FACE_ENROLL;
                    break;

                case FACE_RECOGNITION_RECOGNIZE:
                    recognize_result = recognizer->recognize((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
                    // print_detection_result(detect_results);
                    if (recognize_result.id > 0)
                        ESP_LOGI("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
                    else
                        ESP_LOGE("RECOGNIZE", "Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
                    frame_show_state = SHOW_STATE_HUMAN_FACE_RECOGNIZE;
                    break;

                case FACE_RECOGNITION_DELETE:
                    recognizer->delete_id();
                    ESP_LOGE("DELETE", "% d IDs left", recognizer->get_enrolled_id_num());
                    frame_show_state = SHOW_STATE_HUMAN_FACE_DELETE;
                    break;

                default:
                    break;
                }
            }

            if (frame_show_state != SHOW_STATE_HUMAN_FACE_IDLE)
            {
                switch (frame_show_state)
                {
                case SHOW_STATE_HUMAN_FACE_DELETE:
                    rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", recognizer->get_enrolled_id_num());
                    break;

                case SHOW_STATE_HUMAN_FACE_RECOGNIZE:
                    if (recognize_result.id > 0)
                        rgb_printf(frame, RGB565_MASK_GREEN, "ID %d", recognize_result.id);
                    else
                        rgb_print(frame, RGB565_MASK_RED, "who ?");
                    break;

                case SHOW_STATE_HUMAN_FACE_ENROLL:
                    rgb_printf(frame, RGB565_MASK_BLUE, "Enroll: ID %d", recognizer->get_enrolled_ids().back().id);
                    break;

                default:
                    break;
                }

                if (++frame_count > FRAME_DELAY_NUM)
                {
                    frame_count = 0;
                    frame_show_state = SHOW_STATE_HUMAN_FACE_IDLE;
                }
            }
            else
            {
                frame_count = 0;
            }

            if (detect_results.size())
            {
                draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
            }

            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }

            break;
        }

        case TASK_MOTION_DETECTION:
        {
            camera_fb_t *frame2 = NULL;
            xQueueReceive(xQueueFrameI, &(frame2), portMAX_DELAY);
            uint32_t moving_point_number = dl::image::get_moving_point_number((uint16_t *)frame->buf, (uint16_t *)frame2->buf, frame->height, frame->width, 8, 15);
            if (moving_point_number > 50)
            {
                ESP_LOGI(TAG, "Something moved!");
                dl::image::draw_filled_rectangle((uint16_t *)frame->buf, frame->height, frame->width, 0, 0, 20, 20);
            }

            if (xQueueFrameO)
            {
                esp_camera_fb_return(frame2);
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else
            {
                esp_camera_fb_return(frame2);
                esp_camera_fb_return(frame);
            }

            break;
        }

        default:
        {
            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }

            break;
        }
        }

        if (_gTaskLast != _gTaskNow)
        {
            if (_gTaskNow != TASK_HUMAN_FACE_RECOGNITION)
            {
                frame_count = 0;
                frame_show_state = SHOW_STATE_HUMAN_FACE_IDLE;
            }
            _gTaskLast = _gTaskNow;
        }
    }
}

static void task_switching_handler(void *arg)
{
    int _gEventReceive;
    while (true)
    {
        xQueueReceive(xQueueEvent, &(_gEventReceive), portMAX_DELAY);
        if ((_gEventReceive >= TASK_IDLE) && (_gEventReceive <= TASK_HAND_DETECTION) && (_gEventReceive != gTask))
        {
            ESP_LOGW(TAG, "Task Switching  %d -> %d", gTask, _gEventReceive);
            gTask = _gEventReceive;
        }
        else if ((FACE_RECOGNITION_IDLE >= TASK_IDLE) && (_gEventReceive <= FACE_RECOGNITION_DELETE) && (gTask == TASK_HUMAN_FACE_RECOGNITION))
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            gEvent = _gEventReceive;
            xSemaphoreGive(xMutex);
        }
    }
}

// void register_ai_image_task(const QueueHandle_t frame_i,
//                             const QueueHandle_t event,
//                             const QueueHandle_t result,
//                             const QueueHandle_t frame_o,
//                             const bool camera_fb_return)
// {
//     xQueueFrameI = frame_i;
//     xQueueFrameO = frame_o;
//     xQueueEvent = event;
//     gReturnFB = false;
//     xMutex = xSemaphoreCreateMutex();
//     gTask = TASK_HUMAN_FACE_RECOGNITION;

//     xTaskCreatePinnedToCore(ai_image_handler, "task_ai_image", 16 * 1024, NULL, 5, NULL, 1);
//     xTaskCreatePinnedToCore(task_switching_handler, "task_event_control", 4 * 1024, NULL, 5, NULL, 0);
// }