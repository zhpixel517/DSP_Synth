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

Button_Sound buttons[NUM_KEYS] = {
  {33, 262, 0.2}, // C
  {38, 277, 0.2}, // C#
  {35, 294, 0.2}, // D
  {37, 311, 0.2}, // D#
  {34, 330, 0.2}, // E
  {39, 349, 0.2}, // F
  {36, 370, 0.2}, // F#
  {41, 392, 0.2}, // G
  {40, 415, 0.2}, // G#
  {22, 440, 0.2}, // A
  {43, 466, 0.2}, // A#
  {14, 494, 0.2}, // B
  {17, 523, 0.2}  // C
};

int pitchSteps = 0;
DebouncedButton pitchUpBtn(pitch_up);
DebouncedButton pitchDownBtn(pitch_down);

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
uint16_t buf1[W * 20] __attribute__((aligned(4)));
uint16_t buf2[W * 20] __attribute__((aligned(4)));