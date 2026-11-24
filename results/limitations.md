# Limitations

This is a pilot prototype. It shows the sensing, control, and actuation working together. It doesn't show that the exoskeleton reduces effort.

**Actuation.** The steppers can't deliver enough cable tension under load. With a 5 lb dumbbell the system stalled, and participants didn't notice real lifting help during the trials. During spool release the participant had to lower the arm on purpose to keep the cable taut.

**Angle measurement.** Accurate to about 45°, then it reads 17 to 20° low near 90 to 120°, and it doesn't fully return to zero. The controller compensates with a fixed scale factor (`MEASURED_ANGLE_AT_PHYSICAL_100_DEG` in `config.h`). A real fix needs better IMU mounting or a per-user calibration.

**What the logs measure.** Motor output is logged as step pulses, not torque or cable force. Since the user moves voluntarily during assist, the logs can't separate how much lifting came from the motors.

**Data quirks.** Trial 4 has a sudden orientation jump, and trials 5 and 6 have brief spikes. They're left in the data as recorded. One trial ends partway through an assist.

**EMG.** Timing problems and uneven movement intervals (see [methods](../docs/methods.md)) mean the EMG data can't tell us whether muscle effort went down.

**Next time:** stronger motors, a load cell on the cable, better angle calibration, and IMU and EMG recorded together with a fixed movement protocol.
