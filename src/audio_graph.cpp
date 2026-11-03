#include "config.hpp"
#include "header.hpp"
#include "audio_graph.hpp"

AudioSynthWaveformSine sine[NUM_KEYS];

AudioMixer4 leftMixer1, leftMixer2, leftMixer3, leftMixer4;
AudioMixer4 rightMixer1, rightMixer2, rightMixer3, rightMixer4;

AudioMixer4 masterLeft;
AudioMixer4 masterRight;

AudioPlaySdWav playWav1;
AudioOutputMQS mqs1;

// Left: sines 0-3, 4-7, 8-11, 12
AudioConnection pL0(sine[0], 0, leftMixer1, 0);
AudioConnection pL1(sine[1], 0, leftMixer1, 1);
AudioConnection pL2(sine[2], 0, leftMixer1, 2);
AudioConnection pL3(sine[3], 0, leftMixer1, 3);
AudioConnection pL4(sine[4], 0, leftMixer2, 0);
AudioConnection pL5(sine[5], 0, leftMixer2, 1);
AudioConnection pL6(sine[6], 0, leftMixer2, 2);
AudioConnection pL7(sine[7], 0, leftMixer2, 3);
AudioConnection pL8(sine[8], 0, leftMixer3, 0);
AudioConnection pL9(sine[9], 0, leftMixer3, 1);
AudioConnection pL10(sine[10], 0, leftMixer3, 2);
AudioConnection pL11(sine[11], 0, leftMixer3, 3);
AudioConnection pL12(sine[12], 0, leftMixer4, 0);

// Right: same layout
AudioConnection pR0(sine[0], 0, rightMixer1, 0);
AudioConnection pR1(sine[1], 0, rightMixer1, 1);
AudioConnection pR2(sine[2], 0, rightMixer1, 2);
AudioConnection pR3(sine[3], 0, rightMixer1, 3);
AudioConnection pR4(sine[4], 0, rightMixer2, 0);
AudioConnection pR5(sine[5], 0, rightMixer2, 1);
AudioConnection pR6(sine[6], 0, rightMixer2, 2);
AudioConnection pR7(sine[7], 0, rightMixer2, 3);
AudioConnection pR8(sine[8], 0, rightMixer3, 0);
AudioConnection pR9(sine[9], 0, rightMixer3, 1);
AudioConnection pR10(sine[10], 0, rightMixer3, 2);
AudioConnection pR11(sine[11], 0, rightMixer3, 3);
AudioConnection pR12(sine[12], 0, rightMixer4, 0);

// Mixers to master
AudioConnection pML1(leftMixer1, 0, masterLeft, 0);
AudioConnection pML2(leftMixer2, 0, masterLeft, 1);
AudioConnection pML3(leftMixer3, 0, masterLeft, 2);
AudioConnection pML4(leftMixer4, 0, masterLeft, 3);

AudioConnection pMR1(rightMixer1, 0, masterRight, 0);
AudioConnection pMR2(rightMixer2, 0, masterRight, 1);
AudioConnection pMR3(rightMixer3, 0, masterRight, 2);
AudioConnection pMR4(rightMixer4, 0, masterRight, 3);

// SD WAV player -> granular -> spare input on mixer 4
AudioEffectGranular granularL;
AudioEffectGranular granularR;

AudioConnection patchWavToGranL(playWav1, 0, granularL, 0);
AudioConnection patchWavToGranR(playWav1, 1, granularR, 0);
AudioConnection patchGranL(granularL, 0, leftMixer4, 1);
AudioConnection patchGranR(granularR, 0, rightMixer4, 1);

AudioConnection patchOutL(masterLeft, 0, mqs1, 0);
AudioConnection patchOutR(masterRight, 0, mqs1, 1);


#define GRANULAR_MEMORY_SIZE 12800
int16_t granularMemoryL[GRANULAR_MEMORY_SIZE];
int16_t granularMemoryR[GRANULAR_MEMORY_SIZE];


void audioInit() {
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

  leftMixer3.gain(0, gainSetting); leftMixer3.gain(1, gainSetting); leftMixer3.gain(2, gainSetting); leftMixer3.gain(3, gainSetting);
  rightMixer3.gain(0, gainSetting); rightMixer3.gain(1, gainSetting); rightMixer3.gain(2, gainSetting); rightMixer3.gain(3, gainSetting);
  leftMixer4.gain(0, gainSetting); leftMixer4.gain(1, 1.0);  
  rightMixer4.gain(0, gainSetting); rightMixer4.gain(1, 1.0);

  setMasterVolume(currentVolume);

  masterLeft.gain(0, currentVolume); masterLeft.gain(1, currentVolume);
  masterRight.gain(0, currentVolume); masterRight.gain(1, currentVolume);
}


void setKeyFrequency(int key, float freq) { sine[key].frequency(freq); }
void setKeyAmplitude(int key, float amp)  { sine[key].amplitude(amp); }

void silenceAllKeys() {
    for (int i = 0; i < NUM_KEYS; i++) sine[i].amplitude(0.0);
}

void setMasterVolume(float v) {
    for (int i = 0; i < 4; i++) {
        masterLeft.gain(i, v);
        masterRight.gain(i, v);
    }
}

void setSongSpeed(float ratio) {
    granularL.setSpeed(ratio);
    granularR.setSpeed(ratio);
}

void playFile(const char* name) { playWav1.play(name); }