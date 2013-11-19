/*
 * timertest.c
 *
 * Timer test program
 */

#define __18F97J60
#define __SDCC__
#define THIS_INCLUDES_THE_MAIN_FUNCTION

#include "Include/HardwareProfile.h"
#include "Include/TCPIP_Stack/Delay.h"

#include <string.h>
#include <stdlib.h>

#include "platform/button.h"
#include "platform/display.h"
#include "platform/led.h"
#include "platform/timer.h"

#define LOW(a)     (a & 0xFF)
#define HIGH(a)    ((a>>8) & 0xFF)

/*
 * This main function is written to test the number of ticks per second of TMR0 which
 * we have to use for this project.
 *
 * According to the specs found here http://ww1.microchip.com/downloads/en/DeviceDoc/39762f.pdf
 * we can trust TIMER1 to which uses a built-in oscillator to have a 32768Hz clock speed.
 * When using the 16-bit mode we will thus have a 2 second interval before it overflows.
 *
 * The strategy consists of running timer1, starting timer0 when timer 1 overflows and
 * counting the number of ticks timer0 encounters between the two overflows of timer 1
 *
 * We know the time between two interrupts of timer 1 and we know timer0 is faster then timer1,
 * at a suspected rate of 10.5 MIPS.
 * So we can count the number of ticks timer0 does between two timer1 interrupts,
 * adding 256 to our tick count for every time it overflows and adding the remainder after the second interrupt.
 * The sum of ticks and the current timer0 count gives us the number of ticks in 2 sec
 *
 * Running this several times gets us an averaged value.
 *
 *
 */

unsigned long ticks = 0;

void setupTimers(void);
void displayResult(unsigned long ticks);

void handleInterrupt(void) {
	BYTE timer0_low = TMR0L;
	BYTE timer0_high = TMR0H;
	WORD timer0_value;

	// Button2 is pressed, run experiment again
	if (INTCON3bits.INT1F == 1) {
		ticks = 0;
		// Start experiment
		setupTimers();
		T1CONbits.TMR1ON = 1;
		// Read the bit
		if (BUTTON2_IO)
			;
		INTCON3bits.INT1F = 0;	// Clear INT1 flag
	}

	// Timer1 triggered an interrupt
	if (PIR1bits.TMR1IF) {
		// immediately enable or disable timer0 to start/stop incrementing said timer
		T0CONbits.TMR0ON ^= 1;
		led_toggle_all();

		// If timer0 gets disabled, we have a result and need to display it
		if (T0CONbits.TMR0ON == 0) {
			T1CONbits.TMR1ON = 0; //disable timer 1 as well.
			timer0_value = (timer0_high << 8) | timer0_low;
			ticks += timer0_value;
			ticks /= 2;
			// Print the result
			displayResult(ticks);
		}
		// Clear interrupt flag
		PIR1bits.TMR1IF = 0;
	}

	// Timer0 triggered an overflow, increase ticks
	if (INTCONbits.TMR0IF == 1) {
		ticks += 0x10000; // 16-bits
		// Clear interrupt flag
		INTCONbits.TMR0IF = 0;
	}
}

void high_isr(void)
__interrupt (1) {
	handleInterrupt();
}

void displayResult(unsigned long ticks) {
	char str[17] = { 0 };
	sprintf(str, "Ticks/s:", ticks);
	display_string(0, 0, str);
	sprintf(str, "%lu", ticks);
	display_string(1, 0, str);
}

void setupTimers() {
	//  Disable timer0 and timer1 for setup
	T0CONbits.TMR0ON = 0;
	T1CONbits.TMR1ON = 0;
	/*
	 * Timer0
	 */
	// Reset timer value (first high word, then low word)
	TMR0H = 0x00000000;
	TMR0L = 0x00000000;
	// Use 16-bit timer
	T0CONbits.T08BIT = 0;
	// Use internal instruction cycle clock
	T0CONbits.T0CS = 0;
	// Don't use the prescaler
	T0CONbits.PSA = 1;
	// Enable interrupts
	INTCONbits.TMR0IE = 1;
	/*
	 * Timer1
	 */
	// Use 16-bit read/write operation
	T1CONbits.RD16 = 1;
	// Reset timer value (first high word, then low word)
	TMR1H = 0x00000000;
	TMR1L = 0x00000000;
	// Don't use the prescaler
	T1CONbits.T1CKPS0 = 0;
	T1CONbits.T1CKPS1 = 0;
	// Use the built-in oscillator!!!
	T1CONbits.T1OSCEN = 1;
	// Don't sync clocks
	T1CONbits.T1SYNC = 1;
	// Use the external clock source (= oscillator)
	T1CONbits.TMR1CS = 1;
	// Enable interrupts
	PIE1bits.TMR1IE = 1;
}

void main(void) {
	RCONbits.IPEN = 1;			// enable interrupts priority levels
	INTCONbits.GIE = 1;			// enable high priority interrupts
	//INTCONbits.PEIE = 1;		// don't know if this is necessary

	INTCON3bits.INT1P = 1;	// connect INT1 interrupt (button 2) to high prio
	INTCON2bits.INTEDG1 = 0;	// INT1 interrupts on falling edge
	INTCON3bits.INT1E = 1;		// enable INT1 interrupt (button 2)
	INTCON3bits.INT1F = 0;		// clear INT1 flag

	led_init();
	display_init();

	// Start experiment
	setupTimers();
	T1CONbits.TMR1ON = 1;

	while (TRUE)
		;
}
