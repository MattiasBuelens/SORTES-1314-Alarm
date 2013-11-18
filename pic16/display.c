/*
 * pic16/display.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"
#include "../Include/LCDBlocking.h"

#include "../platform/display.h"

BOOL displayChanged = FALSE;

void display_init(void) {
	LCDInit();
}

BYTE display_get_position(BYTE line, BYTE column) {
	return line * 16 + column;
}

void display_char(BYTE line, BYTE column, char c) {
	BYTE pos = display_get_position(line, column);
	LCDText[pos] = c;
	displayChanged = TRUE;
}

void display_string(BYTE line, BYTE column, const char* text) {
	BYTE pos = display_get_position(line, column);
	BYTE l = strlen(text); // number of actual chars in the string
	BYTE max = 32 - pos; // available space on the lcd
	char *d = (char*) &LCDText[pos];
	const char *s = text;
	size_t n = (l < max) ? l : max;
	// Copy as many bytes as will fit
	if (n != 0) {
		while (n-- != 0)
			*d++ = *s++;
	}
	displayChanged = TRUE;
}

void display_clear() {
	LCDErase();
}

void display_update(){
	displayChanged ^=1;
	LCDUpdate();
}

BOOL display_changed(){
	return displayChanged;
}

