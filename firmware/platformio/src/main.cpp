#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <DallasTemperature.h>
#include <FS.h>
#include <OneWire.h>
#include <SD.h>
#include <SPI.h>
#include <TinyGPSPlus.h>
#include <Wire.h>

#include "config.h"

namespace {

enum class SystemState : uint8_t {
  BOOT,
  SELF_TEST,
  READY,
  LOGGING,
  DEGRADED,
  FAULT
};

enum HealthFlag : uint16_t {
  HF_NONE = 0,
  HF_ADS_MISSING = 1u << 0,
  HF_SD_MISSING = 1u << 1,
  HF_TEMP_MISSING = 1u << 2,
  HF_GPS_NO_FIX = 1u << 3,
  HF_GPS_STALE = 1u << 4,
  HF_SD_WRITE_FAIL = 1u << 5,
};

Adafruit_ADS1115 ads;
OneWire oneWire(san::PIN_ONEWIRE);
DallasTemperature temperatureSensors(&oneWire);
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);
SPIClass sdSpi(VSPI);
File logFile;

SystemState systemState = SystemState::BOOT;
bool adsOk = false;
bool sdOk = false;
bool tempBusStarted = false;
bool tempConversionPending = false;
bool sdWriteFailed = false;

float measuredVoltageV = NAN;
float probeTemperatureC = NAN;
float rpm = 0.0f;

uint32_t sampleSequence = 0;
uint32_t lastSampleMs = 0;
uint32_t lastRpmWindowMs = 0;
uint32_t lastTempRequestMs = 0;
uint32_t tempRequestStartedMs = 0;
uint32_t lastFlushMs = 0;

volatile uint32_t rpmPulseCount = 0;
portMUX_TYPE rpmMux = portMUX_INITIALIZER_UNLOCKED;

char deviceId[16] = {};
char sessionId[32] = {};
char logPath[20] = {};

void IRAM_ATTR onRpmPulse() {
  portENTER_CRITICAL_ISR(&rpmMux);
  ++rpmPulseCount;
  portEXIT_CRITICAL_ISR(&rpmMux);
}

const char* stateName(SystemState state) {
  switch (state) {
    case SystemState::BOOT: return "BOOT";
    case SystemState::SELF_TEST: return "SELF_TEST";
    case SystemState::READY: return "READY";
    case SystemState::LOGGING: return "LOGGING";
    case SystemState::DEGRADED: return "DEGRADED";
    case SystemState::FAULT: return "FAULT";
  }
  return "UNKNOWN";
}

void buildDeviceId() {
  const uint64_t chipId = ESP.getEfuseMac();
  snprintf(deviceId, sizeof(deviceId), "SAN-%06llX",
           static_cast<unsigned long long>(chipId & 0xFFFFFFULL));
}

void feedGps() {
  while (gpsSerial.available() > 0) {
    gps.encode(static_cast<char>(gpsSerial.read()));
  }
}

bool createSessionFile() {
  for (uint16_t index = 1; index <= 9999; ++index) {
    snprintf(logPath, sizeof(logPath), "/SAN%04u.CSV", index);
    if (!SD.exists(logPath)) {
      snprintf(sessionId, sizeof(sessionId), "%s-%04u", deviceId, index);
      logFile = SD.open(logPath, FILE_WRITE);
      break;
    }
  }

  if (!logFile) return false;

  logFile.println(
      "schema_version,device_id,session_id,sample_seq,timestamp_utc,"
      "uptime_ms,state,rpm,voltage_v,probe_temp_c,latitude_deg,"
      "longitude_deg,gps_speed_kph,gps_fix,satellites,hdop,health_flags");
  logFile.flush();
  return static_cast<bool>(logFile);
}

void updateRpm(uint32_t nowMs) {
  const uint32_t elapsedMs = nowMs - lastRpmWindowMs;
  if (elapsedMs < san::RPM_WINDOW_MS) return;

  uint32_t pulses = 0;
  portENTER_CRITICAL(&rpmMux);
  pulses = rpmPulseCount;
  rpmPulseCount = 0;
  portEXIT_CRITICAL(&rpmMux);

  if (san::RPM_PULSES_PER_REV > 0.0f && elapsedMs > 0) {
    rpm = (static_cast<float>(pulses) * 60000.0f) /
          (static_cast<float>(elapsedMs) * san::RPM_PULSES_PER_REV);
  } else {
    rpm = NAN;
  }
  lastRpmWindowMs = nowMs;
}

