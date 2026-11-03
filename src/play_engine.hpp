#pragma once
#include "header.hpp"

#define GRANULAR_MEMORY_SIZE 12800

class PlayEngine {
    private:
        AudioSynthWaveformSine sine[NUM_KEYS];

        AudioMixer4 leftMixer1, leftMixer2, leftMixer3, leftMixer4;
        AudioMixer4 rightMixer1, rightMixer2, rightMixer3, rightMixer4;

        AudioMixer4 masterLeft;
        AudioMixer4 masterRight;

        AudioPlaySdWav playWav1;
        AudioOutputMQS mqs1;

        // SD WAV player -> granular -> spare input on mixer 4
        AudioEffectGranular granularL;
        AudioEffectGranular granularR;

        int16_t granularMemoryL[GRANULAR_MEMORY_SIZE];
        int16_t granularMemoryR[GRANULAR_MEMORY_SIZE];

        int pitchStepCounter;
    

    public:
        PlayEngine();

        void handlePitchUp(DebouncedButton* button);
        void handlePitchDown(DebouncedButton* button);

        void audioInit();
        void setKeyFrequency(int key, float freq);
        void setKeyAmplitude(int key, float amp);
        void silenceAllKeys();
        void setMasterVolume(float v);
        void setSongSpeed(float ratio);
        void playFile(const char* name);
        void applyPitch();
};

