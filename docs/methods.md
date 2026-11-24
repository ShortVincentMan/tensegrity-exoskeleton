# Methods

I ran two separate experiments. They weren't recorded at the same time, so their data can't be lined up against each other.

## IMU controller trials

Six trials (`t1` to `t6`) with the exoskeleton worn and no external weight. The `imu-trial-logger` firmware logs every control sample as CSV at 25 Hz: raw IMU angles, the filtered elbow angle and velocity, the controller state, and the commanded and emitted motor steps. The [protocol](../experiments/imu-controller-demo/protocol.txt) covers setup and the recorder commands.

`analysis/imu/analyze_imu.py` reads the six CSVs and produces per-trial summaries and the figures in `results/figures/imu/`. It doesn't smooth, align, or interpolate anything beyond what the firmware already did.

What these trials can and can't show:

- The angle is the relative rotation between the two IMUs, zeroed with the arm straight. It isn't a calibrated anatomical angle (see the validation table in the main README).
- "Assist" is a controller state. Motor steps are pulses the firmware sent, not measured rotation, torque, or cable force.

## EMG pilot

Twelve recordings with a Shimmer EMG sensor: baseline (no exoskeleton) and wearing the exoskeleton, each with no added weight and with 15 lb, three reps each. `analysis/emg/process_all_emg.m` removes DC offset and computes rolling RMS on channel 2. The raw recordings aren't published, but the processing code and figures are.

The exports had timing problems (around 51 Hz effective rate, repeated timestamps in some files) and uneven rest and movement periods, so I don't draw a conclusion about muscle effort from them.
