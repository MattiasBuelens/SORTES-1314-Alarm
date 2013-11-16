/*
 * pic16/button.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"

#include "../platform/button.h"

void button_init(void) {
	BUTTON0_TRIS = 1; //configure button0 as input
	BUTTON1_TRIS = 1; //configure button1 as input
}

BOOL button0_down() {
	return BUTTON0_IO == 0u;
}

BOOL button1_down() {
	return BUTTON1_IO == 0u;
}
