#ifndef GVRET_H
#define GVRET_H

#include "core/frame.h"
#include "buffer/ringbuffer.h"

#define GVRET_CMD_RESET           0x00
#define GVRET_CMD_GET_VERSION     0x01
#define GVRET_CMD_GET_DEVICE_ID   0x02
#define GVRET_CMD_GET_CAN_SETTINGS 0x04
#define GVRET_CMD_SET_CAN_SETTINGS 0x05
#define GVRET_CMD_GET_CAN_STATES   0x06
#define GVRET_CMD_SET_CAN_STATES   0x07
#define GVRET_CMD_FRAME_RX         0x09
#define GVRET_CMD_FRAME_TX         0x0A

class GVRETProtocol
{
public:
    GVRETProtocol();

    void init();

    void process(uint8_t cmd);
    void processData(uint8_t data);

    bool hasOutgoingFrame();
    bool dequeueOutgoing(CANFrame &frame);
    void enqueueIncoming(const CANFrame &frame);

    void setCANSpeed(uint32_t speed);
    uint32_t getCANSpeed() const;

    void systemReset();

private:
    enum State
    {
        STATE_IDLE,
        STATE_GET_VERSION,
        STATE_GET_DEVICE_ID,
        STATE_GET_CAN_SETTINGS,
        STATE_SET_CAN_SETTINGS,
        STATE_GET_CAN_STATES,
        STATE_SET_CAN_STATES,
        STATE_FRAME_DATA
    };

    void sendResponse(uint8_t cmd, const uint8_t *data, uint8_t len);
    void sendSingleByte(uint8_t b);
    void sendFrameBinary(const CANFrame &frame);

    State state;
    CANFrame txFrame;
    uint8_t txFramePos;
    uint8_t txDataLen;
    uint32_t canSpeed;
};

extern volatile bool canInterruptOccurred;
void canISR();

#endif
