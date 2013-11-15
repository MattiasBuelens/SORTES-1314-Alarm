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
	/*setup,*/clock, set_clock, set_alarm, alarm_mode
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

void updateDisplayTime() {
	displayTimeStruct(4, clock_time);
}

void high_isr (void) // One intterupt every 0.5sec to blink the led, every other interrupt we increment our clock
{
	static int alternate = 1;//need to alternate between 39062 and 39063 for correct time
	if(INTCONbits.TMR0IF  == 1)
	{
		if(alternate == 0){
			time_increment(clock_time);
			updateDisplayTime();

		}
		if(current_mode == alarm_mode){
			soundAlarm();
		}
		LED0_IO ^=1; //Blink the yellow led
		int a =0x10000-39062 - alternate; // Prescaler is used so 39063 is  0.5000064 sec
		TMR0H = HIGH(a);
		TMR0L= LOW(a);
		INTCONbits.TMR0IF  = 0;
		alternate = (alternate==1) ? 0 : 1 ;
	}


}

void main(void) {

	LED0_TRIS = 0; //configure 1st led pin as output (yellow)
	LED1_TRIS = 0;

	BUTTON0_TRIS = 1; //configure button0 as input

	BUTTON1_TRIS = 1; //configure button0 as input

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

	/*
	 * just an idea, use button as interrupt as well?, disable interrupts in interrupt vector?
	 */

	INTCON3bits.INT1P  = 1;   //connect INT1 interrupt (button 2) to high prio
	INTCON2bits.INTEDG1= 0;   //INT1 interrupts on falling edge
	INTCON3bits.INT1E  = 1;   //enable INT1 interrupt (button 2)
	INTCON3bits.INT1F  = 0;   //clear INT1 flag

	DisplayString(0,"Time");
	updateDisplayTime();
	DisplayString(12,"Alarm");
	displayTimeStruct(17,alarm_time);
	while(1){
		/*
		 * if clock_time equals alarm_time sound alarm, could it be that we miss this through intterupts?
		 */
		if(time_equals(clock_time,alarm_time)){
			soundalarm();
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
 * double press functionality?
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
	return false;
}

long getTimeInSeconds(){
	long ret_value =  (clock_time->hours * 60 + clock_time->minutes ) * 60 + clock_time->seconds;
	return ret_value;
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
/*
 * Bad implementation revisit!
 */
displayTimeStruct(BYTE pos, struct time *ptime){
	BYTE        l = 8;
	BYTE      max = 32-pos;      /*available space on the lcd*/
	if(l>max){
		DisplayString(0,"Error");
	}
	char *d = (char*)&LCDText[pos];
	const char *h = itoaSelf(ptime->hours);
	if(strlength(*h) == 1){
		*d++ = '0';
		*d++ = *h++;
	}
	else{
		*d++ = *h++;
		*d++ = *h++;
	}
	*d++ = ':';
	const char *m = itoaSelf(ptime->minutes);
	if(strlength(*m) == 1){
		*d++ = '0';
		*d++ = *m++;
	}
	else{
		*d++ = *m++;
		*d++ = *m++;
	}

	*d++ = '.';
	const char *s = itoaSelf(ptime->seconds);
	if(strlength(*s) == 1){
		*d++ = '0';
		*d++ = *s++;
	}
	else{
		*d++ = *s++;
		*d++ = *s++;
	}

	LCDUpdate();

}

char* itoaSelf(int i){
	char const digit[] = "0123456789";
	char* p = malloc(2);
	int shifter = i;
	do{ //Move to where representation ends
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do{ //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return p;
}

size_t
strlength(const char *str)
{
	const char *s;

	for (s = str; *s; ++s)
		;
	return (s - str);
}

void soundAlarm(void){
	static long startTime = getTimeInSeconds();
	current_mode = alarm_mode;
	long timePassed = getTimeInSeconds() - startTime;
	LED0_IO ^=1;
	if(timePassed > 30){
		LED0_IO =0;
		current_mode = clock;
	}
}



