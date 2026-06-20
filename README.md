# ESP32 Weather Station

## Overview
A compact microclimate sensing core built using an ESP32 DevKit V1, BMP280 pressure sensor, DHT11 temperature/humidity sensor, and an LDR for ambient light sensing.
The system periodically collects environmental data and displays it through the Serial Monitor. Designed with future battery-powered deployment in mind, the project supports low-power operation using ESP32 deep sleep functionality.

---

## Features
* Temperature monitoring using DHT11
* Humidity monitoring using DHT11
* Atmospheric pressure measurement using BMP280
* Altitude estimation based on pressure readings
* Heat Index calculation
* Ambient light sensing using LDR (photoresistor)
* I2C communication for BMP280
* Serial output for real-time monitoring
* Ready for deep sleep power optimization

---

## Hardware Components
| Component                     | Model                   |
| ----------------------------- | ----------------------- |
| Microcontroller               | ESP32 DevKit V1         |
| Temperature & Humidity Sensor | DHT11                   |
| Pressure Sensor               | BMP280                  |
| Light Sensor                  | LDR (Photoresistor) + Voltage Divider Resistor |
| Status LEDs                   | 2 × Green LEDs          |
| Jumper Wires                  | Male-Male / Male-Female |
| Power Source                  | USB or LiPo Battery     |

---

## Wiring

### DHT11 Connections
| DHT11 Pin | ESP32 Pin |
| --------- | --------- |
| VCC       | 3.3V      |
| GND       | GND       |
| DATA      | GPIO 4    |

### BMP280 Connections
| BMP280 Pin | ESP32 Pin |
| ---------- | --------- |
| VCC        | 3.3V      |
| GND        | GND       |
| SDA        | GPIO 21   |
| SCL        | GPIO 22   |

### LDR Connections
| LDR Pin | ESP32 Pin |
| ------- | --------- |
| VCC     | 3.3V      |
| GND     | GND       |
| AOUT    | GPIO 34   |

---

## Required Libraries
Install the following libraries through the Arduino IDE Library Manager:
1. Adafruit BMP280 Library
2. Adafruit Unified Sensor
3. DHT Sensor Library
4. Wire Library (included with Arduino framework)

---

## Sensor Data Collected

### DHT11
* Temperature (°C)
* Relative Humidity (%)
* Heat Index (°C)

### BMP280
* Atmospheric Pressure (hPa)
* Approximate Altitude (m)

### LDR
* Ambient light level (voltage, 0–3.3V)
* Approximate condition: Night / Cloudy-Dim / Daylight / Bright Sun

> **Note:** Light condition thresholds (e.g. what counts as "Daylight" vs "Bright Sun") are calibrated to this specific LDR + resistor combination. If you clone this project with a different resistor value or LDR model, your voltage readings will differ — recalibrate by logging readings at known conditions (dark, dim, daylight, direct sun) before trusting the labels.

---

## Example Serial Output
```text
Temperature: 31.00 °C | Humidity: 64.00 % | Heat Index: 36.25 °C
---------------------------------------------
Pressure = 1008.52 hPa
Approx altitude = 41.83 m
Light (LDR)     : 1.82 V
Condition       : Daylight
```

---

## Uploading the Firmware
1. Connect the ESP32 DevKit V1 to your computer.
2. Open the Arduino IDE.
3. Install the required libraries.
4. Select:
```text
Tools → Board → ESP32 Arduino → ESP32 Dev Module
```
5. Select the correct COM port.
6. Compile and upload the sketch.
7. Open the Serial Monitor at:
```text
115200 baud
```

---

## Wokwi Simulation
A Wokwi simulation is included (`diagram.json`) for anyone who wants to test the logic without physical hardware.

🔗 **[Try it on Wokwi](https://wokwi.com/projects/new/esp32)**

> **Note:** Wokwi does not have exact simulation models for the BMP280 and DHT11. The simulation substitutes the closest available parts — **BMP180** and **DHT22** — which behave similarly but are not identical in accuracy, response time, or I2C address ranges. Treat the simulation as a logic/wiring demo, not a sensor-accurate preview. The actual firmware (`WeatherStation.ino`) is written for the real BMP280 + DHT11 hardware.

### Recommended Upgrades for Better Accuracy
If you're building this for real and want better sensor accuracy than the current sensor lineup, consider:

| Current Sensor | Recommended Upgrade | Why |
| --- | --- | --- |
| DHT11 | **DHT22** or **SHT31** | DHT11 has ±2°C / ±5% RH accuracy and only updates once per ~2s. DHT22 improves to ±0.5°C / ±2–5% RH. SHT31 is tighter still and uses I2C instead of a finicky single-wire protocol. |
| BMP280 | **BME280** | Same pressure/altitude accuracy, but adds humidity sensing on the same chip — could let you drop the DHT entirely and cross-validate humidity too. |
| LDR (analog) | **BH1750** (digital lux sensor, I2C) | LDR gives raw voltage with no real unit — readings depend on your specific resistor and aren't comparable across builds. BH1750 outputs calibrated lux directly over I2C, so anyone cloning your repo gets meaningful numbers without manual calibration. |

Swapping to DHT22 also happens to make the **Wokwi simulation accurate**, since DHT22 is what the sim already uses.

---

## Power Consumption Analysis

### Estimated Current Draw
| Component       | Active Current | Deep Sleep Current |
| --------------- | -------------- | ------------------ |
| ESP32 DevKit V1 | ~240 mA        | ~0.01 mA           |
| BMP280          | ~1 mA          | ~0.0005 mA         |
| DHT11           | ~2.5 mA        | ~0.15 mA           |
| LDR             | ~0.5–1 mA      | ~0.5–1 mA (passive, draws whenever powered) |
| 2 × Green LEDs  | ~40 mA total   | ~40 mA total       |

### Average Current Consumption
Assuming:
* Awake time: 3 seconds
* Sleep time: 297 seconds
* Total cycle: 300 seconds (5 minutes)

#### With LEDs Always On
```text
Average Current ≈ 42 mA
```
Battery life using a 2000 mAh LiPo:
```text
2000 mAh ÷ 42 mA ≈ 47 hours
```
≈ 2 days

#### Without LEDs During Sleep
```text
Average Current ≈ 2.7 mA
```
Battery life using a 2000 mAh LiPo:
```text
2000 mAh ÷ 2.7 mA ≈ 740 hours
```
≈ 30 days

### Key Observation
The LEDs consume significantly more power than the sensors during deep sleep. Disabling or switching off LEDs while sleeping can improve battery life by more than 15×. The LDR, since it's a passive resistive sensor, also draws a small constant current whenever powered — worth switching off with the same MOSFET/BJT used for the LEDs if squeezing out maximum battery life.

---

## Future Improvements
* Wi-Fi weather dashboard
* Data logging to SQLite DB
* Battery voltage monitoring
* Web server interface
* PCB design in KiCad

---

## Project Structure
```text
Weather-Station/
│
├── WeatherStation.ino
├──diagram.json
├── simulation.ino
├── README.md
└── images/
	└── wiring_diagram.png
```
---

## License

This project is released under the MIT License and may be freely modified and distributed for educational and personal use.
