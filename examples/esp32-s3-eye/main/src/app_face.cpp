#include "app_face.hpp"

#include <list>

#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"
#include "fb_gfx.h"

#include "who_ai_utils.hpp"

static const char TAG[] = "App/Face";

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

AppFace::AppFace(AppButton *key,
                 AppSpeech *speech,
                 QueueHandle_t queue_i,
                 QueueHandle_t queue_o,
                 void (*callback)(camera_fb_t *)) : Frame(queue_i, queue_o, callback),
                                                    key(key),
                                                    speech(speech),
                                                    detector(0.3F, 0.3F, 10, 0.3F),
                                                    detector2(0.4F, 0.3F, 10),
                                                    state(IDLE),
                                                    switch_on(false)
{
#if CONFIG_MFN_V1
#if CONFIG_S8
    this->recognizer = new FaceRecognition112V1S8();
#elif CONFIG_S16
    this->recognizer = new FaceRecognition112V1S16();
#endif
#endif

    this->recognizer->set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    this->recognizer->set_ids_from_flash();
}

AppFace::~AppFace()
{
    delete this->recognizer;
}

void AppFace::update()
{
    // Parse key
    if (this->key->pressed > BUTTON_IDLE)
    {
        if (this->key->pressed == BUTTON_MENU)
        {
            this->switch_on = (this->key->menu == MENU_FACE_RECOGNITION) ? true : false;
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");
        }
        else if (this->key->pressed == BUTTON_PLAY)
        {
            this->state = RECOGNIZE;
        }
        else if (this->key->pressed == BUTTON_UP)
        {
            this->state = ENROLL;
        }
        else if (this->key->pressed == BUTTON_DOWN)
        {
            this->state = DELETE;
        }
    }

    // Parse speech recognition
    if (this->speech->command > COMMAND_NOT_DETECTED)
    {
        if (this->speech->command >= MENU_STOP_WORKING && this->speech->command <= MENU_MOTION_DETECTION)
        {
            this->switch_on = (this->speech->command == MENU_FACE_RECOGNITION) ? true : false;
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");
        }
        else if (this->speech->command == ACTION_ENROLL)
        {
            this->state = ENROLL;
        }
        else if (this->speech->command == ACTION_RECOGNIZE)
        {
            this->state = RECOGNIZE;
        }
        else if (this->speech->command == ACTION_DELETE)
        {
            this->state = DELETE;
        }
    }
    ESP_LOGD(TAG, "Human face recognition state = %d", this->state);
}

static void task(AppFace *self)
{
    ESP_LOGD(TAG, "Start");
    camera_fb_t *frame = nullptr;

    while (true)
    {
        if (self->queue_i == nullptr)
            break;

        if (xQueueReceive(self->queue_i, &frame, portMAX_DELAY))
        {
            if (self->switch_on)
            {
                std::list<dl::detect::result_t> &detect_candidates = self->detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                std::list<dl::detect::result_t> &detect_results = self->detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

                if (detect_results.size())
                {
                    // print_detection_result(detect_results);
                    draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                }

                if (self->state && detect_results.size() == 1)
                {
                    switch (self->state)
                    {
                    case ENROLL:
                        self->recognizer->enroll_id((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint, "", true);
                        ESP_LOGI(TAG, "Enroll ID %d", self->recognizer->get_enrolled_ids().back().id);
                        break;

                    case RECOGNIZE:
                        self->recognize_result = self->recognizer->recognize((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
                        // print_detection_result(detect_results);
                        ESP_LOGD(TAG, "Similarity: %f", self->recognize_result.similarity);
                        if (self->recognize_result.id > 0)
                            ESP_LOGI(TAG, "Match ID: %d", self->recognize_result.id);
                        else
                            ESP_LOGI(TAG, "Match ID: %d", self->recognize_result.id);
                        break;

                    case DELETE:
                        vTaskDelay(10);
                        self->recognizer->delete_id(true);
                        ESP_LOGI(TAG, "%d IDs left", self->recognizer->get_enrolled_id_num());
                        break;

                    default:
                        break;
                    }

                    self->state_previous = self->state;
                    self->state = IDLE;
                    self->frame_count = FRAME_DELAY_NUM;
                }

                // Write result on several frames of image
                if (self->frame_count)
                {
                    switch (self->state_previous)
                    {
                    case DELETE:
                        rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", self->recognizer->get_enrolled_id_num());
                        break;

                    case RECOGNIZE:
                        if (self->recognize_result.id > 0)
                            rgb_printf(frame, RGB565_MASK_GREEN, "ID %d", self->recognize_result.id);
                        else
                            rgb_print(frame, RGB565_MASK_RED, "who ?");
                        break;

                    case ENROLL:
                        rgb_printf(frame, RGB565_MASK_BLUE, "Enroll: ID %d", self->recognizer->get_enrolled_ids().back().id);
                        break;

                    default:
                        break;
                    }

                    self->frame_count--;
                }
            }

            if (self->queue_o)
                xQueueSend(self->queue_o, &frame, portMAX_DELAY);
            else
                self->callback(frame);
        }
    }
    ESP_LOGD(TAG, "Stop");
    vTaskDelete(NULL);
}

void AppFace::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)task, TAG, 5 * 1024, this, 5, NULL, 1);
}