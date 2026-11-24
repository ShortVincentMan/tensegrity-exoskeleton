# Tensegrity Exoskeleton Control

Embedded firmware and control system for a feedback-controlled upper-limb tensegrity exoskeleton, integrating dual IMU motion tracking, cable-driven actuation, and adaptive arm assistance.

## Overview

This repository contains firmware developed for research on feedback control of an upper-limb tensegrity exoskeleton.

The system is designed to provide lightweight elbow-flexion assistance while allowing the user to move naturally. Two BNO085 IMUs track upper-arm and forearm motion, while an ESP32 processes sensor data and controls cable-driven actuators mounted on the wearable system.

The current control objective is to:

1. Track upper-arm and forearm motion.
2. Estimate relative elbow motion.
3. Follow voluntary elbow flexion and extension with the motors to maintain cable tension.
4. Detect when intended motion slows, stalls, or fails to progress.
5. Increase motor assistance when additional assistance is required.

The IMUs are used primarily for motion feedback rather than direct physiological measurement of muscle fatigue.

---

## System Architecture

```text
Upper-Arm BNO085 ── UART ──┐
                            │
                            ├──> ESP32
                            │       │
Forearm BNO085 ─── UART ───┘       │
                                    ↓
                            Arm Kinematics
                                    │
                                    ↓
                            Motion Estimation
                                    │
                                    ↓
                           Assist Controller
                                    │
                                    ↓
                             Motor Control
                                    │
                                    ↓
                          Cable-Driven Actuation
                                    │
                                    ↓
                                  User
                                    │
                                    └──> IMU Feedback
```

---

## Hardware

Current prototype hardware includes:

- ESP32 microcontroller
- 2× BNO085 IMUs
- 2× stepper motors
- Stepper motor drivers
- Cable/spool actuation system
- Custom perfboard electronics
- Custom wiring harness
- Upper-limb tensegrity structure
- Wearable shoulder harness
- Adjustable arm cuffs
- 3D-printed mechanical components

---

## IMU System

### BNO085

Two BNO085 IMUs are used to measure the motion of the upper arm and forearm.

The original implementation used I2C. During integration testing, the generic BNO085 modules exhibited unreliable I2C communication despite successful electrical testing.

UART-RVC was subsequently tested and provided reliable simultaneous communication from both sensors.

The current prototype therefore uses **UART-RVC**.

### UART-RVC Configuration

UART-RVC operates at:

```text
115200 baud
8 data bits
No parity
1 stop bit
```

The BNO085 protocol selection is configured as:

```text
PS0 = HIGH
PS1 = LOW
```

On the current BNO085 breakout boards:

```text
PS0 solder jumper = BRIDGED
PS1 solder jumper = OPEN
```

The sensors must be power-cycled after changing protocol-selection pins.

---

## IMU Wiring

### Upper-Arm IMU

```text
BNO085              ESP32
--------------------------
VIN      ---------> 3.3V
GND      ---------> GND
TX       ---------> GPIO33
RX       <--------- GPIO32
```

GPIO33 acts as the ESP32 UART RX pin.

GPIO32 acts as the ESP32 UART TX pin.

### Forearm IMU

```text
BNO085              ESP32
--------------------------
VIN      ---------> 3.3V
GND      ---------> GND
TX       ---------> GPIO26
RX       <--------- GPIO25
```

GPIO26 acts as the ESP32 UART RX pin.

GPIO25 acts as the ESP32 UART TX pin.

### Protocol Configuration

Both IMUs:

```text
PS0 = HIGH
PS1 = LOW
```

Power and ground may share the same rails, but each IMU uses an independent UART communication channel.

---

## Validated IMU Communication

Both BNO085s have been successfully operated simultaneously over independent ESP32 hardware UARTs.

Observed data rate during testing:

```text
IMU 1 ≈ 1900 bytes/sec
IMU 2 ≈ 1900 bytes/sec
```

This confirms simultaneous communication from both sensors without UART packet collisions.

UART-RVC provides:

- Yaw
- Pitch
- Roll
- X acceleration
- Y acceleration
- Z acceleration

---

## Motion Estimation

The upper-arm and forearm IMUs are used together to estimate elbow motion.

```text
Upper-arm orientation
        +
Forearm orientation
        ↓
Relative orientation
        ↓
Elbow angle
        ↓
Change in elbow angle
        ↓
Estimated angular velocity
```

For the current prototype, elbow motion is constrained primarily to flexion and extension.

Angular velocity can be estimated numerically:

```text
angular velocity ≈ change in angle / change in time
```

This allows UART-RVC orientation measurements to provide both position and motion information without requiring direct gyroscope reports.

---

## Control Strategy

The controller is intended to operate as a state-based assist system.

### IDLE

The arm is stationary.

The motor maintains minimal cable preload or remains stationary depending on the mechanical configuration.

```text
Arm stationary
      ↓
IDLE
```

### FOLLOW / TRACKING

The user voluntarily flexes or extends the elbow.

The motors follow the measured arm movement to maintain cable tension without significantly assisting the movement.

```text
User flexes elbow
       ↓
IMUs detect increasing elbow angle
       ↓
Motor reels cable inward
       ↓
Cable remains taut
```

The motor should follow slow movement as well as fast movement.

The objective is not to activate the motor only after assistance is required. The motor should continuously maintain the appropriate cable position.

### ASSIST

If the user is attempting flexion but movement significantly slows or stalls before reaching the desired position, the controller increases assistance.

Potential indicators include:

- Reduced angular velocity
- Motion stall
- Failure to reach expected range of motion
- Reversal of intended movement
- Increased movement instability

A simplified condition could resemble:

