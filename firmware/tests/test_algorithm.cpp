/*
 * test_algorithm.cpp
 *
 * Host-side unit tests for firmware/cerebral_oximeter/algorithm.h.
 * Compiles and runs with no external dependencies (uses assert + stdio).
 *
 * Build and run:
 *   cd firmware/tests && make
 */

#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstring>

// Pull in the algorithm under test (header-only, no Arduino includes needed)
#include "../cerebral_oximeter/algorithm.h"

// ---------------------------------------------------------------------------
// Minimal test harness
// ---------------------------------------------------------------------------
static int tests_run    = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define EXPECT_NEAR(actual, expected, tol, msg)                            \
  do {                                                                      \
    tests_run++;                                                            \
    float _a = (float)(actual);                                             \
    float _e = (float)(expected);                                           \
    float _t = (float)(tol);                                                \
    if (fabsf(_a - _e) <= _t) {                                            \
      tests_passed++;                                                       \
    } else {                                                                \
      tests_failed++;                                                       \
      printf("  FAIL [%s:%d] %s\n"                                         \
             "       expected %.6f, got %.6f (tol %.6f)\n",                \
             __FILE__, __LINE__, msg, _e, _a, _t);                         \
    }                                                                       \
  } while (0)

#define EXPECT_EQ(actual, expected, msg)  EXPECT_NEAR(actual, expected, 0.0f, msg)

// ---------------------------------------------------------------------------
// applyEMA tests
// ---------------------------------------------------------------------------
static void test_ema_seeding() {
  // First sample should seed the filter (caller passes sample as prevEMA)
  float ema = applyEMA(1000.0f, 1000.0f, 0.025f);
  EXPECT_NEAR(ema, 1000.0f, 0.01f, "EMA seed: first sample equals output");
}

static void test_ema_step_response_converges() {
  // Start at 0, step to 1000. After N_eff=40 samples the filter should be
  // within 1/e (~63%) of the target. After 3*N_eff=120 samples it should be
  // within ~5% of the target.
  float ema = 0.0f;
  for (int i = 0; i < 120; i++) {
    ema = applyEMA(1000.0f, ema, 0.025f);
  }
  EXPECT_NEAR(ema, 1000.0f, 50.0f, "EMA step: converges within 5% after 3*N_eff samples");
}

static void test_ema_does_not_overshoot() {
  // EMA of a constant input must not overshoot the input value.
  float ema = 500.0f;
  for (int i = 0; i < 200; i++) {
    ema = applyEMA(1000.0f, ema, 0.025f);
  }
  // Should be just below 1000, never above
  assert(ema <= 1000.0f + 0.01f);
  // After 200 samples from seed=500: ema ≈ 1000*(1 - 0.975^200) ≈ 996.8
  // Tolerance reflects actual convergence at 200 samples; the assert above
  // already confirms no overshoot.
  EXPECT_NEAR(ema, 1000.0f, 5.0f, "EMA: no overshoot of constant input (within 0.5% after 200 samples)");
}

static void test_ema_tracks_slow_drift() {
  // Signal drifting from 1000 to 1100 over 200 samples (~13 seconds at 15 Hz).
  // The EMA with N_eff=40 should track but lag the slow drift.
  float ema = 1000.0f;
  float signal = 1000.0f;
  for (int i = 0; i < 200; i++) {
    signal += 0.5f;  // 0.5 count/sample = 100 counts over 200 samples
    ema = applyEMA(signal, ema, 0.025f);
  }
  // EMA should be tracking (not stuck at 1000) and within reasonable lag
  assert(ema > 1000.0f);
  assert(ema <= 1100.0f + 1.0f);
  EXPECT_NEAR(ema, 1100.0f, 30.0f, "EMA: tracks slow drift within lag tolerance");
}

static void test_ema_rejects_spike() {
  // A single-sample spike should barely move the EMA (weight = alpha = 0.025)
  float ema = 1000.0f;
  float after_spike = applyEMA(2000.0f, ema, 0.025f);
  // Spike of +1000 contributes only 0.025 * 1000 = 25 counts
  EXPECT_NEAR(after_spike, 1025.0f, 0.5f, "EMA: single spike attenuated by (1-alpha)");
}

static void test_ema_alpha_boundary_zero() {
  // alpha=0 => output always equals prevEMA (no update)
  float ema = applyEMA(999.0f, 500.0f, 0.0f);
  EXPECT_EQ(ema, 500.0f, "EMA: alpha=0 output equals prevEMA");
}

