#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

// ── Pin Definitions ─────────────────────────────────────────────────────────
#define DHT_PIN          4      // GPIO4 → DHT11 data line
#define I2C_SDA_PIN      21     // ESP32 DevKit V1 default SDA
#define I2C_SCL_PIN      22     // ESP32 DevKit V1 default SCL
#define LDR_PIN          34     // ESP32 DevKit V1 ADC1_CH6

// ── Sensor Config ───────────────────────────────────────────────────────────
#define DHT_TYPE         DHT11
#define BMP280_ADDR_PRI  0x76
#define BMP280_ADDR_SEC  0x77
#define SEA_LEVEL_HPA    1013.25f
#define TEMP_DELTA_WARN  5.0f
#define ADC_VREF         3.3f
#define ADC_MAX          4095.0f

static DHT             dht(DHT_PIN, DHT_TYPE);
static Adafruit_BMP280 bmp;

struct WeatherData {
  float lightVoltage;  // V — raw ADC converted to volts
  float tempDHT;
  float humidity;
  float heatIndex;
  float tempBMP;
  float pressureHPa;
  float altitudeM;
  bool  dhtValid;
  bool  bmpValid;
};

static bool initBMP280() {
  if (bmp.begin(BMP280_ADDR_PRI)) return true;
  if (bmp.begin(BMP280_ADDR_SEC)) {
    Serial.println(F("[WARN] BMP280 found at 0x77 (secondary address)"));
    return true;
  }
  return false;
}

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

  float pres    = bmp.readPressure();
  float tempBMP = bmp.readTemperature();
  float alt     = bmp.readAltitude(SEA_LEVEL_HPA);
  d.bmpValid    = !(isnan(pres) || isnan(tempBMP) || isnan(alt));
  if (d.bmpValid) {
    d.pressureHPa = pres / 100.0f;
    d.tempBMP     = tempBMP;
    d.altitudeM   = alt;
  }

  int raw = analogRead(LDR_PIN);
  d.lightVoltage = raw * (ADC_VREF / ADC_MAX);

  return d;
}

// ── Map voltage → human-readable condition ───────────────────────────────────
// NOTE: thresholds are placeholders — replace with YOUR calibrated values
// after logging dark / dim / daylight / bright-sun readings on your hardware.
static const char* lightCondition(float v) {
  if (v < 0.5)      return "Night";
  else if (v < 1.5) return "Cloudy / Dim";
  else if (v < 2.5) return "Daylight";
  else              return "Bright Sun";
}

static void printWeather(const WeatherData& d) {
  Serial.println(F("\n╔══ Weather Station ══════════════════╗"));

  if (d.dhtValid) {
    Serial.print(F("║ Temp  (DHT11)  : ")); Serial.print(d.tempDHT, 1);  Serial.println(F(" °C"));
    Serial.print(F("║ Humidity       : ")); Serial.print(d.humidity, 1); Serial.println(F(" %"));
    Serial.print(F("║ Heat Index     : ")); Serial.print(d.heatIndex, 1); Serial.println(F(" °C"));
  } else {
    Serial.println(F("║ [ERR] DHT11 read failed — check wiring"));
  }

  Serial.println(F("║──────────────────────────────────────"));

  if (d.bmpValid) {
    Serial.print(F("║ Pressure       : ")); Serial.print(d.pressureHPa, 2); Serial.println(F(" hPa"));
    Serial.print(F("║ Altitude       : ")); Serial.print(d.altitudeM, 1);   Serial.println(F(" m"));
  } else {
    Serial.println(F("║ [ERR] BMP280 read failed — check I2C"));
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
  delay(1000); // give Serial Monitor time to attach after deep-sleep wake

  Serial.println(F("\n[+] Weather Station starting..."));

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  dht.begin();

  if (!initBMP280()) {
    Serial.println(F("[FATAL] BMP280 not found."));
    Serial.print(F("        Sensor ID: 0x")); Serial.println(bmp.sensorID(), HEX);
    Serial.println(F("        Restarting in 5 s..."));
    delay(5000);
    ESP.restart();
  }

  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X1,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );

  Serial.println(F("[+] BMP280 OK"));
  Serial.println(F("[+] DHT11 OK"));
  Serial.println(F("[+] LDR OK"));
  Serial.println(F("[+] Ready\n"));
}

void loop() {
  printWeather(readSensors());

  Serial.flush();
  esp_sleep_enable_timer_wakeup(60ULL * 1000000ULL); // 1 minute
  esp_deep_sleep_start();
}
