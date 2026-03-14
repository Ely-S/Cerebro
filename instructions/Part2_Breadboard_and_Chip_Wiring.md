# Part 2: Breadboard and Chip Wiring

**Estimated Time:** 3-4 Hours
**Difficulty:** Moderate - attention to pin numbering and grounding discipline is essential

---

## Overview

In this part you will wire the entire analog and digital signal chain on a breadboard:

1. **LED driver circuit** - two TIP31C transistors switching the NIR LEDs
2. **Transimpedance amplifier** - MCP6022 converting photodiode current to voltage
3. **ADC** - ADS1115 digitizing the amplified signal
4. **Arduino Nano 33 IoT** - controlling LEDs and reading the ADC via I2C
5. **Power distribution** - proper rail decoupling and star ground

The single most important wiring concept in this build is **star grounding**. All ground connections must converge at one single point near the ADS1115 GND pin. Violating this rule will inject LED switching noise into your sensitive analog measurements.

---

## Materials Needed

| Item | Specification | Notes |
|:-----|:-------------|:------|
| Arduino Nano 33 IoT | ABX00027 | 3.3V logic - this matters |
| ADS1115 ADC Module | Adafruit 1085 or equivalent | 16-bit, I2C |
| MCP6022-I/P Op-Amp | DIP-8 package | Rail-to-rail, low bias current |
| TIP31C NPN Transistor | TO-220 package, qty 2 | LED driver switches |
| 68 Ohm resistor, 0.5W | qty 2 | LED current limiting |
| 470 Ohm resistor, 1/4W | qty 2 | Transistor base drive |
| 100k Ohm resistor, 1% metal film | qty 1 | TIA feedback (Rf) |
| 47k Ohm resistor | qty 1 | Stage 2 feedback (Rf2) |
| 1k Ohm resistor | qty 1 | Stage 2 ground (Rg) |
| 150 Ohm resistor, 1/4W | qty 1 | Power-on indicator |
| 10pF ceramic capacitor (NPO/C0G) | qty 1 | TIA stability - CRITICAL |
| 0.1uF ceramic capacitor | qty 2 | Local decoupling |
| 47-100uF electrolytic capacitor, 10V+ | qty 1 | 5V bulk decoupling (47uF is acceptable) |
| 10uF capacitor, 6.3V+ | qty 1 | 3.3V bulk decoupling |
| 500mA fuse (resettable PTC preferred) | qty 1 | LED supply protection |
| Green LED, 5mm | qty 1 | Power-on indicator |
| Full-size breadboard | qty 1 | 830 tie-points recommended |
| Hookup wire kit | 22 AWG solid core | Pre-cut jumper kit helpful |

### Tools Required

- Wire strippers
- Needle-nose pliers
- Multimeter (for continuity and voltage checks)
- Magnifying glass or loupe (for inspecting DIP pin connections)

---

## Important Warnings Before You Start

> **WARNING 1:** The Arduino Nano 33 IoT is a **3.3V logic** board. Its I/O pins are NOT 5V tolerant. The ADS1115 VDD must be connected to 3.3V, not 5V. Connecting the ADS1115 to 5V may damage the Arduino's I2C pins.

> **WARNING 2:** For your first power-on, use a **current-limited bench supply** (set to 30-50mA limit), not a USB battery pack. A USB battery can source enough current to destroy components if you have a wiring error. Switch to the battery pack only after confirming correct operation. If you do not have access to a current-limited bench supply, carefully triple-check every connection against the verification checklist in Step 9 before connecting any USB power source. Proceed with extreme caution and have someone else double-check your wiring.

> **WARNING 3:** Observe **star ground discipline** throughout. Do not connect grounds wherever is convenient. All ground connections return to one star point near the ADS1115 GND.

---

## Step 1: Plan Your Breadboard Layout

Before inserting any components, plan the physical layout. A good layout prevents noise problems.

### 1.1 Recommended Zone Layout

Divide your breadboard into three zones:

```
+------------------------------------------------------------------+
|  ZONE A: POWER ENTRY          |  ZONE B: ANALOG FRONT END       |
|  Arduino, fuse, indicator LED |  MCP6022, ADS1115               |
|  TIP31C transistors           |  Feedback resistors/caps         |
|                               |  STAR GROUND POINT here          |
+-------------------------------+----------------------------------+
|  ZONE C: CONNECTIONS TO PROBE                                    |
|  LED wire connections, photodiode wire connections                |
+------------------------------------------------------------------+
```

