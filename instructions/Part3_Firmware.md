# Part 3: Firmware

**Estimated Time:** 4-5 Hours (coding, upload, and signal tuning)
**Difficulty:** Moderate - the code is straightforward, but understanding *why* each piece exists is important

---

## Overview

In this part you will write and upload the Arduino firmware that:

1. Controls the two LEDs in a 4-state time-division multiplexing (TDM) sequence
2. Reads the ADS1115 ADC via I2C after each state
3. Subtracts ambient light to isolate the tissue signal
4. Applies an Exponential Moving Average (EMA) filter for noise reduction
5. Detects a stable baseline during the first 30 seconds
6. Calculates and outputs percent change from baseline at 10 Hz

By the end of this part, you should see live oxygenation trend data on the Arduino Serial Plotter.

---

## Software Setup

### Required Software

1. **Arduino IDE** (version 2.x recommended) - download from arduino.cc
2. **Board package:** Arduino SAMD Boards (for Nano 33 IoT)
   - In Arduino IDE: Tools > Board > Boards Manager > search "Arduino SAMD" > Install
3. **Library:** Adafruit ADS1X15
   - In Arduino IDE: Tools > Manage Libraries > search "Adafruit ADS1X15" > Install
   - This will also install the Adafruit BusIO dependency

### Board Configuration

| Setting | Value |
|:--------|:------|
| Board | Arduino Nano 33 IoT |
| Port | (your USB port) |

---

## Understanding the TDM Measurement Cycle

Before writing code, understand what the firmware does each cycle and why.

### Why Time-Division Multiplexing?

You have two LEDs and one photodiode. If both LEDs were on simultaneously, you couldn't distinguish which wavelength the photodiode is responding to. TDM solves this by turning on one LED at a time and reading the photodiode for each.

### Why Ambient Readings?

Room light, sunlight, and other light sources also hit the photodiode. If you only measured "LED on" readings, ambient light would be mixed into your measurement. By taking an "LED off" reading immediately before each "LED on" reading and subtracting, you cancel out ambient light contamination.

### The 4-State Sequence

```
State 1: All LEDs OFF  --> Read ADC --> Store as AmbientA
State 2: 730nm LED ON  --> Read ADC --> Subtract AmbientA --> "Red signal"
State 3: All LEDs OFF  --> Read ADC --> Store as AmbientB
State 4: 850nm LED ON  --> Read ADC --> Subtract AmbientB --> "IR signal"
```

Each state requires:
- 1ms settling delay after changing GPIO state (LED on/off transition)
- ~15.6ms for one ADS1115 conversion at 64 SPS

Total minimum cycle time: 4 x (1ms + 15.6ms) = **~66.5ms per full cycle** (~15 Hz per channel).

---

## The Firmware - Section by Section

### Section 1: Includes and Constants

```cpp
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// --- Pin Definitions ---
const int LED_730_PIN = 2;   // D2 controls 730nm (Red) LED via TIP31C
const int LED_850_PIN = 3;   // D3 controls 850nm (IR) LED via TIP31C

// --- ADC ---
Adafruit_ADS1115 ads;

// --- EMA Filter ---
// alpha = 0.025 gives N_eff = 1/alpha = 40 samples effective averaging
const float ALPHA = 0.025;
float emaRed = 0.0;
float emaIR  = 0.0;

// --- Baseline Detection ---
const unsigned long BASELINE_PERIOD_MS = 30000;  // 30 seconds
unsigned long startTime = 0;
bool baselineSet = false;
float baselineRed = 0.0;
float baselineIR  = 0.0;

// Running sums for baseline averaging
float baselineSumRed = 0.0;
float baselineSumIR  = 0.0;
unsigned long baselineCount = 0;

// --- Output Timing ---
const unsigned long OUTPUT_INTERVAL_MS = 100;  // 10 Hz display rate
unsigned long lastOutputTime = 0;
```

**Key design decisions:**
- **ALPHA = 0.025**: This means each new sample contributes only 2.5% to the running average. It takes roughly 40 samples (N_eff = 1/0.025 = 40) for the filter to substantially respond to a change. At ~15 Hz sample rate, that's about 2.7 seconds of smoothing, which rejects noise while still tracking the 30-second to multi-minute oxygenation changes seen in POTS.
- **30-second baseline**: Gives enough time for the EMA to fully settle after initial power-on transients.
- **10 Hz output**: The Serial Plotter updates smoothly at this rate. The internal TDM runs faster (~15 Hz) but output is decoupled to 10 Hz.

### Section 2: Setup

