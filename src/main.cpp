#include "core_pins.h"
#include <Arduino.h>
#include <SD.h>
#include <Audio.h>
#include <Encoder.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Button assignments with different waves for simultaneous play
AudioSynthWaveformSine sine[8];

AudioMixer4 leftMixer1;   
AudioMixer4 leftMixer2;   
AudioMixer4 rightMixer1;  
AudioMixer4 rightMixer2;  

AudioMixer4 masterLeft;
AudioMixer4 masterRight;

AudioPlaySdWav playWav1;

AudioOutputMQS mqs1;

AudioConnection patchL1_0(sine[0], 0, leftMixer1, 0);
AudioConnection patchL1_1(sine[1], 0, leftMixer1, 1);
AudioConnection patchL1_2(sine[2], 0, leftMixer1, 2);
AudioConnection patchL1_3(sine[3], 0, leftMixer1, 3);

AudioConnection patchR1_0(sine[0], 0, rightMixer1, 0);
AudioConnection patchR1_1(sine[1], 0, rightMixer1, 1);
AudioConnection patchR1_2(sine[2], 0, rightMixer1, 2);
AudioConnection patchR1_3(sine[3], 0, rightMixer1, 3);

AudioConnection patchL2_4(sine[4], 0, leftMixer2, 0);
AudioConnection patchL2_5(sine[5], 0, leftMixer2, 1);
AudioConnection patchL2_6(sine[6], 0, leftMixer2, 2);
AudioConnection patchL2_7(sine[7], 0, leftMixer2, 3);

AudioConnection patchR2_4(sine[4], 0, rightMixer2, 0);
AudioConnection patchR2_5(sine[5], 0, rightMixer2, 1);
AudioConnection patchR2_6(sine[6], 0, rightMixer2, 2);
AudioConnection patchR2_7(sine[7], 0, rightMixer2, 3);

AudioConnection patchMasterL1(leftMixer1, 0, masterLeft, 0);
AudioConnection patchMasterL2(leftMixer2, 0, masterLeft, 1);

AudioConnection patchMasterR1(rightMixer1, 0, masterRight, 0);
AudioConnection patchMasterR2(rightMixer2, 0, masterRight, 1);

// SD WAV player:
AudioEffectGranular granularL;
AudioEffectGranular granularR;

AudioConnection patchWavToGranL(playWav1, 0, granularL, 0);
AudioConnection patchWavToGranR(playWav1, 1, granularR, 0);
AudioConnection patchGranL(granularL, 0, masterLeft, 2);
AudioConnection patchGranR(granularR, 0, masterRight, 2);

AudioConnection patchOutL(masterLeft, 0, mqs1, 0);
AudioConnection patchOutR(masterRight, 0, mqs1, 1);

// Volume control + menu selection + state switching
const int encoder_A = 32;
const int encoder_B = 31;
const int encoder_button = 30;
const int switch_button = 12;
const int countsPerClick = 4;

// 16x2 display (menu and state information)
const int display_scl = 18;
const int display_sda = 19;

enum UIState {STATE_SYNTH, STATE_SD_MENU};
UIState currentUIState = STATE_SYNTH;

File root;
int fileCount = 0;
int selectFileIndex = 0;

const int MAX_FILES = 100;
String fileNames[MAX_FILES];

bool lastSwitchButtonState = HIGH;
unsigned long lastSwitchDebounceTime = 0;
bool lastSelectButtonState = HIGH;
unsigned long lastSelectDebounceTime = 0;
unsigned long debounceDelay = 50;
bool switchState = HIGH;
bool selectState = HIGH;

// Encoder init (kind of)
Encoder volumeControl(encoder_A, encoder_B);
long oldPosition = -999;
float currentVolume = 0.5;

// Button information
struct Button_Sound {
  int pin; 
  int frequency;
  float amplitude;
};

// Individual button information
Button_Sound buttons[8] = {
  {33, 262, 0.7}, 
  {35, 294, 0.6}, 
  {34, 330, 0.4},
  {39, 349, 0.4}, 
  {41, 392, 0.3}, 
  {22, 440, 0.2}, 
  {13, 494, 0.2}, 
  {17, 523, 0.2}  
};

