#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
using std::array;

#define EVENT_BUFFER_LENGTH 16

enum class EventType {
    NotePress, // an LED note key pad was pressed
    EncoderUpdate, // an encoder was rotated
    EventQueueError
};

// generic event queue for button + encoder events
struct Event {
    EventType kind;
    uint8_t id;
    uint16_t value;
};

class EventQueue {

    public:
        bool push(Event event); // boolean indicates success

        Event consume_once(); // pop the last item and read it

        bool has_event(); // true if array is not empty

        uint8_t length(); // number of events inside buffer

        bool full() {
            return _event_buffer.size() == EVENT_BUFFER_LENGTH;
        }

        bool empty() {
            return _event_buffer.size() == 0;
        }

        EventQueue();

    private:
        
        uint8_t write_index;
        uint8_t read_index;

        uint8_t next_write() {
            return (write_index + 1) % EVENT_BUFFER_LENGTH;
        }

        uint8_t next_read() {
            return (read_index + 1) % EVENT_BUFFER_LENGTH;
        }

        // void wraparound_check() {
        //     if (write_index > (EVENT_BUFFER_LENGTH - 1)) {
        //         write_index = 0;
        //     }
        //     if (read_index > (EVENT_BUFFER_LENGTH - 1)) {
        //         write_index = 0;
        //     }
        // }
        
        std::array<Event, EVENT_BUFFER_LENGTH> _event_buffer{};
};