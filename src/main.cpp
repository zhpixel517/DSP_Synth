#include "button_logic.hpp"
#include "event_queue.hpp"
#include "header.hpp"

static PlayEngine playEngine = PlayEngine();
static EventQueue eventQueue = EventQueue();

// enum Machine_State {
//   Main_Menu = 0,
//   SD_Select_Menu = 1,
//   Piano_Mode = 2,
//   Beat_Maker = 3,
//   Cut_Songs = 4,
//   Tool_Menu = 5
// };

void setup() {
  Serial.begin(115200);

  init_trellis_keys(eventQueue);

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
  trellis.read();
  displayUpdate();
  displayAll();

  playEngine.handlePitchUp(&pitchUpBtn);
  playEngine.handlePitchDown(&pitchDownBtn);
  playEngine.handleKeyEvents(eventQueue);

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






// #include <Arduino.h>
// #include <Audio.h>

// #define CHORUS_DELAY_LENGTH (16 * AUDIO_BLOCK_SAMPLES)
// short chorusDelayLine[CHORUS_DELAY_LENGTH];

// AudioSynthWaveform      osc1, osc2, osc3;
// AudioMixer4             mixer;
// AudioFilterStateVariable filter;
// AudioEffectEnvelope     env;
// AudioEffectChorus       chorus;
// AudioEffectFreeverb     reverb;
// AudioOutputMQS          mqs;

// AudioConnection c1(osc1, 0, mixer, 0);
// AudioConnection c2(osc2, 0, mixer, 1);
// AudioConnection c3(osc3, 0, mixer, 2);
// AudioConnection c4(mixer, 0, filter, 0);
// AudioConnection c5(filter, 0, env, 0);       // output 0 = low-pass
// AudioConnection c6(env, 0, chorus, 0);
// AudioConnection c7(chorus, 0, reverb, 0);
// AudioConnection c8(reverb, 0, mqs, 0);
// AudioConnection c9(reverb, 0, mqs, 1);

// const float NOTE = 261.63f;  // C4; try 523.25f for C5

// void setup() {
//   AudioMemory(40);

//   osc1.begin(0.3f, NOTE,           WAVEFORM_SAWTOOTH);
//   osc2.begin(0.3f, NOTE * 1.003f,  WAVEFORM_SAWTOOTH);  // ~+5 cents
//   osc3.begin(0.3f, NOTE * 0.997f,  WAVEFORM_SAWTOOTH);  // ~-5 cents

//   mixer.gain(0, 0.33f);
//   mixer.gain(1, 0.33f);
//   mixer.gain(2, 0.33f);

//   filter.frequency(4500);
//   filter.resonance(0.7f);

//   env.attack(300);
//   env.decay(0);
//   env.sustain(1.0f);
//   env.release(1500);

//   chorus.begin(chorusDelayLine, CHORUS_DELAY_LENGTH, 2);

//   reverb.roomsize(0.8f);
//   reverb.damping(0.5f);
// }

// void loop() {
//   env.noteOn();
//   delay(3000);
//   env.noteOff();
//   delay(2000);
// }
