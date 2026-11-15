# Bench hardware boundary

The selected source is the 19 August bench package. Its provisional 30-pin configuration is in `firmware/platformio/include/config.h`. The older 10 August manual used a 38-pin description. Neither proves the exact currently held board or the pin markings on a generic breakout.

Bench only: laptop USB power; ADS1115 powered from 3.3 V, A0 between ground and its supply rail; DS18B20 probe named by measurement location; GPIO34 fed only by a conditioned 0–3.3 V bench pulse. The ADS gain range does not permit overvoltage above VDD. GPIO34 is input-only and has no internal pull resistor.

Vehicle power conditioning, transients, grounding, fusing, harness/connector characterization and protection fault tests are future design work. No validated vehicle schematic, PCB, harness or automotive-qualified BOM is supplied here. Do not infer their completion from a module list.
