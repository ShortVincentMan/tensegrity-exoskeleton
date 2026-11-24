#pragma once

// Quaternion helpers for turning two IMU orientations into an elbow angle.
// Kept free of Arduino headers so they can be unit tested on a PC.

struct Quaternion {
    float w;
    float x;
    float y;
    float z;
};

Quaternion quatNormalize(Quaternion value);
Quaternion quatConjugate(Quaternion value);
Quaternion quatMultiply(Quaternion first, Quaternion second);

// BNO085 RVC reports yaw, pitch and roll in degrees.
Quaternion eulerToQuaternion(float yawDeg, float pitchDeg, float rollDeg);

// Rotation of the forearm relative to the upper arm.
Quaternion relativeQuaternion(Quaternion upper, Quaternion forearm);

// Angle (degrees) between the current relative orientation and the neutral
// (straight-arm) relative orientation captured during zeroing.
float rotationAngleDegrees(Quaternion neutral, Quaternion current);
