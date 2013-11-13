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

void main(void) {
	// Insert magic here
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

void set_time_run_loop() {
	char *current_time_value = &clock_time;
	while (1) {
		if (BUTTON0_IO == 0u) {
			// Increment value
			// TODO Cycle value!
			(*current_time_value)++;
		}
		if (BUTTON1_IO == 0u) {
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
