# Wiring

Pin numbers match [`firmware/controller/include/pins.h`](../firmware/controller/include/pins.h). If you change one, change the other.

## IMUs (BNO085, UART-RVC)

Each IMU gets its own ESP32 UART. I started on I2C, but the clone breakout boards kept dropping off the bus, and UART-RVC has been reliable ever since.

| IMU | BNO085 TX → ESP32 RX | ESP32 TX → BNO085 RX | Power |
| --- | --- | --- | --- |
| Upper arm | GPIO33 | GPIO32 | 3.3 V, GND |
| Forearm | GPIO26 | GPIO25 | 3.3 V, GND |

Both run at 115200 baud, 8N1. To put a BNO085 breakout into RVC mode, bridge the **PS0** jumper and leave **PS1** open, then power-cycle the sensor. It only reads the mode pins at startup.

## Stepper drivers (A4988)

| Signal | Motor 1 | Motor 2 |
| --- | --- | --- |
| STEP | GPIO19 | GPIO17 |
| DIR | GPIO18 | GPIO16 |
| ENABLE (shared, active low) | GPIO4 | GPIO4 |

Set each A4988's current limit with its potentiometer before connecting a motor. Keep motor power and logic power on separate traces (the PCB already does this).

## Controls

| Input | Pin | Notes |
| --- | --- | --- |
| Rocker: STOP position | GPIO22 | Active low. Disables the motors. |
| Rocker: AUTO position | GPIO21 | Active low. Runs the controller. |
| Manual wind button | GPIO23 | Active low. Winds the cable while held. |

The middle rocker position (neither pin low) is standby. Flipping from STOP to AUTO zeroes the elbow angle, so hold the arm straight and still with the cable just taut when you do it.
