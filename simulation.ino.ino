#include <Wire.h>
#include <Adafruit_BMP085.h>   
#include <DHT.h>

#define DHT_PIN          4
#define DHT_TYPE         DHT22   
#define I2C_SDA_PIN      21
#define I2C_SCL_PIN      22
#define LDR_PIN          34

#define TEMP_DELTA_WARN  5.0f
#define ADC_VREF         3.3f
#define ADC_MAX          4095.0f

static DHT            dht(DHT_PIN, DHT_TYPE);
static Adafruit_BMP085 bmp;     // BMP180 object, different class name

struct WeatherData {
  float lightVoltage;
  float tempDHT;
  float humidity;
  float heatIndex;
  float tempBMP;
  float pressureHPa;
  float altitudeM;
  bool  dhtValid;
  bool  bmpValid;
};

static WeatherData readSensors() {
  WeatherData d = {};

  float hum   = dht.readHumidity();
  float tempC = dht.readTemperature();
  d.dhtValid  = !(isnan(hum) || isnan(tempC));
  if (d.dhtValid) {
    d.humidity  = hum;
    d.tempDHT   = tempC;
    d.heatIndex = dht.computeHeatIndex(tempC, hum, false);
  }

  // BMP180 API is different — no readAltitude(seaLevel) overload like BMP280
  d.tempBMP     = bmp.readTemperature();
  d.pressureHPa = bmp.readPressure() / 100.0f;
  d.altitudeM   = bmp.readAltitude();
  d.bmpValid    = !(isnan(d.tempBMP) || isnan(d.pressureHPa));

  int raw = analogRead(LDR_PIN);
  d.lightVoltage = raw * (ADC_VREF / ADC_MAX);

  return d;
}

static const char* lightCondition(float v) {
  if (v < 0.5)      return "Night";
  else if (v < 1.5) return "Cloudy / Dim";
  else if (v < 2.5) return "Daylight";
  else              return "Bright Sun";
}

static void printWeather(const WeatherData& d) {
  Serial.println(F("\n╔══ Weather Station ══════════════════╗"));

  if (d.dhtValid) {
    Serial.print(F("║ Temp  (DHT22)  : ")); Serial.print(d.tempDHT, 1);  Serial.println(F(" °C"));
    Serial.print(F("║ Humidity       : ")); Serial.print(d.humidity, 1); Serial.println(F(" %"));
    Serial.print(F("║ Heat Index     : ")); Serial.print(d.heatIndex, 1); Serial.println(F(" °C"));
  } else {
    Serial.println(F("║ [ERR] DHT22 read failed — check wiring"));
  }

  Serial.println(F("║──────────────────────────────────────"));

  if (d.bmpValid) {
    Serial.print(F("║ Pressure       : ")); Serial.print(d.pressureHPa, 2); Serial.println(F(" hPa"));
    Serial.print(F("║ Altitude       : ")); Serial.print(d.altitudeM, 1);   Serial.println(F(" m"));
  } else {
    Serial.println(F("║ [ERR] BMP180 read failed — check I2C"));
  }

  Serial.print(F("║ Light (LDR)    : ")); Serial.print(d.lightVoltage, 2); Serial.println(F(" V"));
  Serial.print(F("║ Condition      : ")); Serial.println(lightCondition(d.lightVoltage));

  if (d.dhtValid && d.bmpValid) {
    float delta = fabsf(d.tempDHT - d.tempBMP);
    if (delta > TEMP_DELTA_WARN) {
      Serial.print(F("║ [WARN] Sensor temp delta: ")); Serial.print(delta, 1);
      Serial.println(F(" °C — check placement/airflow"));
    }
  }

  Serial.println(F("╚══════════════════════════════════════"));
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println(F("\n[+] Weather Station starting..."));

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  dht.begin();

  if (!bmp.begin()) {
    Serial.println(F("[FATAL] BMP180 not found. Restarting in 5s..."));
    delay(5000);
    ESP.restart();
  }

  Serial.println(F("[+] BMP180 OK"));
  Serial.println(F("[+] DHT22 OK"));
  Serial.println(F("[+] Ready\n"));
}

void loop() {
  printWeather(readSensors());

  Serial.flush();
  esp_sleep_enable_timer_wakeup(60ULL * 1000000ULL);
  esp_deep_sleep_start();
}