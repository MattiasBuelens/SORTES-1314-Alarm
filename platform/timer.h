/*
 * platform/timer.h
 */

#ifndef PLATFORM_TIMER_H_
#define PLATFORM_TIMER_H_

#include "../Include/GenericTypeDefs.h"

typedef void (*TIMER_HANDLER)();

void timer_init(void);
void timer_handle_interrupt(void);
void timer_reset(void);

BOOL timer_is_active(void);
void timer_set_active(BOOL active);
void timer_set_handler(TIMER_HANDLER handler);
void timer_set_timeout(unsigned int nb_overflows);

#endif /* PLATFORM_TIMER_H_ */
