/*
 * pic16/timer.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"

#include "../platform/timer.h"

void timer_init(void) {
	// Use 8-bit operation
	T0CONbits.T08BIT = 1;
	// Use internal instruction cycle clock
	T0CONbits.T0CS = 0;
	// Don't use the prescaler
	T0CONbits.PSA = 1;
	// Enable interrupts
	INTCONbits.TMR0IE = 1;
}

void timer_handle_interrupt(void) {

}

void timer_reset(void) {

}

BOOL timer_is_active(void) {

}

void timer_set_active(BOOL active) {

}

void timer_set_handler(TIMER_HANDLER handler) {

}

void timer_set_timeout(unsigned int nb_overflows) {

}
