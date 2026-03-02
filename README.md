# Technical Design Document
## Cerebral Oximeter
### Single-Channel NIRS Relative Oxygenation Monitor
**Revision 3.2** - POTS Demonstration System (Prototype 1 Robustness Update)

- **Course:** EE 201 - Biomedical Instrumentation
- **Application:** POTS/Orthostatic Intolerance Demonstration
- **Precision Target:** +/- 2% (Relative Trend)
- **Safety Criterion:** <10mW time-avg emitted power per channel (bench-verified)

---

## 1. Project Overview

### 1.1 Objective
Design and build a biomedical device capable of detecting relative changes in cerebral oxygenation through the human forehead using near-infrared spectroscopy (NIRS). The device is specifically optimized for demonstrating Postural Orthostatic Tachycardia Syndrome (POTS) and orthostatic intolerance to students.

### 1.2 Clinical Background
POTS is characterized by an excessive heart rate increase (30+ bpm) within 10 minutes of standing. Clinical studies show POTS patients experience cerebral oxygen saturation decreases of 5-15% during standing, compared to only 1-3% in healthy controls. This device enables visualization of these differences for education.

### 1.3 Project Estimates
| Category | Time | Notes |
| :--- | :--- | :--- |
| Probe Assembly | 2-3 Hours | Including headband fabrication |
| Circuit Wiring | 3-4 Hours | Include shielding |
| Coding & Testing | 4-5 Hours | Enhanced signal processing |
| **Total Build Time** | **10-14 Hours** | Allow extra for troubleshooting |

**Component Cost:** $80-110 USD per station

---

## 2. Design Overview

### 2.1 How It Works - Simple Explanation
This device measures how much oxygen is in the blood flowing through your brain. It works by shining special near-infrared light through your forehead and measuring how much light comes back out. Here is the key principle:

**The Basic Idea:**
1. Two LEDs shine near-infrared light (730nm and 850nm) into your forehead
2. This light passes through skin, skull, and reaches brain tissue
3. Oxygen-rich blood absorbs different amounts of light than oxygen-poor blood
4. A photodiode detects the light that bounces back
5. By comparing the two wavelengths, we can track oxygenation changes

Think of it like shining a flashlight through your hand in a dark room - you can see the red glow because red light passes through tissue. Near-infrared light works similarly but penetrates deeper, reaching the brain. Blood with more oxygen absorbs more infrared light at certain wavelengths, so by measuring how much light is absorbed, we can track whether brain oxygenation is going up or down.

### 2.2 Why Two Wavelengths?
We use two different colors of infrared light because oxygenated hemoglobin (HbO2) and deoxygenated hemoglobin (Hb) absorb light differently at each wavelength:
- **730nm (Far-Red):** More sensitive to deoxygenated blood (Hb)
- **850nm (Infrared):** More sensitive to oxygenated blood (HbO2)

By measuring both, we can track the balance between oxygen delivery and oxygen consumption in brain tissue.

### 2.3 What the Device Measures
This device measures relative changes from a baseline, not absolute values. It tells you whether oxygenation is increasing or decreasing compared to when you started measuring. For POTS demonstration:
- When a healthy person stands up, oxygenation may drop 1-3% and quickly recover
- When a POTS patient stands up, oxygenation may drop 5-15% and stay low
- This difference is large enough to clearly see on a real-time graph

### 2.4 System Block Diagram

