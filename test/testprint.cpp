#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    while (true) {
        printf("Hello World!\n");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}