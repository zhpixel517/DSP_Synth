#include "debounced_button.hpp"
#include "config.hpp"

DebouncedButton::DebouncedButton(int pin) : pin(pin), lastReading(HIGH), stable(HIGH), changedAt(0) {}

bool DebouncedButton::pressed() {
    bool r = digitalRead(pin);
    if (r != lastReading) { changedAt = millis(); lastReading = r; }
    if ((millis() - changedAt) > DEBOUNCE_DELAY_MS && r != stable) {
      stable = r;
      return stable == LOW;
    }
    return false;
  
}