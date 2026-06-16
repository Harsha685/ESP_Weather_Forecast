# ESP32 Weather Station

## Overview

A compact microclimate sensing core built using an ESP32 DevKit V1, BMP280 pressure sensor, and DHT11 temperature/humidity sensor.

The system periodically collects environmental data and displays it through the Serial Monitor. Designed with future battery-powered deployment in mind, the project supports low-power operation using ESP32 deep sleep functionality.

---

## Features

* Temperature monitoring using DHT11
* Humidity monitoring using DHT11
* Atmospheric pressure measurement using BMP280
* Altitude estimation based on pressure readings
* Heat Index calculation
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

---

## Example Serial Output

```text
Temperature: 31.00 °C | Humidity: 64.00 % | Heat Index: 36.25 °C
---------------------------------------------
Pressure = 1008.52 hPa
Approx altitude = 41.83 m
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

## Power Consumption Analysis

### Estimated Current Draw

| Component       | Active Current | Deep Sleep Current |
| --------------- | -------------- | ------------------ |
| ESP32 DevKit V1 | ~240 mA        | ~0.01 mA           |
| BMP280          | ~1 mA          | ~0.0005 mA         |
| DHT11           | ~2.5 mA        | ~0.15 mA           |
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

The LEDs consume significantly more power than the sensors during deep sleep. Disabling or switching off LEDs while sleeping can improve battery life by more than 15×.

---

## Future Improvements

* Wi-Fi weather dashboard
* Data logging to sqlit db
* Battery voltage monitoring
* BME280 upgrade for higher accuracy
* Web server interface

---

## Project Structure

```text
Weather-Station/
│
├── WeatherStation.ino
├── README.md
└── images/
	└── wiring_diagram.png
```

---

## License

This project is released under the MIT License and may be freely modified and distributed for educational and personal use.
