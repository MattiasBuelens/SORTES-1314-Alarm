/*
 * pic16/button.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"

#include "../platform/button.h"

BUTTON_HANDLER button0_handler = NULL;
BUTTON_HANDLER button1_handler = NULL;

BOOL button_reset(enum button_e button);

void button_init(void) {
	BUTTON0_TRIS = 1;			// configure button0 as input
	BUTTON1_TRIS = 1;			// configure button1 as input

	INTCONbits.GIE = 1;			// enable interrupts
	RCONbits.IPEN = 1;			// enable interrupts priority levels

	// Button 0
	INTCON3bits.INT1P = 0;		// connect INT1 interrupt to low priority
	INTCON2bits.INTEDG1 = 0;	// INT1 interrupts on falling edge
	INTCON3bits.INT1E = 1;		// enable INT1 interrupt
	INTCON3bits.INT1F = 0;		// clear INT1 flag

	// Button 1
	INTCON3bits.INT3P = 0;		// connect INT3 interrupt to low priority
	INTCON2bits.INTEDG3 = 0;	// INT3 interrupts on falling edge
	INTCON3bits.INT3E = 1;		// enable INT3 interrupt
	INTCON3bits.INT3F = 0;		// clear INT1 flag
}

BOOL button0_down() {
	return BUTTON0_IO == 0u;
}

BOOL button1_down() {
	return BUTTON1_IO == 0u;
}

void button_set_handler(enum button_e button, BUTTON_HANDLER handler) {
	switch (button) {
	case button0:
		button0_handler = handler;
		break;
	case button1:
		button1_handler = handler;
		break;
	}
}

void button_handle_interrupt() {
	if (button_reset(button0)) {
		if (button0_handler)
			button0_handler();
	}
	if (button_reset(button1)) {
		if (button1_handler)
			button1_handler();
	}
}

BOOL button_reset(enum button_e button) {
	// clear interrupt bits
	switch (button) {
	case button0:
		if (INTCON3bits.INT1F == 1) {
			INTCON3bits.INT1F = 0;
			return TRUE;
		}
		break;
	case button1:
		if (INTCON3bits.INT3F == 1) {
			INTCON3bits.INT3F = 0;
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
}
