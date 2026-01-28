#include <Arduino.h>
// #define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex — removed because GPIO2 is valid

RTC_DATA_ATTR int bootCount = 0;

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup(){
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Use D2 (GPIO2) instead of D1
  esp_deep_sleep_enable_gpio_wakeup(BIT(2), ESP_GPIO_WAKEUP_GPIO_LOW);

  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop(){
  //This is not going to be called
}


// #include <Arduino.h>

// // Candidate pins for reed switch on XIAO ESP32-C3
// const int pins[] = {0, 1, 2, 3}; // D0-D3
// const int numPins = sizeof(pins) / sizeof(pins[0]);

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("Pin scanner started");

//   // Configure pins as input with pullup
//   for (int i = 0; i < numPins; i++) {
//     pinMode(pins[i], INPUT_PULLUP);
//   }
// }

// void loop() {
//   for (int i = 0; i < numPins; i++) {
//     int level = digitalRead(pins[i]);
//     Serial.print("GPIO");
//     Serial.print(pins[i]);
//     Serial.print(": ");
//     Serial.print(level);
//     if (level == LOW) Serial.print(" <-- REED CLOSED");
//     Serial.println();
//   }
//   Serial.println("------");
//   delay(500); // scan twice per second
// }