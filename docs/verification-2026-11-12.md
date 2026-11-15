# Host verification — 12 November 2026

Python 3.12.14, Linux. No hardware connected.

`python3 -m unittest discover -s tests -v`: exit 0; **8 passed**. Covers the synthetic reference, mixed devices/sessions, sequence gaps, reversed uptime, wrong schema, wrong rate and empty data.

`python3 firmware/platformio/tools/validate_csv.py data/samples/synthetic_bench.csv`: exit 0; 11 generated rows, duration 1.000 seconds, observed 10.000 Hz. This is a fixture's timing arithmetic, not an achieved device sampling rate.

A separate negative probe replaced a synthetic voltage with `nan`. The original validator still returned 0. This documented limitation confirms it is a structural validator; finite-channel validation and physical plausibility are future work. Source behavior was preserved.

Firmware compilation/upload, module operation, reference calibration, cold boots, 30-minute continuity, power-loss behavior, vehicle electrical compatibility and real OctaneOS integration were NOT RUN.

The source packages and SHA-256 values are recorded in `docs/source-provenance.md`. No file here is described as a genuine acquisition log.
