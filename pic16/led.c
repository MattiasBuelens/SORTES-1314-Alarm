/*
 * pic16/led.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"

void led_init(void) {
	LED0_TRIS = 0; //configure 1st led pin as output (yellow)
	LED1_TRIS = 0; //configure 2nd led pin as output (red)
	LED2_TRIS = 0; //configure 3rd led pin as output (red)
}

BOOL led_get(int ledId) {
	switch (ledId) {
	case 0:
		return LED0_IO != 0u;
	case 1:
		return LED1_IO != 0u;
	case 2:
		return LED2_IO != 0u;
	default:
		return FALSE;
	}
}

void led_set(int ledId, BOOL state) {
	switch (ledId) {
	case 0:
		LED0_IO = (state ? 1u : 0u);
		break;
	case 1:
		LED1_IO = (state ? 1u : 0u);
		break;
	case 2:
		LED2_IO = (state ? 1u : 0u);
		break;
	}
}

void led_set_all(BOOL state) {
	LED0_IO = LED1_IO = LED2_IO = (state ? 1u : 0u);
}

void led_toggle(int ledId) {
	switch (ledId) {
	case 0:
		LED0_IO ^= 1;
		break;
	case 1:
		LED1_IO ^= 1;
		break;
	case 2:
		LED2_IO ^= 1;
		break;
	}
}

void led_toggle_all() {
	LED0_IO ^= 1;
	LED1_IO ^= 1;
	LED2_IO ^= 1;
}
