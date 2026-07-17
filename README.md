Jag är på! Det här är faktiskt ett ganska kul projekt.

### Fas 1 - Målet

Vi ska få detta att fungera:

```text
SavvyCAN (Linux)
       |
   USB Serial
       |
    MegaRET
       |
 Arduino Mega
       |
    SPI
       |
   MCP2515
       |
 CAN-transceiver
       |
    CAN-buss
```

I version 1 ska vi kunna:

* Ansluta Arduino Mega till SavvyCAN.
* Välja den som GVRET-enhet.
* Sniffa CAN-trafik.
* Logga CAN-trafik till SavvyCAN.
* Ingen CAN-sändning ännu (säkrare för bilen).

---

### Fas 2 - Hårdvara

Du behöver:

```text
Arduino Mega 2560
+
MCP2515 CAN-modul
+
SN65HVD230 / TJA1050 / MCP2551
+
OBD2-kabel
```

De flesta MCP2515-moduler ser ut så här:

```text
MCP2515
-------

VCC
GND
CS
SO
SI
SCK
INT
CANH
CANL
```

Kopplas till Mega:

```text
Mega            MCP2515

5V      --->    VCC
GND     --->    GND

50      --->    MISO
51      --->    MOSI
52      --->    SCK

53      --->    CS
2       --->    INT
```

---

### Fas 3 - Projektstruktur

Jag skulle göra ett riktigt Git-projekt.

```text
MegaRET/

src/

MegaRET.ino

GVRET.cpp
GVRET.h

CANBUS.cpp
CANBUS.h

SerialHandler.cpp
SerialHandler.h

Config.h

README.md
```

---

### Fas 4 - Loop

Den blir extremt enkel.

```cpp
void loop()
{
    processUSB();

    processCAN();

    flushPackets();
}
```

---

### Fas 5 - CAN

För CAN skulle jag använda:

```cpp
mcp_can
```

Det är den mest använda biblioteket till Mega + MCP2515.

Exempel:

```cpp
if (CAN_MSGAVAIL == CAN.checkReceive())
{
    CAN.readMsgBuf(...);

    // skicka till SavvyCAN
}
```

---

### Fas 6 - USB

Mega har redan:

```cpp
Serial
```

så SavvyCAN kommer se den som:

```text
/dev/ttyACM0
```

och kommunicera via:

```cpp
Serial.begin(115200);
```

eller:

```cpp
Serial.begin(1000000);
```

Jag lutar åt 1 Mbit för att få bättre throughput.

---

### Fas 7 - SavvyCAN

SavvyCAN kommer att göra ungefär:

```text
Who are you?

Version?

Capabilities?

CAN speed?

Start capture!
```

Då svarar Mega:

```text
MegaRET v0.1

CAN channel = 1

500k supported

Capture enabled
```

Sedan börjar den bara spy ut:

```text
ID
DLC
Timestamp
Bytes
```

---

### Fas 8 - RAM

Mega har:

```text
8 KB RAM
```

Det räcker fint om vi har:

```cpp
struct CANFrame
{

uint32_t id;

uint32_t timestamp;

uint8_t len;

uint8_t data[8];

};
```

32-64 frames i bufferten är inga problem.

---

### Fas 9 - Versioner

Jag skulle köra:

```text
v0.1

RX only

---------

v0.2

CAN TX

---------

v0.3

Dual CAN support

---------

v0.4

OBD2 helper functions

---------

v0.5

Kia CAN database support

---------

v1.0

Full SavvyCAN compatibility
```

---

### Bonus

En rolig grej är att Collin Kidders firmware nästan uteslutande används på dyrare hårdvara som:

* Teensy
* ESP32
* CANable
* Macchina
* M2
* M2 UnderDash

Det finns egentligen ingen anledning att en 15 år gammal Arduino Mega inte skulle kunna sniffa CAN-trafik från en bil för 50 kronor från Tradera.

Så jag tycker att vi satsar på att skriva en egen "MegaRET"-implementation från grunden som pratar GVRET. Den blir sannolikt mindre än 1000 rader C++ och borde fungera utmärkt tillsammans med SavvyCAN på Linux. Det blir dessutom ett trevligt open source-projekt om du vill lägga upp det på GitHub senare.
