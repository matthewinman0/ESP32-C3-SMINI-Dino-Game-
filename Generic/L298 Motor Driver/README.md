# ESP32 + L298N Motor Driver Control

This guide explains how to control DC motors using an ESP32 and the L298N H-bridge motor driver. It covers the basics of how the driver works and how to use the ESP32’s LEDC peripheral to achieve smooth PWM-based speed control.

---

## What is the L298N?

The L298N is a dual H-bridge motor driver IC that allows you to control two DC motors independently. With it, you can control both the **speed** and **direction** of each motor.

It accepts standard logic-level inputs (3.3V or 5V), making it directly compatible with the ESP32 without additional level shifting.

---

## Pin Overview

The L298N uses two main types of control pins:

### Enable Pins (Speed Control)

- **ENA (Motor A)**
- **ENB (Motor B)**

These pins control motor speed using PWM. A higher duty cycle results in a faster motor.

On the ESP32, PWM signals are generated using the **LEDC peripheral**, which provides precise and efficient hardware-based control.

---

### Direction Pins

- **IN1 / IN2** — Control the direction of **Motor A**
- **IN3 / IN4** — Control the direction of **Motor B**

By setting these pins HIGH or LOW in different combinations, you can make the motor spin forward, reverse, coast, or brake.

---
---
## Direction Control Truth Table

### Motor A (IN1 / IN2)

| IN1  | IN2  | Motor Behaviour        |
|------|------|------------------------|
| LOW  | LOW  | Off (coasting)         |
| HIGH | LOW  | Forward                |
| LOW  | HIGH | Reverse                |
| HIGH | HIGH | Brake (motor locked)   |

### Motor B (IN3 / IN4)

> The same logic applies as Motor A.

### Notes

> **Note:** ENA/ENB must have a non-zero PWM value for the motor to actually move, even if the direction pins are set correctly.
## Wiring

| L298N Pin | ESP32 Pin                         | Notes                                 |
|----------|----------------------------------|---------------------------------------|
| ENA      | GPIO 18                          | PWM speed control, Motor A            |
| IN1      | GPIO 19                          | Direction, Motor A                    |
| IN2      | GPIO 21                          | Direction, Motor A                    |
| ENB      | GPIO 14                          | PWM speed control, Motor B            |
| IN3      | GPIO 26                          | Direction, Motor B                    |
| IN4      | GPIO 27                          | Direction, Motor B                    |
| GND      | GND                              | Common ground                         |
| +12V     | Motor supply                     | 7–35V DC motor power                  |
| +5V (out)| 3.3V via regulator / 5V pin      | See note below                        |
⚡ Powering the ESP32 from the L298N
The L298N has an onboard 5V regulator that outputs 5V on its +5V pin (when the jumper is in place). This can be used to power the ESP32 directly via its VIN pin, removing the need for a separate USB or power supply for the microcontroller. Just make sure the motor supply voltage is within the regulator's supported range (typically up to ~35V, and at least ~7V for the regulator to output a stable 5V).
---
## Code Explained:

Pin definitions at the top assign GPIO numbers to each L298N control pin for both motors.
PWM config sets up a 2000 Hz PWM signal with 8-bit resolution (values 0–255) — this is fed into ENA and ENB to control motor speed.
In `setup()`:
All direction pins (IN1–IN4) are set as outputs and initialised LOW (motors off)
`ledcAttachChannel()` binds ENA and ENB to their respective LEDC channels
`ledcWrite()` sets initial speed to 0
In `loop()`:
Both motors are set to full speed (`255`) via `ledcWrite()`
Direction pins are set HIGH/LOW to drive both motors forward
Then direction pins are flipped to reverse both motors
```cpp
// Full speed forward
ledcWrite(ENA, 255);
ledcWrite(ENB, 255);
digitalWrite(in1, HIGH); digitalWrite(in2, LOW);   // Motor A forward
digitalWrite(in3, HIGH); digitalWrite(in4, LOW);   // Motor B forward

// Reverse
digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);  // Motor A reverse
digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);  // Motor B reverse
```
To set an intermediate speed, replace `255` with any value between `0` and `255`.
---
Full Code
```cpp
//MOTOR A
int ENA = 18;
int in1 = 19;
int in2 = 21;

//MOTOR B
int ENB = 14;
int in3 = 26;
int in4 = 27;

//PWM config
const int freq = 2000;
const int pwmChannelA = 0;
const int pwmChannelB = 1;
const int resolution = 8;

void setup() {

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  ledcAttachChannel(ENA, freq, resolution, pwmChannelA);
  ledcAttachChannel(ENB, freq, resolution, pwmChannelB);

  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
}

void loop() {

  ledcWrite(ENA, 255); // Full speed
  ledcWrite(ENB, 255);

  // Forward
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);

  // Reverse
  digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);
}
