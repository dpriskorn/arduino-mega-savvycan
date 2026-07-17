#include "gvret.h"
#include "usb.h"
#include "config.h"

GVRETProtocol::GVRETProtocol()
    : state(STATE_IDLE)
    , txFramePos(0)
    , txDataLen(0)
    , canSpeed(500000)
{
}

void GVRETProtocol::init()
{
    canSpeed = 500000;
    state = STATE_IDLE;
}

void GVRETProtocol::process(uint8_t cmd)
{
    switch (cmd)
    {
        case GVRET_CMD_RESET:
            systemReset();
            break;

        case GVRET_CMD_GET_VERSION:
            sendSingleByte(0x01);
            sendSingleByte(PROTOCOL_VERSION);
            USB.write('\r');
            break;

        case GVRET_CMD_GET_DEVICE_ID:
        {
            uint8_t id[] = {'M', 'e', 'g', 'a', 'R', 'E', 'T', ' ', 'v', '1', '.', '0'};
            USB.write(id, sizeof(id));
            USB.write('\r');
            break;
        }

        case GVRET_CMD_GET_CAN_SETTINGS:
        {
            uint8_t settings[] = {
                0x00,
                (uint8_t)(canSpeed >> 24),
                (uint8_t)(canSpeed >> 16),
                (uint8_t)(canSpeed >> 8),
                (uint8_t)(canSpeed & 0xFF),
                0x00,
                0x00,
                0x00,
                0x00
            };
            sendResponse(GVRET_CMD_GET_CAN_SETTINGS, settings, sizeof(settings));
            break;
        }

        case GVRET_CMD_SET_CAN_SETTINGS:
            state = STATE_SET_CAN_SETTINGS;
            txFramePos = 0;
            break;

        case GVRET_CMD_GET_CAN_STATES:
        {
            uint8_t states[] = {
                0x00,
                0x00,
                0x00,
                0x00,
                0x00,
                0x00
            };
            sendResponse(GVRET_CMD_GET_CAN_STATES, states, sizeof(states));
            break;
        }

        case GVRET_CMD_SET_CAN_STATES:
            state = STATE_SET_CAN_STATES;
            txFramePos = 0;
            break;

        case GVRET_CMD_FRAME_TX:
            state = STATE_FRAME_DATA;
            txFramePos = 0;
            break;
    }
}

void GVRETProtocol::processData(uint8_t data)
{
    switch (state)
    {
        case STATE_SET_CAN_SETTINGS:
            if (txFramePos < 4)
            {
                ((uint8_t*)&canSpeed)[3 - txFramePos] = data;
                txFramePos++;
            }
            else
            {
                state = STATE_IDLE;
            }
            break;

        case STATE_SET_CAN_STATES:
            txFramePos++;
            if (txFramePos >= 2)
            {
                state = STATE_IDLE;
            }
            break;

        case STATE_FRAME_DATA:
            switch (txFramePos)
            {
                case 0:
                    txFrame.id = data << 24;
                    break;
                case 1:
                    txFrame.id |= data << 16;
                    break;
                case 2:
                    txFrame.id |= data << 8;
                    break;
                case 3:
                    txFrame.id |= data;
                    break;
                case 4:
                    txFrame.flags = data;
                    break;
                case 5:
                    txFrame.dlc = data;
                    if (txFrame.dlc > 8) txFrame.dlc = 8;
                    txDataLen = txFrame.dlc;
                    break;
                default:
                    if ((txFramePos - 6) < txDataLen)
                    {
                        txFrame.data[txFramePos - 6] = data;
                    }
                    if ((txFramePos - 5) >= txDataLen)
                    {
                        enqueueIncoming(txFrame);
                        state = STATE_IDLE;
                    }
                    break;
            }
            txFramePos++;
            break;

        default:
            break;
    }
}

bool GVRETProtocol::hasOutgoingFrame()
{
    return false;
}

bool GVRETProtocol::dequeueOutgoing(CANFrame &frame)
{
    return false;
}

void GVRETProtocol::enqueueIncoming(const CANFrame &frame)
{
}

void GVRETProtocol::setCANSpeed(uint32_t speed)
{
    canSpeed = speed;
}

uint32_t GVRETProtocol::getCANSpeed() const
{
    return canSpeed;
}

void GVRETProtocol::systemReset()
{
    state = STATE_IDLE;
    asm volatile ("jmp 0");
}

void GVRETProtocol::sendResponse(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    USB.write(cmd);
    USB.write(data, len);
    USB.write('\r');
}

void GVRETProtocol::sendSingleByte(uint8_t b)
{
    USB.write(b);
}

void GVRETProtocol::sendFrameBinary(const CANFrame &frame)
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
