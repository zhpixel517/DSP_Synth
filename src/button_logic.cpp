#include "header.hpp"
#include <cstdint>
#include "button_logic.hpp"

// Colors: 
uint32_t color = 65000;
// Hue -> 0 = White, Hue -> 255 = True Color
uint32_t hue = 0;
uint32_t hue_2 = 255;
uint32_t brightness = 40;

uint32_t color_2 = 10000;

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

// trellis.begin() -> Init
// trellis.setPixelColor(i, color) -> Color change
// trellis.show() -> Update board
// trellis.read() -> Checks buttons on both boards

// Goal -> Connect button array to new board loadout
// Goal -> Then, have the button's change light when pressed, then go back to a neutral color
// Goal -> Neutral color -- White, color change (Cool if it was a random color)...