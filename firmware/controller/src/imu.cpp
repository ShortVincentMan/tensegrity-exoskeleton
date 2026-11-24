#include "imu.h"

#include <HardwareSerial.h>
#include <math.h>

#include "config.h"
#include "pins.h"
#include "rvc_packet.h"

namespace {
HardwareSerial upperImu(1);
HardwareSerial forearmImu(2);
ImuData upperData = {0, 0, 0, false, 0, 0, 0};
ImuData forearmData = {0, 0, 0, false, 0, 0, 0};
RvcParser upperParser;
RvcParser forearmParser;
Quaternion neutralRelative = {1, 0, 0, 0};

void updateParser(HardwareSerial &port, RvcParser &parser, ImuData &data) {
    while (port.available()) {
        RvcAngles angles;
        const RvcResult result = parser.push(port.read(), angles);
        if (result == RvcResult::Packet) {
            data.yaw = angles.yaw;
            data.pitch = angles.pitch;
            data.roll = angles.roll;
            data.valid = true;
            data.updatedMs = millis();
            ++data.packetCount;
        } else if (result == RvcResult::BadChecksum) {
            ++data.badPacketCount;
        }
    }
}

Quaternion currentRelativeQuaternion() {
    return relativeQuaternion(
        eulerToQuaternion(upperData.yaw, upperData.pitch, upperData.roll),
        eulerToQuaternion(forearmData.yaw, forearmData.pitch, forearmData.roll));
}

bool toggleIsAuto() {
    return digitalRead(AUTO_SWITCH_PIN) == LOW && digitalRead(STOP_SWITCH_PIN) != LOW;
}

bool toggleIsStop() {
    return digitalRead(STOP_SWITCH_PIN) == LOW && digitalRead(AUTO_SWITCH_PIN) != LOW;
}
}

void imuBegin() {
    upperImu.begin(IMU_BAUD, SERIAL_8N1, IMU1_RX_PIN, IMU1_TX_PIN);
    forearmImu.begin(IMU_BAUD, SERIAL_8N1, IMU2_RX_PIN, IMU2_TX_PIN);
}

void imuUpdate() {
    updateParser(upperImu, upperParser, upperData);
    updateParser(forearmImu, forearmParser, forearmData);
}

bool imuSensorsFresh() {
    const unsigned long now = millis();
    return upperData.valid && forearmData.valid &&
           now - upperData.updatedMs <= IMU_TIMEOUT_MS &&
           now - forearmData.updatedMs <= IMU_TIMEOUT_MS;
}

const ImuData &imuUpperData() { return upperData; }
const ImuData &imuForearmData() { return forearmData; }

float imuElbowAngleDegrees() {
    return rotationAngleDegrees(neutralRelative, currentRelativeQuaternion());
}

bool imuAverageNeutralQuaternion(Quaternion &result, bool requireAuto) {
    constexpr int REQUIRED_SAMPLES = 100;
    int sampleCount = 0;
    float sumW = 0, sumX = 0, sumY = 0, sumZ = 0;
    const unsigned long startedMs = millis();
    while (sampleCount < REQUIRED_SAMPLES && millis() - startedMs < 5000) {
        imuUpdate();
        if ((requireAuto && !toggleIsAuto()) || (!requireAuto && !toggleIsStop())) return false;
        if (!imuSensorsFresh()) continue;
        Quaternion sample = currentRelativeQuaternion();
        if (sampleCount > 0 && sample.w * sumW + sample.x * sumX +
            sample.y * sumY + sample.z * sumZ < 0) {
            sample = {-sample.w, -sample.x, -sample.y, -sample.z};
        }
        sumW += sample.w;
        sumX += sample.x;
        sumY += sample.y;
        sumZ += sample.z;
        ++sampleCount;
        delay(10);
    }
    if (sampleCount != REQUIRED_SAMPLES) return false;
    result = quatNormalize({sumW / sampleCount, sumX / sampleCount,
                        sumY / sampleCount, sumZ / sampleCount});
    neutralRelative = result;
    return true;
}
