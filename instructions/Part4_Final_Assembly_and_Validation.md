# Part 4: Final Assembly and Validation

**Estimated Time:** 2-3 Hours
**Difficulty:** Low to Moderate - mostly testing and verification

---

## Overview

In this part you will:

1. Perform the formal pre-use validation tests to confirm your device works correctly
2. Transition from bench power supply to USB battery pack
3. Secure all wiring for portable operation
4. Run a live demonstration on a volunteer using the POTS/orthostatic test protocol

This is where the electronics project becomes a biomedical instrument. Rigor matters here.

---

## Step 1: Pre-Use Validation Tests

These four tests verify the optical, electrical, and signal-processing chain is working end-to-end. Run them in order. If a test fails, do not proceed - go back and fix the issue.

### Test 1: Dark Reading

**Purpose:** Verify that no light leaks from LEDs to photodiode through the probe body.

**Procedure:**
1. Power on the circuit (bench supply, current-limited).
2. Cover the probe completely with thick, opaque material (multiple layers of black electrical tape, or press it face-down into a folded black cloth in a dark room).
3. Let the firmware run through baseline (30 seconds).
4. Observe both Red and IR channels in Serial Monitor or Serial Plotter.

**Pass criteria:** Both channels read near zero, within +/-5% of baseline noise floor.

**If it fails:** Light is leaking inside the probe. Return to Part 1, Step 6 and add more optical isolation. Common leak paths: gap around the 730nm washer stack, thin spots in the foam, cracks at component mounting points.

---

### Test 2: Light Response

**Purpose:** Verify the photodiode and amplifier chain respond to light.

**Procedure:**
1. With the circuit running (after baseline), point the probe's tissue-contact face toward a well-lit room (not direct sunlight).
2. Observe the signal.

**Pass criteria:** Signal increases significantly compared to the dark reading. You should see a clear, large jump in raw values.

**If it fails:** Check photodiode orientation (lens must face outward), shielded wire continuity, op-amp power and feedback components, ADS1115 A0 connection.

---

### Test 3: Finger Test

**Purpose:** Verify the device can detect light absorption by blood-perfused tissue.

**Procedure:**
1. Reset the device (press Arduino reset button or reconnect USB).
2. Press your fingertip firmly over the probe so it covers both LEDs and the photodiode.
3. Wait for baseline to establish (30 seconds, hold your finger still).
4. Observe signal stability.

**Pass criteria:**
- Both channels show strong, non-zero signals during baseline accumulation.
- Signal is substantially different from the dark reading (tissue is transmitting some NIR light).
- You may see pulsatile variation (your heartbeat modulating the signal). This is normal and actually a good sign - it means the device is sensitive enough to detect blood volume changes.

**If it fails:** The probe may not be making good contact, or the amplifier gain may be insufficient. Check the 100k feedback resistor and the 47k/1k gain stage.

---

### Test 4: Baseline Stability

**Purpose:** Verify the EMA filter produces a stable output under controlled conditions.

**Procedure:**
1. Have a volunteer lie supine (face up) on a flat surface.
2. Apply the headband probe to the volunteer's forehead, centered above the left eyebrow, below the hairline.
3. Adjust for snug, consistent pressure (not too tight, not loose).
4. Let the system run for 2 full minutes without moving.
5. After baseline establishes (30 sec), monitor percent change values for the remaining 90 seconds.

**Pass criteria:** Both channels remain within **+/-3%** of zero (baseline) during the stationary observation period. Occasional brief excursions are acceptable if they return to baseline.

**If it fails:**
- Unstable contact pressure: readjust headband.
- Ambient light interference: dim room lights or shield the probe edges.
- Electrical noise: check star ground, decoupling caps, shielded wire.
- Subject moved: repeat with emphasis on staying still.

---

## Step 2: Transition to Battery Power

### 2.1 Battery Pack Requirements

Your USB battery pack must meet these criteria:

| Requirement | Reason |
|:-----------|:-------|
| Output >= 1A | Arduino + LEDs draw ~100-200mA peak |
| "Always-on" capable | Some packs auto-shutoff below ~100mA. The green indicator LED provides ~20mA hold load to help prevent this, but some packs still shut off |
| Stable 5V output | Cheap packs may droop under load, causing signal artifacts |

### 2.2 Battery Validation

1. Disconnect the bench power supply.
2. Connect the USB battery pack to the Arduino's USB port.
3. Verify the green indicator LED is on and stays on.
4. Run all four validation tests again on battery power.
5. Let the system run continuously for **10 minutes** without the battery pack shutting off.

> If the battery auto-shuts off, try a different pack. The green indicator LED draws ~20mA specifically to prevent this, but some battery packs require higher minimum current. As a workaround, you can decrease the indicator resistor from 150 Ohm to 100 Ohm (increases hold current to ~30mA), but do not go lower than 68 Ohm.

---