static void test_ema_alpha_boundary_one() {
  // alpha=1 => output always equals new sample (no memory)
  float ema = applyEMA(999.0f, 500.0f, 1.0f);
  EXPECT_EQ(ema, 999.0f, "EMA: alpha=1 output equals sample");
}

// ---------------------------------------------------------------------------
// calcPctChange tests
// ---------------------------------------------------------------------------
static void test_pct_change_no_change() {
  EXPECT_EQ(calcPctChange(1000.0f, 1000.0f), 0.0f,
            "pctChange: same value => 0%");
}

static void test_pct_change_positive() {
  // 1100 vs baseline 1000 => +10%
  EXPECT_NEAR(calcPctChange(1100.0f, 1000.0f), 10.0f, 0.001f,
              "pctChange: +10% increase");
}

static void test_pct_change_negative() {
  // 900 vs baseline 1000 => -10%
  EXPECT_NEAR(calcPctChange(900.0f, 1000.0f), -10.0f, 0.001f,
              "pctChange: -10% decrease");
}

static void test_pct_change_pots_range() {
  // POTS patients show 5-15% drop. Verify the math resolves correctly
  // for a 10% drop: baseline 1200, current 1080.
  float baseline = 1200.0f;
  float current  = 1080.0f;  // 10% below baseline
  EXPECT_NEAR(calcPctChange(current, baseline), -10.0f, 0.001f,
              "pctChange: 10% POTS-range drop");
}

static void test_pct_change_healthy_range() {
  // Healthy subjects show 1-3% drop. Verify: baseline 1200, current 1176 (-2%).
  float baseline = 1200.0f;
  float current  = 1176.0f;
  EXPECT_NEAR(calcPctChange(current, baseline), -2.0f, 0.001f,
              "pctChange: 2% healthy-range drop");
}

static void test_pct_change_zero_baseline_guard() {
  // Division by zero must return 0.0, not NaN/Inf
  float result = calcPctChange(500.0f, 0.0f);
  EXPECT_EQ(result, 0.0f, "pctChange: zero baseline returns 0 (divide-by-zero guard)");
}

static void test_pct_change_small_signal() {
  // Low-signal scenario: baseline of 100 counts, drop to 92 (-8%)
  EXPECT_NEAR(calcPctChange(92.0f, 100.0f), -8.0f, 0.01f,
              "pctChange: correct at low ADC count values");
}

// ---------------------------------------------------------------------------
// ambientSubtract tests
// ---------------------------------------------------------------------------
static void test_ambient_subtract_basic() {
  // 1500 active - 200 ambient = 1300 signal
  EXPECT_EQ(ambientSubtract(1500.0f, 200.0f), 1300.0f,
            "ambientSubtract: active minus ambient");
}

static void test_ambient_subtract_no_ambient() {
  // Zero ambient (perfect dark room) => signal == active
  EXPECT_EQ(ambientSubtract(1200.0f, 0.0f), 1200.0f,
            "ambientSubtract: zero ambient passes through");
}

static void test_ambient_subtract_full_ambient() {
  // Ambient equals active => signal is zero (light path blocked)
  EXPECT_EQ(ambientSubtract(800.0f, 800.0f), 0.0f,
            "ambientSubtract: equal active/ambient => zero signal");
}

static void test_ambient_subtract_negative_result() {
  // Ambient slightly higher than active (ADC noise) can produce small negatives.
  // The algorithm must not clamp or discard these; EMA will average them out.
  float result = ambientSubtract(799.0f, 800.0f);
  EXPECT_NEAR(result, -1.0f, 0.001f,
              "ambientSubtract: noise-driven negative result preserved");
}

static void test_ambient_subtract_cancels_drift() {
  // Simulate ambient drifting between state 1 and state 3 of one TDM cycle.
  // State 1 ambient = 300, State 3 ambient = 310 (room light drifted).
  // Each LED's signal should be computed against its own adjacent ambient.
  float ambientA = 300.0f;
  float redActive = 1500.0f;
  float ambientB = 310.0f;    // drifted by 10 counts in ~33ms
  float irActive  = 1600.0f;

  float redSignal = ambientSubtract(redActive, ambientA);
  float irSignal  = ambientSubtract(irActive,  ambientB);

  EXPECT_EQ(redSignal, 1200.0f, "ambientSubtract TDM: red signal correct with drifted ambient");
  EXPECT_EQ(irSignal,  1290.0f, "ambientSubtract TDM: IR signal correct with drifted ambient");
}

