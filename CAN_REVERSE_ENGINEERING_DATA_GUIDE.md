# Collecting CAN Data for AI Reverse Engineering

AI models can help a lot with CAN reverse engineering, but the amount of data needed depends heavily on **what you want to discover**.

A model cannot magically understand CAN frames by itself. It needs **structured observations**: raw CAN logs plus information about what was happening in the car at the time.

For your 2007 Kia Ceed, a realistic approach would be:

## Minimum useful dataset

### Finding simple signals (speed, RPM, pedal states)

You might only need:

**5–15 minutes of driving data**

with events like:

* ignition ON/OFF
* engine start
* idle
* revving engine
* accelerating
* braking
* turning steering wheel
* pressing buttons
* opening doors/lights

Example:

```
timestamp    CAN_ID   data
12.001       0x1A0    10 00 00 FF 00 00 00 00
12.005       0x280    04 22 10 00 00 00 00 00
12.010       0x1A0    11 00 00 FF 00 00 00 00
```

Then provide context:

```
12:00 - engine started
12:05 - accelerated from 0-50 km/h
12:07 - pressed brake
12:10 - turned steering wheel left
```

The AI can look for correlations.

---

## For a good DBC reverse engineering attempt

More like:

**1–3 hours of driving data**

with diverse conditions:

* city driving
* highway
* parking
* bumps
* steering movements
* different speeds
* lights on/off
* climate controls
* cruise control (if equipped)

A lot of CAN signals are periodic, so longer captures help.

For example:

Vehicle speed might appear:

```
CAN ID 0x123

00 00 00 00
05 00 00 00
0A 00 00 00
0F 00 00 00
```

every 10 ms.

The AI can infer:

* byte 0 changes
* linear relationship
* likely speed

---

## For a complete car model

If you want something like a professional DBC:

Probably:

**10–50+ hours of targeted captures**

because you need to identify:

* every ECU
* every message
* every signal
* counters
* checksums
* multiplexing
* scaling factors

---

## The biggest factor is not amount of data — it is labels

A 24-hour CAN recording of random driving is often less useful than:

10 minutes where you deliberately do:

```
Start recording

1. Ignition off → on
2. Start engine
3. Wait 60 seconds
4. Press brake 10 times
5. Turn steering full left/right
6. Toggle headlights
7. Open each door
8. Accelerate 0-100 km/h
9. Stop recording
```

The AI loves controlled experiments.

---

## For your Kia Ceed specifically

I would start with:

### Capture 1: ignition/body CAN

10 minutes:

* lock/unlock
* lights
* indicators
* doors
* windows
* wipers
* climate buttons

### Capture 2: driving CAN

30 minutes:

* idle
* slow acceleration
* braking
* cruising
* steering

### Capture 3: diagnostics

Use OBD tools to compare:

* CAN frames
* OBD PID responses

---

## Hardware matters

Your Arduino Mega + MCP2515 CAN shield can record frames, but for reverse engineering I would prefer a Linux CAN adapter because you get tools like:

* `candump`
* `can-utils`
* `Wireshark CAN`
* `Kayak`
* `SavvyCAN`

A BeagleBone Black you mentioned earlier is actually a nice fit because it can run Linux and interface with CAN hardware.

---

One warning: AI is much better at **finding candidate signals** than proving them. It may say "byte 2 looks like speed", but you still verify:

* Does it match GPS speed?
* Does it behave correctly at 30/50/100 km/h?
* Does it survive different conditions?

For a 2007 Ceed, I would expect you could probably decode **a useful subset (speed, RPM, temperatures, switches)** with a few hours of carefully collected CAN data. A full vehicle DBC is a much larger project.

---

## See also

* [commaai/opendbc](https://github.com/commaai/opendbc) - Open DBC files and CAN tools
