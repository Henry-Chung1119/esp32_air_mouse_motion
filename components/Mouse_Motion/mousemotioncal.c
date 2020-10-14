#include "mousemotioncal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

// #include "button.h"
#define TAG "BUTTON"

// typedef struct {
// 	uint8_t pin;
//     bool inverted;
// 	uint16_t history;
//     uint64_t down_time;
// } debounce_t;
// int pin_count = -1;
// debounce_t * debounce;
// QueueHandle_t * queue;

int8_t horizontal_motion_cal(float x, float y, float z) {
    int speedh = 0;
    // if(abs(z-lastz) > 1.0) {
        speedh = (int)(-z*40);
    // }
    return speedh;
}
int8_t vertical_motion_cal(float x, float y, float z) {
    int speedv = 0;
    // if(abs(y-lasty) > 1.0) {
        speedv = (int)(y*40);
    // }
    return speedv;
    // return (int)(y*40);
}

// static void update_button(debounce_t *d) {
//     // gpio_get_level(d->pin) == 0
//     // d->history = d->history << 1
//     // gpio_get_level(d->pin) == 1
//     // d->history = d->history 
//     d->history = (d->history << 1) | gpio_get_level(d->pin);
    
// }

// #define MASK   0b1111000000111111
// static bool button_rose(debounce_t *d) {
//     if ((d->history & MASK) == 0b0000000000111111) {
//         // d->history = 0b0000000000000000
//         d->history = 0xffff;
//         return 1;
//     }
//     return 0;
// }
// // button_fell FALLING EDGE
// static bool button_fell(debounce_t *d) {
//     if ((d->history & MASK) == 0b1111000000000000) {
//         // d->history = 0b1111111111000000
//         d->history = 0x0000;
//         return 1;
//     }
//     return 0;
// }
// static bool button_down(debounce_t *d) {
//     if (d->inverted) return button_fell(d);
//     return button_rose(d);
// }
// static bool button_up(debounce_t *d) {
//     if (d->inverted) return button_rose(d);
//     return button_fell(d);
// }

// #define LONG_PRESS_DURATION (2000)

// static uint32_t millis() {
//     return esp_timer_get_time() / 1000;
// }

// static void send_event(debounce_t db, int ev) {
//     button_event_t event = {
//         .pin = db.pin,
//         .event = ev,
//     };
//     xQueueSend(queue, &event, portMAX_DELAY);
// }


// // debounce[0].pin = 2;
// // debounce[1].pin = 4;
// // debounce[idx].down_time = 0;
// // debounce[idx].inverted = true;
// // debounce[idx].history = 0xffff;

// static void button_task(void *pvParameter)
// {
//     while (1) {
//         for (int idx=0; idx<pin_count; idx++) {
//             update_button(&debounce[idx]);
//             if (debounce[idx].down_time && (millis() - debounce[idx].down_time > LONG_PRESS_DURATION)) {
//                 debounce[idx].down_time = 0;
//                 ESP_LOGI(TAG, "%d LONG", debounce[idx].pin);
//                 int i=0;
//                 while (!button_up(&debounce[idx])) {
//                     if (!i) send_event(debounce[idx], BUTTON_DOWN);
//                     i++;
//                     if (i>=5) i=0;
//                     vTaskDelay(10/portTICK_PERIOD_MS);
//                     update_button(&debounce[idx]);
//                 }
//                 ESP_LOGI(TAG, "%d UP", debounce[idx].pin);
//                 send_event(debounce[idx], BUTTON_UP);
//             } else if (button_down(&debounce[idx])) {
//                 debounce[idx].down_time = millis();
//                 ESP_LOGI(TAG, "%d DOWN", debounce[idx].pin);
//                 send_event(debounce[idx], BUTTON_DOWN);
//             } else if (button_up(&debounce[idx])) {
//                 debounce[idx].down_time = 0;
//                 ESP_LOGI(TAG, "%d UP", debounce[idx].pin);
//                 send_event(debounce[idx], BUTTON_UP);
//             }
//         }
//         vTaskDelay(10/portTICK_PERIOD_MS);
//     }
// }

// QueueHandle_t * button_init(unsigned long long pin_select) {
//     return pulled_button_init(pin_select, GPIO_FLOATING);
// }


// QueueHandle_t * pulled_button_init(unsigned long long pin_select, gpio_pull_mode_t pull_mode)
// {
//     if (pin_count != -1) {
//         ESP_LOGI(TAG, "Already initialized");
//         return NULL;
//     }

//     // Configure the pins
//     gpio_config_t io_conf;
//     io_conf.mode = GPIO_MODE_INPUT;
//     io_conf.pull_up_en = (pull_mode == GPIO_PULLUP_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN);
//     io_conf.pull_down_en = (pull_mode == GPIO_PULLDOWN_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN);;
//     io_conf.pin_bit_mask = pin_select;
//     gpio_config(&io_conf);

//     // Scan the pin map to determine number of pins

//     pin_count = 0;
//     for (int pin=0; pin<=39; pin++) {
//         if ((1ULL<<pin) & pin_select) {
//             pin_count++;
//         }
//     }
//     printf("pin_count=%d\n",pin_count);
//     // pin_count=2
//     // Initialize global state and queue
//     debounce = calloc(pin_count, sizeof(debounce_t));
//     queue = xQueueCreate(4, sizeof(button_event_t));
//     // 4 => The maximum number of items the queue can hold at any one time.

//     // Scan the pin map to determine each pin number, populate the state
//     uint32_t idx = 0;
//     for (int pin=0; pin<=39; pin++) {
//         if ((1ULL<<pin) & pin_select) {
//             // pin = 2,4
//             ESP_LOGI(TAG, "Registering button input: %d", pin);
//             debounce[idx].pin = pin;
//             debounce[idx].down_time = 0;
//             debounce[idx].inverted = true;
//             if (debounce[idx].inverted) debounce[idx].history = 0xffff;
//             idx++;
//         }
//     }

//     // Spawn a task to monitor the pins
//     xTaskCreate(&button_task, "button_task", 4096, NULL, 10, NULL);

//     return queue;
// }