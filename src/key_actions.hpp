#pragma once

#include <array>
#include "play_engine.hpp"
#include <button_logic.hpp>
#include <cstdint>

#define NUM_TRELLIS_KEYS 32U

using KeyHandler = void (*)(PlayEngine& playEngine, uint8_t id, int8_t value);

void handleKeyEvents(EventQueue &event_queue, PlayEngine &playEngine);