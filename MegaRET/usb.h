#ifndef USB_H
#define USB_H

#include <Arduino.h>

#define USB Serial

void USBInit();
bool USBAvailable();
uint8_t USBRead();
void USBWrite(uint8_t b);
void USBWrite(const uint8_t *data, size_t len);

#endif
