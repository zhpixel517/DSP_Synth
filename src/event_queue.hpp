#pragma once
#include <algorithm>
#include <Arduino.h>
#include <array>
#include <cstdint>

enum class EventType {
    NotePress, // an LED note key pad was pressed
    EncoderUpdate, // an encoder was rotated
    EventQueueError
};

// generic event queue for button + encoder events
struct Event {
    EventType kind;
    int8_t id;
    int8_t value;
};

// FIFO buffer for key press events. I used std::array here to perevent runtime dynamic allocation, this helps it stay performant
// std::array size property is always fixed, so checking the state is always dependent on the read and write index
class EventQueue {

    public:
        [[nodiscard]] bool push(Event event); // boolean indicates success

        [[nodiscard]] bool consume_once(Event& out); // pop the oldest item and read it

        // number of events inside buffer
        uint8_t length() const {
            // distance from read to write with wrap-around
            return (_write_index - _read_index + event_buffer_length) % event_buffer_length;
        }

        bool full() const {
            // next write index equals read index. this always enforces 1 slot reserved rule
            return next_index_write() == _read_index;
        }

        bool empty() const{
            return _read_index == _write_index;
        }

        void get_overflow_count() const {
            Serial.printf("Sample overflow count: %d\n", sample_overflow_count);
        }

        EventQueue();

        static constexpr uint8_t event_buffer_length = 16;

    private:
        
        uint8_t _write_index = 0;
        uint8_t _read_index = 0;

        uint8_t next_index_write() const {
            return (_write_index + 1) % event_buffer_length;
        }

        uint8_t next_index_read() const {
            return (_read_index + 1) % event_buffer_length;
        }
        
        uint16_t sample_overflow_count = 0;

        std::array<Event, event_buffer_length> _event_buffer{};
};