```cpp
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }  // Wait for serial connection

  // Configure LED control pins
  pinMode(LED_730_PIN, OUTPUT);
  pinMode(LED_850_PIN, OUTPUT);
  digitalWrite(LED_730_PIN, LOW);  // Both LEDs off initially
  digitalWrite(LED_850_PIN, LOW);

  // Initialize I2C and ADS1115
  Wire.begin();
  if (!ads.begin()) {
    Serial.println("ERROR: ADS1115 not found. Check wiring.");
    while (1) { delay(1000); }  // Halt - cannot proceed without ADC
  }

  // Configure ADS1115
  ads.setGain(GAIN_TWO);       // +/- 2.048V range
  ads.setDataRate(RATE_ADS1115_64SPS);  // 64 samples per second

  startTime = millis();
  Serial.println("Cerebral Oximeter starting...");
  Serial.println("Establishing baseline - keep subject supine and still.");
}
```

**Notes:**
- `GAIN_TWO` sets the PGA to +/-2.048V, giving a resolution of 0.0625mV per count (2.048V * 2 / 65536).
- `RATE_ADS1115_64SPS` sets each conversion to ~15.6ms. Lower rates (8 or 16 SPS) give less noise but reduce your TDM cycle rate.
- The `while (!Serial)` wait is useful during development so you don't miss startup messages. You may remove it for battery-only operation.

### Section 3: ADC Read Helper

```cpp
int16_t readADC() {
  return ads.readADC_SingleEnded(0);  // Read channel A0
}
```

> **Spec note:** The design document (Section 5.4) specifies ADS1115 in continuous mode. This firmware uses single-shot mode instead via `readADC_SingleEnded()`. Single-shot mode is used here because it is simpler (one function call per reading, no stale-data risk) and the timing still closes: `readADC_SingleEnded()` blocks until the conversion completes, so each call takes ~15.6ms at 64 SPS plus minor I2C overhead. The 4-state TDM cycle completes well within the design target. If your instructor requires continuous mode, replace `readADC()` with `ads.getLastConversionResults()` and add `ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, true)` at the end of `setup()`.

In single-ended mode, the ADS1115 returns a 16-bit signed integer. For our +/-2.048V range with single-ended input (0V to VDD), valid readings are 0 to ~26400 counts (0V to 3.3V).

### Section 4: TDM Cycle

```cpp
void performTDMCycle(float &redSignal, float &irSignal) {
  int16_t ambient, active;

  // State 1: Ambient A (all LEDs off)
  digitalWrite(LED_730_PIN, LOW);
  digitalWrite(LED_850_PIN, LOW);
  delay(1);  // 1ms settling
  ambient = readADC();

  // State 2: 730nm ON
  digitalWrite(LED_730_PIN, HIGH);
  delay(1);  // 1ms settling
  active = readADC();
  digitalWrite(LED_730_PIN, LOW);
  redSignal = (float)(active - ambient);

  // State 3: Ambient B (all LEDs off)
  delay(1);  // 1ms settling
  ambient = readADC();

  // State 4: 850nm ON
  digitalWrite(LED_850_PIN, HIGH);
  delay(1);  // 1ms settling
  active = readADC();
  digitalWrite(LED_850_PIN, LOW);
  irSignal = (float)(active - ambient);
}
```

**Why subtract ambient separately for each LED?** Ambient light can drift (clouds, someone walking past, etc.). By taking a fresh ambient reading immediately before each LED measurement, you track and cancel even fast ambient changes.

### Section 5: Main Loop

```cpp
void loop() {
  float redSignal, irSignal;

  // Run one complete TDM cycle
  performTDMCycle(redSignal, irSignal);

  // Apply EMA filter
  if (emaRed == 0.0 && emaIR == 0.0) {
    // First sample: initialize EMA to avoid slow ramp-up
    emaRed = redSignal;
    emaIR  = irSignal;
  } else {
    emaRed = ALPHA * redSignal + (1.0 - ALPHA) * emaRed;
    emaIR  = ALPHA * irSignal  + (1.0 - ALPHA) * emaIR;
  }

  // Baseline accumulation phase
  if (!baselineSet) {
    baselineSumRed += emaRed;
    baselineSumIR  += emaIR;
    baselineCount++;

    if (millis() - startTime >= BASELINE_PERIOD_MS) {
      baselineRed = baselineSumRed / baselineCount;
      baselineIR  = baselineSumIR  / baselineCount;
      baselineSet = true;
      Serial.println("Baseline established.");
    }
  }

  // Output at 10 Hz
  unsigned long now = millis();
  if (now - lastOutputTime >= OUTPUT_INTERVAL_MS) {
    lastOutputTime = now;

    if (baselineSet && baselineRed > 0 && baselineIR > 0) {
      float pctRed = ((emaRed - baselineRed) / baselineRed) * 100.0;
      float pctIR  = ((emaIR  - baselineIR)  / baselineIR)  * 100.0;

      // Output format for Arduino Serial Plotter
      // Two comma-separated values: Red%change, IR%change
      Serial.print(pctRed, 2);
      Serial.print(",");
      Serial.println(pctIR, 2);
    } else if (!baselineSet) {
      // During baseline period, show raw EMA values
      Serial.print("BASELINE:");
      Serial.print(emaRed, 1);
      Serial.print(",");
      Serial.println(emaIR, 1);
    }
  }
}
```