```text
+------------------+
|     ARDUINO      |
|   Nano 33 IoT    |
|                  |     +------------------+     +------------------+
| D2,D3: LED Ctrl  |---->|    LED DRIVER    |---->|    730nm LED     |---+
|                  |     |   (TIP31C x2)    |     |    850nm LED     |---|-+
| A4,A5: I2C       |<----|                  |     +------------------+   | |
|                  |     +------------------+             |              | |
| Serial Output    |                                      v              | |
+------------------+                               Light through         | |
         |                                        forehead tissue        | |
         v                                                |              | |
+------------------+                                      |              | |
|                  |     +------------------+             |              | |
|   TO COMPUTER    |     |    PHOTODIODE    |<------------+              | |
|                  |     |     (BPW34)      |<---------------------------+ |
+------------------+     +------------------+                              |
                                  |                                        |
                                  | Tiny electrical current (nano-amps)    |
                                  v                                        |
                         +------------------+                              |
                         |    AMPLIFIER     |                              |
                         |    (MCP6022)     |                              |
                         |   2-stage TIA    |                              |
                         +------------------+                              |
                                  |                                        |
                                  | Voltage signal (0-3V)                  |
                                  v                                        |
                         +------------------+                              |
                         |       ADC        |                              |
                         |    (ADS1115)     |                              |
                         |  16-bit, 64 SPS  |                              |
                         +------------------+                              |
```

### 2.5 Complete Wiring Diagram

```text
================================================================================
COMPLETE WIRING DIAGRAM
================================================================================
EXTERNAL 5V POWER SUPPLY (Wall Adapter, 1A minimum)
|
+-----[ FUSE 500mA ]-----+--------------------------+
                         |                          |
                      +--+--+                    +--+--+
                      | 5V  |                    | 5V  |
                      | Rail|                    | Rail|
                      +--+--+                    +--+--+
                         |                          |
                         |                          |
+------+--------+  +--------+-------+  +-----------+-----------+
| 730nm LED     |  | 850nm LED      |  | Arduino Nano 33 IoT   |
|               |  |                |  |                       |
| Anode --+----+------| Anode --+------+------| D2 (Red LED control)    |
|         |     |  |        |       |  | D3 (IR LED control)     |
|     [68 Ohm]  |  |    [68 Ohm]    |  | 3.3V output -----+      |
|         |     |  |        |       |  |                  |      |
| Cathode-+----+------| Cathode-+------+------| GND -------------+--+    |
|         |     |  |        |       |  | A4 (SDA) --------+  |   |
|      +---+    |  |     +---+      |  | A5 (SCL) --------+  |   |
|      |TIP31C |   |     |TIP31C |   |  |                  |  |   |
|      |  C    |   |     |  C    |   |  +------------------+  |   |
|      |B   E  |   |     |B   E  |   |                        |   |
|      |       |   |     |       |   |                        |   |
+------+       |   +---+------+------+                        |   |
       |       |       |      |                               |   |
     [470]     |     [470]    |                               |   |
       |       |       |      |                               |   |
+------+-+----+---+--------+-+----+---+----------------------------+-+
|      GND   GND                           |                         |
|                                          |                         |
|  PHOTODIODE ASSEMBLY                     |                         |
|                                          |                         |
|  +------------------+                    |                         |
|  |      BPW34       |                    |                         |
|  |    Photodiode    | (Mounted in EVA foam probe,                  |
|  |                  |  30mm from LEDs)                             |
|  | Cathode ---+----+-------------------------------------------|   |
|  |            |                                                |   |
|  | Anode -----+----+-------------------------------------------|-+ |
|  +------------------+ GND                                          |
|                     |                                              |
|                     | (Shielded cable, < 5cm length)               |
|                     v                                              |
|  +------------------+                                              |
|  |     MCP6022      |                                              |
|  |     Op-Amp       |                                              |
|  |                  |                                              |
|  | Pin 1: OUT1 ----+--------------------------+                    |
|  | Pin 2: IN1- <--+---[100k]--+               |                    |
|  | Pin 3: IN1+ <--+--- GND    |               |                    |
|  | Pin 4: GND  <--+-----------+--------------+                     |
|  | Pin 8: V+   <--+--- 3.3V   |                                    |
|  |                |   [4.7pF] |                                    |
|  | Stage 1: TIA   |           |                                    |
|  +------------------+       +-----+                                |
|                             |                                      |
|                             | (5mV signal from Stage 1)            |
|                             v                                      |
|  +------------------+       |                                      |
|  |     MCP6022      | Stage 2: Gain x48                            |
|  |   (same chip)    |       |                                      |
|  |                  |       |                                      |
|  | Pin 5: IN2+ <--+--- Stage 1 Output                              |
|  | Pin 6: IN2- <--+---[1k]---+                                     |
|  | Pin 7: OUT2 ----+----------+----------> To ADC                  |
|  |                 |  [47k]   |                                    |
|  +------------------+         |                                    |
|         GND                   |                                    |
|                             |                                      |
+-----------------------------------+--------------------------------+
                                    |
                                    v
+------------------+      +------------------+
|   ADS1115 ADC    |      |   TO COMPUTER    |
|                  |      |                  |
|   VDD ---- 3.3V  |      | USB cable from   |
|   GND ---- GND   |      | Arduino          |
|   SCL ---- A5    |      |                  |
|   SDA ---- A4    |      | Serial Plotter   |
|   ADDR --- GND   |      | shows real-time  |
|   A0 ----- Signal|      | oxygenation %    |
|            from  |      +------------------+
|            TIA   |
+------------------+
```

