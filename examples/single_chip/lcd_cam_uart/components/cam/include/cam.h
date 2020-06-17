#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t bit_width;
    uint32_t xclk_fre;
    union {
        struct {
            uint32_t xclk:   8;
            uint32_t pclk:   8;
            uint32_t vsync:  8;
            uint32_t hsync:  8;
        };
        uint32_t val;
    } pin;
    uint8_t pin_data[16];
    uint8_t vsync_invert;
    uint8_t hsync_invert;
    union {
        struct {
            uint32_t width:   16;
            uint32_t high:    16;
        };
        uint32_t val;
    } size;
    uint32_t max_buffer_size; // DMA used
    uint32_t task_stack;
    uint8_t task_pri;
    union {
        struct {
            uint32_t jpeg:   1; 
        };
        uint32_t val;
    } mode;
    uint8_t *frame1_buffer;
    uint8_t *frame2_buffer;
} cam_config_t;

void cam_start(void);
void cam_stop(void);
size_t cam_take(uint8_t **buffer_p);
void cam_give(uint8_t *buffer);
void cam_deinit();
int cam_init(const cam_config_t *config);

#ifdef __cplusplus
}
#endif