void updateVoltage() {
  if (!adsOk) {
    measuredVoltageV = NAN;
    return;
  }

  const int16_t raw = ads.readADC_SingleEnded(0);
  const float adcVoltage = ads.computeVolts(raw);
  measuredVoltageV =
      ((adcVoltage * san::VOLTAGE_DIVIDER_RATIO) *
       san::VOLTAGE_CALIBRATION_GAIN) +
      san::VOLTAGE_CALIBRATION_OFFSET_V;
}

void updateTemperature(uint32_t nowMs) {
  if (!tempBusStarted) {
    probeTemperatureC = NAN;
    return;
  }

  if (!tempConversionPending &&
      (nowMs - lastTempRequestMs >= san::TEMP_REQUEST_PERIOD_MS)) {
    temperatureSensors.requestTemperatures();
    tempRequestStartedMs = nowMs;
    lastTempRequestMs = nowMs;
    tempConversionPending = true;
  }

  if (tempConversionPending &&
      (nowMs - tempRequestStartedMs >= san::TEMP_CONVERSION_MS)) {
    const float reading = temperatureSensors.getTempCByIndex(0);
    probeTemperatureC =
        (reading == DEVICE_DISCONNECTED_C || reading < -55.0f ||
         reading > 125.0f)
            ? NAN
            : reading;
    tempConversionPending = false;
  }
}

uint16_t healthFlags(uint32_t nowMs) {
  uint16_t flags = HF_NONE;
  if (!adsOk) flags |= HF_ADS_MISSING;
  if (!sdOk) flags |= HF_SD_MISSING;
  if (isnan(probeTemperatureC)) flags |= HF_TEMP_MISSING;
  if (!gps.location.isValid()) flags |= HF_GPS_NO_FIX;
  if (gps.location.isValid() && gps.location.age() > san::GPS_STALE_MS) {
    flags |= HF_GPS_STALE;
  }
  if (sdWriteFailed) flags |= HF_SD_WRITE_FAIL;
  return flags;
}

void updateState(uint16_t flags) {
  if (!sdOk || sdWriteFailed) {
    systemState = SystemState::DEGRADED;
    return;
  }
  const uint16_t sensorFaults = HF_ADS_MISSING | HF_TEMP_MISSING;
  systemState = (flags & sensorFaults) ? SystemState::DEGRADED
                                      : SystemState::LOGGING;
}

void printFloatOrBlank(Print& out, double value, uint8_t decimals) {
  if (!isnan(value)) out.print(value, decimals);
}

void printTimestampOrBlank(Print& out) {
  if (!(gps.date.isValid() && gps.time.isValid())) return;

  char timestamp[24] = {};
  snprintf(timestamp, sizeof(timestamp), "%04u-%02u-%02uT%02u:%02u:%02uZ",
           gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(),
           gps.time.minute(), gps.time.second());
  out.print(timestamp);
}

void writeCsvRow(Print& out, uint32_t nowMs, uint16_t flags) {
  out.print(san::CSV_SCHEMA_VERSION);
  out.print(',');
  out.print(deviceId);
  out.print(',');
  out.print(sessionId);
  out.print(',');
  out.print(sampleSequence);
  out.print(',');
  printTimestampOrBlank(out);
  out.print(',');
  out.print(nowMs);
  out.print(',');
  out.print(stateName(systemState));
  out.print(',');
  printFloatOrBlank(out, rpm, 1);
  out.print(',');
  printFloatOrBlank(out, measuredVoltageV, 4);
  out.print(',');
  printFloatOrBlank(out, probeTemperatureC, 2);
  out.print(',');
  if (gps.location.isValid()) out.print(gps.location.lat(), 7);
  out.print(',');
  if (gps.location.isValid()) out.print(gps.location.lng(), 7);
  out.print(',');
  if (gps.speed.isValid()) out.print(gps.speed.kmph(), 2);
  out.print(',');
  out.print(gps.location.isValid() ? 1 : 0);
  out.print(',');
  if (gps.satellites.isValid()) out.print(gps.satellites.value());
  out.print(',');
  if (gps.hdop.isValid()) out.print(gps.hdop.hdop(), 2);
  out.print(',');
  out.println(flags);
}