#### Important Notes
1. **DO NOT connect ADS1115 VDD to 5V** - use 3.3V only (Arduino Nano 33 IoT is 3.3V logic)
2. LED power comes from EXTERNAL 5V supply, NOT Arduino 5V pin
3. All grounds (GND) must be connected together
4. Photodiode to op-amp wire must be SHORT (< 5cm) and preferably shielded
5. Place 0.1uF decoupling capacitors near MCP6022 and ADS1115 power pins
6. Add bulk rail capacitors: 47-100uF at 5V entry and 10uF near analog 3.3V rail
7. The 4.7pF capacitor across 100k feedback is CRITICAL for stability

**RESISTOR VALUES:**
- LED current limit (730nm): 68 Ohm, 0.5W
- LED current limit (850nm): 68 Ohm, 0.5W
- Base resistor: 470 Ohm (both transistors)
- TIA feedback: 100k Ohm, metal film 1%
- Stage 2 feedback: 47k Ohm
- Stage 2 ground: 1k Ohm

**CAPACITOR VALUES:**
- TIA stability: 4.7pF ceramic (NPO/COG type preferred)
- Power decoupling (local HF): 0.1uF ceramic (x2)
- Bulk decoupling (5V rail entry): 47-100uF electrolytic (x1)
- Bulk decoupling (analog 3.3V rail): 10uF electrolytic or ceramic (x1)

---

## 3. Technical Specifications

| Parameter | Specification | Notes |
| :--- | :--- | :--- |
| **Measurement Type** | Relative Trend | Changes from baseline, not absolute |
| **Precision (Relative)** | Target +/- 2% @ 95% conf | Requires bench closure per V&V Test 4.6 |
| **Channels** | 1 (Deep Tissue) | 30mm source-detector separation |
| **Wavelengths** | 730nm & 850nm | Industry-standard NIRS wavelengths |
| **Sample Rate** | ADS1115 64 SPS state rate | 4-state TDM gives 16 Hz raw/channel (AmbientA/Red/AmbientB/IR) |
| **Safety** | Time-avg emitted power < 10mW per channel | Bench-measured under final TDM timing and drive settings |

---

## 4. System Architecture

| Stage | Function | Key Components |
| :--- | :--- | :--- |
| **1. Emitter** | High-power NIR LED drive | TIP31C, 68 Ohm (730nm) + 68 Ohm (850nm), New-Energy Star Board / ams OSRAM |
| **2. Detector** | Photocurrent amplification | BPW34, MCP6022 (2-stage TIA) |
| **3. Digital** | High-resolution sampling | ADS1115 @ 64 SPS |
| **4. Processing** | Signal filtering & output | Arduino Nano 33 IoT |
| **5. Optical Probe** | Tissue interface | EVA foam, headband mount |

