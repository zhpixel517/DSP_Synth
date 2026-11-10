#include "header.hpp"

static lv_obj_t * label;
static lv_obj_t * volumeBar;

// Only used in this file, so it can be static again
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((uint16_t *)px_map, w * h);
  tft.endWrite();

  lv_display_flush_ready(disp);
}

void displayInit() {
  tft.begin();
  tft.setRotation(1);

  lv_init();   // must be first LVGL call
  lv_tick_set_cb([]() -> uint32_t { return millis(); });

  lv_display_t *disp = lv_display_create(W, H);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(disp, flush_cb);
  lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_obj_t * screen = lv_screen_active();
  lv_obj_set_style_bg_color(screen, lv_color_white(), 0);

  label = lv_label_create(screen);  
  volumeBar = lv_bar_create(screen);

  lv_label_set_text(label, "VOLUME");
  lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -35);

  lv_obj_set_size(volumeBar, 200, 20);
  lv_obj_align(volumeBar, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_bar_set_range(volumeBar, 0, 100);
  lv_bar_set_value(volumeBar, 0, LV_ANIM_OFF);
}

void displayUpdate() {
  static uint32_t last = 0;
  if (millis() - last >= 5) {
    last = millis();
    lv_timer_handler();
  }
}
  
// Handles display during void loop function in main
void displayAll() { 
  int v = (int)(currentVolume * 100);            
  if (v < 0) v = 0;
  if (v > 100) v = 100;
  lv_bar_set_value(volumeBar, v, LV_ANIM_OFF);

}