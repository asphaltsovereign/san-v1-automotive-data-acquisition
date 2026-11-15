#!/usr/bin/env python3
"""Validate the structural SAN CSV evidence produced by bench firmware."""

from __future__ import annotations

import argparse
import csv
import math
from pathlib import Path


EXPECTED_COLUMNS = [
    "schema_version",
    "device_id",
    "session_id",
    "sample_seq",
    "timestamp_utc",
    "uptime_ms",
    "state",
    "rpm",
    "voltage_v",
    "probe_temp_c",
    "latitude_deg",
    "longitude_deg",
    "gps_speed_kph",
    "gps_fix",
    "satellites",
    "hdop",
    "health_flags",
]


def validate(path: Path, expected_hz: float) -> int:
    errors: list[str] = []
    row_count = 0
    first_uptime: int | None = None
    last_uptime: int | None = None
    previous_sequence: int | None = None
    previous_uptime: int | None = None
    devices: set[str] = set()
    sessions: set[str] = set()

    with path.open("r", newline="", encoding="utf-8-sig") as handle:
        reader = csv.DictReader(handle)
        if reader.fieldnames != EXPECTED_COLUMNS:
            errors.append(
                f"header mismatch: expected {EXPECTED_COLUMNS}, got {reader.fieldnames}"
            )

        for line_number, row in enumerate(reader, start=2):
            row_count += 1
            try:
                sequence = int(row["sample_seq"])
                uptime = int(row["uptime_ms"])
                int(row["health_flags"])
            except (KeyError, TypeError, ValueError) as exc:
                errors.append(f"line {line_number}: invalid required integer: {exc}")
                continue

            if row.get("schema_version") != "san_csv_1.0":
                errors.append(f"line {line_number}: unsupported schema version")

            if previous_sequence is not None and sequence != previous_sequence + 1:
                errors.append(
                    f"line {line_number}: sequence jumped from "
                    f"{previous_sequence} to {sequence}"
                )
            if previous_uptime is not None and uptime <= previous_uptime:
                errors.append(
                    f"line {line_number}: uptime is not strictly increasing"
                )

            first_uptime = uptime if first_uptime is None else first_uptime
            last_uptime = uptime
            previous_sequence = sequence
            previous_uptime = uptime
            devices.add(row.get("device_id", ""))
            sessions.add(row.get("session_id", ""))

    if row_count == 0:
        errors.append("no data rows")

    duration_s = 0.0
    observed_hz = math.nan
    if first_uptime is not None and last_uptime is not None:
        duration_s = max(0.0, (last_uptime - first_uptime) / 1000.0)
        if duration_s > 0:
            observed_hz = max(0, row_count - 1) / duration_s

    if len(devices) != 1:
        errors.append(f"expected one device_id, found {sorted(devices)}")
    if len(sessions) != 1:
        errors.append(f"expected one session_id, found {sorted(sessions)}")
    if not math.isnan(observed_hz) and abs(observed_hz - expected_hz) > 0.5:
        errors.append(
            f"sample rate {observed_hz:.3f} Hz outside "
            f"{expected_hz:.1f} +/-0.5 Hz"
        )

    print(f"file: {path}")
    print(f"rows: {row_count}")
    print(f"duration_s: {duration_s:.3f}")
    print(
        "observed_hz: "
        + ("unavailable" if math.isnan(observed_hz) else f"{observed_hz:.3f}")
    )
    print(f"result: {'FAIL' if errors else 'PASS'}")
    for error in errors[:50]:
        print(f"ERROR: {error}")
    if len(errors) > 50:
        print(f"ERROR: {len(errors) - 50} additional error(s) omitted")
    return 1 if errors else 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("csv_file", type=Path)
    parser.add_argument("--expected-hz", type=float, default=10.0)
    args = parser.parse_args()
    return validate(args.csv_file, args.expected_hz)


if __name__ == "__main__":
    raise SystemExit(main())

