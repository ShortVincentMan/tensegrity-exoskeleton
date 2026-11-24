#include "control_logic.h"

#include <math.h>

Toggle decodeToggle(bool autoActive, bool stopActive) {
    if (autoActive && !stopActive) return Toggle::Auto;
    if (stopActive && !autoActive) return Toggle::Stop;
    if (!autoActive && !stopActive) return Toggle::Between;
    return Toggle::Invalid;
}

namespace {
long clampLong(long value, long low, long high) {
    return value < low ? low : (value > high ? high : value);
}
}

long tautStepsForAngle(const SpoolMap &map, float angleDeg) {
    return clampLong(lroundf(angleDeg * map.stepsPerMeasuredDegree), 0L, map.maxWindSteps);
}

long followStepsForAngle(const SpoolMap &map, float angleDeg) {
    return clampLong(tautStepsForAngle(map, angleDeg) - map.slackSteps,
                     -map.slackSteps, map.maxWindSteps);
}

void StallDetector::reset() {
    elevatedStartMs_ = 0;
    dipStartMs_ = 0;
    referenceAngle_ = 0;
}

bool StallDetector::update(float angleDeg, float velocityDegS, unsigned long nowMs) {
    if (angleDeg < params_.minElevatedAngleDeg) {
        reset();
        return false;
    }
    if (elevatedStartMs_ == 0) {
        elevatedStartMs_ = nowMs;
        referenceAngle_ = angleDeg;
        return false;
    }
    if (angleDeg > referenceAngle_) {
        referenceAngle_ = angleDeg;
        dipStartMs_ = 0;
    }
    if (velocityDegS <= params_.intentionalLoweringSpeedDegS) {
        elevatedStartMs_ = nowMs;
        referenceAngle_ = angleDeg;
        dipStartMs_ = 0;
        return false;
    }
    if (nowMs - elevatedStartMs_ < params_.minElevatedTimeMs ||
        referenceAngle_ - angleDeg < params_.dipTriggerDeg) {
        dipStartMs_ = 0;
        return false;
    }
    if (dipStartMs_ == 0) dipStartMs_ = nowMs;
    return nowMs - dipStartMs_ >= params_.dipConfirmMs;
}