---

## 5. Hardware Design Details

### 5.1 LED Specifications
| Component | Part Number | Specifications |
| :--- | :--- | :--- |
| **730nm LED** | New-Energy LST1-01G01-FRD1-00 | Star Board (Luminus SST-10), 130 deg angle |
| **850nm LED** | ams OSRAM SFH4253B | 850nm centroid (860nm peak), SMT PLCC-2, 120 deg beam |
| **Alt 850nm** | Epitex L850-05AU | Legacy option if SFH4253B unavailable |

### 5.2 LED Driver Circuit
Each LED is controlled by a TIP31C NPN transistor acting as a switch. The Arduino GPIO (3.3V logic) drives the transistor base through a 470 Ohm resistor (approximately 5.5mA base current), improving saturation margin without over-driving the MCU pin. LED current limits are intentionally asymmetric for robustness:
- 730nm channel uses 68 Ohm (~40mA nominal)
- 850nm channel uses 68 Ohm (~47mA nominal)

This keeps both channels in the target electrical range while adding optical-safety margin in the 850nm path.

### 5.3 Two-Stage Transimpedance Amplifier
- **Stage 1 (TIA):** 100k Ohm feedback resistor with 4.7pF stability capacitor. Converts photodiode current (nano-amps) to voltage. MCP6022 op-amp chosen for low input bias current and rail-to-rail output.
- **Stage 2 (Gain):** Non-inverting amplifier with 47k/1k = 48x voltage gain.
- **Total Transimpedance:** 100k x 48 = 4.8M Ohm equivalent.

### 5.4 ADS1115 ADC Configuration
| Parameter | Setting | Rationale |
| :--- | :--- | :--- |
| **PGA Range** | +/- 2.048V | Matches signal range |
| **Sample Rate** | 64 SPS | Lower noise than higher rates |
| **Mode** | Continuous | Smooth data stream |

> ⚠️ **Warning:** Connect ADS1115 VDD to 3.3V only (NOT 5V). Arduino Nano 33 IoT uses 3.3V logic levels.

---

## 6. Bill of Materials

| Component | Qty | Part Number | Cost |
| :--- | :--- | :--- | :--- |
| Arduino Nano 33 IoT | 1 | ABX00027 | $20-25 |
| ADS1115 ADC Module | 1 | Adafruit 1085 | $10-15 |
| MCP6022-I/P Op-Amp | 1 | DIP-8 | $1.50-2 |
| BPW34 Photodiode | 1 | Vishay | $2-4 |
| LED 730nm Star Board | 1 | New-Energy LST1-01G01 | $5-7 |
| LED 850nm | 1 | ams OSRAM SFH4253B | $1-4 |
| TIP31C Transistor | 2 | TO-220 | $1-2 |
| Elastic Headband | 1 | 2cm wide | $3-5 |
| Black EVA Foam | 1 | 2x4x2cm | $3-5 |
| 5V Wall Adapter | 1 | 1A min | $5-8 |
| Electrolytic Capacitor 47-100uF | 1 | 10V min | $0.20-0.50 |
| Capacitor 10uF | 1 | 6.3V min | $0.10-0.30 |
| Misc (resistors, caps, wire) | set | - | $5-10 |

**Estimated Total: $60-96 USD**

---

## 7. Firmware Design for +/-2% Precision

### 7.1 Key Requirements
To achieve +/-2% precision for POTS demonstration:
1. Exponential Moving Average (EMA) filter with alpha = 0.025 and explicit convention N_eff = 1/alpha = 40
2. 30-second automatic baseline detection
3. Percent change calculation from baseline
4. 4-state TDM sequence (AmbientA -> Red -> AmbientB -> IR) for ambient rejection and optical-duty control
5. 10 Hz output rate for Serial Plotter visualization (decoupled from TDM cycle timing)

