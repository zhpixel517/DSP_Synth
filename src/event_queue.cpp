#include <array>
#include "event_queue.hpp"
#include <cstdint>
using std::array;

EventQueue::EventQueue() {}

bool EventQueue::push(Event event) {
    if (!full()) _event_buffer[write_index] = event;
    write_index++;
}

Event EventQueue::consume_once() {
    Event event; 
    if (!empty()) event = _event_buffer[read_index];
    read_index++;
    return event;
}

uint8_t EventQueue::length() {
    return _event_buffer.size();
}

bool EventQueue::has_event() {
        return !(_event_buffer.empty());
}



// class EventQueue {

//     void push(Event event);

    

    

//     private:
//         std::array<Event, 16> _buffer;
// };