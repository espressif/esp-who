#include "who_color_detection.hpp"

#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"
#include "fb_gfx.h"
#include "color_detector.hpp"

#include "who_ai_utils.hpp"

using namespace std;
using namespace dl;

static const char *TAG = "color_detection";

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueEvent = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static QueueHandle_t xQueueResult = NULL;

static color_detection_state_t gEvent = COLOR_DETECTION_IDLE;
static bool register_mode = false;
static bool gReturnFB = true;
static bool draw_box = true;

static SemaphoreHandle_t xMutex;

vector<color_info_t> std_color_info = {{{156, 10, 70, 255, 90, 255}, 64, "red"},
                                       {{11, 22, 70, 255, 90, 255}, 64, "orange"},
                                       {{23, 33, 70, 255, 90, 255}, 64, "yellow"},
                                       {{34, 75, 70, 255, 90, 255}, 64, "green"},
                                       {{76, 96, 70, 255, 90, 255}, 64, "cyan"},
                                       {{97, 124, 70, 255, 90, 255}, 64, "blue"},
                                       {{125, 155, 70, 255, 90, 255}, 64, "purple"},
                                       {{0, 180, 0, 40, 220, 255}, 64, "white"},
                                       {{0, 180, 0, 50, 50, 219}, 64, "gray"},
                                    //    {{0, 180, 0, 255, 0, 45}, 64, "black"}
                                       };

#define RGB565_LCD_RED 0x00F8
#define RGB565_LCD_ORANGE 0x20FD
#define RGB565_LCD_YELLOW 0xE0FF
#define RGB565_LCD_GREEN 0xE007
#define RGB565_LCD_CYAN 0xFF07
#define RGB565_LCD_BLUE 0x1F00
#define RGB565_LCD_PURPLE 0x1EA1
#define RGB565_LCD_WHITE 0xFFFF
#define RGB565_LCD_GRAY 0x1084
#define RGB565_LCD_BLACK 0x0000

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

static void draw_color_detection_result(uint16_t *image_ptr, int image_height, int image_width, vector<color_detect_result_t> &results, uint16_t color)
{
    for (int i = 0; i < results.size(); ++i)
    {
        dl::image::draw_hollow_rectangle(image_ptr, image_height, image_width,
                                         results[i].box[0],
                                         results[i].box[1],
                                         results[i].box[2],
                                         results[i].box[3],
                                         color);
    }
}

