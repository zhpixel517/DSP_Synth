#include "header.hpp"

UIState currentUIState = STATE_SYNTH;
File root;
int fileCount = 0;
int selectFileIndex = 0;
String fileNames[MAX_FILES];

bool lastSwitchButtonState = HIGH;
unsigned long lastSwitchDebounceTime = 0;
bool lastSelectButtonState = HIGH;
unsigned long lastSelectDebounceTime = 0;
bool switchState = HIGH;
bool selectState = HIGH;

Encoder volumeControl(encoder_A, encoder_B);
long oldPosition = -999;
float currentVolume = 0.0;

Button_Sound buttonFrequencies[NUM_KEYS] = {
  {33, 262, 0.7}, // C
  {38, 277, 0.7}, // C#
  {35, 294, 0.7}, // D
  {37, 311, 0.7}, // D#
  {34, 330, 0.7}, // E
  {39, 349, 0.7}, // F
  {36, 370, 0.7}, // F#
  {41, 392, 0.7}, // G
  {40, 415, 0.7}, // G#
  {22, 440, 0.1}, // A
  {43, 466, 0.1}, // A#
  {14, 494, 0.1}, // B
  {17, 523, 0.1}  // C
};

DebouncedButton pitchUpBtn(pitch_up);
DebouncedButton pitchDownBtn(pitch_down);

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
uint16_t buf1[W * 20] __attribute__((aligned(4)));
uint16_t buf2[W * 20] __attribute__((aligned(4)));