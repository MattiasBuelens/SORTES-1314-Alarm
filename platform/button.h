/*
 * platform/button.h
 */

#ifndef PLATFORM_BUTTON_H_
#define PLATFORM_BUTTON_H_

#include "../Include/GenericTypeDefs.h"

enum button_e {
	button0, button1
};

void button_init(void);
void button_handle_interrupt(void);

// TODO Remove
BOOL button0_down(void);
BOOL button1_down(void);

typedef void (*BUTTON_HANDLER)();
void button_set_handler(enum button_e button, BUTTON_HANDLER handler);

#endif /* PLATFORM_BUTTON_H_ */
