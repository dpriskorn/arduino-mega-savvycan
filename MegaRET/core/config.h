#ifndef CONFIG_H
#define CONFIG_H

#define MCP2515_CS_PIN     53
#define MCP2515_IRQ_PIN    2

#define CAN_BUFFER_SIZE    128

#define SERIAL_BAUDRATE    1000000

#define CAN0_SPEED         CAN_500KBPS
#define CAN0_CLOCK         MCP_8MHZ

#define FIRMWARE_VERSION   "MegaRET v1.0"
#define PROTOCOL_VERSION   1

#endif
