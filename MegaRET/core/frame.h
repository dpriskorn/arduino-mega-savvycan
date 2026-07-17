#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>

#define CAN_FRAME_FLAG_EXTENDED   0x01
#define CAN_FRAME_FLAG_RTR         0x02
#define CAN_FRAME_FLAG_ERROR      0x04
#define CAN_FRAME_FLAG_CANFD      0x08

struct CANFrame
{
    uint32_t id;
    uint32_t timestamp;
    uint8_t dlc;
    uint8_t flags;
    uint8_t data[8];
};

#endif