void logSample(uint32_t nowMs) {
  updateVoltage();
  const uint16_t flags = healthFlags(nowMs);
  updateState(flags);

  ++sampleSequence;
  writeCsvRow(Serial, nowMs, flags);

  if (sdOk && logFile) {
    const size_t bytesBefore = logFile.size();
    writeCsvRow(logFile, nowMs, flags);
    if (logFile.getWriteError() || logFile.size() <= bytesBefore) {
      sdWriteFailed = true;
    }
  }
}

void initialiseHardware() {
  systemState = SystemState::SELF_TEST;

  Wire.begin(san::PIN_I2C_SDA, san::PIN_I2C_SCL);
  Wire.setClock(100000);
  adsOk = ads.begin(0x48, &Wire);
  if (adsOk) {
    ads.setGain(GAIN_ONE);  // +/-4.096 V FSR; absolute pin limit still applies.
    ads.setDataRate(RATE_ADS1115_128SPS);
  }

  temperatureSensors.begin();
  temperatureSensors.setResolution(12);
  temperatureSensors.setWaitForConversion(false);
  tempBusStarted = temperatureSensors.getDeviceCount() > 0;

  gpsSerial.begin(san::GPS_BAUD, SERIAL_8N1, san::PIN_GPS_RX,
                  san::PIN_GPS_TX);

  pinMode(san::PIN_RPM_INPUT, INPUT);
  attachInterrupt(digitalPinToInterrupt(san::PIN_RPM_INPUT), onRpmPulse,
                  RISING);

#if SAN_ENABLE_BENCH_PULSE_GENERATOR
  pinMode(san::PIN_BENCH_PULSE_OUT, OUTPUT);
  tone(san::PIN_BENCH_PULSE_OUT, san::BENCH_PULSE_HZ);
#endif

  sdSpi.begin(san::PIN_SD_SCK, san::PIN_SD_MISO, san::PIN_SD_MOSI,
              san::PIN_SD_CS);
  sdOk = SD.begin(san::PIN_SD_CS, sdSpi, san::SD_SPI_HZ);
  if (sdOk) sdOk = createSessionFile();

  systemState = sdOk ? SystemState::READY : SystemState::DEGRADED;
}

}  // namespace

void setup() {
  Serial.begin(san::SERIAL_BAUD);
  delay(300);
  buildDeviceId();

  Serial.println();
  Serial.println("SAN-V1 bench firmware boot");
  Serial.print("Device: ");
  Serial.println(deviceId);

  initialiseHardware();
  const uint32_t nowMs = millis();
  lastSampleMs = nowMs;
  lastRpmWindowMs = nowMs;
  lastTempRequestMs = nowMs - san::TEMP_REQUEST_PERIOD_MS;
  lastFlushMs = nowMs;

  Serial.print("ADS1115: ");
  Serial.println(adsOk ? "PASS" : "FAIL");
  Serial.print("DS18B20: ");
  Serial.println(tempBusStarted ? "PASS" : "FAIL");
  Serial.print("microSD: ");
  Serial.println(sdOk ? logPath : "FAIL");
  Serial.println("GPS fix may require outdoor sky view.");
}

void loop() {
  const uint32_t nowMs = millis();

  feedGps();
  updateRpm(nowMs);
  updateTemperature(nowMs);

  if (nowMs - lastSampleMs >= san::SAMPLE_PERIOD_MS) {
    lastSampleMs += san::SAMPLE_PERIOD_MS;
    logSample(nowMs);
  }

  if (sdOk && logFile && nowMs - lastFlushMs >= san::SD_FLUSH_PERIOD_MS) {
    logFile.flush();
    lastFlushMs = nowMs;
  }
}

