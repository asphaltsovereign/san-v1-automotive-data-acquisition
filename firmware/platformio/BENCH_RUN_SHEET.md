# SAN-V1 V1 Bench Run Sheet

Date: __________  Operator: __________  Firmware commit/version: __________

## A. Identification before power

- [ ] Module front/back photographs taken
- [ ] Exact ESP32 module marking recorded: ______________________________
- [ ] ADS board marking recorded: ______________________________________
- [ ] Temperature sensor type verified: _________________________________
- [ ] SD module used (A/B) recorded: ____________________________________
- [ ] NEO-6M breakout input-voltage marking recorded: ____________________
- [ ] microSD formatted FAT32 and empty
- [ ] Wiring continuity checked with power off
- [ ] No bike or buck-converter connection present

## B. Power-off resistance/short checks

| Check | Expected | Measured | Pass |
|---|---:|---:|:---:|
| 3V3 to GND | No near-short | | [ ] |
| 5V to GND | No near-short | | [ ] |
| ADS A0 test divider top resistor | Recorded value | | [ ] |
| ADS A0 test divider lower resistor | Same value | | [ ] |
| DS18B20 DATA pull-up | ~4.7 kOhm | | [ ] |
| RPM input pull-down | ~10 kOhm | | [ ] |

## C. Powered measurements

| Measurement | Target | Actual | Pass |
|---|---:|---:|:---:|
| ESP32 5V pin | USB-dependent, record | | [ ] |
| ESP32 3V3 pin | ~3.3 V | | [ ] |
| ADS A0 divider | ~1.65 V | | [ ] |
| Firmware ADS voltage | Multimeter +/-0.03 V | | [ ] |
| Room probe temperature | Plausible/stable | | [ ] |
| Warmed probe response | Clear positive change | | [ ] |
| RPM self-test (optional) | 6000 +/-2% | | [ ] |

## D. GPS and storage

- [ ] Outdoor GPS fix achieved
- Time-to-first-fix: __________ seconds
- Satellites after fix: __________
- [ ] Latitude/longitude plausible
- [ ] New SANxxxx.CSV created
- [ ] 30-minute log completed
- Rows expected at 10 Hz: about 18,000; actual: __________
- [ ] Sample sequence monotonic
- [ ] No malformed column counts found
- [ ] Abrupt USB removal recovery test passed

## E. Gate decision

- [ ] PASS: V1 isolated bench complete
- [ ] CONDITIONAL: repeat failed item(s)
- [ ] FAIL: hardware or firmware defect unresolved

Failed observations / evidence paths:

________________________________________________________________________

________________________________________________________________________

Engineer sign-off: ____________________  Time: ____________________

