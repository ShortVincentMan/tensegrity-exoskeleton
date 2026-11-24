# Firmware

Each folder is its own PlatformIO project targeting an ESP32 (`esp32dev`), written in Arduino C++.

| Project | What it does | USB serial baud |
| --- | --- | --- |
| [`controller`](controller) | **Main firmware.** Reads both IMUs, computes the elbow angle, and runs the 100 Hz follow / assist / release state machine that drives the two cable spools. | 921600 |
| [`imu-trial-logger`](imu-trial-logger) | Controller variant that logs every control sample as CSV at 25 Hz for the recorded trials. | 115200 |
| [`emg-trial-logger`](emg-trial-logger) | Logger used during the EMG pilot sessions. | 921600 |
| [`calibration/hinge-axis`](calibration/hinge-axis) | Sketch for finding the elbow hinge axis between the two IMUs. | 115200 |
| [`examples/dual_imu_angle_reader_test`](examples/dual_imu_angle_reader_test) | Standalone test that both IMUs stream over UART and the angle looks right. | 115200 |

## Controller layout

| File | Role |
| --- | --- |
| `include/config.h` | Every tunable: spool geometry, control rates, filter constants, and assist thresholds |
| `include/pins.h` | Pin assignments for the drivers, IMU UARTs, and switches |
| `lib/exo_core/` | The hardware-independent logic, kept free of Arduino code so it can be unit tested: the BNO085 UART-RVC packet parser (`rvc_packet`), the quaternion elbow-angle math (`quaternion`), and switch decoding, angle-to-steps mapping and stall detection (`control_logic`) |
| `src/imu.cpp` | Reads both IMU UARTs through the parser and tracks sensor freshness and neutral zeroing |
| `src/motor.cpp` | Step generation and position tracking for the A4988 drivers |
| `src/controller.cpp` | State machine, safety checks, and serial logging |
| `test/` | Unity tests for `lib/exo_core`, run on the host with `pio test -e native` |

## Build

```sh
pio run -d firmware/controller
```

Pin assignments and IMU setup are in [docs/wiring.md](../docs/wiring.md).
