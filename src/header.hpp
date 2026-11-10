#include <Arduino.h>
#include <SD.h>
#include <Audio.h>
#include <Encoder.h>
#include <lvgl.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Adafruit_NeoTrellis.h"
#include <Array.h> 
#include <Adafruit_NeoPixel.h>

#include "button_logic.hpp"
#include "debounced_button.hpp"
#include "config.hpp"
#include "play_engine.hpp"
#include "functions.hpp"
#include "display.hpp"

