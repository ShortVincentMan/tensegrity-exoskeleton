# Build log

A condensed version of my SURP lab notebook (summer 2026). The original is handwritten and stays private; this is the engineering story.

## 1. Getting oriented: making the tensegrity arm wearable

I started from Israel Villegas' tensegrity arm from the previous project. The first problem was obvious once I tried it: you couldn't fit an arm inside it, and the forearm section compressed in a way Dr. Zhang said wasn't ideal. I built a simple tensegrity model to get a feel for the caps and cables (the caps frustrated me for most of the summer), then started rebuilding around my own arm.

Dr. Zhang's early feedback was that my first version was too cable-driven and not really tensegrity: the cables should form triangles, the rods should sit on different planes, and the structure should stand on its own instead of relying on the arm. I studied Tom Flemons' tensegrity arm models, recreated one, and split it into two halves so an arm could slide through. The forearm turned out great and durable, so I kept it.

A biology class helped frame the design: bones act as levers and muscles act as cables on those levers. In this structure the rods are the levers and the cables are the muscles.

## 2. Actuation: where do the motors go?

Instead of putting a motor at the elbow like the earlier design, which was clunky, I moved actuation to the shoulder. I traced which cable actually does elbow flexion, replaced it with a longer one, and routed it up to the shoulder through a pulley so everything heavy could live in one place.

- First motor test (SY20STH30-0604A stepper, A4988 driver, ESP32): one motor pulling both wrist cables in opposite directions was too much load, so I split it into two motors.
- I added a second, stronger stepper (11HS18-0674S) and wrote a manual controller with rocker switches so I could test without a laptop.
- I printed new spools and fairleads, then kept iterating on them because the string kept slipping, wrapping around the motor horn, or catching on the flange.

## 3. Swapping elastic for string and springs

The elastic cables made motion unpredictable, so I replaced every one with string plus a spring of the right stiffness: stabilizers, wrist and forearm cables, and the main lift cable. That meant about 40 cables, each one cut, knotted, pretensioned and tied, with no tangles. It was the most painful part of the project. Afterwards Dr. Zhang had me document every rod, node, spring constant and string length so someone else could rebuild it.

## 4. Electronics and wearability

- Moved from breadboard to perfboard, drew the schematic in KiCad, and soldered everything. On first power-up a battery overheated, which I traced with a multimeter to a wrong connection.
- Designed a shoulder harness, velcro mounting plates and a printed electronics box (it ended up looking a bit like an arc reactor). Wearing it for an hour was comfortable, just hot.
- Load test: holding a 5 lb dumbbell, the system stalled. The tensegrity lets the arm move freely and does transmit cable force into elbow flexion, but the actuators can't provide the tension needed. From then on, I treated the motors as proof of concept.

## 5. Sensing: two IMUs and a lot of debugging

I chose IMUs over EMG for control, because EMG takes about 30 minutes to calibrate. EMG was kept for evaluating effort at the end.

The BNO085 boards kept dropping off the I2C bus. After isolating every variable (new boards, a fresh setup, continuity and voltage checks), the hardware all checked out and I2C still failed on the clone boards. So I switched both IMUs to UART-RVC on independent ESP32 serial ports. RVC gives yaw, pitch, roll and acceleration at 100 Hz in a 19-byte checksummed packet. I moved the firmware from counting bytes to properly decoding packets, and both IMUs have streamed together reliably since.

The elbow angle comes from the relative orientation of the forearm IMU to the upper-arm IMU, measured against a neutral (straight-arm) reference quaternion.

## 6. Control: narrowing the problem

My first IMU-to-motor mapping made the motor act like a servo following pitch, but it was laggy and got stuck turning one way. Talking it through with Dr. Zhang narrowed the goal: I wasn't building a general-purpose powered arm. I was building elbow assistance for a known motion, a biceps curl, that kicks in when the muscle starts to give out.

That became a state machine:

1. **Follow:** the spools track the arm and keep the cable just slack.
2. **Detect a stall:** the arm is held up, then dips slightly without the user meaning to lower it.
3. **Assist:** wind the cable to restore the lost angle.
4. **Release:** let go when motion resumes, the arm is lowered, or a time limit is hit.

It runs at 100 Hz with filtering, a deadband, travel limits, sensor-loss shutdown and a manual stop.

## 7. Measuring the prototype

- **Spool:** 18 mm diameter, about 56.5 mm of cable per revolution. Going from 0 to 100° of elbow flexion takes 200 to 220 mm of cable, so I capped travel at about 3.5 revolutions.
- **Angle validation:** accurate from 0 to 45°, but underestimates by 17 to 20° at higher angles, plus a return-to-zero offset (results table in the main README).

## 8. Custom PCB (Rev A)

Then the ESP32's USB connector snapped off the perfboard. Everything was soldered down, so instead of a tricky micro-USB repair I redesigned the electronics to be modular:

- Socketed ESP32 and A4988 drivers on female headers, and JST-XH connectors for the IMUs and motors.
- Motor power kept separate from logic, with a Mini360 buck converter and an optional XT30 LiPo input.
- Trace widths sized by current: 0.5 mm for logic, 1 mm for motor power, 1.5 mm for the XT30 input.
- An ESP32 antenna keep-out, and a fix for polarized capacitors I'd almost placed backwards.
- Rerouted until ERC and DRC were clean, checked the Gerbers, and ordered through OSH Park.

I kept the current motors on purpose. Rev A is a control-system prototype, not the final power platform.

## 9. Bring-up and trials

Robert joined to help assemble and debug the new board. We fixed several wiring and pin-mapping problems, and spent about two hours on a "motor problem" that turned out to be a dead battery. After that, I ran the six IMU controller trials and the EMG pilot that are in this repo. One known issue is still open: in some states one motor turns the opposite way from the other.
