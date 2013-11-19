/*
 * pic16/timer.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"
#include <pic18fregs.h>

#include "../platform/timer.h"

#define LOW(a)     (a & 0xFF)
#define HIGH(a)    ((a>>8) & 0xFF)
#define TIMER_CYCLES_PER_SECOND 6249248

BOOL timer_repeating = FALSE;
WORD timer_overflows = 0xFFFF;
TIMER_HANDLER timer_handler = NULL;
WORD nb_remaining = 0;

void timer_init(void) {
	RCONbits.IPEN = 1;			// enable interrupts priority levels
	INTCON2bits.TMR0IP = 1;		// set timer interrupt as high priority
	INTCONbits.GIE = 1;			// enable high priority interrupts

	timer_set_enabled(FALSE);	// disable timer0
	T0CONbits.T08BIT = 0;		// use timer0 16-bit counter
	T0CONbits.T0CS = 0;			// use timer0 instruction cycle clock
	timer_set_scale(1);			// disable prescaler

	INTCONbits.TMR0IE = 1;		// enable external interrupt procedure
	INTCONbits.TMR0IF = 0;
}

BOOL timer_is_interrupted() {
	return INTCONbits.TMR0IF == 1;
}

void timer_handle_interrupt(void) {
	if (timer_is_interrupted()) {		// timer0 overflowed
		if (timer_is_repeating()) {
			// Repeating, restart timer
			timer_restart();
		}
		if (timer_handler) {
			timer_handler();			// call handler
		}
		else {
			// Non-repeating, disable timer
			timer_set_enabled(FALSE);
		}
		timer_reset();
	}
}

void timer_reset(void) {
	INTCONbits.TMR0IF = 0;					// clear timer0 overflow bit
}

void timer_restart(void) {
	TMR0H = HIGH(nb_remaining);
	TMR0L = LOW(nb_remaining);// reset timeout
}

BOOL timer_is_enabled(void) {
	return T0CONbits.TMR0ON != 0;			// read timer0 status
}

void timer_set_enabled(BOOL enabled) {
	T0CONbits.TMR0ON = (enabled ? 1 : 0);	// write timer0 status
}

BOOL timer_is_repeating() {
	return timer_repeating;
}

void timer_set_repeating(BOOL repeating) {
	timer_repeating = repeating;
}

void timer_set_handler(TIMER_HANDLER handler) {
	timer_handler = handler;
}

void timer_set_interrupt_time(WORD milliseconds){
	//determine counter overflow amount for intterupt
	WORD nb_overflows = (milliseconds * TIMER_CYCLES_PER_SECOND) /1000;
	//determine prescaler value
	WORD scale = 1;
	while(nb_overflows > 2^16){
		nb_overflows >>= 2;
		scale <<=1;
	}
	timer_set_scale(scale);
	timer_set_overflows(nb_overflows);
	// do something smart with remainder
	// if remainder > 0.5 round up else round down:
	// every 1/remainder time nb_overflows + 1 (if <=0.5) corrects to fast
	// every 1/(1-remainder) time nb_overflows -1 (if>0.5) corrects to slow

}

void timer_set_scale(WORD scale) {
	if (scale == 1u) {
		// Disable timer0 prescaler
		T0CONbits.PSA = 1;
	} else {
		BYTE scaleExponent = 0;
		// Shave off last bit
		scale >>= 1;
		// Calculate exponent of power-of-two
		// That is, (2 ^ scaleExponent) == scale
		while (scale >>= 1)
			++scaleExponent;
		// Enable timer0 prescaler
		T0CONbits.PSA = 0;
		// Set prescaler bits
		T0CONbits.T0PS2 = (scaleExponent >> 2) & 1;
		T0CONbits.T0PS1 = (scaleExponent >> 1) & 1;
		T0CONbits.T0PS0 = (scaleExponent >> 0) & 1;
	}
}

void timer_set_overflows(WORD nb_overflows) {
	nb_remaining = 0x10000 - nb_overflows;
}

