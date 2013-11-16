/*
 * platform/led.h
 */

#ifndef PLATFORM_LED_H_
#define PLATFORM_LED_H_

#include "../Include/GenericTypeDefs.h"

void led_init(void);
BOOL led_get(int ledId);
void led_set(int ledId, BOOL state);
void led_set_all(BOOL state);
void led_toggle(int ledId);
void led_toggle_all();

#endif /* PLATFORM_LED_H_ */
