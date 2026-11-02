#pragma once

void audioInit();
void setKeyFrequency(int key, float freq);
void setKeyAmplitude(int key, float amp);
void silenceAllKeys();
void setMasterVolume(float v);
void setSongSpeed(float ratio);
void playFile(const char* name);