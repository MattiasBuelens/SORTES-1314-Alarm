/*
 * platform/display.h
 */

#ifndef PLATFORM_DISPLAY_H_
#define PLATFORM_DISPLAY_H_

#include "../Include/GenericTypeDefs.h"

void display_init(void);

void display_char(BYTE line, BYTE column, char c);
void display_string(BYTE line, BYTE column, const char* text);
void display_clear();

#endif /* PLATFORM_DISPLAY_H_ */
