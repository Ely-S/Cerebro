/*
 * Cerebral Oximeter - NIRS Oxygenation Trend Monitor
 * EE 201 - Biomedical Instrumentation
 *
 * Hardware: Arduino Nano 33 IoT + ADS1115 + MCP6022 + BPW34
 * Revision: 3.2 (DCN-002: TIA Cf = 10pF)
 *
 * Signal chain:
 *   730nm/850nm LEDs (TIP31C drivers, D2/D3)
 *   -> BPW34 photodiode
 *   -> MCP6022 2-stage TIA (100k Rf, 10pF Cf, 48x gain)
 *   -> ADS1115 16-bit ADC (A0, I2C 0x48)
 *   -> Arduino Nano 33 IoT (A4=SDA, A5=SCL)
 *
 * Algorithm:
 *   4-state TDM: AmbientA -> 730nm -> AmbientB -> 850nm
 *   1ms settling delay per state transition
 *   EMA filter: alpha=0.025, N_eff = 1/alpha = 40
 *   30-second automatic baseline averaging
 *   10 Hz serial output (percent change from baseline)
 *
 * Serial Plotter output format (after baseline):
 *   <Red_%change>,<IR_%change>
 *   e.g.: -1.23,0.45
 *
 * Safety: Time-avg emitted power < 10mW per channel (bench-verified).
 *         For educational use only. NOT for clinical diagnosis.
 */

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "algorithm.h"

// --- Pin Definitions ---
const int LED_730_PIN = 2;  // D2: 730nm (Red/Far-Red) LED via TIP31C Q1
const int LED_850_PIN = 3;  // D3: 850nm (IR) LED via TIP31C Q2

// --- ADC ---
Adafruit_ADS1115 ads;

// --- EMA Filter ---
// alpha = 0.025 => N_eff = 1/alpha = 40 samples effective averaging
// At ~15 Hz TDM rate, N_eff=40 gives ~2.7 seconds of smoothing.
// This rejects noise while tracking the 30s-to-minutes oxygenation
// changes characteristic of POTS.
const float ALPHA = 0.025;
float emaRed = 0.0;
float emaIR  = 0.0;

// --- Baseline Detection ---
// 30 seconds allows EMA to fully settle after power-on transients.
const unsigned long BASELINE_PERIOD_MS = 30000;
unsigned long startTime    = 0;
bool          baselineSet  = false;
float         baselineRed  = 0.0;
float         baselineIR   = 0.0;

// Running sums for baseline averaging (more stable than a single snapshot)
float         baselineSumRed   = 0.0;
float         baselineSumIR    = 0.0;
unsigned long baselineCount    = 0;

// --- Output Timing ---
// TDM runs at ~15 Hz internally; output is decoupled to 10 Hz for
// smooth Serial Plotter display.
const unsigned long OUTPUT_INTERVAL_MS = 100;
unsigned long lastOutputTime = 0;

// ---------------------------------------------------------------------------
// readADC()
//
// Single-shot read of ADS1115 channel A0.
// readADC_SingleEnded() blocks ~15.6ms at 64 SPS until conversion completes,
// satisfying the TDM timing budget without stale-data risk.
//
// Note: The design document (Section 5.4) specifies continuous mode.
// Single-shot is used here for simplicity and identical timing behavior.
// To switch to continuous mode: replace readADC_SingleEnded(0) with
// ads.getLastConversionResults() and add
// ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, true) in setup().
// ---------------------------------------------------------------------------
int16_t readADC() {
  return ads.readADC_SingleEnded(0);
}

// ---------------------------------------------------------------------------
// performTDMCycle()
//
// Executes one complete 4-state time-division multiplex cycle:
//   State 1: All LEDs OFF  -> 1ms settle -> Read ADC -> AmbientA
//   State 2: 730nm LED ON  -> 1ms settle -> Read ADC -> Red = active - AmbientA
//   State 3: All LEDs OFF  -> 1ms settle -> Read ADC -> AmbientB
//   State 4: 850nm LED ON  -> 1ms settle -> Read ADC -> IR  = active - AmbientB
//
// Separate ambient reads for each LED channel cancel fast ambient drift
// (e.g., someone walking past, cloud cover changes).
//
// Minimum cycle time: 4 x (1ms settle + 15.6ms conversion) = ~66.5ms (~15 Hz)
// ---------------------------------------------------------------------------
void performTDMCycle(float &redSignal, float &irSignal) {
  int16_t ambient, active;

  // State 1: Ambient A (all LEDs off)
  digitalWrite(LED_730_PIN, LOW);
  digitalWrite(LED_850_PIN, LOW);
  delay(1);
  ambient = readADC();

  // State 2: 730nm ON
  digitalWrite(LED_730_PIN, HIGH);
  delay(1);
  active = readADC();
  digitalWrite(LED_730_PIN, LOW);
  redSignal = ambientSubtract((float)active, (float)ambient);

  // State 3: Ambient B (all LEDs off)
  delay(1);
  ambient = readADC();

  // State 4: 850nm ON
  digitalWrite(LED_850_PIN, HIGH);
  delay(1);
  active = readADC();
  digitalWrite(LED_850_PIN, LOW);
  irSignal = ambientSubtract((float)active, (float)ambient);
}

