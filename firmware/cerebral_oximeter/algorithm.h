/*
 * algorithm.h - Pure algorithm functions for the cerebral oximeter firmware.
 *
 * Extracted from cerebral_oximeter.ino so the math can be compiled and tested
 * on the host (without Arduino hardware) via firmware/tests/.
 *
 * All functions are stateless and take/return plain floats, making them easy
 * to unit-test and reason about in isolation.
 */

#ifndef ALGORITHM_H
#define ALGORITHM_H

// ---------------------------------------------------------------------------
// applyEMA()
//
// One step of the Exponential Moving Average filter.
//
//   new_ema = alpha * sample + (1 - alpha) * prev_ema
//
// alpha = 0.025, N_eff = 1/alpha = 40.
// First call should seed with the raw sample (pass sample as prev_ema) to
// avoid a false ramp-up artifact.
// ---------------------------------------------------------------------------
inline float applyEMA(float sample, float prevEMA, float alpha) {
  return alpha * sample + (1.0f - alpha) * prevEMA;
}

// ---------------------------------------------------------------------------
// calcPctChange()
//
// Percent change of current relative to baseline.
//
//   pct = ((current - baseline) / baseline) * 100
//
// Returns 0.0 if baseline == 0 (guard against divide-by-zero).
// If baseline is negative (possible when ambient subtraction produces a
// negative 30s mean), the returned sign is inverted relative to physiological
// expectation. The firmware guards against this with a `baseline > 0` check
// before calling this function.
// ---------------------------------------------------------------------------
inline float calcPctChange(float current, float baseline) {
  if (baseline == 0.0f) return 0.0f;
  return ((current - baseline) / baseline) * 100.0f;
}

// ---------------------------------------------------------------------------
// ambientSubtract()
//
// Isolates the LED-specific signal by removing ambient light contribution.
//
//   signal = active - ambient
//
// Both values are raw ADC counts (int16_t cast to float).
// ---------------------------------------------------------------------------
inline float ambientSubtract(float active, float ambient) {
  return active - ambient;
}

// ---------------------------------------------------------------------------
// runningMean()
//
// Incremental mean: given a previous running sum and count, returns the
// updated mean after adding one new value. Updates sum and count in place.
//
// WARNING — single-caller-per-cycle only:
// This function increments `count` on every call. If you call it more than
// once per accumulation cycle with a shared `count`, the denominator will be
// multiplied by the number of calls, producing a baseline that is 1/N of the
// true mean (where N = calls per cycle). For dual-channel accumulation (Red +
// IR) in the firmware, use separate manual sums and ONE shared count++:
//
//   baselineSumRed += emaRed;
//   baselineSumIR  += emaIR;
//   baselineCount++;
//   baselineRed = baselineSumRed / (float)baselineCount;
//   baselineIR  = baselineSumIR  / (float)baselineCount;
// ---------------------------------------------------------------------------
inline float runningMean(float newValue, float &sum, unsigned long &count) {
  sum += newValue;
  count++;
  return sum / (float)count;
}

#endif // ALGORITHM_H
