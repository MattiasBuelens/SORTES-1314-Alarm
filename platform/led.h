/*
 * platform/led.h
 */

#ifndef PLATFORM_LED_H_
#define PLATFORM_LED_H_

#include "../Include/GenericTypeDefs.h"

enum led_e {
	LED0, LED1, LED2
};

void led_init(void);
BOOL led_get(enum led_e led);
void led_set(enum led_e led, BOOL state);
void led_set_all(BOOL state);
void led_toggle(enum led_e led);
void led_toggle_all();

#endif /* PLATFORM_LED_H_ */
