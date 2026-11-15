# SAN-V1 V0 Bench Firmware

This package is the safe first firmware and bench contract for the Asphalt
Sovereign SAN-V1 telemetry node. It is deliberately a **data acquisition
logger**, not an OBD scanner and not a vehicle-ready interface.

## Safety boundary

- Power the system from the ESP32 USB port for V0 bench work.
- Do not connect the buck converter to the motorcycle during this test.
- Do not connect the ESP32, ADS1115, or breadboard directly to battery positive,
  injector, ignition coil, crank sensor, ECU, TPS, or any R15 wire.
- The ADS1115 input must remain between ground and its supply rail. A selected
  +/-4.096 V ADC range does not permit a pin voltage above VDD.
- GPIO34 accepts only a conditioned 0-3.3 V pulse. It is not 5 V tolerant and
  has no internal pull resistor.
- The waterproof probe is treated as `probe_temp_c`; its location and thermal
  attachment determine what it actually represents.

## Provisional pin map

| Subsystem | Module pin | ESP32 pin | Bench note |
|---|---|---:|---|
| ADS1115 | VDD / GND | 3V3 / GND | Power at 3.3 V |
| ADS1115 | SDA / SCL | GPIO21 / GPIO22 | I2C address assumed 0x48 |
| ADS1115 | A0 | Protected bench signal | 0-3.3 V only |
| DS18B20 | VDD / GND | 3V3 / GND | Use 3-wire powered mode |
| DS18B20 | DATA | GPIO27 | 4.7 kOhm DATA-to-3V3 pull-up |
| NEO-6M | TX | GPIO16 (RX2) | Required |
| NEO-6M | RX | GPIO17 (TX2) | Optional; insert 1 kOhm series |
| NEO-6M | VCC / GND | Per breakout marking / GND | Module-board input must be identified |
| SD module | SCK / MISO / MOSI / CS | 18 / 19 / 23 / 13 | One SD module only in V0 |
| RPM bench | conditioned pulse | GPIO34 | 10 kOhm pull-down required |

All grounds are common on the isolated USB-powered bench.

## First analog test

Create a known, safe ADC voltage with two equal-value resistors:

1. Connect one resistor from ESP32 3V3 to ADS1115 A0.
2. Connect the second equal resistor from A0 to GND.
3. Verify A0 with the multimeter before powering the ADS connection.
4. Expected reading is approximately 1.65 V.

Do not use unknown resistor values. Measure each resistor with power removed.

## Optional RPM self-test

1. Set `SAN_ENABLE_BENCH_PULSE_GENERATOR=1` as a build flag.
2. Put 1 kOhm in series between GPIO25 and GPIO34.
3. Put 10 kOhm from GPIO34 to GND.
4. A 100 Hz test signal at one pulse/revolution should report about 6000 RPM.
5. Remove this jumper and return the flag to 0 before any later harness work.

## Build and flash with PlatformIO

1. Install VS Code and the PlatformIO extension.
2. Open this folder as a PlatformIO project.
3. Connect a known data-capable USB-A-to-Micro-B cable.
4. Use **Build**, then **Upload**, then open the serial monitor at 115200 baud.

For the optional pulse self-test, add this line under `build_flags` in
`platformio.ini`:

```ini
  -DSAN_ENABLE_BENCH_PULSE_GENERATOR=1
```

## CSV contract

Every 100 ms the logger emits:

```text
schema_version,device_id,session_id,sample_seq,timestamp_utc,uptime_ms,state,rpm,voltage_v,probe_temp_c,latitude_deg,longitude_deg,gps_speed_kph,gps_fix,satellites,hdop,health_flags
```

Blank values mean unavailable data. `health_flags` is a bit mask:

| Bit/value | Meaning |
|---:|---|
| 1 | ADS1115 missing |
| 2 | SD missing |
| 4 | Temperature probe missing/invalid |
| 8 | No GPS fix |
| 16 | GPS location stale |
| 32 | SD write failure |

After copying a log to the computer, validate its structure and sampling rate:

```powershell
python tools\validate_csv.py D:\path\to\SAN0001.CSV
```

## V1 bench acceptance gate

The bench passes only if all of the following evidence is saved:

- 10 cold boots without wiring changes; no boot loop.
- ADS reading agrees with the multimeter within the provisional target of
  +/-0.03 V at the 1.65 V test point.
- Temperature reading is stable at room temperature and clearly changes when
  the probe is warmed by hand.
- GPS produces valid latitude/longitude outdoors; record time-to-first-fix.
- SD creates a new file, keeps monotonic sample sequence, and contains at least
  30 continuous minutes at 10 Hz.
- Power is removed without using software shutdown, then the card is checked
  for a readable prior file and a newly created next session.
- Optional pulse test reports 6000 RPM within +/-2%.
- No component, jumper, or regulator becomes hot; record ESP32 5 V and 3.3 V
  rails with the multimeter.

## Required photographs before final wiring approval

Provide clear front-and-back photos of the ESP32, ADS board, both SD modules,
GPS board, temperature probe lead/connector, buck converter, resistor markings,
capacitor markings, USB connector, and the R15 V2 harness/part label. Generic
modules differ in regulators and level shifters, so their power pins cannot be
approved from names alone.
