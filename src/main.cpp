#include "buttonLogic.hpp"
#include "header.hpp"
#include <config.hpp>

static PlayEngine playEngine = PlayEngine();

void setup() {
  Serial.begin(115200);
  
  trellis.begin();
  basic_lighting();
  trellis.show();

  displayInit();
  playEngine.audioInit();

  for (int i = 0; i < NUM_KEYS; i++) {
    pinMode(buttonFrequencies[i].pin, INPUT_PULLUP);
  }

  pinMode(encoder_A, INPUT_PULLUP);
  pinMode(encoder_B, INPUT_PULLUP);
  pinMode(encoder_button, INPUT_PULLUP);
  pinMode(switch_button, INPUT_PULLUP);
  pinMode(pitch_up, INPUT_PULLUP);
  pinMode(pitch_down, INPUT_PULLUP);

  oldPosition = volumeControl.read();  
}

void loop() {
  displayUpdate();
  displayAll();

  playEngine.handlePitchUp(&pitchUpBtn);
  playEngine.handlePitchDown(&pitchDownBtn);

  // Switch between SD mode and SYNTH mode
  int switchRead = digitalRead(switch_button);
  if (switchRead != lastSwitchButtonState) {
    lastSwitchDebounceTime = millis();
  }
  if ((millis() - lastSwitchDebounceTime) > DEBOUNCE_DELAY_MS) {
    if (switchRead == LOW && lastSwitchButtonState == HIGH) {
      if (currentUIState == STATE_SYNTH) {
        currentUIState = STATE_SD_MENU;
        playEngine.silenceAllKeys();
      } else {
        currentUIState = STATE_SYNTH;
      }
    }
  }
  lastSwitchButtonState = switchRead;

  // SD mode: encoder button plays the selected file
  int selectRead = digitalRead(encoder_button);
  if (selectRead != lastSelectButtonState) {
    lastSelectDebounceTime = millis();
  }
  if ((millis() - lastSelectDebounceTime) > DEBOUNCE_DELAY_MS && selectRead != selectState) {
    selectState = selectRead;
    if (selectState == LOW) {
      if (currentUIState == STATE_SD_MENU && fileCount > 0) {
        String filenameToPlay = fileNames[selectFileIndex];

        // INSERT Display Logic

        playEngine.playFile(filenameToPlay.c_str());
        delay(800);
      }
    }
  }
  lastSelectButtonState = selectRead;

  // SYNTH logic with volume control
  if (currentUIState == STATE_SYNTH) {
    for (int i = 0; i < NUM_KEYS; i++) {
      if (digitalRead(buttonFrequencies[i].pin) == LOW) {
        playEngine.setKeyAmplitude(i, buttonFrequencies[i].amplitude);
      } else {
        playEngine.setKeyAmplitude(i, 0.0);
      }
    }
    long newPosition = volumeControl.read();

    if (newPosition != oldPosition) {
      long change = newPosition - oldPosition;
      oldPosition = newPosition;

      currentVolume += (change * 0.04);

      if (currentVolume > 1.0) currentVolume = 1.0;
      if (currentVolume < 0) currentVolume = 0;

      playEngine.setMasterVolume(currentVolume);
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

    if (changed) printf("Hello! ");
  }
}




