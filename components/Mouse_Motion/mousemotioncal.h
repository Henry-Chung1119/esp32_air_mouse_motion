#ifndef _MOUSEMOTIONCAL_H_
#define _MOUSEMOTIONCAL_H_
#include <stdint.h>
// #include "freertos/queue.h"
// #include "driver/gpio.h"


// C++ call C
#ifdef __cplusplus
extern "C" {
#endif

// #define PIN_BIT(x) (1ULL<<x)
// #define BUTTON_DOWN (1)
// #define BUTTON_UP (2)

// typedef struct {
// 	uint8_t pin;
//     uint8_t event;
// } button_event_t;

// QueueHandle_t * button_init(unsigned long long pin_select);
// QueueHandle_t * pulled_button_init(unsigned long long pin_select, gpio_pull_mode_t pull_mode);
// int8_t horizontal_motion_cal(float x, float y, float z, float lastx, float lasty, float lastz);
// int8_t vertical_motion_cal(float x, float y, float z, float lastx, float lasty, float lastz);
int8_t horizontal_motion_cal(float x, float y, float z);
int8_t vertical_motion_cal(float x, float y, float z);
#ifdef __cplusplus
}
#endif

#endif