### 7.2 TDM Measurement Sequence
Each full TDM cycle has a theoretical minimum of 62.5ms at ADS1115 64 SPS:
1. All LEDs OFF -> Read ADC -> Store as AmbientA
2. Red LED ON -> Read ADC -> Subtract AmbientA -> Store as Red signal
3. All LEDs OFF -> Read ADC -> Store as AmbientB
4. IR LED ON -> Read ADC -> Subtract AmbientB -> Store as IR signal
5. Apply EMA filter to both channels
6. Calculate percent change from baseline
7. Output to Serial every 100ms (10 Hz display rate)

**Notes:**
- 62.5ms is a theoretical minimum (4 conversions x 15.625ms). Actual runtime cycle is measured in V&V Test 3.7.
- Per-channel raw update rate is 16 Hz at 64 SPS with 4-state TDM.

### 7.3 Core Algorithm
```cpp
// EMA Filter (alpha=0.025, convention N_eff = 1/alpha = 40)
const float ALPHA = 0.025;
emaRed = ALPHA * redSample + (1 - ALPHA) * emaRed;
emaIR = ALPHA * irSample + (1 - ALPHA) * emaIR;

// Percent Change from Baseline
float pctChangeRed = ((emaRed - baselineRed) / baselineRed) * 100.0;
float pctChangeIR = ((emaIR - baselineIR) / baselineIR) * 100.0;
```

---

## 8. Assembly Instructions

The optical probe is the most critical component of the system. Proper mechanical and optical isolation is required to prevent light from leaking directly from the LEDs to the photodiode.

### 8.1 Optical Probe Assembly

