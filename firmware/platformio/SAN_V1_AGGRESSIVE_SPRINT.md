# SAN-V1 Aggressive Sprint Command

**Sprint start:** 19 August 2026  
**V1 isolated-bench gate:** 20 August 2026  
**Rule:** speed comes from fixed gates and evidence, not from bypassing protection.

## 1. Current inventory decision

| Item | Use now | Engineering decision |
|---|:---:|---|
| ESP32 30-pin development board | Yes | Main logger/controller; exact module marking must be photographed |
| ADS analog-to-digital board | Conditional | Assumed ADS1115 at address 0x48; identify chip and breakout first |
| Waterproof temperature probe | Conditional | Assumed 3-wire DS18B20; identify wire colours/type before power |
| Two SD modules | One only | Test A and B separately; never connect both in V0 |
| NEO-6M GPS board | Conditional | Use UART at 9600; confirm breakout input-power marking |
| Digital multimeter | Essential | Every voltage/continuity gate; cannot characterize fast ECU pulses |
| Resistors/capacitors | Conditional | Values and ratings must be measured/identified |
| Breadboard/Dupont jumpers | Bench only | Prohibited on the motorcycle |
| Perfboard | Later | Use only after isolated bench passes; not automatically vehicle-grade |
| Buck converter | Bench characterization only | Do not power from R15 until transient/reverse protection exists |
| 16 GB microSD | Yes | FAT32; record make/class and use a card reader for evidence extraction |
| USB cable described as Type-B | Conditional | ESP32 likely needs USB-A-to-Micro-B; prove it carries data |
| R15 V2 wiring harness kit | Workshop only | Photograph label/couplers; verify genuine/correct year before installation |

## 2. Missing items that change scope

### Needed to complete the isolated V1 bench cleanly

- Verified 4.7 kOhm resistor for the DS18B20 data pull-up.
- Two equal measured resistors, preferably 10 kOhm, for the 1.65 V ADC test.
- 1 kOhm series resistor and 10 kOhm pull-down for optional RPM pulse self-test.
- microSD-to-PC card reader if the laptop has no slot.
- Known data-capable USB-A-to-Micro-B cable matching the ESP32 connector.

### Mandatory before motorcycle power or signal connection

- Inline fuse holder and correctly selected low-current fuse.
- Reverse-polarity protection.
- AEC-qualified TVS and designed input filter selected against the final power
  converter ratings.
- Automotive-suitable power converter; the generic LM2596 module is not
  accepted as the final vehicle supply without characterization/protection.
- 1% input-divider resistors, RC filtering, current limiting, and clamps for
  every analog channel.
- Designed RPM/speed pulse conditioner or isolation interface. No direct ECU,
  injector, crank, or ignition connection.
- Sealed connectors, automotive wire, proper open-barrel crimp tool, heat
  shrink, braided sleeve, strain relief, enclosure, and mechanical mounting.
- Access to an automotive/isolated oscilloscope or competent workshop scope.
  A multimeter cannot validate ignition, injector, crank, or tach waveforms.
- Correct R15 V2 service information and connector pin verification.

Confirm whether the previously discussed DS3231 RTC, PC817 optocoupler, and
inline fuse are actually available; they were not included in today's inventory.

## 3. Execution blocks for 19 August

### Block A — 30 minutes: identification freeze

1. Photograph every module front and back in bright light.
2. Photograph the ESP32 USB connector and metal-module marking.
3. Photograph resistor colour bands and capacitor markings.
4. Photograph the R15 harness label and every unique connector.
5. Assign physical labels: SD-A, SD-B, ADS-A, GPS-A, TEMP-A.

**Output:** one inventory folder; no unidentified board receives power.

### Block B — 45 minutes: power-off wiring

1. Wire only ESP32 + ADS + temperature probe.
2. Measure the DS18B20 pull-up and ADC-divider resistors before installation.
3. Use the DMM continuity mode to exclude 3V3-to-GND and 5V-to-GND shorts.
4. Power by ESP32 USB only.

**Stop condition:** any hot component, unstable USB connection, wrong rail, or
module identity conflict.

### Block C — 60 minutes: module-by-module firmware proof

1. Flash the provided firmware.
2. Confirm ADS self-test and compare the 1.65 V divider against the DMM.
3. Confirm room-temperature probe reading and hand-warm response.
4. Add SD-A and prove file creation; repeat separately with SD-B only if needed.
5. Add GPS last; take it outdoors and measure time-to-first-fix.

**Output:** serial screenshot plus first CSV.

### Block D — 45 minutes: integrated soak

1. Perform 10 cold boots.
2. Run 30 minutes continuously at 10 Hz.
3. Do one abrupt USB power-removal recovery test.
4. Validate row count, columns, sample order, and blank values/health flags.

