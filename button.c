/*
 * button.c
 */

#include "time.h"
#include "platform/button.h"
#include "button.h"

BOOL button0_pressed(void) {
	static BOOL wasPressed = FALSE;
	if (button0_down()) {
		wasPressed = TRUE;
		return FALSE;
	} else if (wasPressed) {
		wasPressed = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL button1_pressed(void) {
	static BOOL wasPressed = FALSE;
	if (button1_down()) {
		wasPressed = TRUE;
		return FALSE;
	} else if (wasPressed) {
		wasPressed = FALSE;
		return TRUE;
	}
	return FALSE;
}
