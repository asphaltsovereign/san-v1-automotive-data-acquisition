# Run SAN-V1 on Windows with Arduino IDE

Complete the stages in order. Do not connect the motorcycle, buck converter,
ADS1115, GPS, SD board, or temperature probe during Stage 1.

## Stage 1 — Install Arduino IDE and prove the ESP32

### 1. Install Arduino IDE

1. Download the latest **Arduino IDE 2.x Windows Win 10 and newer, 64-bit**
   installer from: <https://www.arduino.cc/en/software>
2. Run the downloaded `.exe`.
3. Accept the driver prompts during installation.
4. Open Arduino IDE.

Do not install the legacy Arduino IDE 1.8.19.

### 2. Add ESP32 support

1. Open **File > Preferences**.
2. In **Additional Boards Manager URLs**, add:

   ```text
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```

3. Click **OK**.
4. Open **Tools > Board > Boards Manager**.
5. Search for `esp32`.
6. Install **esp32 by Espressif Systems**. Use the latest stable release.
7. Wait until installation finishes; it can take several minutes.

### 3. Connect the bare board

1. Remove every jumper and module from the ESP32.
2. Connect it directly to the laptop using a data-capable USB cable.
3. Wait 20–30 seconds for Windows to install the USB serial device.
4. In Arduino IDE, open **Tools > Port** and note the new `COM` port.

If no new port appears:

- Try a different USB cable; many cables supply power but carry no data.
- Try a different laptop USB port without a hub.
- Open Windows **Device Manager** and inspect **Ports (COM & LPT)** and
  **Other devices**.
- Photograph the small USB-to-serial chip near the USB connector. Typical
  boards use CP2102/CP210x or CH340/CH341, but the marking must be confirmed
  before installing a separate driver.

### 4. Select conservative board settings

Use:

| Arduino menu | Setting |
|---|---|
| Tools > Board > esp32 | **ESP32 Dev Module** |
| Tools > Port | The new ESP32 COM port |
| Upload Speed | **115200** |
| CPU Frequency | **240 MHz (WiFi/BT)** |
| Flash Frequency | **80 MHz** |
| Flash Mode | **QIO** |
| Partition Scheme | **Default 4MB with spiffs** |
| Core Debug Level | **None** |
| PSRAM | **Disabled** |

These are provisional generic ESP32-WROOM settings. If the metal module says
something other than ESP32-WROOM-32/WROOM-32D/WROOM-32E, stop and send a photo.

### 5. Upload the USB test

1. Extract the complete package first.
2. In Arduino IDE, select **File > Open**.
3. Open:

   ```text
   01_ESP32_USB_TEST\01_ESP32_USB_TEST.ino
   ```

4. Click **Verify** (check mark).
5. Click **Upload** (right arrow).
6. Open **Tools > Serial Monitor** and select **115200 baud**.

Expected output starts with:

```text
SAN-V1 ESP32 USB TEST: PASS
Chip model: ESP32...
SAN alive: 0 s
```

If upload stops at `Connecting...`:

1. Press and hold the board's **BOOT** button.
2. Click **Upload**.
3. When `Writing at...` appears, release **BOOT**.
4. If needed, briefly press **EN/RST** after upload.

Do not continue until the serial output is readable and the chip/flash details
have been photographed or copied into the bench record.

## Stage 2 — Install the SAN-V1 libraries

Open **Tools > Manage Libraries** and install these exact libraries:

| Search text | Install |
|---|---|
| `Adafruit ADS1X15` | **Adafruit ADS1X15 by Adafruit** |
| `OneWire` | **OneWire by Paul Stoffregen** |
| `DallasTemperature` | **DallasTemperature by Miles Burton** |
| `TinyGPSPlus` | **TinyGPSPlus by Mikal Hart** |

When Arduino asks to install dependencies such as **Adafruit BusIO**, choose
**Install All**. `Wire`, `SPI`, `SD`, and `FS` arrive with the ESP32 board core;
do not install similarly named random replacements.

## Stage 3 — Compile and upload SAN-V1

Keep all external modules disconnected for the first firmware upload.

1. Open:

   ```text
   SAN_V1_Arduino\SAN_V1_Arduino.ino
   ```

2. Confirm the `san_firmware.cpp` and `config.h` tabs appear beside the sketch.
3. Confirm **ESP32 Dev Module** and the correct COM port remain selected.
4. Click **Verify**.
5. Save the full error text if compilation fails.
6. If verification succeeds, click **Upload**.
7. Open Serial Monitor at **115200 baud**.

With no sensors connected, this output is expected:

```text
SAN-V1 bench firmware boot
Device: SAN-xxxxxx
ADS1115: FAIL
DS18B20: FAIL
microSD: FAIL
GPS fix may require outdoor sky view.
```

The repeated CSV rows and `DEGRADED` state prove that the firmware is running.
Sensor failures are correct because Stage 3 deliberately uses the bare board.

## Stage 4 — Stop point for today

Today's software gate passes when all four are true:

- [ ] Arduino IDE 2.x installed
- [ ] `esp32 by Espressif Systems` installed
- [ ] USB test uploaded and serial output captured
- [ ] SAN-V1 sketch verifies and uploads, showing `DEGRADED` CSV output

Do not connect all modules at once. The next physical stage is ADS1115 only,
after front-and-back board photographs confirm its pins and voltage design.

## Troubleshooting evidence to send

If anything fails, send one uncropped screenshot containing:

- the complete red error/output panel;
- selected board and COM port;
- the last 20–30 console lines;
- a clear photo of the ESP32 front and back;
- a close-up of the USB-to-serial chip and metal ESP32 module marking.

Do not paraphrase the error; the exact first error line identifies the cause.
