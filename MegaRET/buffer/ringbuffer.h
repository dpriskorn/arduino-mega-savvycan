#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "core/frame.h"
#include "core/config.h"

class RingBuffer
{
public:
    RingBuffer();

    bool isEmpty() const;
    bool isFull() const;
    uint8_t count() const;

    bool enqueue(const CANFrame &frame);
    bool dequeue(CANFrame &frame);
    bool peek(CANFrame &frame) const;

    void clear();

private:
    CANFrame buffer[CAN_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count_;
};

#endif
