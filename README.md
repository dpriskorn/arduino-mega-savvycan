# MegaRET

CAN sniffer for Arduino Mega 2560 with MCP2515, implementing the GVRET protocol for SavvyCAN compatibility.

Inspired by [esp32_can](https://github.com/collin80/esp32_can).

## Hardware

- Arduino Mega 2560
- MCP2515 CAN module with SPI interface
- SN65HVD230 / TJA1050 / MCP2551 transceiver

### Recommended CAN Module

This [MCP2515 module on Amazon](https://www.amazon.se/gp/product/B086TXSFD8) works well with the wiring below.

### OBD2 Cable

This [DB9 to OBD2 cable](https://www.amazon.se/gp/product/B0F8VN8S17) connects the MCP2515 module to your vehicle's OBD2 port.

For OBD2 pinout and introduction, see [css-selectronics OBD2 explained](https://www.csselectronics.com/pages/obd2-explained-simple-intro).

![OBD2 Connector Pinout](https://www.csselectronics.com/cdn/shop/files/obd2-connector-pinout-socket.svg?v=1633690039)

### CAN Pins on OBD2

| Pin | Function | Description |
|-----|----------|-------------|
| 6   | CAN High | J1850 / ISO 11989-2 CAN-H (11-bit) |
| 14  | CAN Low  | J1850 / ISO 11989-2 CAN-L (11-bit) |
| 7   | K-Line   | ISO 9141-2 / ISO 14230 K-LINE |
| 15  | L-Line   | ISO 9141-2 / ISO 14230 L-LINE |

Most modern vehicles use pins 6 (CAN H) and 14 (CAN L) for the CAN bus.

### Wiring

| Mega Pin | MCP2515 | Function |
|----------|---------|----------|
| 5V       | VCC     | Power    |
| GND      | GND     | Ground   |
| 50       | MISO    | SPI Data In |
| 51       | MOSI    | SPI Data Out |
| 52       | SCK     | SPI Clock |
| 53       | CS      | Chip Select |
| 2        | INT     | Interrupt |

### Pinout References

- **Arduino Mega 2560**: [Official pinout PDF](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf)
- **MCP2515 module**: Most modules have pins labeled on the board. The standard pinout is:
  - `VCC` - 5V power
  - `GND` - Ground
  - `SI` - Serial Data In (MOSI)
  - `SO` - Serial Data Out (MISO)
  - `SCK` - Clock
  - `CS` - Chip Select
  - `INT` - Interrupt output

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
├── MegaRET.ino        # Main sketch
├── config.h           # Configuration (pins, speed, buffer size)
├── frame.h            # CANFrame structure
├── mcp2515.cpp/h      # MCP2515 driver with interrupt support
├── ringbuffer.cpp/h
├── gvret.cpp/h        # GVRET binary protocol
├── usb.cpp/h          # USB CDC interface
└── license.txt        # MIT License
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

- [mcp_can library](https://github.com/coryjfowler/mcp_can_lib) (installed via CLI, not copied)
- Arduino IDE 1.8+ or arduino-cli

## Usage

1. Install the mcp_can library:
   ```
   arduino-cli lib install "mcp_can"
   ```
2. Open MegaRET.ino in Arduino IDE
3. Select Arduino Mega 2560 as board
4. Upload
5. Connect to SavvyCAN using GVRET connection type

## See Also

- [CAN Bus Reverse Engineering with AI/LLM](https://www.csselectronics.com/pages/can-bus-reverse-engineering-ai-llm-claude) - Using AI to decode CAN traffic

## License

MIT License - see [license.txt](MegaRET/license.txt)