### 1.2 Key Placement Rules

- Place the MCP6022 and ADS1115 **close together** in Zone B.
- Place the TIP31C transistors in Zone A, **physically separated** from the analog ICs.
- The **star ground point** should be at or adjacent to the ADS1115 GND pin.
- Keep the photodiode shielded wire connection as close to the MCP6022 input pin as possible.

---

## Step 2: Insert the Arduino Nano 33 IoT

### 2.1 Placement

- Insert the Arduino Nano 33 IoT into the breadboard, straddling the center channel.
- Position it so you have access to these pins:
  - **D2** - 730nm LED control
  - **D3** - 850nm LED control
  - **A4 (SDA)** - I2C data to ADS1115
  - **A5 (SCL)** - I2C clock to ADS1115
  - **5V** - Power output for LED drivers and indicator
  - **3.3V** - Power for analog ICs
  - **GND** - Ground reference

### 2.2 Identify Pin Locations

- Consult the Arduino Nano 33 IoT pinout diagram for your specific board revision.
- Double-check that you are reading pins from the correct end (USB connector orientation).

---

## Step 3: Wire the Power Rails

### 3.1 Establish the Star Ground Point

- Choose one specific tie-point on the breadboard near where the ADS1115 GND pin will go. This is your **STAR_GND**.
- Mark it with a small piece of tape if it helps you remember.
- **Every ground connection in the entire circuit** will route back to this single point.

> **Do NOT use the breadboard's long power rail strips as a ground bus.** This is the most common beginner mistake with star grounding. A power rail strip is a long conductive trace with resistance along its length - current flowing through one end creates a small voltage drop that contaminates signals at the other end. Instead, run an individual jumper wire from each ground point directly to the single STAR_GND tie-point. Yes, this means multiple wires converging on one spot. That is exactly what "star" means.

### 3.2 5V Power Rail

Wire from Arduino 5V pin through the fuse to a 5V distribution point:

```
Arduino 5V --> [500mA PTC Fuse] --> 5V_NODE
```

> **What is a PTC fuse?** A resettable PTC (Positive Temperature Coefficient) fuse is a small two-terminal device that looks like a ceramic capacitor or small disc. It has no polarity - either lead can go in either direction. Wire it **in series** between the Arduino 5V pin and 5V_NODE: one lead connects to the row with the Arduino 5V pin, the other lead connects to the row you designate as 5V_NODE. Under normal current (<500mA) it acts like a wire. If current exceeds its rating (due to a short circuit), it heats up and becomes highly resistive, protecting your components. It resets itself when power is removed.

From 5V_NODE, three things connect:
1. The two LED current-limiting resistors (68 Ohm each)
2. The power-on indicator (150 Ohm + green LED)
3. The 100uF bulk decoupling capacitor to STAR_GND

### 3.3 Install 5V Bulk Decoupling

- Connect a **47-100uF electrolytic capacitor** between 5V_NODE and STAR_GND. Any value in this range works; 100uF is ideal, 47uF is acceptable.
- **Observe polarity:** the longer lead (or the lead opposite the stripe) is positive and goes to 5V_NODE.

### 3.4 3.3V Power Rail

- Run a wire from Arduino 3.3V pin to a 3.3V distribution point near the analog ICs.
- Connect a **10uF capacitor** between this 3.3V point and STAR_GND.
- This rail powers the MCP6022 (V+, pin 8) and the ADS1115 (VDD).

### 3.5 Connect Arduino GND to STAR_GND

- Run a wire from the Arduino GND pin to STAR_GND.

---

## Step 4: Wire the Power-On Indicator

This LED serves two purposes: it shows you the circuit is powered, and it draws ~20mA from the 5V rail which prevents some USB battery packs from auto-shutting off due to low current draw.

```
5V_NODE --> [150 Ohm] --> Green LED (anode) --> Green LED (cathode) --> STAR_GND
```

- The 150 Ohm resistor limits current to approximately 20mA at 5V (accounting for ~2V LED forward voltage).
- Verify the LED lights when you first power on (Step 10).

---

## Step 5: Wire the LED Driver Circuits

