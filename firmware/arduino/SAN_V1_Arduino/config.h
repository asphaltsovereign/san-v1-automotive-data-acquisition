#pragma once

#include <Arduino.h>

namespace san {

// Classic ESP32 30-pin DevKit bench pin map.
constexpr uint8_t PIN_I2C_SDA = 21;
constexpr uint8_t PIN_I2C_SCL = 22;

constexpr uint8_t PIN_SD_SCK = 18;
constexpr uint8_t PIN_SD_MISO = 19;
constexpr uint8_t PIN_SD_MOSI = 23;
constexpr uint8_t PIN_SD_CS = 13;

constexpr uint8_t PIN_GPS_RX = 16;  // Connect to NEO-6M TX.
constexpr uint8_t PIN_GPS_TX = 17;  // Optional: connect to NEO-6M RX through 1 kOhm.
constexpr uint8_t PIN_ONEWIRE = 27;

// GPIO34 is input-only and has no internal pull-up/pull-down.
// Never connect this pin directly to an injector, ignition coil, crank sensor,
// ECU wire, or any vehicle signal. A validated conditioner is mandatory.
constexpr uint8_t PIN_RPM_INPUT = 34;
constexpr uint8_t PIN_BENCH_PULSE_OUT = 25;

constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t GPS_BAUD = 9600;
constexpr uint32_t SD_SPI_HZ = 8000000;

constexpr uint32_t SAMPLE_PERIOD_MS = 100;       // 10 Hz CSV rows.
constexpr uint32_t RPM_WINDOW_MS = 500;
constexpr uint32_t TEMP_REQUEST_PERIOD_MS = 1000;
constexpr uint32_t TEMP_CONVERSION_MS = 750;     // DS18B20, 12-bit.
constexpr uint32_t SD_FLUSH_PERIOD_MS = 1000;
constexpr uint32_t GPS_STALE_MS = 3000;

constexpr float RPM_PULSES_PER_REV = 1.0f;

// V0 bench: ADS A0 voltage is measured directly and must stay within 0..3.3 V.
// Keep this at 1.0 until a documented divider/protection network is built and
// validated. Changing only this number does NOT make a bike connection safe.
constexpr float VOLTAGE_DIVIDER_RATIO = 1.0f;
constexpr float VOLTAGE_CALIBRATION_GAIN = 1.0f;
constexpr float VOLTAGE_CALIBRATION_OFFSET_V = 0.0f;

// Set to 1 only on the isolated bench. With a 1 kOhm series resistor, jumper
// GPIO25 to GPIO34 and add 10 kOhm from GPIO34 to GND. The 100 Hz square wave
// should report approximately 6000 RPM at one pulse/revolution.
#ifndef SAN_ENABLE_BENCH_PULSE_GENERATOR
#define SAN_ENABLE_BENCH_PULSE_GENERATOR 0
#endif

constexpr uint16_t BENCH_PULSE_HZ = 100;
constexpr char CSV_SCHEMA_VERSION[] = "san_csv_1.0";

}  // namespace san

