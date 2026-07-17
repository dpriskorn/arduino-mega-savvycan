#include "ringbuffer.h"

RingBuffer::RingBuffer() : head(0), tail(0), count_(0)
{
}

bool RingBuffer::isEmpty() const
{
    return count_ == 0;
}

bool RingBuffer::isFull() const
{
    return count_ == CAN_BUFFER_SIZE;
}

uint8_t RingBuffer::count() const
{
    return count_;
}

bool RingBuffer::enqueue(const CANFrame &frame)
{
    if (isFull()) return false;

    buffer[head] = frame;
    head = (head + 1) % CAN_BUFFER_SIZE;
    count_++;
    return true;
}

bool RingBuffer::dequeue(CANFrame &frame)
{
    if (isEmpty()) return false;

    frame = buffer[tail];
    tail = (tail + 1) % CAN_BUFFER_SIZE;
    count_--;
    return true;
}

bool RingBuffer::peek(CANFrame &frame) const
{
    if (isEmpty()) return false;

    frame = buffer[tail];
    return true;
}

void RingBuffer::clear()
{
    head = 0;
    tail = 0;
    count_ = 0;
}
