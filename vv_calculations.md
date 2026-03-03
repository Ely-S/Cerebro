# V&V Recalculation Sheet (Rev 3.2 Design Baseline)

This file documents first-principles calculations for the current design baseline:

- 850nm resistor = `68 ohm`
- 730nm resistor = `68 ohm`
- TIP31C base resistor = `470 ohm`
- 4-state TDM (`25%` duty per LED channel)
- Safety criterion: `< 10mW` time-averaged emitted power per channel

## 1) Assumptions

- `V_supply = 5.0V` (USB-derived LED rail via Arduino 5V branch)
- `V_gpio = 3.3V` (Arduino Nano 33 IoT)
- `V_BE = 0.7V` (TIP31C base-emitter, nominal)
- `V_CE(sat) = 0.2V` (TIP31C low-side switch, nominal)
- `V_F_730 = 2.1V` (730nm source typical)
- `V_F_850 = 1.60V` (SFH4253B typ value at 100mA from datasheet)

## 2) TIP31C Base Current

Formula:

`I_B = (V_gpio - V_BE) / R_B`

Substitution:

`I_B = (3.3 - 0.7) / 470 = 0.00553A = 5.53mA`

Result:

- `I_B = 5.53mA`

## 3) LED Collector/Forward Currents

Formula:

`I_F = (V_supply - V_CE(sat) - V_F) / R_LED`

### 730nm channel

`I_730 = (5.0 - 0.2 - 2.1) / 68 = 0.0397A = 39.7mA`

### 850nm channel

`I_850 = (5.0 - 0.2 - 1.60) / 68 = 0.0471A = 47.1mA`

Results:

- `I_730 = 39.7mA`
- `I_850 = 47.1mA`

## 4) Forced Beta Check (TIP31C Saturation Margin)

Formula:

`beta_forced = I_C / I_B`

### 730nm

`beta_forced_730 = 39.7 / 5.53 = 7.2`

### 850nm

`beta_forced_850 = 47.1 / 5.53 = 8.5`

Results:

- `beta_forced_730 = 7.2`
- `beta_forced_850 = 8.5`

Interpretation:

- Both channels are in a practical forced-beta range for saturated switching.

## 5) Resistor Power Dissipation

Formula:

`P_R = I^2 * R`

### Instantaneous (LED ON)

`P_R_730 = (0.0397)^2 * 68 = 0.107W`

`P_R_850 = (0.0471)^2 * 68 = 0.151W`

### Time-averaged under 4-state TDM (25% duty)

`P_R_730_avg = 0.107 * 0.25 = 0.0268W`

`P_R_850_avg = 0.151 * 0.25 = 0.0378W`

Results:

- `P_R_730 = 0.107W` (ON), `0.0268W` (avg)
- `P_R_850 = 0.151W` (ON), `0.0378W` (avg)
- Both are well below `0.5W` resistor rating.

## 6) Optical Power Interpolation and Safety Check

## 6.1 850nm (SFH4253B)

Datasheet anchor points:

- `Phi_e(70mA) = 55mW`
- `Phi_e(100mA) = 80mW`

Linear interpolation to `I = 47.1mA`:

`slope = (80 - 55) / (100 - 70) = 0.833 mW/mA`

`Phi_e_850_peak = 55 + 0.833 * (47.1 - 70) = 35.9mW` (approx)

Time-averaged at 25% duty:

`Phi_e_850_avg = 35.9 * 0.25 = 9.0mW` (approx)

## 6.2 730nm

Reference point from source family:

- `310mW @ 350mA`

Linear scaling to `39.7mA`:

`Phi_e_730_peak = 310 * (39.7 / 350) = 35.2mW` (approx)

Time-averaged at 25% duty:

`Phi_e_730_avg = 35.2 * 0.25 = 8.8mW` (approx)

Results:

- `Phi_e_730_avg ~ 8.8mW`
- `Phi_e_850_avg ~ 9.0mW`

Interpretation:

- Paper estimate passes the `<10mW` criterion for both channels.
- Margin is limited; bench optical measurement (`V&V Test 3.10`) is mandatory.

## 7) AFE Gain and ADC Clipping Check

Stage gains:

- Stage 1 TIA: `R_f = 100k ohm`
- Stage 2 non-inverting gain: `1 + (47k / 1k) = 48`
- Total transimpedance: `100k * 48 = 4.8 Mohm`

ADS1115 PGA:

- `+/-2.048V`

Photodiode current at ADC full-scale:

`I_PD_clip = 2.048 / 4.8e6 = 4.27e-7A = 427nA`

Result:

- ADC clipping threshold corresponds to about `427nA` photodiode current.

## 8) Checklist Summary

- LED current targets met (`730: 35-50mA`, `850: 40-50mA`): **YES**
- TIP31C forced beta in acceptable switched-saturation range: **YES**
- LED resistor dissipation below `0.5W`: **YES**
- Time-averaged optical power estimate `<10mW` per channel: **YES (paper estimate)**
- Formal closure still needs bench evidence: **YES**
