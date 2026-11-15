# Validator characterization

The host suite exercises the recovered validator without claiming firmware compilation or hardware performance. It checks the synthetic reference, sequence gaps, reversed uptime, mixed sessions, wrong schema and incorrect sampling rate. Negative cases must return failure. Fixture values are generated and contain no locations.

Known coverage boundary: the original validator performs structural checks. It does not reject every non-finite sensor value, verify physical units/calibration or prove UTC correctness. These are recorded limitations rather than hidden as passing integration claims.