Each NIR LED is switched by a TIP31C NPN transistor. The Arduino GPIO pin turns the transistor on/off; the transistor switches the higher-current LED path.

### 5.1 730nm LED Driver (Q1)

```
Arduino D2 --> [470 Ohm] --> Q1 Base
5V_NODE --> [68 Ohm, 0.5W] --> 730nm LED anode (from headband pigtail)
730nm LED cathode (from headband pigtail) --> Q1 Collector
Q1 Emitter --> LED_GND_RETURN wire --> STAR_GND
```

#### TIP31C Pin Identification (TO-220 package)

Hold the transistor with the **printed/labeled side facing you** (you should be able to read the "TIP31C" text) and the leads pointing down. The metal heatsink tab is on the back, facing away from you.

```
     ___________
    |           |
    |  TIP31C   |   <-- You can read the text (printed side faces you)
    |           |
    |___________|
     |   |   |
     B   C   E
   Base Coll Emit
```

- **Pin 1 (left):** Base
- **Pin 2 (center):** Collector
- **Pin 3 (right):** Emitter

> **Common mistake:** If you hold the transistor with the metal tab facing you (wrong side), the pinout appears reversed as E-C-B. This will short the LED supply through the base resistor. Always orient by the printed text.

### 5.2 850nm LED Driver (Q2)

```
Arduino D3 --> [470 Ohm] --> Q2 Base
5V_NODE --> [68 Ohm, 0.5W] --> 850nm LED anode (from headband pigtail)
850nm LED cathode (from headband pigtail) --> Q2 Collector
Q2 Emitter --> LED_GND_RETURN wire --> STAR_GND
```

### 5.3 LED Ground Return Path

- The emitters of both Q1 and Q2 connect to a **dedicated LED_GND_RETURN** trace.
- This trace goes directly to STAR_GND but must be a **separate wire** from the analog ground returns.
- The reason: LED switching creates current spikes. If those spikes share a ground wire with the photodiode amplifier, the noise couples directly into your measurement.

> **Do not** connect Q1/Q2 emitters to the nearest ground point on the breadboard. Run dedicated wires back to STAR_GND.

---

## Step 6: Wire the MCP6022 Transimpedance Amplifier

The MCP6022 is a dual op-amp in a DIP-8 package. You will use both op-amp channels: one for the transimpedance stage (current-to-voltage) and one for the gain stage.

### 6.1 MCP6022 DIP-8 Pinout

```
        +----U----+
 OUT1  1|         |8  V+
  IN1- 2|         |7  OUT2
  IN1+ 3| MCP6022 |6  IN2-
    V- 4|         |5  IN2+
        +---------+
```

- The notch or dot on the IC indicates Pin 1.

### 6.2 Power the Op-Amp

```
Pin 8 (V+) --> 3.3V rail
Pin 4 (V-) --> STAR_GND
```

- Place a **0.1uF ceramic capacitor** between Pin 8 and Pin 4, as close to the IC as physically possible. This is the local decoupling capacitor.
- On a breadboard, the MCP6022 straddles the center channel. Pin 8 and Pin 4 are on opposite sides. The decoupling capacitor must bridge across the center channel, with one lead in the row containing Pin 8 and the other in the row containing Pin 4.

### 6.3 Stage 1: Transimpedance Amplifier (TIA)

This stage converts the photodiode's tiny current (nano-amps) into a proportional voltage.

```
Photodiode cathode wire (from headband) --> Pin 2 (IN1-)
Photodiode anode wire (from headband)   --> STAR_GND

Pin 3 (IN1+) --> STAR_GND

Between Pin 1 (OUT1) and Pin 2 (IN1-):
  - 100k Ohm metal film resistor (Rf)
  - 10pF ceramic capacitor (Cf) IN PARALLEL with the 100k resistor
```

#### Installing the Feedback Components

- The 100k resistor and 10pF capacitor both connect between the same two points: Pin 1 and Pin 2.
- On a breadboard, place them in adjacent rows bridging the same two nodes.

> **The 10pF capacitor is CRITICAL for stability.** Without it, the TIA will oscillate. Use an NPO/C0G type ceramic capacitor - these have stable capacitance vs temperature. Do not substitute a different value without recalculating stability.

### 6.4 Stage 2: Voltage Gain Amplifier

This stage amplifies the TIA output by 48x (47k/1k + 1 = 48).

