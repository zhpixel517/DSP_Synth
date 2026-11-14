#pragma once
#include "header.hpp"
#include "event_queue.hpp"

extern Adafruit_NeoTrellis t_array[1][2];
extern Adafruit_MultiTrellis trellis;

extern uint32_t color;
extern uint32_t color_2;
extern uint32_t brightness;
extern uint32_t brightness_2;
extern uint32_t hue;
extern uint32_t hue_2;

#define Y_DIM 4 //number of rows of key
#define X_DIM 8 //number of columns of keys

void basic_lighting();
TrellisCallback on_trellis_key_event(keyEvent evt);
void init_trellis_keys(EventQueue& queue_ptr);
