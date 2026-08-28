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


Camera
  ↓
ESP32-S3 AI Camera
  ↓
FOMO Object Detection
  ↓
Target Position
  ↓
Movement Decision
  ↓
UART
  ↓
ESP32-WROOM
  ↓
L298N Motor Driver
  ↓
DC Motors

## How It Works

## Hardware

## Edge AI Model
  - Dataset
  - Edge Impulse
  - FOMO
  - Training
  - Optimization
  - On-device inference

## Firmware Architecture
  - ESP32-S3 AI Camera
  - ESP32-WROOM

## Autonomous Decision Pipeline

## Results

## Challenges & Engineering Decisions

## Repository Structure

## Future Improvements

## Author
