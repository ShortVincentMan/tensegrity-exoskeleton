# IMU controller demonstration

Six `t1` through `t6` recordings document arm-on controller activity without external weight. Each original CSV has a serial `.raw.log` sidecar. The internal trial label repeats, so the source filename identifies the trial.

## Files

- `raw/`: the six analysis CSVs and their original serial logs.
- `derived/spreadsheets/`: manual processing exports, separate from original CSVs.
- `processed/`: imported summary, combined derived table, and interpretation notes.
- `protocol.txt`: original acquisition instructions and measurement definitions.
- `metadata/trials.json`: trial source identities and unresolved metadata.

The `imu_analysis/upload` copies were verified byte-identical to collection CSVs and omitted. Extra recordings remain available for review rather than being silently excluded from the archive.

## Reproduction

From the repository root:

```sh
python analysis/imu/analyze_imu.py --output analysis/reproduced/imu
```

The analysis retains all six trials without additional smoothing, phase alignment, or interpolation. Results figures are under `results/figures/imu/`.

## Interpretation

Logged angle is a relative IMU rotation proxy, not independently calibrated anatomical elbow angle. ASSIST shading records controller state. Emitted step counts do not measure motor rotation, cable force, or torque. Participant notes report no noticeable lifting assistance and deliberate lowering during spool release.

Trial 4 has an abrupt orientation change; trials 5 and 6 have brief angle excursions. Preserve these notes with the traces. A recording ending during ASSIST has unknown end time, not a completed interval. These recordings establish sensing and controller activation, not efficacy or reduced muscle effort.
