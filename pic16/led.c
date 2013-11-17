/*
 * pic16/led.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"

#include "../platform/led.h"

void led_init(void) {
	LED0_TRIS = 0; //configure 1st led pin as output (yellow)
	LED1_TRIS = 0; //configure 2nd led pin as output (red)
	LED2_TRIS = 0; //configure 3rd led pin as output (red)
}

BOOL led_get(enum led_e led) {
	switch (led) {
	case LED0:
		return LED0_IO != 0u;
	case LED1:
		return LED1_IO != 0u;
	case LED2:
		return LED2_IO != 0u;
	}
}

void led_set(enum led_e led, BOOL state) {
	switch (led) {
	case LED0:
		LED0_IO = (state ? 1u : 0u);
		break;
	case LED1:
		LED1_IO = (state ? 1u : 0u);
		break;
	case LED2:
		LED2_IO = (state ? 1u : 0u);
		break;
	}
}

void led_set_all(BOOL state) {
	LED0_IO = LED1_IO = LED2_IO = (state ? 1u : 0u);
}

void led_toggle(enum led_e led) {
	switch (led) {
	case LED0:
		LED0_IO ^= 1;
		break;
	case LED1:
		LED1_IO ^= 1;
		break;
	case LED2:
		LED2_IO ^= 1;
		break;
	}
}

void led_toggle_all() {
	LED0_IO ^= 1;
	LED1_IO ^= 1;
	LED2_IO ^= 1;
}