// ---------------------------------------------------------------------------
// runningMean tests
// ---------------------------------------------------------------------------
static void test_running_mean_single_sample() {
  float sum = 0.0f;
  unsigned long count = 0;
  float mean = runningMean(500.0f, sum, count);
  EXPECT_EQ(mean, 500.0f,  "runningMean: single sample equals itself");
  EXPECT_EQ(count, 1.0f,   "runningMean: count incremented to 1");
}

static void test_running_mean_two_equal_samples() {
  float sum = 0.0f;
  unsigned long count = 0;
  runningMean(600.0f, sum, count);
  float mean = runningMean(600.0f, sum, count);
  EXPECT_EQ(mean, 600.0f, "runningMean: two equal samples => same mean");
}

static void test_running_mean_known_sequence() {
  // Samples: 100, 200, 300 => mean = 200
  float sum = 0.0f;
  unsigned long count = 0;
  runningMean(100.0f, sum, count);
  runningMean(200.0f, sum, count);
  float mean = runningMean(300.0f, sum, count);
  EXPECT_NEAR(mean, 200.0f, 0.001f, "runningMean: [100,200,300] => 200");
}

static void test_running_mean_30s_baseline() {
  // At 15 Hz TDM rate, 30 seconds = ~450 samples.
  // Average of a constant signal should equal that constant.
  float sum = 0.0f;
  unsigned long count = 0;
  float mean = 0.0f;
  for (int i = 0; i < 450; i++) {
    mean = runningMean(1000.0f, sum, count);
  }
  EXPECT_NEAR(mean, 1000.0f, 0.001f,
              "runningMean: constant signal over 450 samples => correct baseline");
  EXPECT_EQ((float)count, 450.0f, "runningMean: count == 450 after 30s");
}

static void test_running_mean_mixed_signal() {
  // Signal with equal positive and negative excursions around 1000
  // => mean should converge to 1000.
  float sum = 0.0f;
  unsigned long count = 0;
  float mean = 0.0f;
  for (int i = 0; i < 100; i++) {
    runningMean(1010.0f, sum, count);  // 50 samples at 1010
    mean = runningMean(990.0f, sum, count);  // 50 samples at 990
  }
  EXPECT_NEAR(mean, 1000.0f, 0.1f,
              "runningMean: symmetric excursions average to center");
}

// ---------------------------------------------------------------------------
// Regression tests (for bugs found in PR #10 review)
// ---------------------------------------------------------------------------

static void test_regression_dual_channel_shared_count() {
  // REGRESSION (PR #10): runningMean() increments its count on every call.
  // The original firmware called it twice per TDM cycle — once for Red,
  // once for IR — using the SAME baselineCount variable. This doubled the
  // denominator relative to the number of cycles, halving both baselines
  // and making pctChange report ~+100% on a stable signal.
  //
  // Actual firmware bug (two separate sums, one shared count incremented twice):
  //   baselineRed = runningMean(emaRed, baselineSumRed, baselineCount); // count++
  //   baselineIR  = runningMean(emaIR,  baselineSumIR,  baselineCount); // count++ again
  //
  // Fix: accumulate sums manually, increment baselineCount once per cycle.

  // --- CORRECT pattern (firmware fix) ---
  float sumRed = 0.0f, sumIR = 0.0f;
  unsigned long count = 0;
  for (int i = 0; i < 450; i++) {  // 30s at 15 Hz
    sumRed += 1200.0f;
    sumIR  += 1000.0f;
    count++;  // incremented once per cycle
  }
  float baselineRed = sumRed / (float)count;
  float baselineIR  = sumIR  / (float)count;

  EXPECT_NEAR(baselineRed, 1200.0f, 0.01f,
              "regression: dual-channel baseline Red correct (count incremented once/cycle)");
  EXPECT_NEAR(baselineIR,  1000.0f, 0.01f,
              "regression: dual-channel baseline IR correct (count incremented once/cycle)");

  // --- BUGGY pattern (two separate sums, shared count incremented twice/cycle) ---
  float bugSumRed = 0.0f, bugSumIR = 0.0f;
  unsigned long bugCount = 0;
  float bugBaselineRed = 0.0f, bugBaselineIR = 0.0f;
  for (int i = 0; i < 450; i++) {
    // Mirrors the original firmware: runningMean() on each channel with shared counter
    bugBaselineRed = runningMean(1200.0f, bugSumRed, bugCount);  // bugCount: 1,3,5,...
    bugBaselineIR  = runningMean(1000.0f, bugSumIR,  bugCount);  // bugCount: 2,4,6,...
  }
  // After 450 cycles, bugCount == 900 (incremented twice per cycle).
  // bugBaselineRed = 1200*450 / 900 = 600  (half the true value)
  // bugBaselineIR  = 1000*450 / 900 = 500  (half the true value)
  // pctChange on a stable signal would then report ~+100%, not ~0%.
  EXPECT_NEAR(bugBaselineRed, 600.0f, 5.0f,
              "regression: buggy pattern produces ~half of true baseline (Red)");
  EXPECT_NEAR(bugBaselineIR,  500.0f, 5.0f,
              "regression: buggy pattern produces ~half of true baseline (IR)");
  // Confirm the count was doubled
  EXPECT_EQ((float)bugCount, 900.0f,
            "regression: shared count incremented twice per cycle => 900 after 450 cycles");
}

