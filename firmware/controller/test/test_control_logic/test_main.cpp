#include <unity.h>

#include "config.h"
#include "control_logic.h"

namespace {
const StallParams kParams = {MIN_ELEVATED_ANGLE_DEG, MIN_ELEVATED_TIME_MS, DIP_TRIGGER_DEG,
                             DIP_CONFIRM_MS, INTENTIONAL_LOWERING_SPEED_DEG_S};
const SpoolMap kMap = {10.0f, 700, 18};

// Holds the arm at `angle` from `startMs` for `durationMs`, 10 ms per tick.
// Returns the time of the first tick that reported a stall, or 0 if none did.
unsigned long hold(StallDetector &d, float angle, float velocity,
                   unsigned long startMs, unsigned long durationMs) {
    for (unsigned long t = startMs; t < startMs + durationMs; t += 10)
        if (d.update(angle, velocity, t)) return t;
    return 0;
}
}

void setUp() {}
void tearDown() {}

void test_toggle_decoding() {
    TEST_ASSERT_TRUE(decodeToggle(true, false) == Toggle::Auto);
    TEST_ASSERT_TRUE(decodeToggle(false, true) == Toggle::Stop);
    TEST_ASSERT_TRUE(decodeToggle(false, false) == Toggle::Between);
    TEST_ASSERT_TRUE(decodeToggle(true, true) == Toggle::Invalid);
}

void test_taut_steps_scale_and_clamp() {
    TEST_ASSERT_EQUAL(0, tautStepsForAngle(kMap, -5.0f));
    TEST_ASSERT_EQUAL(300, tautStepsForAngle(kMap, 30.0f));
    TEST_ASSERT_EQUAL(700, tautStepsForAngle(kMap, 200.0f));
}

void test_follow_steps_leave_slack() {
    TEST_ASSERT_EQUAL(300 - 18, followStepsForAngle(kMap, 30.0f));
    TEST_ASSERT_EQUAL(-18, followStepsForAngle(kMap, 0.0f));
    TEST_ASSERT_EQUAL(700 - 18, followStepsForAngle(kMap, 200.0f));
}

void test_no_stall_below_elevated_angle() {
    StallDetector d(kParams);
    TEST_ASSERT_EQUAL(0, hold(d, MIN_ELEVATED_ANGLE_DEG - 1.0f, 0.0f, 1000, 5000));
}

void test_steady_hold_does_not_trigger() {
    StallDetector d(kParams);
    TEST_ASSERT_EQUAL(0, hold(d, 60.0f, 0.0f, 1000, 10000));
}

void test_dip_after_hold_triggers_after_confirm_time() {
    StallDetector d(kParams);
    hold(d, 60.0f, 0.0f, 1000, MIN_ELEVATED_TIME_MS + 100);
    const unsigned long dipStart = 1000 + MIN_ELEVATED_TIME_MS + 100;
    const unsigned long fired = hold(d, 60.0f - DIP_TRIGGER_DEG - 1.0f, -2.0f, dipStart, 2000);
    TEST_ASSERT_EQUAL(dipStart + DIP_CONFIRM_MS, fired);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 60.0f, d.referenceAngle());
}

void test_small_dip_does_not_trigger() {
    StallDetector d(kParams);
    hold(d, 60.0f, 0.0f, 1000, MIN_ELEVATED_TIME_MS + 100);
    TEST_ASSERT_EQUAL(0, hold(d, 60.0f - DIP_TRIGGER_DEG * 0.5f, -1.0f,
                              1000 + MIN_ELEVATED_TIME_MS + 100, 3000));
}

void test_dip_too_soon_after_lifting_does_not_trigger() {
    StallDetector d(kParams);
    hold(d, 60.0f, 0.0f, 1000, 500);
    TEST_ASSERT_EQUAL(0, hold(d, 50.0f, -2.0f, 1500, DIP_CONFIRM_MS + 50));
}

void test_intentional_lowering_does_not_trigger() {
    StallDetector d(kParams);
    hold(d, 60.0f, 0.0f, 1000, MIN_ELEVATED_TIME_MS + 100);
    const float fast = INTENTIONAL_LOWERING_SPEED_DEG_S - 5.0f;
    TEST_ASSERT_EQUAL(0, hold(d, 50.0f, fast, 1000 + MIN_ELEVATED_TIME_MS + 100, 2000));
}

void test_brief_dip_resets_confirm_timer() {
    StallDetector d(kParams);
    unsigned long t = 1000;
    hold(d, 60.0f, 0.0f, t, MIN_ELEVATED_TIME_MS + 100);
    t += MIN_ELEVATED_TIME_MS + 100;
    // Dip for less than the confirm time, then recover.
    TEST_ASSERT_EQUAL(0, hold(d, 55.0f, -2.0f, t, DIP_CONFIRM_MS - 100));
    t += DIP_CONFIRM_MS - 100;
    TEST_ASSERT_EQUAL(0, hold(d, 60.0f, 2.0f, t, 200));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_toggle_decoding);
    RUN_TEST(test_taut_steps_scale_and_clamp);
    RUN_TEST(test_follow_steps_leave_slack);
    RUN_TEST(test_no_stall_below_elevated_angle);
    RUN_TEST(test_steady_hold_does_not_trigger);
    RUN_TEST(test_dip_after_hold_triggers_after_confirm_time);
    RUN_TEST(test_small_dip_does_not_trigger);
    RUN_TEST(test_dip_too_soon_after_lifting_does_not_trigger);
    RUN_TEST(test_intentional_lowering_does_not_trigger);
    RUN_TEST(test_brief_dip_resets_confirm_timer);
    return UNITY_END();
}