static void task_process_handler(void *arg)
{
    camera_fb_t *frame = NULL;

    ColorDetector detector;
    detector.set_detection_shape({80, 80, 1});
    for (int i = 0; i < std_color_info.size(); ++i)
    {
        detector.register_color(std_color_info[i].color_thresh, std_color_info[i].area_thresh, std_color_info[i].name);
    }

    vector<vector<int>> color_thresh_boxes = {{110, 110, 130, 130}, {100, 100, 140, 140}, {90, 90, 150, 150}, {80, 80, 160, 160}, {60, 60, 180, 180}, {40, 40, 200, 200}, {20, 20, 220, 220}};
    int color_thresh_boxes_num = color_thresh_boxes.size();
    int color_thresh_boxes_index = color_thresh_boxes_num / 2;
    vector<int> color_area_threshes = {1, 4, 16, 32, 64, 128, 256, 512, 1024};
    int color_area_thresh_num = color_area_threshes.size();
    int color_area_thresh_index = color_area_thresh_num / 2;
    
    detector.set_area_thresh({color_area_threshes[color_area_thresh_index]});


    vector<uint16_t> draw_lcd_colors = {RGB565_LCD_RED, 
                                        RGB565_LCD_ORANGE, 
                                        RGB565_LCD_YELLOW,
                                        RGB565_LCD_GREEN, 
                                        RGB565_LCD_CYAN, 
                                        RGB565_LCD_BLUE,
                                        RGB565_LCD_PURPLE, 
                                        RGB565_LCD_WHITE, 
                                        RGB565_LCD_GRAY, 
                                        // RGB565_LCD_BLACK
                                        };
    int draw_colors_num = draw_lcd_colors.size();

    color_detection_state_t _gEvent;
    vector<uint8_t> color_thresh;

    while (true)
    {
        xSemaphoreTake(xMutex, portMAX_DELAY);
        _gEvent = gEvent;
        gEvent = COLOR_DETECTION_IDLE;
        xSemaphoreGive(xMutex);

        if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
        {
            if (register_mode)
            {
                switch (_gEvent)
                {
                case INCREASE_COLOR_AREA:
                    color_thresh_boxes_index = min(color_thresh_boxes_num - 1, color_thresh_boxes_index + 1);
                    ets_printf("increase color area\n");
                    dl::image::draw_hollow_rectangle((uint16_t *)frame->buf, (int)frame->height, (int)frame->width,
                                                     color_thresh_boxes[color_thresh_boxes_index][0],
                                                     color_thresh_boxes[color_thresh_boxes_index][1],
                                                     color_thresh_boxes[color_thresh_boxes_index][2],
                                                     color_thresh_boxes[color_thresh_boxes_index][3],
                                                     draw_lcd_colors[1]);
                    break;

                case DECREASE_COLOR_AREA:
                    color_thresh_boxes_index = max(0, color_thresh_boxes_index - 1);
                    ets_printf("decrease color area\n");
                    dl::image::draw_hollow_rectangle((uint16_t *)frame->buf, (int)frame->height, (int)frame->width,
                                                     color_thresh_boxes[color_thresh_boxes_index][0],
                                                     color_thresh_boxes[color_thresh_boxes_index][1],
                                                     color_thresh_boxes[color_thresh_boxes_index][2],
                                                     color_thresh_boxes[color_thresh_boxes_index][3],
                                                     draw_lcd_colors[1]);
                    break;

                case REGISTER_COLOR:
                    color_thresh = detector.cal_color_thresh((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, color_thresh_boxes[color_thresh_boxes_index]);
                    detector.register_color(color_thresh);
                    ets_printf("register color, color_thresh: %d, %d, %d, %d, %d, %d\n", color_thresh[0], color_thresh[1], color_thresh[2], color_thresh[3], color_thresh[4], color_thresh[5]);
                    // detector.register_color((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, color_thresh_boxes[color_thresh_boxes_index]);
                    xSemaphoreTake(xMutex, portMAX_DELAY);
                    register_mode = false;
                    xSemaphoreGive(xMutex);
                    break;

                case CLOSE_REGISTER_COLOR_BOX:
                    ets_printf("close register color box \n");
                    xSemaphoreTake(xMutex, portMAX_DELAY);
                    register_mode = false;
                    xSemaphoreGive(xMutex);
                    break;

                default:
                    dl::image::draw_hollow_rectangle((uint16_t *)frame->buf, (int)frame->height, (int)frame->width,
                                                     color_thresh_boxes[color_thresh_boxes_index][0],
                                                     color_thresh_boxes[color_thresh_boxes_index][1],
                                                     color_thresh_boxes[color_thresh_boxes_index][2],
                                                     color_thresh_boxes[color_thresh_boxes_index][3],
                                                     draw_lcd_colors[1]);
                    break;
                }
            }
            else
            {
                switch (_gEvent)
                {
                case INCREASE_COLOR_AREA:
                    color_area_thresh_index = min(color_area_thresh_num - 1, color_area_thresh_index + 1);
                    detector.set_area_thresh({color_area_threshes[color_area_thresh_index]});
                    ets_printf("increase color area thresh to %d\n", color_area_threshes[color_area_thresh_index]);
                    break;

                case DECREASE_COLOR_AREA:
                    color_area_thresh_index = max(0, color_area_thresh_index - 1);
                    detector.set_area_thresh({color_area_threshes[color_area_thresh_index]});
                    ets_printf("decrease color area thresh to %d\n", color_area_threshes[color_area_thresh_index]);
                    break;

                case DELETE_COLOR:
                    detector.delete_color();
                    ets_printf("delete color \n");
                    break;

                default:
                    std::vector<std::vector<color_detect_result_t>> &results = detector.detect((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                    if (draw_box)
                    {
                        for (int i = 0; i < results.size(); ++i)
                        {
                            draw_color_detection_result((uint16_t *)frame->buf, (int)frame->height, (int)frame->width, results[i], draw_lcd_colors[i % draw_colors_num]);
                        }
                    }
                    else
                    {
                        detector.draw_segmentation_results((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, draw_lcd_colors, true, 0x0000);
                    }
                    break;
                }

            }

            // if (frame_show_state != SHOW_STATE_IDLE)
            // {
            //     static int frame_count = 0;
            //     switch (frame_show_state)
            //     {
            //     case SHOW_STATE_DELETE:
            //         rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", recognizer->get_enrolled_id_num());
            //         break;

            //     case SHOW_STATE_RECOGNIZE:
            //         if (recognize_result.id > 0)
            //             rgb_printf(frame, RGB565_MASK_GREEN, "ID %d", recognize_result.id);
            //         else
            //             rgb_print(frame, RGB565_MASK_RED, "who ?");
            //         break;

            //     case SHOW_STATE_ENROLL:
            //         rgb_printf(frame, RGB565_MASK_BLUE, "Enroll: ID %d", recognizer->get_enrolled_ids().back().id);
            //         break;

            //     default:
            //         break;
            //     }

            //     if (++frame_count > FRAME_DELAY_NUM)
            //     {
            //         frame_count = 0;
            //         frame_show_state = SHOW_STATE_IDLE;
            //     }
            // }

            // if (detect_results.size())
            // {
            //     draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
            // }
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
    }
}

static void task_event_handler(void *arg)
{
    color_detection_state_t _gEvent;
    while (true)
    {
        xQueueReceive(xQueueEvent, &(_gEvent), portMAX_DELAY);
        xSemaphoreTake(xMutex, portMAX_DELAY);
        gEvent = _gEvent;
        xSemaphoreGive(xMutex);
        if (gEvent == OPEN_REGISTER_COLOR_BOX)
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            register_mode = true;
            xSemaphoreGive(xMutex);
        }
        else if(gEvent == SWITCH_RESULT)
        {
            draw_box = !draw_box;
        }
    }
}

void register_color_detection(const QueueHandle_t frame_i,
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

    xTaskCreatePinnedToCore(task_process_handler, TAG, 4 * 1024, NULL, 5, NULL, 1);
    if (xQueueEvent)
        xTaskCreatePinnedToCore(task_event_handler, TAG, 4 * 1024, NULL, 5, NULL, 1);
}
