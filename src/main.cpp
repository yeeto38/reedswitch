#include <Arduino.h>
#include "reed-switch.h"
#include "wifi-server.h"

// ===== Your setup =====
// You said: Board pin D3 corresponds to GPIO4 in code
static const int REED_GPIO = 4;

// AP credentials
static const char* AP_SSID = "HiveNode-ESP32";
static const char* AP_PASS = "hivepassword"; // 8+ chars required

ReedSwitch reed(REED_GPIO, /*activeLow=*/true, /*debounceMs=*/30);
WifiServerAP wifi(AP_SSID, AP_PASS);

void setup() {
  delay(2000);
  Serial.begin(115200);

  // If you rely on USB CDC, you can optionally wait for Serial:
  unsigned long start = millis();
  while (!Serial && (millis() - start < 2000)) { delay(10); }

  Serial.println("\n[BOOT] Starting Hive Node (Reed + AP Server)");

  // If you're using a 3-pin reed *module* with V/G/S:
  // It usually already has pull resistors, but INPUT_PULLUP is often fine too.
  // If you KNOW the module drives the output strongly, you can set false and use INPUT instead.
  reed.enableSerialLogging(true);
  reed.begin(/*useInternalPullup=*/true);

  wifi.begin(&reed);

  Serial.println("[BOOT] Ready. Connect to AP and open http://192.168.4.1/");
}

void loop() {
  reed.update();     // handle debounce + timing + change prints
  wifi.loop();       // handle web requests
}
