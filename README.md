# Manual Holonomic Control

Bluetooth gamepad teleoperation for a 4 omni-wheel holonomic robot: ESP32 reads the gamepad, STM32F407 drives the motors.

<!-- TODO: add demo GIF/photo -->

## Overview

A manual control setup for R2C's wheeled holonomic soccer robot base (the same drive layout used for KRSBI-B). It is useful for testing the drivetrain, tuning kinematics and moving the robot without the vision PC.

The work is split across two boards:

- **Soccer-Robot-Controller** (ESP32): connects to a Bluetooth gamepad through Bluepad32, shapes the stick input, and streams velocity commands as text over UART.
- **Soccer-Robot-Receiver** (STM32F407 Discovery): parses the commands, runs holonomic inverse kinematics and drives four motors through H-bridges.

## Features

- Full holonomic teleoperation: left stick for translation (X/Y), right stick X for rotation.
- Exponential input shaping (normalized, squared, sign preserved) with ±200 command range for fine low-speed control.
- Simple line-based protocol `M <x> <y> <z>\n`, parsed with `sscanf` and validated by field count.
- Hardware enable switch on PA0: when off, the motor driver is disabled and all wheel commands are zeroed.

## Architecture

```
Gamepad --Bluetooth--> ESP32 (Bluepad32)
                         | input shaping
                         | "M x y z\n" @ 115200 baud (UART0)
                         v
                  STM32F407 (USART2, PA2/PA3)
                         | holonomic(vx, vy, vz)
                         v
                  4x H-bridge PWM --> 4 omni-wheel motors
```

## Holonomic Kinematics

Four omni wheels sit at 45°, 135°, 225° and 315° around the robot center. Each wheel's speed is the projection of the body velocity onto the wheel's rolling direction plus a rotation term:

```
v_i = -sin(θ_i) * vx + cos(θ_i) * vy + R * ω
```

With `sin/cos(45°) ≈ 0.707` and a 0.5 scale factor, this becomes the matrix used in the receiver:

```c
motor_1 = (-0.35 * vx) + ( 0.35 * vy) + (0.25 * vz);
motor_2 = (-0.35 * vx) + (-0.35 * vy) + (0.25 * vz);
motor_3 = ( 0.35 * vx) + (-0.35 * vy) + (0.25 * vz);
motor_4 = ( 0.35 * vx) + ( 0.35 * vy) + (0.25 * vz);
```

The sign of each result selects which H-bridge input gets PWM, and the magnitude is the duty cycle. This version is open loop (no encoder feedback).

## Hardware and Software Stack

| Item | Details |
| --- | --- |
| Controller board | ESP32 Dev Module V4 (Bluepad32 Arduino board package) |
| Receiver board | STM32F407VGTx Discovery (STM32duino core) |
| Input | Bluetooth gamepad supported by Bluepad32 |
| Drive | 4 DC motors, omni wheels, H-bridge drivers with a shared enable pin (PE7) |
| Link | UART, 115200 baud, ASCII |
| Language | Arduino C++ |

## Repository Structure

```
├── Soccer-Robot-Controller/
│   ├── Soccer-Robot-Controller.ino  # gamepad processing, command output
│   ├── Global.h                     # Bluepad32 callbacks
│   └── Motion.h                     # input mapping functions
└── Soccer-Robot-Receiver/
    └── Soccer-Robot-Receiver.ino    # UART parser, holonomic kinematics, motor output
```

## Build and Upload

**Controller (ESP32)**

1. Install the Bluepad32 board package for Arduino ([guide](https://bluepad32.readthedocs.io/en/latest/plat_arduino/)).
2. Open `Soccer-Robot-Controller/Soccer-Robot-Controller.ino`, select ESP32 Dev Module from the Bluepad32 package and upload.
3. The ESP32 V4 has one usable UART, so disconnect the TX/RX wires to the STM32 while uploading.

**Receiver (STM32)**

1. Install the [STM32duino core](https://github.com/stm32duino/Arduino_Core_STM32).
2. Open `Soccer-Robot-Receiver/Soccer-Robot-Receiver.ino`, select the STM32F407 Discovery board and upload.
3. Wire ESP32 TX to the STM32 USART2 RX and share ground.

Pair the gamepad, switch PA0 on, and the robot follows the sticks.

## My Role

I (Moses Jaguar Ersaptiarso) wrote both firmwares at R2C: gamepad integration, the UART command protocol, holonomic kinematics and motor control.
