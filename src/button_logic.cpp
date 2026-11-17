#include "header.hpp"
#include <cstdint>
#include "button_logic.hpp"
#include "event_queue.hpp"

// button action mappings

ButtonSound idToNoteMap[NUM_NOTE_KEYS] = { // TODO: refactor to trellis button id / tone map
  {33, 262, 0.7}, // C
  {38, 277, 0.7}, // C#
  {35, 294, 0.7}, // D
  {37, 311, 0.7}, // D#
  {34, 330, 0.7}, // E
  {39, 349, 0.7}, // F
  {36, 370, 0.7}, // F#
  {41, 392, 0.7}, // G
  {40, 415, 0.7}, // G#
  {22, 440, 0.7}, // A
  {43, 466, 0.7}, // A#
  {14, 494, 0.7}, // B
  {17, 523, 0.7}  // C
};

(void*) keyIdToActionMap[NUM_KEYS] = {
};

// ========= TRELLIS KEYPAD CONFIG ==========

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

// ========= EVENT HANDLING ==========

static EventQueue* event_queue_ptr = nullptr;

void handleKeyEvents(EventQueue& event_queue, PlayEngine& playEngine) {
  Event e;
  while (event_queue.consume_once(e)) {
    // buttonActionMap[e.id]();
    switch (e.kind) {
      case EventType::NotePress:
        // playNote(e.id);
        // if (e.id >= 0 && e.id < NUM_KEYS) {
        //   setKeyAmplitude(e.id, e.value * buttonFrequencies[e.id].amplitude);
        // }
        break;
      case EventType::EncoderUpdate:
        break; // todo
      case EventType::GenericButton:
       // functionMap[e.id]();
    }
  }
}

// trellis.begin() -> Init
// trellis.setPixelColor(i, color) -> Color change
// trellis.show() -> Update board
// trellis.read() -> Checks buttons on both boards

// Goal -> Connect button array to new board loadout
// Goal -> Then, have the button's change light when pressed, then go back to a neutral color
// Goal -> Neutral color -- White, color change (Cool if it was a random color)...