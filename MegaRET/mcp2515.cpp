#include "mcp2515.h"

volatile bool canInterruptOccurred = false;

void canISR()
{
    canInterruptOccurred = true;
}

MCP2515Driver::MCP2515Driver() : can(MCP2515_CS_PIN), irqFlag(false)
{
}

bool MCP2515Driver::begin()
{
    if (can.begin(MCP_ANY, CAN_500KBPS, CAN0_CLOCK) != CAN_OK)
    {
        return false;
    }

    if (can.setMode(MCP_NORMAL) != CAN_OK)
    {
        return false;
    }

    pinMode(MCP2515_IRQ_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(MCP2515_IRQ_PIN), canISR, FALLING);

    return true;
}

void MCP2515Driver::reset()
{
    irqFlag = false;
}

bool MCP2515Driver::setSpeed(uint8_t speed)
{
    return can.begin(MCP_ANY, speed, CAN0_CLOCK) == CAN_OK;
}

bool MCP2515Driver::setNormalMode()
{
    return can.setMode(MCP_NORMAL) == CAN_OK;
}

bool MCP2515Driver::isInterruptPending()
{
    return irqFlag;
}

void MCP2515Driver::clearInterrupt()
{
    irqFlag = false;
}

bool MCP2515Driver::available()
{
    return digitalRead(MCP2515_IRQ_PIN) == LOW;
}

bool MCP2515Driver::readFrame(CANFrame &frame)
{
    if (can.checkReceive() != CAN_MSGAVAIL)
    {
        return false;
    }

    uint32_t id;
    uint8_t ext;
    uint8_t len;
    can.readMsgBuf(&id, &ext, &len, frame.data);

    frame.id = id;
    frame.dlc = len;
    frame.flags = 0;
    if (ext) frame.flags |= CAN_FRAME_FLAG_EXTENDED;

    if (can.checkReceive() == CAN_MSGAVAIL)
    {
    }

    frame.timestamp = micros();

    return true;
}

bool MCP2515Driver::sendFrame(const CANFrame &frame)
{
    uint8_t len = frame.dlc;
    if (len > 8) len = 8;

    uint8_t ext = (frame.flags & CAN_FRAME_FLAG_EXTENDED) ? 1 : 0;

    uint8_t sendStat = can.sendMsgBuf(
        frame.id,
        ext,
        len,
        frame.data
    );

    return sendStat == CAN_OK;
}
