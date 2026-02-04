# Paul: The Open-Source Quad-Interval Rotator

## Overview
This firmware drives the "Paul" Auto-Sampler, a 4-axis laboratory rotation system. Designed for the Arduino Uno R3, it allows for **independent, asynchronous control** of four separate stepper motors. Unlike standard rotators that spin all samples continuously, this system allows each motor to have its own timeline without blocking the others.

## Hardware Requirements
* **Microcontroller:** Arduino Uno R3 (or compatible)
* **Shield:** CNC shield Rev3 (for Arduino)
* **Drivers:** Standard Stepper Drivers (e.g., A4988, DRV8825)
* **Motors:** 4x NEMA 17 Stepper Motors
* **Library:** `AccelStepper` by Mike McCauley

## Wiring Configuration
This firmware uses specific pin assignments given in the CNC shield. Ensure your shield wires the the drivers to the arduino as follows:

| Function | Arduino Pin |
| :--- | :--- |
| **Global Enable** | Pin 8 |
| **Motor X (Paul 1)** | Step: 2 / Dir: 5 |
| **Motor Y (Paul 2)** | Step: 3 / Dir: 6 |
| **Motor Z (Paul 3)** | Step: 4 / Dir: 7 |
| **Motor A (Paul 4)** | Step: 12 / Dir: 13 |

For motor A, I had to connect jumper wires from pins 12/13 to pins A.stp and A.dir in my shield. I guess this is the default if you want to have a 4th independent motor.

## Installation
1.  Install the **Arduino IDE**.
2.  Go to **Sketch > Include Library > Manage Libraries**.
3.  Search for and install **AccelStepper**.
4.  Open `autosampler_paul.ino`.
5.  Upload to your Arduino.

---

## Configuration Guide

### 1. Setting the Sampling Interval
The time between rotations is controlled by the `interval` variable. It is calculated in milliseconds.

Find this line in the code:
```cpp
const unsigned long interval = 6L * 60L * 60L * 1000L;
```

* **6 Hours:** `6L * 60L * 60L * 1000L`
* **1 Hour:** `1L * 60L * 60L * 1000L`
* **30 Minutes:** `30L * 60L * 1000L`
* **10 Seconds (Test Mode):** `10000L`

### 2. Turning Off Specific Motors
You can disable specific motors (e.g., if you only have samples in slots 1 and 3) by commenting them out in the `motors` array. The code will automatically adjust to run only the active motors.

**Example: Disabling Motor 2 and 4**
```cpp
Motor motors[] = {
  {stepper1, "Paul 1", MOVING, 0, 0},
  // {stepper2, "Paul 2", MOVING, 0, 0},  <-- Add // to disable
  {stepper3, "Paul 3", MOVING, 0, 0},
  // {stepper4, "Paul 4", MOVING, 0, 0}   <-- Add // to disable
};
```

### 3. Adjusting Rotation Angle
To change how far the motor turns each time it wakes up, edit the `check_motor_status` function.

Find this line:
```cpp
motor.currentPosition += 20;
```
* **20:** Moves ~36 degrees (assuming 1.8° motor, full step).
* **200:** Moves 360 degrees (1 full revolution).
* **400:** Moves 2 revolutions.

### 4. Adjust the Rotation speed/acceleration
TO change how fast the motor turns, edit these lines:
```cpp
    ...
    motors[i].stepper.setMaxSpeed(4); // <-- Adjust max speed
    motors[i].stepper.setAcceleration(3); // <-- Adjust acceleration
    ...
```

## Troubleshooting
* **Motors just vibrate?** Check your motor wiring pairs (coils) and ensure the correct current limit on your driver.
* **Motors don't move at all?** Ensure the external power supply (12V) is connected and turned on. The USB cable alone cannot power the motors.