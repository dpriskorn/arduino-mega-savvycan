#include "core/config.h"
#include "core/frame.h"
#include "drivers/mcp2515.h"
#include "buffer/ringbuffer.h"
#include "protocol/gvret.h"
#include "usb/usb.h"

RingBuffer canRxBuffer;
RingBuffer canTxBuffer;
MCP2515Driver canDriver;
GVRETProtocol gvret;

void setup()
{
    USBInit();
    canDriver.begin();
    gvret.init();
}

void loop()
{
    if (canInterruptOccurred)
    {
        canInterruptOccurred = false;
        readCANFrames();
    }

    processUSBCommands();
    sendCANFrames();
}

void readCANFrames()
{
    CANFrame frame;
    while (canDriver.available())
    {
        if (canDriver.readFrame(frame))
        {
            canRxBuffer.enqueue(frame);
            gvret.enqueueIncoming(frame);
        }
    }
}

void processUSBCommands()
{
    while (USBAvailable())
    {
        uint8_t byte = USBRead();
        gvret.process(byte);
        gvret.processData(byte);
    }
}

void sendCANFrames()
{
    CANFrame frame;
    while (!canRxBuffer.isEmpty())
    {
        canRxBuffer.dequeue(frame);
        sendGVRETFrame(frame);
    }
}

void sendGVRETFrame(const CANFrame &frame)
{
    USB.write(GVRET_CMD_FRAME_RX);

    USB.write((frame.id >> 24) & 0xFF);
    USB.write((frame.id >> 16) & 0xFF);
    USB.write((frame.id >> 8) & 0xFF);
    USB.write(frame.id & 0xFF);

    USB.write(frame.flags);
    USB.write(frame.dlc);

    for (uint8_t i = 0; i < frame.dlc && i < 8; i++)
    {
        USB.write(frame.data[i]);
    }

    USB.write('\r');
}