#### Required Materials
- Black EVA foam block (approx. 2cm x 4cm x 2cm)
- 730nm Star Board LED
- 850nm SMT LED on carrier board
- BPW34 Photodiode
- Black rubber washers (1/8" / 3mm Inner Diameter) x 2-3
- Superglue and opaque adhesive (or opaque electrical tape)
- Shielded wire (< 5cm length)
- Elastic headband

#### Step-by-Step Instructions

1. **Prepare the Foam Base**
   - Cut the black EVA foam block to `2cm (height) x 4cm (length) x 2cm (width)`.
   - Punch a `7mm` hole cleanly through one side for the BPW34 photodiode.
   - Measure exactly `30mm` (center-to-center) from the photodiode hole and carve a `20mm` wide flat recess to seat the 730nm Star Board.
   - Carve a small flat pocket immediately next to the 730nm recess for the 850nm SMT LED carrier board.

2. **Prepare the 730nm Baffle**
   - Superglue a stack of 2-3 black rubber washers directly over the 730nm Star Board LED dome. 
   - *Purpose:* This acts as an essential optical baffle to restrict the wide 130-degree beam angle and guide the light forward into the tissue.

3. **Mount the Emitters**
   - Insert the 730nm Star Board (with glued washers) into its carved recess.
   - Mount the 850nm SFH4253B (already soldered to a small SMT carrier/perf adapter) in its pocket.
   - Ensure the emitting surfaces (top of washers / top of 850nm LED) are perfectly flush with the foam surface that will touch the skin.
   - Secure the boards with opaque adhesive. Label the wires extending from the back to avoid crossing the channels later.

4. **Mount the Detector**
   - Insert the BPW34 photodiode into the 7mm hole. 
   - The convex lens must face outward toward the tissue. (The flat side is the cathode and faces the back).
   - Solder the short, shielded wire to the photodiode leads. Keep this connection under 5cm to minimize electrical noise.

5. **Optical Isolation (CRITICAL STEP)**
   - Wrap additional black foam or opaque electrical tape around the back and sides of the LED and photodiode housings.
   - Ensure the rubber washer stack on the 730nm LED is perfectly seated with no gaps around its base.
   > ⚠️ **CRITICAL WARNING:** Light MUST NOT pass directly from the LEDs to the photodiode through the foam body or across the surface gap. Direct LED-to-photodiode light paths will completely overwhelm the faint tissue signal. This is the #1 failure point in construction. Take extra time to ensure perfect light blocking.

6. **Headband Integration**
   - Attach the completed foam block to the center of the elastic headband.
   - Add soft padding (like medical gauze or soft fabric) around the hard edges of the foam/boards for patient comfort.

7. **Verify Assembly (Dark Test)**
   - Point the probe into a dark space (or cover it completely with thick, opaque material).
   - Power the circuit. The signal reading should be near zero.
   - *Any significant reading indicates a light leak that must be patched before proceeding.*

### 8.2 Circuit Assembly Notes

**Power Rails:**
- **5V rail** (from external wall adapter): Powers the LED driver circuit (TIP31C collectors) ONLY.
- **3.3V rail** (from Arduino 3.3V pin): Powers the Op-amp (MCP6022 V+) and ADC (ADS1115 VDD).
- **Common ground:** All grounds (Arduino, ADC, Op-amp, external 5V supply, LED drivers) MUST be tied together.

**Wire Guidelines & Decoupling:**
- **Photodiode to op-amp:** Keep this wire as short as possible (max 5cm) and use shielded cable to prevent picking up 60Hz mains noise.
- Keep analog signal lines (photodiode, op-amp output) physically separated from digital lines (I2C, LED PWM).
- **Local Decoupling:** Add 0.1uF ceramic capacitors directly at the power pins of the MCP6022 and ADS1115, connecting to ground.
- **Bulk Decoupling:** Add a 47-100uF electrolytic capacitor where the 5V rail enters the board, and a 10uF capacitor near the analog 3.3V rail. Pay attention to capacitor polarity.

---

## 9. Calibration and Validation

### 9.1 Pre-Use Tests
- **Test 1 - Dark Reading:** Cover probe completely with opaque material. Signal should be near zero (+/- 5%).
- **Test 2 - Light Response:** Point probe at room light. Signal should increase significantly.
- **Test 3 - Finger Test:** Place finger over probe. Should see strong signal (blood absorbs light).
- **Test 4 - Baseline Stability:** Subject supine for 30 sec. Signal should be stable within +/- 3%.

### 9.2 Before Each Demonstration
1. Apply probe to subject forehead with consistent pressure.
2. Subject lies supine for 2 minutes (stabilization).
3. Wait for 'Baseline established' message.
4. Verify signal stability over 30 seconds before starting test.

---

## 10. POTS/Orthostatic Intolerance Demonstration Protocol

### 10.1 Equipment Required
- Cerebral oximeter device (assembled and calibrated)
- Examination table or tilt table
- Pulse oximeter for simultaneous HR monitoring
- Computer with Arduino Serial Plotter
- Chair for subject safety during recovery

### 10.2 Subject Preparation
1. Explain procedure and obtain informed consent.
2. Subject avoids caffeine, large meals, exercise for 2 hours prior.
3. Clean forehead with alcohol swab (remove oils for better contact).
4. Apply headband with probe over left forehead (avoid hairline).
5. Adjust for snug but comfortable fit.

### 10.3 Supine-to-Stand Test Protocol
| Phase | Duration | Activity | Expected Response |
| :--- | :--- | :--- | :--- |
| **1. Rest** | 2 min | Subject supine | Signal stabilizes |
| **2. Baseline** | 3 min | Continue supine | Stable SctO2, HR 60-80 |
| **3. Transition** | 15 sec | Stand quickly | Brief dip, partial recovery |
| **4. Standing** | 5-10 min | Stand still | See interpretation table |
| **5. Recovery** | 3 min | Return supine | SctO2 returns to baseline |

### 10.4 Expected Results by Subject Type
| Parameter | Healthy Control | POTS Patient | Vasovagal |
| :--- | :--- | :--- | :--- |
| **HR Change** | +10-20 bpm | +30-50+ bpm | +20-40 bpm |
| **Peak HR** | <100 bpm | >120 bpm | Variable |
| **SctO2 Change** | -1 to -3% | -5 to -15% | -10 to -20% |
| **Pattern** | Quick adaptation | Sustained drop | Progressive decline |
| **Recovery** | <30 sec | 1-3 min | Variable |

### 10.5 Real-Time Interpretation for Students
**Normal Response:**
Heart rate increases 10-20 bpm to compensate for gravity. Blood vessels in legs constrict to push blood upward. Cerebral oxygenation drops only 1-3% - the brain remains well-oxygenated. Body quickly adapts and maintains stable perfusion.

**POTS Response:**
Excessive heart rate increase (30+ bpm) indicates inadequate vasoconstriction. Blood pools in lower extremities despite rapid heart rate. Cerebral oxygenation drops 5-15% - the brain is becoming hypoperfused. Patient experiences lightheadedness and palpitations. Recovery is slower because the autonomic system struggles to re-adapt.

### 10.6 Safety Guidelines
Stop the test immediately if:
- Subject feels faint or vision blurs
- Subject appears pale, sweaty, or confused
- SctO2 drops more than 20% from baseline
- Heart rate exceeds 150 bpm
- Subject requests to stop

**Recovery procedure:** Help subject lie down immediately, elevate legs if possible, monitor until symptoms resolve.

> **IMPORTANT:** This is for educational demonstration only. It is NOT a diagnostic test for POTS. Clinical POTS diagnosis requires comprehensive autonomic testing by qualified medical professionals.

---

## 11. Expected Results and Interpretation

| Signal Change | Physiological Meaning | Clinical Significance |
| :--- | :--- | :--- |
| 0 to -2% | Normal adaptation | Healthy response |
| -2 to -5% | Mild hypoperfusion | May be normal or mild OI |
| -5 to -10% | Moderate hypoperfusion | Suggests orthostatic intolerance |
| -10 to -15% | Significant hypoperfusion | Consistent with POTS/VVS |
| >-15% | Severe hypoperfusion | High syncope risk |

**Teaching Points:** This demonstration teaches autonomic physiology, baroreceptor reflex, cerebral autoregulation, POTS pathophysiology, NIRS technology, and signal processing principles.

---

## 12. Design Assumptions

| Category | Assumption | Rationale |
| :--- | :--- | :--- |
| **Physiological** | NIR penetrates skull to cortex | Optical window 650-950nm |
| **Physiological** | POTS causes 5-15% SctO2 drop | Clinical studies confirm |
| **Technical** | EMA convention fixed to N_eff = 1/alpha = 40 | Ensures consistent filter interpretation across docs/tests |
| **Technical** | Two-stage TIA optimal | Better noise performance |
| **Safety** | Criterion basis is explicit and bench-verifiable | Time-avg emitted power per channel < 10mW in final TDM mode |

---

## 13. Limitations
- **Device:** Relative measurements only, single-channel, forehead only, subject variability.
- **Confounding:** Extracranial contamination, motion artifact, ambient light, probe contact variations.
- **Safety:** Not FDA evaluated, not for clinical diagnosis, educational use only.

---

## 14. References
1. Novak P. 'Cerebral Blood Flow in Orthostatic Intolerance.' JAHA. 2025.
2. Novak V et al. 'Cerebral oximetry in POTS patients.' Clin Auton Res. 2019.
3. Schie H et al. 'Cerebral oxygenation responses to postural changes.' Eur J Appl Physiol. 2019.
4. ISO 9919:2005. 'Pulse oximeter equipment.'
5. IEC 60825-1:2014. 'Safety of laser products.'
6. TI SBOA055. 'Compensate Transimpedance Amplifiers.'
7. Vishay BPW34 Datasheet.
