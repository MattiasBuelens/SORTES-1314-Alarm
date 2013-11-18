/*
 * timertest.c
 *
 * Timer test program
 */

#define __18F97J60
#define __SDCC__
#define THIS_INCLUDES_THE_MAIN_FUNCTION

#include "Include/HardwareProfile.h"
#include "Include/LCDBlocking.h"
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
 * When using de 16-bit mode we will thus have a 2 second interval before it overflows.
 *
 * The strategy consists of running timer1, starting timer0 when timer 1 overflows and
 * counting the number of ticks timer0 encounters between the two overflows of timer 1
 *
 * We know the time between two interrupts of timer 1 and we know timer0 is faster then timer1,
 * at a of suspected 10.5MIPS.
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

void interruptServiceRoutine (void)__interrupt (1)
{
	//button is pressed run experiment again.
	if(INTCON3bits.INT1F  == 1){
		ticks = 0
		setupTimers();
		// Start timer1...
		T1CONbits.TMR1ON = 1;
		if(BUTTON0_IO);  //just read the bit
		INTCON3bits.INT1F  = 0;   //clear INT1 flag

	}

	// timer1 triggered an interrupt
	if (PIR1bits.TMR1IF) {
		// immediately enable or disable timer0 to start/stop incrementing said timer
		T0CONbits.TMR0ON ^= 1;
		led_toggle_all();

		// If timer0 gets disabled, we have a result and need to display it
		if (!timer_is_enabled()) {
			T1CONbits.TMR1ON = 0; //disable timer 1 as well.
			unsigned short remainder = timer_get_low();
			ticks += remainder;
			ticks /=2;
			// Print the result

			char str[33];
			sprintf(str, "Ticks / sec:  %lu", ticks);
			display_string(0,0,str);

		}

		// Clear interrupt flag
		PIR1bits.TMR1IF = 0;
	}

	// timer0 triggered an overflow, increase the count
	if (INTCONbits.TMR0IF == 1) {
		ticks += 256;
		// Clear interrupt flag
		INTCONbits.TMR0IF = 0;
	}
}

void setupTimers() {

	//  Disable timer0 and timer1 for setup
	timer_set_enabled(FALSE);
	T1CONbits.TMR1ON = 0;
	//reset timer 0 registers
	TMR0H = 0x00000000;
	TMR0L = 0x00000000;
	// use 8-bit
	T0CONbits.T08BIT = 1;
	// Use internal instruction cycle clock
	T0CONbits.T0CS = 0;
	// Don't use the prescaler
	T0CONbits.PSA = 1;
	// Enable interrupts
	INTCONbits.TMR0IE = 1;
	// Set up timer1
	// Use 16-bit read/write operation
	T1CONbits.RD16 = 1;
	// Clear the timer value. One must first set the high byte ,
	// Only copied when low byte is set, also with timer0
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

void main(void)
{
	RCONbits.IPEN = 1;			// enable interrupts priority levels
	INTCONbits.GIE = 1;			// enable high priority interrupts
	//INTCONbits.PEIE = 1; dont know if this is necessarry

    INTCON3bits.INT1P  = 1;   //connect INT1 interrupt (button 2) to high prio
    INTCON2bits.INTEDG1= 0;   //INT1 interrupts on falling edge
    INTCON3bits.INT1E  = 1;   //enable INT1 interrupt (button 2)
    INTCON3bits.INT1F  = 0;   //clear INT1 flag

	led_init();

	setupTimers();

	// Configure the LCD display
	// initialize the display
	LCDInit()
	// LCD backlight
	LED3_TRIS = 0;
	LED3_IO = 1;
	// Start timer1...
	T1CONbits.TMR1ON = 1;

	while(1);
}
