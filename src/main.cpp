#include <Arduino.h>
#include "esp_sleep.h"
#define REEDPIN 0 // we are using d10 (gpio10) to the S (source?) pin on the reed switch. swap as needed
// pin 2345 usable for ext1 but i only want ext0 here so i dont need to use 2345
RTC_DATA_ATTR int bootcount = 0;

void setup() {
  Serial.begin(115200);
  delay(500); // remove during real implementation - only here to allow for usb monitor to initialize
  ++bootcount;
  Serial.println("Boot #" + String(bootcount));
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
  if (reason == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Good morning!");
  } else {
    Serial.println("This isn't right.");
  }
  pinMode(REEDPIN, INPUT_PULLUP); // set it to pullup
  esp_sleep_enable_ext0_wakeup((gpio_num_t)REEDPIN, 0); // wake up when state 0 (LOW)
  Serial.println("Good night...");
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
  // this will never run!
}