**Output:** completed `BENCH_RUN_SHEET.md`, photos, CSV, serial screenshot.

### Parallel founder action — workshop calls

Call the shortlisted workshops using the screening script below. Do not promise
SAN installation yet; request harness replacement and factory baseline only.

## 4. Bench plan for 20 August

The V1 bench is a repeatable validation, not a demonstration that merely turns
on once.

1. Rebuild from the wiring table without relying on memory.
2. Complete all power-off checks on the run sheet.
3. Record DMM rails under normal logging.
4. Run ADC, temperature, GPS, storage, reboot, and recovery cases.
5. If available, run the isolated GPIO25-to-GPIO34 pulse test.
6. Archive the exact firmware, wiring photos, run sheet, and raw CSV together.

**PASS:** every mandatory run-sheet item passes.  
**CONDITIONAL:** GPS alone lacks fix indoors; repeat outdoors.  
**FAIL:** unstable power, corrupt SD, wrong ADC value, boot loop, hot part, or
unknown module identity.

## 5. R15 V2 baseline gate before SAN integration

The replacement harness and bike must first become a known stock reference.

### Workshop work order

- Confirm the harness part number/application and compare every coupler.
- Photograph original routing, grounds, connector locks, and damage before work.
- Install without cut wires, twisted joints, insulation-piercing probes, or
  household tape repairs.
- Verify battery resting voltage, cranking minimum, idle charging voltage, and
  charging voltage at a documented elevated RPM.
- Check fuses, earth voltage drop, connector pin fit, fuel pump prime, injector,
  ignition, fan, coolant sensor, TPS, crank signal, switches, lamps, and cluster.
- Record any diagnostic fault indication/code before clearing it.
- Verify cold start, hot idle, fan cycle, coolant leaks, road operation, and
  post-test fault state.
- Return the old harness and replaced parts.

### Baseline evidence session

- Petrol only; no ethanol and no SAN electrical connection.
- Cold start video and ambient/probe temperatures.
- Idle for five minutes; record charging voltage and coolant/fan behavior.
- Controlled road loop with repeatable route, rider, fuel, tyre pressure, and
  weather notes.
- Record odometer, fuel level, faults, and all deviations.

Only after the stock baseline passes can SAN receive a separate, fused,
read-only harness design review.

## 6. Chennai workshop call order

This is a screening order, not a blind endorsement. Select the named senior
technician only after the call answers are acceptable.

| Order | Candidate | Best role | Public contact |
|---:|---|---|---|
| 1 | Walters Yamaha / Motohub, Kilpauk | Authorized harness fitment and OEM baseline | 9003174444 / 9677715330 |
| 2 | Premium Bike Care, Kodungaiyur | R15 V2-aware independent mechanical baseline | 9080286132 |
| 3 | NT Auto Electrical, Royapettah | Specialist wiring continuity/connector/electrical diagnosis | 7401438404 |
| 4 | Jain Moto Bike Yamaha, Anna Nagar | Nearby authorized fallback | 9941890000 |

### Exact phone script

> I have a 2015 Yamaha R15 V2 and a complete replacement wiring harness. I need
> a senior technician who has personally replaced a full R15 V2 harness. The
> work must be no-cut and no-splice, with connector-by-connector verification,
> battery and charging measurements, ECU/sensor/fuel/ignition checks, cold and
> hot baseline testing, photos, and return of the old harness. This is for an
> engineering test vehicle, so I need written measurements. Who will do the
> work, what instruments do you have, how long will it take, and what is the
> labour estimate?

### Reject the workshop if

- It will not identify the technician before accepting the bike.
- It says the harness will be made to fit by cutting/splicing.
- It refuses before/after photographs or old-part return.
- It has only a multimeter for high-speed waveform diagnosis but claims it can
  validate injector/crank/ignition signals.
- It cannot give a scope, evidence list, or fault-escalation path.

## 7. SAN-V1 research sequence after baseline

1. Freeze a verified R15 connector/pin/signal matrix.
2. Observe target signals with appropriate protected instruments.
3. Select only read-only signals that do not disturb ECU operation.
4. Design and simulate each input conditioner and power front end.
5. Bench-inject normal, over-voltage, reverse, open, short, and noise cases.
6. Move from breadboard to revision-controlled perfboard/PCB prototype.
7. Validate thermal, vibration, connector retention, reboot, SD corruption, and
   fail-disconnected behavior.
8. Install on TV-001 with a fused detachable sub-harness and stock-bypass path.
9. Repeat petrol baseline before any ethanol experimental program.

The first SAN motorcycle version should observe and log. It must not control
fuel or ignition.

