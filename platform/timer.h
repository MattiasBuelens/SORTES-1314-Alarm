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
void timer_restart(void);

BOOL timer_is_enabled(void);
void timer_set_enabled(BOOL enabled);
BOOL timer_is_repeating();
void timer_set_repeating(BOOL repeating);
void timer_set_handler(TIMER_HANDLER handler);
//void timer_set_scale(WORD scale);
//void timer_set_overflows(WORD nb_overflows);
void timer_set_timeout(WORD milliseconds);

#endif /* PLATFORM_TIMER_H_ */
