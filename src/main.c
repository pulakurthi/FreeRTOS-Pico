#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"


#define LED_DELAY_MS 1000

QueueHandle_t blinkQueue;

// Perform initialisation
int pico_led_init(void) {
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
}

// Turn the led on or off
void pico_set_led(bool led_on) {
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}

void vBlinkTask() {
    int counter = 0;
    for (;;) {
        pico_set_led(true);
        //printf("LED ON \n");
        vTaskDelay(pdMS_TO_TICKS(LED_DELAY_MS));
        pico_set_led(false);
        counter++;
        xQueueSend(blinkQueue, &counter, portMAX_DELAY);
        //printf("LED OFF \n");
        vTaskDelay(pdMS_TO_TICKS(LED_DELAY_MS));
    }
}

void vBlinkCount() {
    int counter;
    for (;;) {
        if(xQueueReceive(blinkQueue, &counter, portMAX_DELAY) ==  pdPASS)
        {
            printf("LED Blinked %d Times.\n", counter);
        }
    }
}


int main() {
    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    blinkQueue = xQueueCreate(1, sizeof(int));
    xTaskCreate(vBlinkTask, "Blink Task", 256, NULL, 1, NULL);
    xTaskCreate(vBlinkCount, "Blink Count", 256, NULL, 1, NULL);
    vTaskStartScheduler();
}
