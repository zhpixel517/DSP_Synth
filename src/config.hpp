#pragma once

#include <Arduino.h>
#include <cstdint>
#include <SD.h>
#include <Encoder.h>
#include <Adafruit_ILI9341.h>
#include "debounced_button.hpp"

#define DEBOUNCE_DELAY_MS 50

constexpr int NUM_KEYS = 13;

// Pins and constants
const int encoder_A = 32;
const int encoder_B = 31;
const int encoder_button = 30;
const int switch_button = 8;
const int countsPerClick = 4;
const int sound_fx_button = 25;
const int pitch_up = 7;
const int pitch_down = 24;
const int MAX_FILES = 100;

#define TFT_CS   4
#define TFT_DC   5
#define TFT_RST  6
#define TFT_MOSI 11
#define TFT_CLK  13
#define TFT_MISO -1

static const uint32_t W = 320, H = 240;

// Types
enum UIState {STATE_SYNTH, STATE_SD_MENU};

struct Button_Sound {
  int pin;
  int frequency;
  float amplitude;
};

// Variables defined in config.cpp
extern UIState currentUIState;
extern File root;
extern int fileCount;
extern int selectFileIndex;
extern String fileNames[MAX_FILES];

extern bool lastSwitchButtonState;
extern unsigned long lastSwitchDebounceTime;
extern bool lastSelectButtonState;
extern unsigned long lastSelectDebounceTime;
extern bool switchState;
extern bool selectState;

extern Encoder volumeControl;
extern long oldPosition;
extern float currentVolume;

extern Button_Sound buttons[NUM_KEYS];
extern int pitchSteps;
extern DebouncedButton pitchUpBtn;
extern DebouncedButton pitchDownBtn;

extern Adafruit_ILI9341 tft;
extern uint16_t buf1[W * 20];
extern uint16_t buf2[W * 20];