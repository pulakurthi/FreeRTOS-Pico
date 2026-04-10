#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"
#include "hardware/gpio.h"
#include <semphr.h>

#define INPUT_PIN 15

SemaphoreHandle_t xGpioSemaphore;

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

// isr
void gpio_isr_callback(uint gpio, uint32_t events){
   if (gpio == INPUT_PIN){
       BaseType_t xHigherPriorityTaskWoken = pdFALSE;




       xSemaphoreGiveFromISR(xGpioSemaphore, &xHigherPriorityTaskWoken);
       // If the woken/unblocked task priority is higher than the interrupted task
       // do not continue running the previousy interrupted take, run unblocked task
       portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
}

// GPIO Init
void gpio_input_init(){
   gpio_init(INPUT_PIN);
   gpio_set_dir(INPUT_PIN, GPIO_IN);
   gpio_pull_up(INPUT_PIN);
 
   gpio_set_irq_enabled_with_callback(INPUT_PIN,
       GPIO_IRQ_EDGE_FALL, true, &gpio_isr_callback);
}

void vLedToggle() {
   bool led_state = false;
   // because INCLUDE_vTaskSuspend is 1 we block this task
   // indefinitely untill an interupt has occured
   TickType_t ticksToWait = portMAX_DELAY;
   for (;;) {
       if(xSemaphoreTake(xGpioSemaphore, ticksToWait) == pdPASS)
       {
           led_state = !led_state;
           pico_set_led(led_state);
           //DEBOUNCE
           vTaskDelay(pdMS_TO_TICKS(500));
           // returns immediatly, flushes any interupts that have occored
           xSemaphoreTake(xGpioSemaphore, 0);

       }
   }
}

int main() {
   stdio_init_all();
   int rc = pico_led_init();
   hard_assert(rc == PICO_OK);

   gpio_input_init();

   xGpioSemaphore = xSemaphoreCreateBinary();

   xTaskCreate(vLedToggle, "LED Toggle", 256, NULL, 1, NULL);
   vTaskStartScheduler();

   // should never reach here
   for(;;);
}
