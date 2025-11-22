Smart Helmet – Crash Detection System

A simple, low‑cost Arduino‑based Smart Helmet prototype that detects whether the helmet is worn, senses a crash using the MPU6050 accelerometer/gyroscope, sounds a buzzer alarm, and sends an SOS message to a paired smartphone via Bluetooth (HC‑05).

This project is designed for students and beginners. All components are inexpensive and easy to assemble.

-Features

Wear Detection using a push button (helmet must be worn to activate ride mode).

Crash Detection using:

Sudden acceleration spike (impact)

Immobility after impact

Helmet tilt/orientation change

Buzzer Alarm after confirmed crash.

Bluetooth SOS sent to a paired smartphone (Android) using HC‑05.

Simple Hardware: Arduino Nano + MPU6050 + Buzzer + Button.

Breadboard‑friendly wiring.

-Hardware Used

Arduino Nano (ATmega328P)

MPU6050 (GY‑521) Accelerometer + Gyroscope

HC‑05 Bluetooth Module

Active Buzzer (2‑pin)

Push Button (Tactile Switch)

-Breadboard

Male‑to‑Male Jumper Wires

USB Data Cable (for programming Nano)

Images from your build:

Arduino Nano image: /mnt/data/ABFDCAD5-5DDA-4924-86EC-512172F71855.jpeg

-Circuit Connections
MPU6050 → Arduino Nano

VCC → 5V

GND → GND

SDA → A4

SCL → A5

HC‑05 → Arduino Nano

VCC → 5V

GND → GND

TX → D10

RX → D11

-Buzzer
(positive) → D8

(negative) → GND

-Wear Button

One leg → D2

Opposite diagonal leg → GND# smart-crash-detection
Arduino based crash detection prototype using accelerometer sensor

