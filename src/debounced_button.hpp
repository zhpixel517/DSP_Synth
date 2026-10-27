#pragma once
#include "core_pins.h"

class DebouncedButton {
    private: 
        int pin;
        bool lastReading;
        bool stable;
        unsigned long changedAt;

    public: 
        DebouncedButton(int p);

        bool pressed();
};