```
Pin 5 (IN2+) --> Pin 1 (OUT1)    [connects Stage 1 output to Stage 2 input]

Pin 6 (IN2-) --> Junction node:
  - 1k Ohm resistor (Rg) from this node to STAR_GND
  - 47k Ohm resistor (Rf2) from this node to Pin 7 (OUT2)

Pin 7 (OUT2) --> ADS1115 A0 input
```

The gain of this stage is: **G = 1 + (Rf2 / Rg) = 1 + (47k / 1k) = 48 V/V**

Combined with the 100k TIA, total transimpedance is: **100k x 48 = 4.8 Megohms**

---

## Step 7: Wire the ADS1115 ADC

### 7.1 ADS1115 Module Connections

Most ADS1115 breakout boards (like Adafruit 1085) have clearly labeled pins:

```
ADS1115 VDD  --> 3.3V rail (NOT 5V!)
ADS1115 GND  --> STAR_GND (this IS the star ground node)
ADS1115 SDA  --> Arduino A4
ADS1115 SCL  --> Arduino A5
ADS1115 ADDR --> STAR_GND (sets I2C address to 0x48)
ADS1115 A0   --> MCP6022 Pin 7 (OUT2)
```

### 7.2 Local Decoupling

- Place a **0.1uF ceramic capacitor** between ADS1115 VDD and GND, as close to the module pins as possible.

### 7.3 I2C Connections

- The Adafruit ADS1115 module has built-in 10k pull-up resistors on SDA and SCL. You do not need to add external pull-ups.
- If using a bare ADS1115 chip (not a module), you will need 4.7k pull-ups on both SDA and SCL to 3.3V.

---

## Step 8: Connect the Headband Probe Wires

### 8.1 LED Connections

Connect the pigtail wires from the headband to the LED driver circuits:

- **730nm LED anode wire** --> the node after the 68 Ohm resistor (toward the LED, away from 5V)
- **730nm LED cathode wire** --> Q1 Collector
- **850nm LED anode wire** --> the node after its 68 Ohm resistor
- **850nm LED cathode wire** --> Q2 Collector

> **Double-check polarity!** A reversed LED will not emit light and your channel will read zero. Worse, if you later think you have a circuit problem and start changing other things, you'll waste hours.

### 8.2 Photodiode Connections

- **Photodiode cathode** (shielded wire center conductor) --> MCP6022 Pin 2 (IN1-)
- **Photodiode anode** (shielded wire shield/ground) --> STAR_GND

> **Keep this shielded wire run as short as possible.** The signal at this node is in the nano-amp range. Every centimeter of unshielded wire is an antenna for 60Hz mains noise.

---

## Step 9: Verify Wiring Before Power-On

Go through this checklist with a multimeter before applying any power.

### 9.1 Continuity Checks (Multimeter in Continuity/Beep Mode)

| Test | Expected Result |
|:-----|:---------------|
| Arduino GND to STAR_GND | Continuity (beep) |
| ADS1115 GND to STAR_GND | Continuity |
| MCP6022 Pin 4 to STAR_GND | Continuity |
| Q1 Emitter to STAR_GND | Continuity (via LED_GND_RETURN) |
| Q2 Emitter to STAR_GND | Continuity (via LED_GND_RETURN) |
| 5V_NODE to 3.3V rail | **No continuity** (open) |
| MCP6022 Pin 1 to Pin 2 | Continuity (through 100k Rf) |
| ADS1115 VDD to 5V_NODE | **No continuity** (must be on 3.3V) |
| ADS1115 VDD to 3.3V rail | Continuity |

### 9.2 Short Circuit Checks

| Test | Expected Result |
|:-----|:---------------|
| 5V_NODE to GND | **No continuity** (should be open or show capacitor charge) |
| 3.3V rail to GND | **No continuity** (should be open or show capacitor charge) |

> If you find a short between any power rail and ground, **do not power on**. Find and fix the short first.

---

## Step 10: First Power-On (Current-Limited)

### 10.1 Setup

- Connect a **current-limited bench power supply** to the Arduino USB port (or use a USB cable from the bench supply).
- Set the current limit to **30-50mA**.
- Set the voltage to 5V.

### 10.2 Power On and Observe

