# Selected raw bench contract

Schema marker: `san_csv_1.0`.

```text
schema_version,device_id,session_id,sample_seq,timestamp_utc,uptime_ms,state,rpm,voltage_v,probe_temp_c,latitude_deg,longitude_deg,gps_speed_kph,gps_fix,satellites,hdop,health_flags
```

The synthetic example retains those headers but leaves location fields blank. `voltage_v` is the safe ADC bench measurement with the original divider ratio 1.0; it must not be relabelled vehicle battery voltage. Blank UTC is legitimate when the hardware lacks a valid reference. Keep uptime and sequence rather than inventing a wall clock.

The OctaneOS workshop importer requires `vehicle_id`, `schema_version=1.0`, `gps_speed_kmh`, `battery_v` and `ambient_temp_c` among its fields. A future adapter must define identity, time basis, channel calibration and missing-data behavior. Direct interoperability is not implemented in this workspace.
