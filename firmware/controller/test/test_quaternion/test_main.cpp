#include <unity.h>

#include <math.h>

#include "quaternion.h"

void setUp() {}
void tearDown() {}

void test_zero_euler_is_identity() {
    const Quaternion q = eulerToQuaternion(0, 0, 0);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, q.w);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, q.x);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, q.y);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, q.z);
}

void test_normalize_handles_zero_quaternion() {
    const Quaternion q = quatNormalize({0, 0, 0, 0});
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, q.w);
}

void test_euler_output_is_unit_length() {
    const Quaternion q = eulerToQuaternion(123.4f, -45.6f, 78.9f);
    const float norm = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    TEST_ASSERT_FLOAT_WITHIN(1e-5f, 1.0f, norm);
}

void test_same_orientation_gives_zero_angle() {
    const Quaternion upper = eulerToQuaternion(30, 10, -5);
    const Quaternion rel = relativeQuaternion(upper, upper);
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 0.0f, rotationAngleDegrees({1, 0, 0, 0}, rel));
}

void test_forearm_pitch_maps_to_elbow_angle() {
    // Upper arm fixed, forearm pitched by 0, 45, 90, 120 degrees.
    const Quaternion upper = eulerToQuaternion(0, 0, 0);
    const Quaternion neutral = relativeQuaternion(upper, eulerToQuaternion(0, 0, 0));
    const float pitches[] = {45.0f, 90.0f, 120.0f};
    for (float p : pitches) {
        const Quaternion rel = relativeQuaternion(upper, eulerToQuaternion(0, p, 0));
        TEST_ASSERT_FLOAT_WITHIN(0.05f, p, rotationAngleDegrees(neutral, rel));
    }
}

void test_angle_ignores_shared_body_rotation() {
    // Turning the whole body (both IMUs) shouldn't change the elbow angle.
    const Quaternion neutral = relativeQuaternion(eulerToQuaternion(0, 0, 0),
                                                  eulerToQuaternion(0, 0, 0));
    const Quaternion turn = eulerToQuaternion(70, 0, 0);
    const Quaternion forearm = quatMultiply(turn, eulerToQuaternion(0, 60, 0));
    const Quaternion rel = relativeQuaternion(turn, forearm);
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 60.0f, rotationAngleDegrees(neutral, rel));
}

void test_angle_is_measured_from_neutral() {
    // If the arm was zeroed slightly bent, angles are relative to that pose.
    const Quaternion upper = eulerToQuaternion(0, 0, 0);
    const Quaternion neutral = relativeQuaternion(upper, eulerToQuaternion(0, 10, 0));
    const Quaternion rel = relativeQuaternion(upper, eulerToQuaternion(0, 70, 0));
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 60.0f, rotationAngleDegrees(neutral, rel));
}

void test_sign_flipped_quaternion_gives_same_angle() {
    const Quaternion q = eulerToQuaternion(0, 50, 0);
    const Quaternion flipped = {-q.w, -q.x, -q.y, -q.z};
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, rotationAngleDegrees({1, 0, 0, 0}, q),
                             rotationAngleDegrees({1, 0, 0, 0}, flipped));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_zero_euler_is_identity);
    RUN_TEST(test_normalize_handles_zero_quaternion);
    RUN_TEST(test_euler_output_is_unit_length);
    RUN_TEST(test_same_orientation_gives_zero_angle);
    RUN_TEST(test_forearm_pitch_maps_to_elbow_angle);
    RUN_TEST(test_angle_ignores_shared_body_rotation);
    RUN_TEST(test_angle_is_measured_from_neutral);
    RUN_TEST(test_sign_flipped_quaternion_gives_same_angle);
    return UNITY_END();
}