**Key points in the main loop:**
- **EMA initialization**: The first sample seeds the EMA directly. Without this, the filter starts at 0 and takes ~40 samples to ramp up to the actual signal level, producing a false "rising signal" artifact.
- **Baseline averaging**: During the 30-second baseline period, we average all EMA-filtered values. This is more stable than using a single snapshot.
- **Division-by-zero guard**: `baselineRed > 0 && baselineIR > 0` prevents NaN/infinity if baseline somehow captured zero (would indicate a hardware problem).
- **Output format**: Two comma-separated values per line. The Arduino Serial Plotter interprets each comma-separated value as a separate trace.

---

## Complete Firmware Listing

Combine all sections above into a single `.ino` file named `cerebral_oximeter.ino`:

```cpp
/*
 * Cerebral Oximeter - NIRS Oxygenation Trend Monitor
 * EE 201 - Biomedical Instrumentation
 *
 * 4-state TDM, dual-wavelength (730nm/850nm)
 * EMA filter (alpha=0.025, N_eff=40)
 * 30-second automatic baseline
 * 10 Hz serial output for Arduino Serial Plotter
 */

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

const int LED_730_PIN = 2;
const int LED_850_PIN = 3;

Adafruit_ADS1115 ads;

const float ALPHA = 0.025;
float emaRed = 0.0;
float emaIR  = 0.0;

const unsigned long BASELINE_PERIOD_MS = 30000;
unsigned long startTime = 0;
bool baselineSet = false;
float baselineRed = 0.0;
float baselineIR  = 0.0;
float baselineSumRed = 0.0;
float baselineSumIR  = 0.0;
unsigned long baselineCount = 0;

const unsigned long OUTPUT_INTERVAL_MS = 100;
unsigned long lastOutputTime = 0;

int16_t readADC() {
  return ads.readADC_SingleEnded(0);
}

void performTDMCycle(float &redSignal, float &irSignal) {
  int16_t ambient, active;

  // State 1: Ambient A
  digitalWrite(LED_730_PIN, LOW);
  digitalWrite(LED_850_PIN, LOW);
  delay(1);
  ambient = readADC();

  // State 2: 730nm ON
  digitalWrite(LED_730_PIN, HIGH);
  delay(1);
  active = readADC();
  digitalWrite(LED_730_PIN, LOW);
  redSignal = (float)(active - ambient);

  // State 3: Ambient B
  delay(1);
  ambient = readADC();

  // State 4: 850nm ON
  digitalWrite(LED_850_PIN, HIGH);
  delay(1);
  active = readADC();
  digitalWrite(LED_850_PIN, LOW);
  irSignal = (float)(active - ambient);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  pinMode(LED_730_PIN, OUTPUT);
  pinMode(LED_850_PIN, OUTPUT);
  digitalWrite(LED_730_PIN, LOW);
  digitalWrite(LED_850_PIN, LOW);

  Wire.begin();
  if (!ads.begin()) {
    Serial.println("ERROR: ADS1115 not found. Check wiring.");
    while (1) { delay(1000); }
  }

  ads.setGain(GAIN_TWO);
  ads.setDataRate(RATE_ADS1115_64SPS);

  startTime = millis();
  Serial.println("Cerebral Oximeter starting...");
  Serial.println("Establishing baseline - keep subject supine and still.");
}

void loop() {
  float redSignal, irSignal;

  performTDMCycle(redSignal, irSignal);

  if (emaRed == 0.0 && emaIR == 0.0) {
    emaRed = redSignal;
    emaIR  = irSignal;
  } else {
    emaRed = ALPHA * redSignal + (1.0 - ALPHA) * emaRed;
    emaIR  = ALPHA * irSignal  + (1.0 - ALPHA) * emaIR;
  }

  if (!baselineSet) {
    baselineSumRed += emaRed;
    baselineSumIR  += emaIR;
    baselineCount++;

    if (millis() - startTime >= BASELINE_PERIOD_MS) {
      baselineRed = baselineSumRed / baselineCount;
      baselineIR  = baselineSumIR  / baselineCount;
      baselineSet = true;
      Serial.println("Baseline established.");
    }
  }

  unsigned long now = millis();
  if (now - lastOutputTime >= OUTPUT_INTERVAL_MS) {
    lastOutputTime = now;

    if (baselineSet && baselineRed > 0 && baselineIR > 0) {
      float pctRed = ((emaRed - baselineRed) / baselineRed) * 100.0;
      float pctIR  = ((emaIR  - baselineIR)  / baselineIR)  * 100.0;

      Serial.print(pctRed, 2);
      Serial.print(",");
      Serial.println(pctIR, 2);
    } else if (!baselineSet) {
      Serial.print("BASELINE:");
      Serial.print(emaRed, 1);
      Serial.print(",");
      Serial.println(emaIR, 1);
    }
  }
}
```

