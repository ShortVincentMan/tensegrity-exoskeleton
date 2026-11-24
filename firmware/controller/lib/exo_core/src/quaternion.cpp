#include "quaternion.h"

#include <math.h>

namespace {
// Same value as Arduino's PI macro, kept as a double so results match the
// original firmware bit for bit.
constexpr double kPi = 3.1415926535897932384626433832795;
}

Quaternion quatNormalize(Quaternion value) {
    const float norm = sqrtf(value.w * value.w + value.x * value.x +
                             value.y * value.y + value.z * value.z);
    if (norm < 1.0e-6f) return {1, 0, 0, 0};
    return {value.w / norm, value.x / norm, value.y / norm, value.z / norm};
}

Quaternion quatConjugate(Quaternion value) {
    return {value.w, -value.x, -value.y, -value.z};
}

Quaternion quatMultiply(Quaternion first, Quaternion second) {
    return {
        first.w * second.w - first.x * second.x - first.y * second.y - first.z * second.z,
        first.w * second.x + first.x * second.w + first.y * second.z - first.z * second.y,
        first.w * second.y - first.x * second.z + first.y * second.w + first.z * second.x,
        first.w * second.z + first.x * second.y - first.y * second.x + first.z * second.w};
}

Quaternion eulerToQuaternion(float yawDeg, float pitchDeg, float rollDeg) {
    const float cy = cosf(yawDeg * kPi / 360.0f), sy = sinf(yawDeg * kPi / 360.0f);
    const float cp = cosf(pitchDeg * kPi / 360.0f), sp = sinf(pitchDeg * kPi / 360.0f);
    const float cr = cosf(rollDeg * kPi / 360.0f), sr = sinf(rollDeg * kPi / 360.0f);
    return quatNormalize({cr * cp * cy + sr * sp * sy,
                          sr * cp * cy - cr * sp * sy,
                          cr * sp * cy + sr * cp * sy,
                          cr * cp * sy - sr * sp * cy});
}

Quaternion relativeQuaternion(Quaternion upper, Quaternion forearm) {
    return quatNormalize(quatMultiply(quatConjugate(upper), forearm));
}

float rotationAngleDegrees(Quaternion neutral, Quaternion current) {
    const Quaternion change = quatNormalize(quatMultiply(quatConjugate(neutral), current));
    float boundedW = fabsf(change.w);
    if (boundedW < 0.0f) boundedW = 0.0f;
    if (boundedW > 1.0f) boundedW = 1.0f;
    return 2.0f * acosf(boundedW) * 180.0f / kPi;
}
