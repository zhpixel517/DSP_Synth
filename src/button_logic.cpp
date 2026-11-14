#include "header.hpp"
#include <cstdint>
#include "button_logic.hpp"
#include "event_queue.hpp"

static EventQueue* event_queue_ptr = nullptr;

// Colors: 
uint32_t color = 65000;
uint32_t color_2 = 10000;
// Hue -> 0 = White, Hue -> 255 = True Color
uint32_t hue = 0;
uint32_t hue_2 = 255;
uint32_t brightness = 1;
uint32_t brightness_2 = 40;

#define LED_ARRAY_1 0x2E
#define LED_ARRAY_2 0x2F // short-circuit soldered pin A0

//create a matrix of trellis panels
Adafruit_NeoTrellis t_array[Y_DIM/4][X_DIM/4] = {
  { Adafruit_NeoTrellis(LED_ARRAY_1), Adafruit_NeoTrellis(LED_ARRAY_2) }
};

// Init class for trellis lighting
Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, 1, 2);

// For all 32 lights (8 x 4 display), the lighting for rainbow color gradients 
void basic_lighting() {
  for (int i = 0; i < 32; i++) {
    trellis.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(color, hue, brightness));
  }
    trellis.show();
}

TrellisCallback on_trellis_key_event(keyEvent evt) {
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    trellis.setPixelColor(evt.bit.NUM, Adafruit_NeoPixel::ColorHSV(random(1000000), hue_2, brightness_2));
    trellis.show();
    Event e {EventType::NotePress, evt.bit.NUM, 1};
    event_queue_ptr->push(e); // so clean
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    trellis.setPixelColor(evt.bit.NUM, Adafruit_NeoPixel::ColorHSV(color, hue, brightness));
    trellis.show();
    (void)event_queue_ptr->push(Event{EventType::NotePress, (int8_t)evt.bit.NUM, 0});
  } 
  return 0;
}


void init_trellis_keys(EventQueue& queue_ptr) {
  event_queue_ptr = &queue_ptr; // dependency injection

  // Init lighting on button pad
  trellis.begin();

  for (int y = 0; y < Y_DIM; y++) {
    for (int x = 0; x < X_DIM; x++) {
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, on_trellis_key_event);
    }
  }

  basic_lighting();
  trellis.show();
}


// trellis.begin() -> Init
// trellis.setPixelColor(i, color) -> Color change
// trellis.show() -> Update board
// trellis.read() -> Checks buttons on both boards

// Goal -> Connect button array to new board loadout
// Goal -> Then, have the button's change light when pressed, then go back to a neutral color
// Goal -> Neutral color -- White, color change (Cool if it was a random color)...