## Step 3: Secure the Wiring for Portable Use

The device needs to survive being carried to a demonstration table and worn by a volunteer. Loose breadboard wires will come out.

### 3.1 Wire Dress

- Press all breadboard wires firmly into their holes. Wiggle each connection gently - if it moves, it will fail during the demo.
- Use short, direct jumper wires. Replace any long, looping wires with shorter ones that lay flat against the breadboard.
- Secure the headband cable to the breadboard or table with tape to create a strain relief point. If someone tugs the headband cable, the force should hit the tape anchor, not pull wires out of the breadboard.

### 3.2 Label Everything

- Label the breadboard with the following identifiers near their respective components:
  - "730nm" and "850nm" near the transistor drivers
  - "TIA" near the MCP6022
  - "ADC" near the ADS1115
  - "STAR GND" at the star ground node
  - "5V" and "3.3V" on the power rails

### 3.3 Protect the Breadboard

- Place the breadboard on a rigid surface (piece of cardboard, small cutting board, etc.) and tape it down.
- This prevents the breadboard from flexing, which can pop wires out of their holes.

---

## Step 4: Running the Demonstration

### 4.1 Equipment Checklist

Before starting the demonstration, gather:

- [ ] Cerebral oximeter (headband + breadboard + battery pack) - all validated
- [ ] Computer with Arduino IDE and Serial Plotter open
- [ ] USB data cable connecting Arduino to computer
- [ ] Pulse oximeter or heart rate monitor (for simultaneous HR monitoring)
- [ ] Examination table, bed, or reclining chair
- [ ] Regular chair (for subject safety during standing phase)
- [ ] Alcohol swabs (for forehead cleaning)
- [ ] Stopwatch or timer

### 4.2 Subject Preparation

1. **Informed consent:** Explain the procedure to the volunteer. This is an educational demonstration, not a clinical test.
2. **Pre-test requirements:** The volunteer should have avoided caffeine, large meals, and exercise for at least 2 hours prior.
3. **Forehead prep:** Clean the volunteer's forehead with an alcohol swab to remove skin oils. This improves optical contact.
4. **Headband placement:**
   - Position the probe over the **left forehead**, centered above the eyebrow.
   - Avoid the hairline - hair blocks NIR light.
   - Adjust for snug, consistent pressure. Too loose = motion artifacts. Too tight = discomfort and altered blood flow.

### 4.3 Supine-to-Stand Test Protocol

| Phase | Duration | Instructions | What to Watch |
|:------|:---------|:------------|:-------------|
| **1. Rest** | 2 minutes | Volunteer lies supine (face up), relaxed | Signal should stabilize on the plotter |
| **2. Baseline** | 3 minutes | Continue supine, remain still | Both traces should be steady and flat. The firmware baseline captures during the first 30 seconds; this additional time confirms stability |
| **3. Transition** | ~15 seconds | Volunteer stands up quickly but safely | Note the exact moment of standing. You should see a dip in both traces |
| **4. Standing** | 5-10 minutes | Volunteer stands still, can hold onto chair for balance | Watch for recovery pattern (see interpretation below) |
| **5. Recovery** | 3 minutes | Volunteer returns to supine | Both traces should return toward 0% (baseline) |

### 4.4 Reading the Serial Plotter During the Test

The Serial Plotter displays two traces:
- **Trace 1 (Red channel, 730nm):** More sensitive to deoxygenated hemoglobin. When this drops, it suggests more deoxygenated blood.
- **Trace 2 (IR channel, 850nm):** More sensitive to oxygenated hemoglobin. When this drops, it suggests less oxygenated blood.

**Both traces dropping together** after standing indicates reduced cerebral blood flow/oxygenation - the hemodynamic response you are looking for.

### 4.5 Interpreting the Results

| Observation | Interpretation | What It Means |
|:-----------|:--------------|:-------------|
| Both traces dip 1-3% and recover within 30 sec | Normal autonomic response | Baroreceptor reflex is working. Vasoconstriction redirects blood to the brain. |
| Both traces dip 5-15% and stay low for minutes | Consistent with POTS/OI | Autonomic regulation is impaired. Brain is not receiving adequate blood flow despite compensatory tachycardia. |
| Progressive decline over several minutes | Vasovagal pattern | Blood pressure and cerebral perfusion are progressively failing. Watch for pre-syncope. |
| No change at all | Possible probe issue OR very robust autonomic function | Verify probe contact. If contact is good, the subject has excellent orthostatic tolerance. |

### 4.6 Teaching Points to Highlight

During the demonstration, explain to students:

1. **The baroreceptor reflex:** When you stand, blood pools in your legs due to gravity. Baroreceptors in the carotid artery detect reduced blood pressure and trigger vasoconstriction + increased heart rate to compensate.

