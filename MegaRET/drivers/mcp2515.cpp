#include "mcp2515.h"
#include "core/config.h"

MCP2515Driver::MCP2515Driver() : irqFlag(false)
{
}

bool MCP2515Driver::begin()
{
    if (CAN.begin(MCP_ANY, CAN0_SPEED, CAN0_CLOCK) != CAN_OK)
    {
        return false;
    }

    CAN.setMode(MCP_NORMAL);
    reset();

    pinMode(MCP2515_IRQ_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(MCP2515_IRQ_PIN), canISR, FALLING);

    return true;
}

void MCP2515Driver::reset()
{
    CAN.reset();
    irqFlag = false;
}

bool MCP2515Driver::setSpeed(uint8_t speed)
{
    return CAN.setBitrate(speed, CAN0_CLOCK) == CAN_OK;
}

bool MCP2515Driver::setNormalMode()
{
    return CAN.setMode(MCP_NORMAL) == CAN_OK;
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
    if (CAN.checkReceive() != CAN_MSGAVAIL)
    {
        return false;
    }

    frame.id = CAN.getCanId();

    uint8_t extFlag = 0;
    CAN.readMsgBuf(&frame.dlc, frame.data);

    extFlag = CAN.isExtendedFrame();
    frame.flags = 0;
    if (extFlag) frame.flags |= CAN_FRAME_FLAG_EXTENDED;

    if (CAN.isRemoteRequest()) frame.flags |= CAN_FRAME_FLAG_RTR;

    frame.timestamp = micros();

    return true;
}

bool MCP2515Driver::sendFrame(const CANFrame &frame)
{
    uint8_t len = frame.dlc;
    if (len > 8) len = 8;

    uint8_t sendStat = CAN.sendMsgBuf(
        frame.id,
        (frame.flags & CAN_FRAME_FLAG_EXTENDED) ? true : false,
        (frame.flags & CAN_FRAME_FLAG_RTR) ? true : false,
        len,
        frame.data
    );

    return sendStat == CAN_OK;
}
