#include "app_video.h"
#include "human_face_detect.hpp"

void draw_rectangle_on_canvas(lv_obj_t *canvas, const std::vector<int> &box) {
  lv_draw_rect_dsc_t rect_dsc;
  lv_draw_rect_dsc_init(&rect_dsc);
  rect_dsc.bg_opa = LV_OPA_TRANSP;
  rect_dsc.border_width = 2;
  rect_dsc.border_color = lv_palette_main(LV_PALETTE_RED);
  rect_dsc.radius = 5;

  lv_area_t coords_rect;
  coords_rect.x1 = box[0];
  coords_rect.y1 = box[1];
  coords_rect.x2 = box[2];
  coords_rect.y2 = box[3];

  lv_layer_t layer;
  lv_canvas_init_layer(canvas, &layer);
  lv_draw_rect(&layer, &rect_dsc, &coords_rect);
  lv_canvas_finish_layer(canvas, &layer);
}

void draw_landmarks_on_canvas(lv_obj_t *canvas,
                              const std::vector<int> &landmarks) {
  assert(landmarks.size() == 10);
  lv_layer_t layer;
  lv_canvas_init_layer(canvas, &layer);
  lv_draw_arc_dsc_t dsc;
  lv_draw_arc_dsc_init(&dsc);
  dsc.color = lv_palette_main(LV_PALETTE_RED);
  dsc.width = 5;
  dsc.radius = 5;
  dsc.start_angle = 0;
  dsc.end_angle = 360;

  for (int i = 0; i < 5; i++) {
    dsc.center.x = landmarks[2 * i];
    dsc.center.y = landmarks[2 * i + 1];
    lv_draw_arc(&layer, &dsc);
  }

  lv_canvas_finish_layer(canvas, &layer);
}

extern "C" void app_main(void) {
  bsp_display_start();
  ESP_ERROR_CHECK(bsp_display_backlight_on());
  ESP_ERROR_CHECK(video_init(VIDEO_FMT_RGB565, 2));

  // Create LVGL canvas for camera image
  bsp_display_lock(0);
  lv_obj_t *camera_canvas = lv_canvas_create(lv_scr_act());
  lv_obj_center(camera_canvas);
  bsp_display_unlock();

  HumanFaceDetect *detect = new HumanFaceDetect();

  while (1) {
    video_fb_t *fb = video_fb_get();
    bsp_display_lock(0);
    lv_canvas_set_buffer(camera_canvas, fb->buf, 1280, 720,
                         LV_COLOR_FORMAT_NATIVE);
    auto detect_results = detect->run((uint16_t *)fb->buf, {720, 1280, 3});
    for (const auto &res : detect_results) {
      draw_rectangle_on_canvas(camera_canvas, res.box);
      draw_landmarks_on_canvas(camera_canvas, res.keypoint);
    }
    bsp_display_unlock();
    video_fb_return();
  }
}