// Sound effects (each pitch to be replaced with) 
const int sound_fx_button = 25;

// Pitch up and down buttons
const int pitch_up = 7;
const int pitch_down = 24;

#define GRANULAR_MEMORY_SIZE 12800
int16_t granularMemoryL[GRANULAR_MEMORY_SIZE];
int16_t granularMemoryR[GRANULAR_MEMORY_SIZE];

int pitchSteps = 0;   // semitones, limited to -12..+12

struct DebouncedButton {
  int pin;
  bool lastReading;
  bool stable;
  unsigned long changedAt;

  DebouncedButton(int p) : pin(p), lastReading(HIGH), stable(HIGH), changedAt(0) {}

  bool pressed() {   // true once per press
    bool r = digitalRead(pin);
    if (r != lastReading) { changedAt = millis(); lastReading = r; }
    if ((millis() - changedAt) > debounceDelay && r != stable) {
      stable = r;
      return stable == LOW;
    }
    return false;
  }
};

DebouncedButton pitchUpBtn(pitch_up);
DebouncedButton pitchDownBtn(pitch_down);

void applyPitch() {
  float ratio = pow(2.0, pitchSteps / 12.0);

  for (int i = 0; i < 8; i++) {
    sine[i].frequency(buttons[i].frequency * ratio);   // notes
  }

  granularL.setSpeed(ratio);                           // songs
  granularR.setSpeed(ratio);
}

// SD init function
void loadFilesFromSD() {
  root = SD.open("/");
  fileCount = 0;
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    String name = String(entry.name());
    String lower = name;
    lower.toLowerCase();

    if (!entry.isDirectory() && fileCount < 
    MAX_FILES && !name.startsWith(".") 
    && lower.endsWith(".wav")) {    
      fileNames[fileCount] = name;
      fileCount++;
    }
    entry.close();
  }
  root.close();
}

// Display update function
void updateDisplay() {
  lcd.clear();
  if (currentUIState == STATE_SYNTH) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Synth");
    lcd.setCursor(0, 1);
    lcd.print("Vol: ");
    lcd.print(currentVolume);
  } else if (currentUIState == STATE_SD_MENU) {
    lcd.setCursor(0, 0);
    lcd.print("SD Files:");
    lcd.setCursor(0, 1);
    if (fileCount > 0) {
      lcd.print(fileNames[selectFileIndex]);
    } else {
      lcd.print("No files found");
    }
  }
  lcd.setCursor(13, 0);
  if (pitchSteps > 0) lcd.print("+");
  lcd.print(pitchSteps);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Teensy4.1 Synth");

  if (!SD.begin(BUILTIN_SDCARD)) {
    lcd.setCursor(0, 1);
    lcd.print("No SD-Card found");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("SD card located");
    loadFilesFromSD();
  }

  delay(1500);
  lcd.clear();

  for (int i = 0; i < 8; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
    sine[i].frequency(buttons[i].frequency);
    sine[i].amplitude(0.0); 
  }

  AudioMemory(50); 
  
  granularL.begin(granularMemoryL, GRANULAR_MEMORY_SIZE);
  granularR.begin(granularMemoryR, GRANULAR_MEMORY_SIZE);
  granularL.beginPitchShift(50.0);   // grain length in ms
  granularR.beginPitchShift(50.0);
  
  float gainSetting = 0.15;

  leftMixer1.gain(0, gainSetting); leftMixer1.gain(1, gainSetting); leftMixer1.gain(2, gainSetting); leftMixer1.gain(3, gainSetting);
  leftMixer2.gain(0, gainSetting); leftMixer2.gain(1, gainSetting); leftMixer2.gain(2, gainSetting); leftMixer2.gain(3, gainSetting);
  rightMixer1.gain(0, gainSetting); rightMixer1.gain(1, gainSetting); rightMixer1.gain(2, gainSetting); rightMixer1.gain(3, gainSetting);
  rightMixer2.gain(0, gainSetting); rightMixer2.gain(1, gainSetting); rightMixer2.gain(2, gainSetting); rightMixer2.gain(3, gainSetting);
  
  masterLeft.gain(0, 0.5); masterLeft.gain(1, 0.5);
  masterRight.gain(0, 0.5); masterRight.gain(1,0.5);
  
  pinMode(encoder_A, INPUT_PULLUP);
  pinMode(encoder_B, INPUT_PULLUP);
  pinMode(encoder_button, INPUT_PULLUP);
  pinMode(switch_button, INPUT_PULLUP);
  pinMode(pitch_up, INPUT_PULLUP);
  pinMode(pitch_down, INPUT_PULLUP);

  oldPosition = volumeControl.read();

  updateDisplay();
}