```text
Flexion previously detected
        +
Target angle not reached
        +
Angular velocity falls below threshold
        +
Condition persists
        ↓
ASSIST
```

### HOLD

When the desired position is reached, the motor maintains the appropriate cable position.

### EXTENSION

When the user lowers the arm, the motors release cable while continuing to follow the measured elbow position.

```text
Elbow extends
      ↓
Motor releases cable
      ↓
Cable remains controlled
```

---

## Cable-Following Control

The intended control architecture maps measured elbow position to required cable position.

```text
Elbow angle
    ↓
Cable-length relationship
    ↓
Desired spool position
    ↓
Motor position
```

During normal movement:

```text
Motor position ≈ required cable position
```

During assistance:

```text
Required cable position
        +
Assist offset
        ↓
Motor provides additional pull
```

This allows the motor to maintain cable tension during normal voluntary motion while increasing assistance only when necessary.

---

## Fatigue and Task-Failure Characterization

The IMUs do **not** directly measure physiological muscle fatigue.

Instead, they measure kinematic changes that may occur as the user becomes unable to maintain the desired movement.

Potential indicators include:

- Decreasing movement velocity
- Reduced range of motion
- Increasing repetition duration
- Motion stalls
- Failure to reach target position
- Increased movement variability
- Oscillation during position holding
- Inability to maintain an intended trajectory

The current control system therefore focuses on detecting **difficulty completing the intended motion** rather than directly diagnosing muscle fatigue.

Future EMG measurements may be used to evaluate muscle effort and compare assisted and unassisted movement.

---

## Firmware Architecture

The firmware is being separated into reusable modules.

```text
firmware/
│
├── main/
│   └── exoskeleton_control.ino
│
├── imu/
│   ├── BNO085RVC.h
│   └── BNO085RVC.cpp
│
├── motor/
│   ├── MotorControl.h
│   └── MotorControl.cpp
│
└── control/
    ├── AssistController.h
    └── AssistController.cpp
```

The intended software flow is:

```text
readIMUs()
     ↓
calculateElbowAngle()
     ↓
estimateMotion()
     ↓
updateControlState()
     ↓
updateMotors()
```

This keeps sensor communication, kinematics, control logic, and motor actuation separated.

---

## Repository Structure

```text
tensegrity-exoskeleton-control/
│
├── README.md
├── .gitignore
│
├── firmware/
│   ├── main/
│   ├── imu/
│   ├── motor/
│   └── control/
│
├── tests/
│   ├── imu_single/
│   ├── imu_dual/
│   ├── motor_test/
│   └── imu_motor_test/
│
├── hardware/
│   ├── wiring/
│   └── pinout.md
│
├── data/
│   └── README.md
│
└── docs/
    ├── architecture.md
    ├── control-strategy.md
    └── development-log.md
```

---

## Dependencies

### Arduino Board Package

Install through Arduino IDE Board Manager:

```text
ESP32 by Espressif Systems
```

### Arduino Libraries

Install through Arduino IDE Library Manager:

```text
Adafruit BNO08x
Adafruit BNO08x RVC
Adafruit BusIO
```

The current UART-RVC firmware primarily uses:

```cpp
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Adafruit_BNO08x_RVC.h>
```

`Arduino.h` and `HardwareSerial.h` are provided by the ESP32 Arduino core.

---

## Development Environment

Current firmware development uses:

- C/C++
- Arduino framework
- Arduino IDE
- Visual Studio Code
- ESP32
- Git
- GitHub

---

## Testing

Hardware and firmware functionality should be tested independently before full-system integration.

Current test programs include:

### Single IMU

Confirms:

- UART configuration
- BNO085 RVC mode
- Orientation decoding

### Dual IMU

Confirms:

- Both ESP32 UARTs operate simultaneously
- Both BNO085s continuously transmit data
- No communication conflicts occur

### Motor Test

Confirms:

- Motor direction
- Step control
- Limit behavior
- Manual operation

### IMU + Motor Test

Integrates motion sensing with motor response before deployment on the full wearable system.

---

## Development Status

### Mechanical

- [x] Tensegrity upper-limb structure
- [x] Wearable shoulder mounting
- [x] Cable-driven actuation
- [x] Motor mounting
- [x] Spool/fairlead development
- [x] Arm cuff development

### Electronics

- [x] ESP32 control system
- [x] Stepper motor drivers
- [x] Custom perfboard electronics
- [x] Manual motor controls
- [x] IMU wiring architecture
- [x] Dual UART architecture

### Firmware

- [x] Manual motor control
- [x] Single BNO085 UART-RVC communication
- [x] Dual BNO085 UART-RVC communication
- [x] Simultaneous dual-UART data acquisition
- [ ] Dual-IMU orientation decoding
- [ ] Relative elbow-angle estimation
- [ ] Cable-following control
- [ ] Assist-state controller
- [ ] Integrated IMU/motor firmware
- [ ] Full wearable closed-loop testing

### Experimental

- [ ] Establish baseline elbow motion
- [ ] Characterize cable-position relationship
- [ ] Tune motor-follow behavior
- [ ] Define assist thresholds
- [ ] Test voluntary flexion
- [ ] Test assisted flexion
- [ ] Compare assisted and unassisted motion
- [ ] Future EMG evaluation

---

## Research Context

This repository supports research into feedback control of an upper-limb tensegrity exoskeleton.

The broader project investigates whether tensegrity-based wearable robotic systems can provide useful arm assistance while maintaining a lightweight, compliant, and wearable structure.

The sensing system tracks user motion, the control system responds to that motion, and the actuation system provides cable-driven elbow-flexion assistance.

---

## Status

**Research prototype — active development.**

Hardware, firmware, control strategies, thresholds, and system architecture may change as experimental testing continues.