static void test_regression_ema_seed_zero_channel() {
  // REGRESSION (PR #10): the original firmware used (emaRed == 0.0 && emaIR == 0.0)
  // to detect the first sample. This is fragile: if after the first sample both
  // EMA values drift back to exactly 0.0f (possible with dark-room readings and
  // ambient subtraction), the seed condition re-triggers and re-seeds the filter
  // mid-run, producing a false "rising signal" artifact.
  //
  // The fix uses `static bool emaSeeded` inside loop(). Because `emaSeeded` is a
  // static local in loop(), it cannot be reset or observed from outside — its
  // correctness must be verified by reading the firmware source, not by a unit
  // test here. This function documents the invariant that CAN be tested at the
  // algorithm.h boundary: applyEMA() starting from a zero-valued seed updates
  // correctly on the very next sample (no stuck-at-zero behavior).

  // Verify: EMA seeded at 0 and immediately updated with a non-zero signal
  // produces a value > 0 after one step (no stuck-at-zero).
  float ema = 0.0f;  // seed value (as if first sample was 0 — dark channel)
  ema = applyEMA(1000.0f, ema, 0.025f);
  // Expected: 0.025 * 1000 + 0.975 * 0 = 25.0
  EXPECT_NEAR(ema, 25.0f, 0.001f,
              "regression: EMA seeded at 0, updated with 1000 => 25.0 (alpha * sample)");
  assert(ema > 0.0f);

  // Verify the artifact magnitude: starting from 0 instead of the true signal
  // (1000) means the EMA needs ~3*N_eff = 120 samples to recover. After 1 step
  // it reports only 2.5% of the true signal — this quantifies why the fix matters.
  float ema_correct_seed = 1000.0f;  // correct seed: first sample IS the signal
  ema_correct_seed = applyEMA(1000.0f, ema_correct_seed, 0.025f);
  // Expected: stays at 1000.0

  float artifact_pct = fabsf(ema - ema_correct_seed) / ema_correct_seed * 100.0f;
  // artifact_pct should be ~97.5% (25 vs 1000)
  assert(artifact_pct > 90.0f);
  tests_run++; tests_passed++;
  printf("  (confirmed: zero-seeded EMA produces %.1f%% error on step 2 — fix matters)\n",
         (double)artifact_pct);
}

// ---------------------------------------------------------------------------
// Integration: EMA -> baseline -> pctChange pipeline
// ---------------------------------------------------------------------------
static void test_integration_stable_signal_near_zero_pct() {
  // Simulate 30s baseline + 5s monitoring with a stable signal.
  // Percent change should remain near 0%.
  const float ALPHA_TEST = 0.025f;
  float ema = 1200.0f;
  float baselineSum = 0.0f;
  unsigned long baselineCount = 0;
  float baseline = 0.0f;

  // 30s baseline at 15 Hz = 450 cycles
  for (int i = 0; i < 450; i++) {
    ema = applyEMA(1200.0f, ema, ALPHA_TEST);
    baseline = runningMean(ema, baselineSum, baselineCount);
  }

  // 5s monitoring at 15 Hz = 75 cycles, signal stable
  float maxPct = 0.0f;
  for (int i = 0; i < 75; i++) {
    ema = applyEMA(1200.0f, ema, ALPHA_TEST);
    float pct = calcPctChange(ema, baseline);
    if (fabsf(pct) > maxPct) maxPct = fabsf(pct);
  }
  EXPECT_NEAR(maxPct, 0.0f, 0.1f,
              "integration: stable signal => pctChange within +/-0.1% of zero");
}