- Turn on the supply. Current draw should be in the 20-40mA range (Arduino + indicator LED + quiescent IC current).
- The **green indicator LED should light up**. If it doesn't, check its polarity and the 150 Ohm resistor.
- If the supply hits the current limit immediately, **turn off and check for shorts**.

### 10.3 Voltage Checks

With the circuit powered, verify these voltages with your multimeter (referenced to STAR_GND):

| Test Point | Expected Voltage | Tolerance |
|:-----------|:----------------|:----------|
| Arduino 5V pin | ~5.0V | +/- 0.25V |
| Arduino 3.3V pin | ~3.3V | +/- 0.1V |
| MCP6022 Pin 8 (V+) | ~3.3V | +/- 0.1V |
| MCP6022 Pin 4 (V-) | ~0V | < 50mV |
| ADS1115 VDD | ~3.3V | +/- 0.1V |
| MCP6022 Pin 7 (OUT2) | 0V-3.3V range | Should be low with no light on probe |

If all voltages check out, your wiring is correct. You can now switch to the USB battery pack for further testing.

---

## Complete Wiring Summary

```
================================================================================
SIGNAL PATH SUMMARY
================================================================================

POWER:
  USB Battery --> Arduino USB --> Arduino 5V --> [Fuse] --> 5V_NODE
  Arduino 3.3V --> 3.3V_RAIL
  All grounds --> STAR_GND (at ADS1115 GND)

LED DRIVE:
  Arduino D2 --> [470R] --> Q1 Base
  5V_NODE --> [68R] --> 730nm LED(+) --> 730nm LED(-) --> Q1 Coll
  Q1 Emit --> LED_GND_RETURN --> STAR_GND

  Arduino D3 --> [470R] --> Q2 Base
  5V_NODE --> [68R] --> 850nm LED(+) --> 850nm LED(-) --> Q2 Coll
  Q2 Emit --> LED_GND_RETURN --> STAR_GND

ANALOG FRONT END:
  BPW34 cathode --> MCP6022 Pin 2 (IN1-)
  BPW34 anode --> STAR_GND
  MCP6022 Pin 3 (IN1+) --> STAR_GND
  100k + 10pF in parallel between Pin 1 and Pin 2
  MCP6022 Pin 5 (IN2+) <-- Pin 1 (OUT1)
  47k between Pin 7 (OUT2) and Pin 6 (IN2-)
  1k between Pin 6 (IN2-) and STAR_GND
  MCP6022 Pin 7 (OUT2) --> ADS1115 A0

DIGITAL:
  ADS1115 SDA --> Arduino A4
  ADS1115 SCL --> Arduino A5
  ADS1115 ADDR --> STAR_GND

DECOUPLING:
  100uF: 5V_NODE to STAR_GND
  10uF: 3.3V_RAIL to STAR_GND
  0.1uF: MCP6022 Pin 8 to Pin 4
  0.1uF: ADS1115 VDD to GND
================================================================================
```

---

## Common Mistakes

| Mistake | Consequence | Prevention |
|:--------|:-----------|:-----------|
| ADS1115 VDD connected to 5V | May damage Arduino I2C pins | Triple-check: 3.3V only |
| No star ground discipline | LED switching noise corrupts analog signal | Every ground wire goes to one point |
| Missing 10pF TIA cap | Op-amp oscillates, garbage readings | Install it. NPO/C0G type only |
| LED ground shares analog ground trace | Noise coupling | Use dedicated LED_GND_RETURN wire |
| Photodiode wired backwards | Zero or inverted signal | Cathode to Pin 2, anode to GND |
| First power-on without current limit | Destroyed components from wiring fault | Always use bench supply first |
| Missing decoupling capacitors | Noisy, unstable readings | Install all four caps close to IC pins |

---

## Checkpoint

Before moving to Part 3, verify:

- [ ] Green indicator LED lights on power-on
- [ ] All voltage rails measure correctly (5V, 3.3V, grounds at 0V)
- [ ] ADS1115 is powered from 3.3V (not 5V)
- [ ] Star ground is implemented - all grounds converge at one node
- [ ] LED ground return is a separate wire from analog ground
- [ ] 10pF cap is installed across the TIA feedback resistor
- [ ] All four decoupling caps are installed close to IC pins
- [ ] No shorts between power rails and ground
- [ ] Headband probe wires are connected with correct polarity

**Proceed to [Part 3: Firmware](Part3_Firmware.md)**
