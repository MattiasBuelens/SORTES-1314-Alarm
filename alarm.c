#define __18F97J60
#define __SDCC__
#define THIS_INCLUDES_THE_MAIN_FUNCTION
#include "Include/HardwareProfile.h"

#include <string.h>
#include <stdlib.h>

#include <pic18fregs.h>

#include "Include/LCDBlocking.h"
#include "Include/TCPIP_Stack/Delay.h"

#define LOW(a)     (a & 0xFF)
#define HIGH(a)    ((a>>8) & 0xFF)

#define CLOCK_FREQ 40000000 // 40 Mhz
#define EXEC_FREQ CLOCK_FREQ/4 // 4 clock cycles to execute an instruction

typedef int boolean;
enum {
	false, true
};

struct time {
	char hours;
	char minutes;
	char seconds;
};

// Times
struct time clock_time = { 0, 0, 0 };
struct time alarm_time = { 7, 0, 0 };

// Seconds remaining until alarm stops
// <= 0 : alarm stopped
int alarm_remaining;

// Program mode
enum mode {
	/*setup,*/clock, set_clock, set_alarm
};
enum mode current_mode = clock;

// State for set time (clock/alarm)
enum set_time_state {
	hours, minutes, seconds
};
enum set_time_state current_set_time_state = hours;

void time_increment(struct time *ptime);
boolean time_equals(struct time *ptime1, struct time *ptime2);
boolean button0pressed(void);
boolean button1pressed(void);
void set_time_run_loop(struct time *ptime)


void high_isr (void) // interrupt 1
{
    static int alternate = 1;//need to alternate between 39062 and 39063 for correct time
	if(INTCONbits.TMR0IF  == 1)
    {
    	if(alternate == 0){
    		time_increment(clock_time);
    	}

    	int a =0x10000-39062 - alternate; // Prescaler is used so 39063 is  0.5000064 sec
        TMR0H = HIGH(a);
        TMR0L= LOW(a);
    	INTCONbits.TMR0IF  = 0;
    	alternate = (alternate==1) ? 0 : 1 ;
    }


}

void main(void) {

	//Intitialize the LCD
    LCDInit();
    delay_ms(1000);

    //Ask to set time
    DisplayString (0,"Set Current Time");
	set_time_run_loop(clock_time);

	//Initialize timer0 to get interrupt every second.
	RCONbits.IPEN      = 1;   //enable interrupts priority levels
	INTCON2bits.TMR0IP = 1;
	INTCONbits.GIE     = 1;   //enable high priority interrupts
	T0CONbits.TMR0ON=0; // disable timer0
	int a =0x10000-39062; //Prescaler is used so 39062 is  0.4999936 sec
	TMR0H = HIGH(a);
	TMR0L= LOW(a);
	T0CONbits.T08BIT=0; // use timer0 16-bit counter
	T0CONbits.T0CS=0; // use timer0 instruction cycle clock
	T0CONbits.PSA=0; // disable timer0 prescaler
	T0CONbits.TOPS2 =1;// the timer only increments every 128 ticks, so 10000 clock ticks will now be 128ms
	T0CONbits.TOPS1 =1;// half a second is thus 39062.5 ticks
	T0CONbits.TOPS0 =0;
	INTCONbits.TMR0IF=0; // clear timer0 overflow bit
	INTCONbits.TMR0IE=1; //enable external interrupt procedure
	T0CONbits.TMR0ON=1; // enable timer0

	while(1){


		if(time_equals(clock_time,alarm_time)){
			soundalarm();
		}

		if(button0doublepressed()){
			set_time_run_loop(clock_time);
		}

		if(button1doublepressed()){
			set_alarm_run_loop(alarm_time);
		}
	}
}

void time_increment(struct time *ptime) {
	if (++(ptime->seconds) == 60) {
		ptime->seconds = 0;
		if (++(ptime->minutes) == 60) {
			ptime->minutes = 0;
			if (++(ptime->hours) == 24) {
				ptime->hours = 0;
			}
		}
	}
}

boolean time_equals(struct time *ptime1, struct time *ptime2) {
	return ptime1->hours == ptime2->hours && ptime1->minutes == ptime2->minutes
			&& ptime1->seconds == ptime2->seconds;
}

/*
 * refactored this to methods so we can test the clock on pc.
 */
boolean button0pressed() {
	return BUTTON0_IO == 0u;
}

boolean button1pressed() {
	return BUTTON1_IO == 0u;
}

void set_time_run_loop(struct time *ptime) {
	displayString("Hours");
	char *current_time_value = &(ptime->hours);
	while (1) {
		displayTimeStruct(*ptime);
		if (button0pressed()) {
			// Increment value
			if(current_set_time_state == hours && *current_time_value == 23 ){
				*current_time_value = 0;
			}
			else if((current_set_time_state == minutes || current_set_time_state == seconds) && current_time_value == 59){
				*current_time_value = 0;
			}
			else{
				(*current_time_value)++;
			}

		}
		if (button1pressed()) {
			// Next state
			switch (current_set_time_state) {
			case hours:
				current_set_time_state = minutes;
				displayString("minutes");
				current_time_value++;
				break;
			case minutes:
				current_set_time_state = seconds;
				displayString("seconds");
				current_time_value++;
				break;
			case seconds:
				// TODO Where do we return to?
				return;
			}
		}
	}
}

/*
 * to start one of the mode you have to double press or long press the button...
 * haven't tested it yet...
 */
boolean button0doublepressed(){
	if(!button0pressed()){
		return false;
	}
	static long lasttimepressed = getTimeInSeconds();
	static long wasPressed = 0;
	long timeBetweenPress = getTimeInSeconds() - lasttimepressed;
	if(timeBetweenPress < 0 || timeBetweenPress >20 ){
		return false;
	}
	if(wasPressed && timeBetweenPress > 2){
		lasttimepressed = getTimeInSeconds();
		return true;
	}
	wasPressed = button0pressed();
}

long getTimeInSeconds(){
	long ret_value =  (clock_time->hours * 60 + clock_time->minutes ) + clock_time->seconds;
	return ret_value;
}

// wait for approx 1ms
void delay_1ms(void) {
	TMR0H=(0x10000-EXEC_FREQ/1000)>>8;
	TMR0L=(0x10000-EXEC_FREQ/1000)&0xff;// zet nog 10000 op de teller voor overflows
	T0CONbits.TMR0ON=0; // disable timer0
	T0CONbits.T08BIT=0; // use timer0 16-bit counter
	T0CONbits.T0CS=0; // use timer0 instruction cycle clock
	T0CONbits.PSA=1; // disable timer0 prescaler
	INTCONbits.T0IF=0; // clear timer0 overflow bit
	T0CONbits.TMR0ON=1; // enable timer0
	while (!INTCONbits.T0IF) {} // wait for timer0 overflow
	INTCONbits.T0IF=0; // clear timer0 overflow bit
	T0CONbits.TMR0ON=0; // disable timer0
}

// wait for some ms
void delay_ms(unsigned int ms) {
	while (ms--) {
		delay_1ms();
	}
}

void DisplayString(BYTE pos, char* text)
{
   BYTE        l = strlen(text);/*number of actual chars in the string*/
   BYTE      max = 32-pos;      /*available space on the lcd*/
   char       *d = (char*)&LCDText[pos];
   const char *s = text;
   size_t      n = (l<max)?l:max;
   /* Copy as many bytes as will fit */
    if (n != 0)
      while (n-- != 0)*d++ = *s++;
   LCDUpdate();

}

