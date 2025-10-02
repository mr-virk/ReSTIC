# Renewable Sun Tracker and Smart Irrigation Controller (ReSTIC)

## Abstract

This project unites dynamic solar tracking with intelligent irrigation control for energy-efficient, sustainable agriculture and off-grid automation. The **ReSTIC system** uses dual LDR sensors and a servo to maintain solar panel orientation for maximum sunlight. Its agriculture module features ESP8266 WiFi, soil moisture and rain detection, temperature/humidity sensing (DHT), and relay-based pump switching. Operations—including weather-based irrigation, manual override, and real-time notifications—are displayed on an LCD and managed via Blynk IoT cloud for full visibility and remote control. Ideal for renewable-powered gardens, smart greenhouses, and off-grid water automation.

---

## Hardware Components

- **Sun Tracker**
  - Arduino Nano/UNO
  - Servo motor (panel actuation)
  - 2x LDR sensors (East/West)
- **Smart Irrigation**
  - ESP8266 microcontroller
  - Soil moisture sensor
  - Rain sensor
  - DHT temp/humidity sensor
  - Relay module (for water pump)
  - 16x2 I2C LCD display
  - Power and wiring acc., Blynk IoT app

---


## Features

- **Sun Tracking:** Panel always faces most intense sunlight—boosts power capture
- **Auto-Irrigation:** Real-time soil moisture & rain sensing for optimized watering
- **Manual override:** Pump can be controlled via app
- **Live feedback:** LCD and Blynk app notifications/status
- **Minimal adjustment, water/energy savings**

---

## UML Class Diagram

```mermaid
classDiagram
class SunTracker {
+setup()
+loop()
+servoControl()
+ldrRead()
}
class IrrigationController {
+setup()
+loop()
+dhtSensorUpdate()
+soilMoistureCheck()
+rainCheck()
+pumpFunction()
+homeScreen()
}
SunTracker --> Servo
SunTracker --> LDRSensors
IrrigationController --> SoilMoistureSensor
IrrigationController --> RainSensor
IrrigationController --> DHTSensor
IrrigationController --> Relay
IrrigationController --> LCD
IrrigationController --> BlynkCloud
```

---

## Flowchart

```mermaid
flowchart TD
StartTracker[Start Tracker] --> InitTracker[Init LDRs Servo]
InitTracker --> LoopTracker[Panel At Start]
LoopTracker --> TestLDRs{LDR Delta > Tolerance?}
TestLDRs -- Yes --> Track[Move Servo Toward Sun]
Track --> LoopTracker
TestLDRs -- No --> Hold[Hold Position]
Hold --> LoopTracker
StartIrr[Start Irrigation] --> InitIrr[Init Sensors LCD Pump]
InitIrr --> ReadSense[Read Soil Rain DHT]
ReadSense --> CheckMoist{Soil Moisture Low AND No Rain?}
CheckMoist -- Yes --> PumpOn[Pump ON]
CheckMoist -- No --> PumpOff[Pump OFF]
PumpOn --> UpdateLCD1[Notify LCD & IoT]
PumpOff --> UpdateLCD2[Notify LCD & IoT]
UpdateLCD1 --> ReadSense
UpdateLCD2 --> ReadSense
ManualOverride[Manual Pump Override] --> PumpOn
```
---

## State "Flowchart" 

```mermaid
flowchart TD
IdleIrrigation[Idle] -->|Timer Update| Sensing
Sensing -->|Moisture Low and Dry| Irrigating
Sensing -->|Moisture Okay or Rain| IdleIrrigation
Irrigating -->|Soil Wet or Rain| IdleIrrigation
IdleIrrigation -->|Manual Pump| Irrigating
Irrigating -->|Manual Off| IdleIrrigation
```

---