// ---------------------------------------------------------------------------
// setup()
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }  // Wait for USB serial (remove for battery-only use)

  pinMode(LED_730_PIN, OUTPUT);
  pinMode(LED_850_PIN, OUTPUT);
  digitalWrite(LED_730_PIN, LOW);
  digitalWrite(LED_850_PIN, LOW);

  Wire.begin();
  if (!ads.begin()) {
    Serial.println("ERROR: ADS1115 not found. Check wiring:");
    Serial.println("  - ADS1115 VDD -> 3.3V (NOT 5V)");
    Serial.println("  - ADS1115 GND -> STAR_GND");
    Serial.println("  - ADS1115 SDA -> Arduino A4");
    Serial.println("  - ADS1115 SCL -> Arduino A5");
    Serial.println("  - ADS1115 ADDR -> GND (address 0x48)");
    while (1) { delay(1000); }  // Halt - ADC required
  }

  // +/-2.048V PGA range: resolution = 2.048*2/65536 = 0.0625mV/count
  // Valid single-ended range at 3.3V supply: 0 to ~26400 counts
  ads.setGain(GAIN_TWO);
  ads.setDataRate(RATE_ADS1115_64SPS);

  startTime = millis();
  Serial.println("Cerebral Oximeter starting...");
  Serial.println("Establishing baseline - keep subject supine and still.");
  Serial.println("(30 seconds)");
}

// ---------------------------------------------------------------------------
// loop()
// ---------------------------------------------------------------------------
void loop() {
  float redSignal, irSignal;

  // Run one complete TDM cycle (~66.5ms)
  performTDMCycle(redSignal, irSignal);

  // Apply EMA filter.
  // First sample seeds the EMA directly to avoid a false "rising signal"
  // artifact that would occur if the filter started at zero.
  if (emaRed == 0.0 && emaIR == 0.0) {
    emaRed = redSignal;
    emaIR  = irSignal;
  } else {
    emaRed = applyEMA(redSignal, emaRed, ALPHA);
    emaIR  = applyEMA(irSignal,  emaIR,  ALPHA);
  }

  // Baseline accumulation: average all EMA-filtered samples over 30 seconds.
  if (!baselineSet) {
    baselineRed = runningMean(emaRed, baselineSumRed, baselineCount);
    baselineIR  = runningMean(emaIR,  baselineSumIR,  baselineCount);

    if (millis() - startTime >= BASELINE_PERIOD_MS) {
      // baselineRed / baselineIR already hold the running mean
      baselineSet = true;
      Serial.println("Baseline established.");
    }
  }

  // Output at 10 Hz (decoupled from ~15 Hz TDM rate)
  unsigned long now = millis();
  if (now - lastOutputTime >= OUTPUT_INTERVAL_MS) {
    lastOutputTime = now;

    if (baselineSet && baselineRed > 0 && baselineIR > 0) {
      // Percent change from baseline (Modified Beer-Lambert relative trend)
      // Target precision: +/-2% @ 95% confidence (per V&V Test 4.6)
      float pctRed = calcPctChange(emaRed, baselineRed);
      float pctIR  = calcPctChange(emaIR,  baselineIR);

      // Arduino Serial Plotter interprets comma-separated values as separate traces:
      //   Trace 1 (Red): 730nm channel - more sensitive to deoxygenated Hb
      //   Trace 2 (IR):  850nm channel - more sensitive to oxygenated HbO2
      Serial.print(pctRed, 2);
      Serial.print(",");
      Serial.println(pctIR, 2);

    } else if (!baselineSet) {
      // During baseline: show raw EMA counts for monitoring stability
      // Absolute values vary by subject/probe contact - look for stability, not magnitude
      Serial.print("BASELINE:");
      Serial.print(emaRed, 1);
      Serial.print(",");
      Serial.println(emaIR, 1);
    }
  }
}
