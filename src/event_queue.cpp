#include <array>
#include "event_queue.hpp"

EventQueue::EventQueue() {}

bool EventQueue::push(Event event)
{
    if (!full())
    {
        _event_buffer[_write_index] = event;
        _write_index = next_index_write();
        return true;
    }
    sample_overflow_count++;
    return false;
}

bool EventQueue::consume_once(Event &out)
{
    if (!empty())
    {
        out = _event_buffer[_read_index];
        _read_index = next_index_read();
        return true;
    }
    else
    {
        out = Event {
            .kind = EventType::EventQueueError,
            .id = -1,
            .value = -1,
        };
        return false;
    }
}