void loop() {
  // Pitch up / down (affects notes and songs)
  if (pitchUpBtn.pressed() && pitchSteps < 12) {
    pitchSteps++;
    applyPitch();
    updateDisplay();
  }
  if (pitchDownBtn.pressed() && pitchSteps > -12) {
    pitchSteps--;
    applyPitch();
    updateDisplay();
  }
  
  // Button logic to switch state between SD mode and SYNTH mode
  int switchRead = digitalRead(switch_button);
  if (switchRead != lastSwitchButtonState) {
    lastSwitchDebounceTime = millis();
  }
  if ((millis() - lastSelectDebounceTime) > debounceDelay) {
    if (switchRead == LOW && lastSwitchButtonState == HIGH) {
      if (currentUIState == STATE_SYNTH) {
        currentUIState = STATE_SD_MENU;

        for (int i = 0; i < 8; i++) {
          sine[i].amplitude(0.0);
        }

      } else {
        currentUIState = STATE_SYNTH;
      }
      updateDisplay();
    }
  }
  lastSwitchButtonState = switchRead;

  // SD mode logic: encoder button plays the selected file
  int selectRead = digitalRead(encoder_button);
  if (selectRead != lastSelectButtonState) {
    lastSelectDebounceTime = millis();
  }
  if ((millis() - lastSelectDebounceTime) > debounceDelay && selectRead != selectState) {
    selectState = selectRead;
    if (selectState == LOW) {
      if (currentUIState == STATE_SD_MENU && fileCount > 0) {
        String filenameToPlay = fileNames[selectFileIndex];

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing:");
        lcd.setCursor(0, 1);
        lcd.print(filenameToPlay);

        playWav1.play(filenameToPlay.c_str());
        delay(800);
        updateDisplay();
      }
    }
  }
  lastSelectButtonState = selectRead;

  // SYNTH logic with volume control
  if (currentUIState == STATE_SYNTH) {
    for (int i = 0; i < 8; i++) {
      if (digitalRead(buttons[i].pin) == LOW) {
        sine[i].amplitude(buttons[i].amplitude); 
      } else {
        sine[i].amplitude(0.0); 
      }
    }
    long newPosition = volumeControl.read();

    if (newPosition != oldPosition) {
      long change = newPosition - oldPosition;
      oldPosition = newPosition;

      currentVolume += (change * 0.04);

      if (currentVolume > 3.0) currentVolume = 3.0;
      if (currentVolume < 0) currentVolume = 0;

      masterLeft.gain(0, currentVolume); 
      masterLeft.gain(1, currentVolume);
      masterRight.gain(0, currentVolume);
      masterRight.gain(1, currentVolume);
      updateDisplay();
    }
  }
  else if (currentUIState == STATE_SD_MENU) {
    long diff = volumeControl.read() - oldPosition;
    bool changed = false;

    while (diff >= countsPerClick) {
      if (selectFileIndex < fileCount - 1) selectFileIndex++;
      oldPosition += countsPerClick;
      diff -= countsPerClick;
      changed = true;
    }
    while (diff <= -countsPerClick) {
      if (selectFileIndex > 0) selectFileIndex--;
      oldPosition -= countsPerClick;
      diff += countsPerClick;
      changed = true;
    }

    if (changed) updateDisplay();
  }
} 