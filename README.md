# MegaRET

CAN sniffer for Arduino Mega 2560 with MCP2515, implementing the GVRET protocol for SavvyCAN compatibility.

Inspired by [esp32_can](https://github.com/collin80/esp32_can).

## Hardware

- Arduino Mega 2560
- MCP2515 CAN module
- SN65HVD230 / TJA1050 / MCP2551 transceiver

### Wiring

| Mega Pin | MCP2515 |
|----------|---------|
| 5V       | VCC     |
| GND      | GND     |
| 50       | MISO    |
| 51       | MOSI    |
| 52       | SCK     |
| 53       | CS      |
| 2        | INT     |

## Architecture

```
SavvyCAN
    |
USB CDC
    |
GVRET binary protocol
    |
MegaRET
    |
ringbuffer (128 frames)
    |
MCP2515 (interrupt-driven)
    |
CAN Bus
```

### Data Flow

```
1. CAN Bus → MCP2515 generates interrupt
2. canISR() sets flag → canInterruptOccurred = true
3. loop() detects flag, calls readCANFrames()
4. MCP2515Driver reads frame via SPI
5. Frame stored in RingBuffer
6. loop() sends frames via USB using GVRET binary protocol
```

### Key Features

- **Interrupt-driven**: No polling, uses MCP2515 IRQ pin (Mega pin 2)
- **Binary GVRET protocol**: No ASCII output, minimal bandwidth
- **Ringbuffer**: 128 frame buffer for burst traffic
- **Modular design**: Easy to port to ESP32, BeagleBone, etc.

## Directory Structure

```
MegaRET/
├── MegaRET.ino          # Main sketch
├── core/
│   ├── config.h        # Configuration (pins, speed, buffer size)
│   └── frame.h         # CANFrame structure
├── drivers/
│   └── mcp2515.cpp/h   # MCP2515 driver with interrupt support
├── buffer/
│   └── ringbuffer.cpp/h
├── protocol/
│   └── gvret.cpp/h     # GVRET binary protocol
└── usb/
    └── usb.cpp/h        # USB CDC interface
```

## CANFrame Structure

```cpp
struct CANFrame
{
    uint32_t id;         // CAN ID (11/29-bit)
    uint32_t timestamp;  // Microseconds
    uint8_t dlc;         // Data length (0-8)
    uint8_t flags;       // Extended, RTR, Error, CAN-FD
    uint8_t data[8];
};
```

## GVRET Commands

| Command | Name |
|---------|------|
| 0x00 | RESET |
| 0x01 | GET_VERSION |
| 0x02 | GET_DEVICE_ID |
| 0x04 | GET_CAN_SETTINGS |
| 0x05 | SET_CAN_SETTINGS |
| 0x06 | GET_CAN_STATES |
| 0x07 | SET_CAN_STATES |
| 0x09 | FRAME_RX |
| 0x0A | FRAME_TX |

## Requirements

- [mcp_can library](https://github.com/coryjfowler/mcp_can_lib)
- Arduino IDE 1.8+

## Usage

1. Install the mcp_can library in your Arduino/libraries folder
2. Open MegaRET.ino in Arduino IDE
3. Select Arduino Mega 2560 as board
4. Upload
5. Connect to SavvyCAN using GVRET connection type

## License

MIT License - see [license.txt](MegaRET/license.txt)
