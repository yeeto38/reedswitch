#include <Arduino.h>

// Choose a safe GPIO. GPIO 5 is commonly OK on many ESP32-S3 dev boards.
// If you already use GPIO 5 for something else, pick another free GPIO.
static const int REED_PIN = 4;

// Debounce settings
static const uint32_t DEBOUNCE_MS = 30;

static int stableState = HIGH;          // last accepted stable reading
static int lastReading = HIGH;          // last raw reading
static uint32_t lastChangeTime = 0;     // when raw reading last changed

// Helper: interpret reading into ON/OFF
// With pull-up wiring: LOW means switch closed (magnet present) => ON
static const char* stateToText(int reading) {
  return (reading == LOW) ? "ON (magnet present)" : "OFF (magnet not present)";
}

void setup() {
  delay(2000);
  Serial.begin(115200);

  // Internal pull-up: pin reads HIGH when switch open, LOW when closed to GND
  pinMode(REED_PIN, INPUT_PULLUP);

  // Initialize states
  stableState = digitalRead(REED_PIN);
  lastReading = stableState;
  lastChangeTime = millis();

  Serial.println("Reed switch test started.");
  Serial.print("Initial state: ");
  Serial.println(stateToText(stableState));
}

void loop() {
  int reading = digitalRead(REED_PIN);

  // If the raw reading changed, reset debounce timer
  if (reading != lastReading) {
    lastReading = reading;
    lastChangeTime = millis();
  }

  // If reading has been stable long enough, accept it as the new stable state
  if ((millis() - lastChangeTime) >= DEBOUNCE_MS) {
    if (stableState != lastReading) {
      stableState = lastReading;

      // Print ONLY on state changes
      Serial.print("Reed state changed: ");
      Serial.println(stateToText(stableState));
    }
  }
}
