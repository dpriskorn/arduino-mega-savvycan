#include "usb.h"
#include "config.h"

void USBInit()
{
    USB.begin(SERIAL_BAUDRATE);
    while (!USB);
}

bool USBAvailable()
{
    return USB.available() > 0;
}

uint8_t USBRead()
{
    return USB.read();
}

void USBWrite(uint8_t b)
{
    USB.write(b);
}

void USBWrite(const uint8_t *data, size_t len)
{
    USB.write(data, len);
}
