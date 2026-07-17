#ifndef MCP2515_H
#define MCP2515_H

#include "frame.h"
#include "config.h"
#include <mcp_can.h>

extern volatile bool canInterruptOccurred;
void canISR();

class MCP2515Driver
{
public:
    MCP2515Driver();

    bool begin();
    void reset();

    bool setSpeed(uint8_t speed);
    bool setNormalMode();

    bool isInterruptPending();
    void clearInterrupt();

    bool readFrame(CANFrame &frame);
    bool sendFrame(const CANFrame &frame);

    bool available();

private:
    MCP_CAN can;
    bool irqFlag;
};

#endif
