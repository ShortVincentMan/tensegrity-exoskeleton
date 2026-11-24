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
| `src/imu.cpp` | BNO085 UART-RVC packet parser (19-byte frames with a checksum), Euler-to-quaternion conversion, and relative elbow angle |
| `src/motor.cpp` | Step generation and position tracking for the A4988 drivers |
| `src/controller.cpp` | State machine, stall detection, safety checks, and serial logging |

## Build

```sh
pio run -d firmware/controller
```

A successful build only means the code compiles. It hasn't been tested on hardware. Wiring details are in the [original wiring notes](../archive/documentation/README-before-research-organization.md).