2. **Why heart rate alone is insufficient:** A subject's heart rate can increase 30+ bpm (tachycardia) and their blood pressure can remain normal, yet cerebral blood flow is still reduced. This is the core problem in POTS - the compensation is present (heart rate goes up) but ineffective (brain blood flow still drops).

3. **What NIRS measures vs. what pulse oximetry measures:**
   - Pulse oximetry (SpO2) measures arterial oxygen saturation in a finger - it tells you about lung function and oxygen loading.
   - NIRS (SctO2) measures oxygenation of tissue including venous blood - it tells you about the balance between oxygen delivery and consumption in the brain itself.
   - A person can have 99% SpO2 and still have reduced cerebral oxygenation if blood flow to the brain is inadequate.

4. **The modified Beer-Lambert law:** The math behind NIRS. Two wavelengths give two equations, allowing you to solve for two unknowns (HbO2 and Hb concentrations). This project uses a simplified approach (relative trend only), but the principle is the same as commercial cerebral oximeters.

---

## Step 5: Safety During the Demonstration

### Mandatory Stopping Criteria

**Stop the test immediately if ANY of these occur:**

- Volunteer reports feeling faint, dizzy, or vision blurring
- Volunteer appears pale, sweaty, or confused
- SctO2 (either channel) drops more than **20%** from baseline
- Heart rate exceeds **150 bpm**
- Volunteer asks to stop for any reason

### Recovery Procedure

1. Help the volunteer lie down immediately on the examination table or floor.
2. Elevate legs if possible (place a chair cushion or backpack under their calves).
3. Monitor until all symptoms resolve.
4. Do not allow the volunteer to stand again until they feel completely normal.
5. Offer water.

> **This device is for educational demonstration only.** It is NOT a diagnostic test. It is NOT FDA evaluated. It should NOT be used to make clinical decisions about any person's health. Clinical diagnosis of POTS requires comprehensive autonomic testing by qualified medical professionals.

---

## Step 6: Post-Demonstration

### 6.1 Data Recording

- If you want to save the data, you can copy the Serial Monitor output to a text file.
- Alternatively, use a serial logging program (PuTTY, CoolTerm, or Python pyserial script) to capture data to a CSV file automatically.
- Record the timestamp of each phase transition (rest start, baseline start, standing moment, recovery start) separately so you can align events to the data.

### 6.2 Cleanup

- Remove the headband gently from the volunteer.
- Wipe the probe's tissue-contact surface with an alcohol swab before the next volunteer.
- Disconnect the battery pack when not in use.

---

## Final Verification Summary

All of these should be confirmed before you consider the project complete:

| V&V Test | Description | Status |
|:---------|:-----------|:-------|
| Dark reading | Probe covered: signal near zero (+/-5%) | [ ] Pass |
| Light response | Probe aimed at room light: clear signal increase | [ ] Pass |
| Finger test | Finger on probe: strong signal, pulsatile component visible | [ ] Pass |
| Baseline stability | Supine on forehead: within +/-3% for 90 seconds | [ ] Pass |
| Battery operation | Runs for 10+ minutes without battery shutoff | [ ] Pass |
| Standing response | Visible dip on Serial Plotter when subject stands | [ ] Pass |

---

## Troubleshooting Quick Reference

| Problem | Check First | Check Second | Check Third |
|:--------|:-----------|:-------------|:------------|
| No signal at all | Power indicator LED on? | ADS1115 found? (Serial Monitor) | Op-amp powered? (3.3V on pin 8) |
| Signal but very noisy | Star ground wired? | Decoupling caps installed? | Photodiode wire shielded and short? |
| Signal saturated (maxed out) | Light leak in probe? | Room too bright? | Gain stage resistors correct? |
| Battery keeps shutting off | Indicator LED drawing current? | Try different battery pack | Reduce indicator resistor to 100 Ohm |
| One channel dead | That LED polarity correct? | That transistor wired correctly? | Swap D2/D3 in code to test |
| Baseline never stabilizes | Probe moving? | Ambient light changing? | Let circuit warm up 5 min first |

---

## You're Done

You have built a working cerebral oximeter from discrete components. This device can visualize in real time how standing up affects blood flow to the brain - the same physiological phenomenon that causes POTS patients to feel lightheaded, fatigued, and cognitively impaired every time they get out of bed.

The total signal chain you built:

```
Photon from LED
  --> through skull and brain tissue
  --> absorbed differently by HbO2 vs Hb
  --> returned photon hits BPW34 (nano-amps)
  --> TIA converts to voltage (MCP6022 stage 1, 100k)
  --> Gain amplifies 48x (MCP6022 stage 2)
  --> ADC digitizes at 16-bit resolution (ADS1115)
  --> TDM separates wavelengths, ambient subtraction removes room light
  --> EMA filter smooths noise
  --> Baseline-referenced percent change quantifies the oxygenation trend
  --> Serial Plotter visualizes it in real time
```

Every stage in that chain exists for a reason, and you understand each one.
