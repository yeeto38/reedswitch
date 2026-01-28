#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define REEDPIN GPIO_NUM_0 // we are using d10 (gpio10) to the S (source?) pin on the reed switch. swap as needed
// pin 2345 usable for ext1 but i only want ext0 here so i dont need to use 2345
#define MASK 1ULL << REEDPIN
#define RTAG "REEDSWITCH"
// RTC_DATA_ATTR int bootcount = 0;

extern "C" void app_main() { // apparentally we only recognize app_main if its in c???
  // ++bootcount;
  // ESP_LOGI(RTAG, "Boot #%d", bootcount);
  
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
  if (reason == ESP_SLEEP_WAKEUP_GPIO) {
    ESP_LOGI(RTAG, "Woke up from GPIO (reed switch)"); // this is what we want to see
  } else {
    ESP_LOGI(RTAG, "Hmm. That's weird. %d", reason);
  }
  
  gpio_config_t io_conf = {
    .pin_bit_mask = MASK,           // bit mask to choose the right pin to measure
    .mode = GPIO_MODE_INPUT,        // set gpio to input
    .pull_up_en = GPIO_PULLUP_ENABLE,   // enable pull up
    .pull_down_en = GPIO_PULLDOWN_DISABLE, // disable pull down
    .intr_type = GPIO_INTR_DISABLE      // disable interrupting (?)
  };
  gpio_config(&io_conf);
  esp_deep_sleep_enable_gpio_wakeup(MASK, ESP_GPIO_WAKEUP_GPIO_LOW);

  // esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); // genuinely cannot get this to work. help on bootcount ig

  ESP_LOGI(RTAG, "Good Night...");
  ESP_LOGI(RTAG, "GPIO0 level: %d", gpio_get_level(REEDPIN));
  fflush(stdout);
  esp_deep_sleep_start();
}
