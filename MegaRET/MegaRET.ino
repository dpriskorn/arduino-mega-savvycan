#include "config.h"
#include "frame.h"
#include "mcp2515.h"
#include "ringbuffer.h"
#include "gvret.h"
#include "usb.h"

RingBuffer canRxBuffer;
RingBuffer canTxBuffer;
MCP2515Driver canDriver;
GVRETProtocol gvret;

void setup()
{
    USBInit();
    USB.println("MegaRET initializing...");
    USB.println("Waiting for MCP2515 CAN controller...");

    if (!canDriver.begin())
    {
        USB.println("ERROR: Cannot communicate with MCP2515!");
        USB.println("Check wiring:");
        USB.println("  - CS pin (53) to MCP2515 CS");
        USB.println("  - MOSI (51) to MCP2515 SI");
        USB.println("  - MISO (50) to MCP2515 SO");
        USB.println("  - SCK (52) to MCP2515 SCK");
        USB.println("  - GND to MCP2515 GND");
        USB.println("");
        USB.println("Is the MCP2515 module connected and powered?");
        while (1) { }
    }

    USB.println("MCP2515 found and configured!");
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