---

## Upload and Initial Testing

### Step 1: Compile

- Open the sketch in Arduino IDE.
- Select **Tools > Board > Arduino Nano 33 IoT**.
- Click **Verify** (checkmark button). Fix any compilation errors.
- Common error: missing library. Go back to library manager and install Adafruit ADS1X15.

### Step 2: Upload

- Connect the Arduino via USB.
- Select the correct **Port** under Tools.
- Click **Upload** (arrow button).
- The green indicator LED on your breadboard should be on.

### Step 3: Open Serial Monitor

- Open **Tools > Serial Monitor** (set baud to 115200).
- You should see: `Cerebral Oximeter starting...` followed by `Establishing baseline...`
- Baseline values should appear every 100ms: `BASELINE:xxx.x,yyy.y`
- After 30 seconds: `Baseline established.`
- Then percent change values: `x.xx,y.yy`

### Step 4: Open Serial Plotter

- Close Serial Monitor. Open **Tools > Serial Plotter**.
- You should see two traces (Red %, IR %) updating at 10 Hz.
- With the probe covered (dark), both traces should hover near 0% after baseline.

---

## Debugging Common Firmware Issues

| Symptom | Likely Cause | Fix |
|:--------|:------------|:----|
| "ADS1115 not found" error | I2C wiring wrong, ADS1115 not powered, ADDR pin floating | Check A4->SDA, A5->SCL, VDD->3.3V, ADDR->GND |
| Both channels read ~0 always | LEDs not firing, photodiode disconnected, or TIA not working | Check D2/D3 pin assignments, LED polarity, op-amp power |
| Signal is extremely noisy | Missing decoupling caps, long photodiode wire, no star ground | Review Part 2 wiring discipline |
| Signal saturates (pegged high) | Too much ambient light, light leak in probe, gain too high | Cover probe, check optical isolation |
| Signal drifts continuously | Probe not in contact with tissue, thermal drift during warm-up | Let circuit warm up 5 min, ensure good contact |
| One channel works, other reads 0 | Specific LED not wired, wrong polarity, or transistor problem | Check that specific LED channel's driver circuit |

---

## Understanding the Output

### During Baseline (first 30 seconds)

```
BASELINE:1250.3,980.7
```

These are raw EMA-filtered ADC counts. The absolute values depend on probe contact, ambient light, skin pigmentation, etc. What matters is that they are **stable** (not drifting more than a few percent).

### After Baseline

```
-0.45,0.12
-0.52,0.08
-0.61,-0.15
```

These are **percent change from baseline** for each channel. In a healthy standing test:
- Both values should hover within +/-2% during supine rest
- Upon standing, expect a brief 1-3% dip that recovers within 30 seconds

---

## Checkpoint

Before moving to Part 4, verify:

- [ ] Firmware compiles and uploads without errors
- [ ] Serial Monitor shows startup messages and baseline data
- [ ] "ADS1115 not found" error does NOT appear
- [ ] Both Red and IR channels show non-zero values when probe touches skin
- [ ] Values stabilize within ~30 seconds (EMA settling)
- [ ] "Baseline established" message appears after 30 seconds
- [ ] Percent change values appear and hover near 0% with probe stationary on skin
- [ ] Serial Plotter shows two smooth traces

**Proceed to [Part 4: Final Assembly and Validation](Part4_Final_Assembly_and_Validation.md)**
