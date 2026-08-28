# Edge AI Autonomous Robot

> An autonomous mobile robot that detects and tracks a moving target using real-time, on-device AI inference on an ESP32-S3.

![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Edge_AI-blue)
![Edge Impulse](https://img.shields.io/badge/Edge_Impulse-FOMO-purple)
![Computer Vision](https://img.shields.io/badge/Computer_Vision-Object_Detection-green)
![Embedded](https://img.shields.io/badge/Embedded-C%2FC%2B%2B-orange)

<p align="center">
  <img src="docs/images/robot-overview.jpg" width="700" alt="Edge AI Autonomous Robot">
</p>

An embedded Edge AI system designed to detect and autonomously follow a moving target without relying on a cloud server or external computer.

The system runs a **FOMO (Faster Objects, More Objects)** object detection model directly on an **ESP32-S3 AI Camera**. The detected object's position is converted into autonomous movement decisions and transmitted via **UART** to an **ESP32-WROOM**, which controls the robot's motors through an **L298N motor driver**.


## Real-World Test

<p align="center">
  <img src="docs/media/robot-tracking-demo.gif" width="700" alt="Autonomous target tracking demo">
</p>

The robot detects the target entirely on-device and autonomously adjusts its movement according to the target's position in the camera frame.

## Overview

This project is a fully embedded autonomous mobile robot that combines computer vision, Edge AI, and real-time motor control.

A custom object detection model was trained using Edge Impulse and deployed directly to an ESP32-S3 AI Camera. The model detects a target in the camera frame and determines its relative position.

Based on the detection result, the ESP32-S3 generates movement commands such as **LEFT**, **RIGHT**, **FORWARD**, and **STOP**. These commands are transmitted via UART to a separate ESP32-WROOM responsible for motor control.

The entire perception-to-action pipeline runs locally on the robot without requiring a cloud service or external computer.

## System Architecture

The system is divided into two embedded controllers with separate responsibilities:

- **ESP32-S3 AI Camera — Perception & Decision**
  - Captures camera frames
  - Runs the FOMO object detection model on-device
  - Determines the target's position in the frame
  - Generates autonomous movement commands

- **ESP32-WROOM — Motion Control**
  - Receives movement commands from the ESP32-S3 via UART
  - Controls the L298N motor driver
  - Drives the DC motors according to the received command

### Data Flow


Camera<br>
↓<br>
ESP32-S3 AI Camera<br>
↓<br>
FOMO Object Detection<br>
↓<br>
Target Position<br>
↓<br>
Movement Decision<br>
↓<br>
UART<br>
↓<br>
ESP32-WROOM<br>
↓<br>
L298N Motor Driver<br>
↓<br>
DC Motors

## How It Works

1. The ESP32-S3 AI Camera continuously captures images from the environment.

2. Each frame is processed by the on-device FOMO object detection model.

3. When the target is detected, its position in the camera frame is analyzed.

4. The ESP32-S3 converts the target position into a movement decision:
   - **LEFT** — target is located on the left side of the frame
   - **RIGHT** — target is located on the right side of the frame
   - **FORWARD** — target is positioned near the center
   - **STOP** — no valid movement condition is detected

5. The movement command is transmitted to the ESP32-WROOM via UART.

6. The ESP32-WROOM translates the command into motor control signals and drives the motors through the L298N motor driver.

This creates a complete on-device **perception → decision → action** loop.

## Hardware

The robot is built around a dual-microcontroller architecture that separates AI-based perception from motor control.

| Component | Role |
|---|---|
| **ESP32-S3 AI Camera** | Image capture, FOMO inference, target localization, and movement decision |
| **ESP32-WROOM** | Receives movement commands and handles motor control |
| **L298N Motor Driver** | Drives the DC motors based on signals from the ESP32-WROOM |
| **DC Motors** | Provides robot movement |
| **2× 18650 Batteries** | Main power source |
| **Voltage Regulator** | Provides regulated power to the electronic components |

Separating perception and motion control between two microcontrollers keeps the system modular and allows the ESP32-S3 to focus on the Edge AI workload.

## Edge AI Model
 
The object detection model was developed and trained using **Edge Impulse** and deployed directly to the ESP32-S3 AI Camera for fully on-device inference.

### Dataset

A custom image dataset was collected specifically for the target object. During development, the dataset was expanded with additional samples and **negative examples** after early tests showed that the initial dataset was not sufficiently robust.

The final dataset contained approximately **442 labeled images**.

### Model

The project uses **FOMO (Faster Objects, More Objects)**, an object detection architecture designed for resource-constrained embedded devices.

The input images are resized to **96 × 96 pixels**, allowing the model to operate within the memory and computational limitations of the ESP32-S3 while still providing sufficient information for target localization.

### On-Device Inference

The trained model runs entirely on the ESP32-S3:

- No cloud inference
- No external computer
- No remote AI API
- Detection results are processed directly by the robot

This allows the complete vision pipeline to operate locally on the embedded hardware.

### Model Performance

The final model achieved:

| Metric | Result |
|---|---:|
| **F1 Score** | **91.4%** |
| **Precision** | **0.87** |
| **Recall** | **0.96** |
| **Inference Time** | **616 ms** |
| **Peak RAM Usage** | **119.2 KB** |
| **Flash Usage** | **68.9 KB** |

These results demonstrate that the object detection model can run within the resource constraints of the ESP32-S3 while providing reliable target detection for the autonomous control pipeline.

## Firmware Architecture
  - ESP32-S3 AI Camera
  - ESP32-WROOM

## Autonomous Decision Pipeline

## Results

## Challenges & Engineering Decisions

## Repository Structure

## Future Improvements

## Author
