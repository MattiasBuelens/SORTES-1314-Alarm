/*
 * platform/button.h
 */

#ifndef PLATFORM_BUTTON_H_
#define PLATFORM_BUTTON_H_

#include "../Include/GenericTypeDefs.h"

void button_init(void);

BOOL button0_down(void);
BOOL button1_down(void);

typedef void (*BUTTON_HANDLER)();

void button_set_handler(BYTE buttonNumber, BUTTON_HANDLER handler);

#endif /* PLATFORM_BUTTON_H_ */
