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
// ---------------------------------------------------------------------------
inline float runningMean(float newValue, float &sum, unsigned long &count) {
  sum += newValue;
  count++;
  return sum / (float)count;
}

#endif // ALGORITHM_H
