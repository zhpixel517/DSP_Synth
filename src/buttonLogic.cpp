#include "buttonLogic.hpp"

uint32_t color = 0x200020;

Adafruit_NeoTrellis t_array[1][2] = {
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x2F) }
};

Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, 1, 2);

void basic_lighting() {
  for (int i = 0; i < 32; i++) {
    trellis.setPixelColor(i, color); 
  }
  trellis.show();
}

// trellis.begin() -> Init
// trellis.setPixelColor(i, color) -> Color change
// trellis.show() -> Update board
// trellis.read() -> Checks buttons on both boards