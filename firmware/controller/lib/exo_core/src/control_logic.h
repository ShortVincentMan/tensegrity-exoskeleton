#pragma once

#include <stdint.h>

// Pure pieces of the controller: switch decoding, angle-to-steps mapping,
// and stall detection. No Arduino calls, so they run in unit tests.

enum class Toggle { Unknown, Auto, Stop, Between, Invalid };

// Inputs are true when the switch contact is pulled LOW (active).
Toggle decodeToggle(bool autoActive, bool stopActive);

struct SpoolMap {
    float stepsPerMeasuredDegree;
    long maxWindSteps;
    long slackSteps;
};

// Steps that make the cable just taut at this elbow angle.
long tautStepsForAngle(const SpoolMap &map, float angleDeg);
// Same as taut, minus a little slack so the cable doesn't drag during follow.
long followStepsForAngle(const SpoolMap &map, float angleDeg);

struct StallParams {
    float minElevatedAngleDeg;
    unsigned long minElevatedTimeMs;
    float dipTriggerDeg;
    unsigned long dipConfirmMs;
    float intentionalLoweringSpeedDegS;
};

// Watches the arm while it's held up. Returns true from update() when the
// arm has dipped below its recent peak without the user meaning to lower it.
class StallDetector {
public:
    explicit StallDetector(const StallParams &params) : params_(params) {}
    void reset();
    bool update(float angleDeg, float velocityDegS, unsigned long nowMs);
    float referenceAngle() const { return referenceAngle_; }
    // Used by the "testassist" serial command to force an assist.
    void setReferenceAngle(float angleDeg) { referenceAngle_ = angleDeg; }

private:
    StallParams params_;
    unsigned long elevatedStartMs_ = 0;
    unsigned long dipStartMs_ = 0;
    float referenceAngle_ = 0;
};