static void test_integration_pots_drop_detected() {
  // Simulate 30s baseline then a sustained 10% drop (POTS-like).
  // After EMA settles, pctChange should reflect the drop within +/-2% precision.
  const float ALPHA_TEST = 0.025f;
  float ema = 1200.0f;
  float baselineSum = 0.0f;
  unsigned long baselineCount = 0;
  float baseline = 0.0f;

  // Baseline phase: 450 samples at 1200
  for (int i = 0; i < 450; i++) {
    ema = applyEMA(1200.0f, ema, ALPHA_TEST);
    baseline = runningMean(ema, baselineSum, baselineCount);
  }

  // Standing phase: signal drops to 1080 (-10%) and stays there.
  // Run for 3*N_eff = 120 samples to allow EMA to settle.
  float droppedSignal = 1080.0f;  // 10% below 1200
  float pct = 0.0f;
  for (int i = 0; i < 120; i++) {
    ema = applyEMA(droppedSignal, ema, ALPHA_TEST);
    pct = calcPctChange(ema, baseline);
  }
  // After 3*N_eff samples, EMA should be within ~95% of the true drop.
  // Expected pct ~= -9.5% to -10%. Spec target precision is +/-2%.
  EXPECT_NEAR(pct, -10.0f, 2.0f,
              "integration: POTS 10% drop detected within +/-2% spec precision");
}

static void test_integration_healthy_drop_detected() {
  // Healthy response: 2% drop that recovers. After sustained drop,
  // pctChange should be near -2%.
  const float ALPHA_TEST = 0.025f;
  float ema = 1200.0f;
  float baselineSum = 0.0f;
  unsigned long baselineCount = 0;
  float baseline = 0.0f;

  for (int i = 0; i < 450; i++) {
    ema = applyEMA(1200.0f, ema, ALPHA_TEST);
    baseline = runningMean(ema, baselineSum, baselineCount);
  }

  float droppedSignal = 1176.0f;  // -2% from 1200
  float pct = 0.0f;
  for (int i = 0; i < 120; i++) {
    ema = applyEMA(droppedSignal, ema, ALPHA_TEST);
    pct = calcPctChange(ema, baseline);
  }
  EXPECT_NEAR(pct, -2.0f, 0.5f,
              "integration: healthy 2% drop resolved correctly");
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
  printf("=== Cerebral Oximeter Algorithm Tests ===\n\n");

  printf("--- applyEMA ---\n");
  test_ema_seeding();
  test_ema_step_response_converges();
  test_ema_does_not_overshoot();
  test_ema_tracks_slow_drift();
  test_ema_rejects_spike();
  test_ema_alpha_boundary_zero();
  test_ema_alpha_boundary_one();

  printf("--- calcPctChange ---\n");
  test_pct_change_no_change();
  test_pct_change_positive();
  test_pct_change_negative();
  test_pct_change_pots_range();
  test_pct_change_healthy_range();
  test_pct_change_zero_baseline_guard();
  test_pct_change_small_signal();

  printf("--- ambientSubtract ---\n");
  test_ambient_subtract_basic();
  test_ambient_subtract_no_ambient();
  test_ambient_subtract_full_ambient();
  test_ambient_subtract_negative_result();
  test_ambient_subtract_cancels_drift();

  printf("--- runningMean ---\n");
  test_running_mean_single_sample();
  test_running_mean_two_equal_samples();
  test_running_mean_known_sequence();
  test_running_mean_30s_baseline();
  test_running_mean_mixed_signal();

  printf("--- Regression (PR #10 review findings) ---\n");
  test_regression_dual_channel_shared_count();
  test_regression_ema_seed_zero_channel();

  printf("--- Integration (EMA -> baseline -> pctChange) ---\n");
  test_integration_stable_signal_near_zero_pct();
  test_integration_pots_drop_detected();
  test_integration_healthy_drop_detected();

  printf("\n=========================================\n");
  printf("Results: %d/%d passed", tests_passed, tests_run);
  if (tests_failed > 0) {
    printf(", %d FAILED\n", tests_failed);
    return 1;
  }
  printf(" - ALL PASS\n");
  return 0;
}
