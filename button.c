/*
 * button.c
 */

#include "time.h"
#include "platform/button.h"
#include "button.h"

BOOL button0_dblpressed(void) {
	static long lastTimePressed = -1;
	long currentTime, timeBetweenPress;
	static BOOL wasPressed = FALSE;

	if (!button0_pressed()) {
		return FALSE;
	}
	currentTime = uptime_in_seconds();
	if (lastTimePressed < 0) {
		lastTimePressed = currentTime;
	}
	timeBetweenPress = currentTime - lastTimePressed;
	if (timeBetweenPress < 0 || timeBetweenPress > 20) {
		return FALSE;
	}
	if (wasPressed && timeBetweenPress > 2) {
		lastTimePressed = currentTime;
		return TRUE;
	}
	wasPressed = button0_pressed();
	return FALSE;
}

BOOL button1_dblpressed(void) {
	static long lastTimePressed = -1;
	long currentTime, timeBetweenPress;
	static BOOL wasPressed = FALSE;

	if (!button1_pressed()) {
		return FALSE;
	}
	currentTime = uptime_in_seconds();
	if (lastTimePressed < 0) {
		lastTimePressed = currentTime;
	}
	timeBetweenPress = currentTime - lastTimePressed;
	if (timeBetweenPress < 0 || timeBetweenPress > 20) {
		return FALSE;
	}
	if (wasPressed && timeBetweenPress > 2) {
		lastTimePressed = currentTime;
		return TRUE;
	}
	wasPressed = button1_pressed();
	return FALSE;
}
