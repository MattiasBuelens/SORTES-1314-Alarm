#define __18F97J60
#define __SDCC__
#define THIS_INCLUDES_THE_MAIN_FUNCTION
#include "Include/HardwareProfile.h"

#include <string.h>
#include <stdlib.h>

#include "Include/LCDBlocking.h"
#include "Include/TCPIP_Stack/Delay.h"

#define LOW(a)     (a & 0xFF)
#define HIGH(a)    ((a>>8) & 0xFF)

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

void main(void) {
	while(1){
		time_increment(clock_time);

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

boolean button0pressed() {
	return BUTTON0_IO == 0u;
}

boolean button1pressed() {
	return BUTTON1_IO == 0u;
}

void set_time_run_loop(struct time *ptime) {
	char *current_time_value = &(ptime->hours);
	while (1) {
		if (button0pressed()) {
			// Increment value
			// TODO Cycle value!
			(*current_time_value)++;
		}
		if (button1pressed()) {
			// Next state
			switch (current_set_time_state) {
			case hours:
				current_set_time_state = minutes;
				current_time_value++;
				break;
			case minutes:
				current_set_time_state = seconds;
				current_time_value++;
				break;
			case seconds:
				// TODO Where do we return to?
				return;
			}
		}
	}
}

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



