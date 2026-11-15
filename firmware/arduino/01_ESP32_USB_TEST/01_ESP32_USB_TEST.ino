#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(800);

  Serial.println();
  Serial.println("================================");
  Serial.println("SAN-V1 ESP32 USB TEST: PASS");
  Serial.println("================================");
  Serial.printf("Chip model: %s\n", ESP.getChipModel());
  Serial.printf("Chip revision: %u\n", ESP.getChipRevision());
  Serial.printf("CPU frequency: %u MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash size: %u bytes\n", ESP.getFlashChipSize());
  Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
  Serial.println("If this text is readable, USB upload and serial monitoring work.");
}

void loop() {
  static uint32_t counter = 0;
  Serial.printf("SAN alive: %lu s\n", static_cast<unsigned long>(counter++));
  delay(1000);
}

