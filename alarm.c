/*
 * alarm.c
 *
 * Main program
 */

#define __SDCC__
#define THIS_INCLUDES_THE_MAIN_FUNCTION

#include "platform/button.h"
#include "platform/display.h"
#include "platform/led.h"
#include "platform/timer.h"

#include "button.c"
#include "time.c"

#define LOW(a)     (a & 0xFF)
#define HIGH(a)    ((a>>8) & 0xFF)

// Time
struct time clock_time = { 0, 0, 0 };
struct time alarm_time = { 7, 0, 0 };

struct time *current_set_time = NULL;
static enum set_time_state state = hours;

// Timer
#define TIMER_SCALE 128
#define TIMER_OVERFLOWS 39062
void handle_half_second(void);

// Seconds remaining until alarm stops
// <= 0 : alarm stopped
int alarm_remaining;

// Program mode
//enum mode {
//	mode_show_clock, mode_set_clock_time, mode_set_alarm_time
//};
enum mode current_mode;
void show_clock();
void set_clock_time();
void set_alarm_time();

// Set time (clock/alarm) mode
enum set_time_state {
	hours, minutes, seconds
};
void set_time_run_loop(BYTE column, struct time *ptime);

// Display
void display_time(BYTE line, BYTE column, struct time *ptime);

// Alarm
#define ALARM_DURATION (30 * 2)		// 30 * 2 per half-secondvoid alarm_start();
void alarm_stop();
BOOL alarm_is_running();
void alarm_run_tick();

//Buttons
void handle_button0_during(void);
void handle_button0_initiate(void);
void handle_button1_initiate(void);

long uptime = 0;
long uptime_in_seconds(void) {
	return uptime;
}

/**
 * Main routine.
 */
void main(void) {
	// Initialize timer
	timer_init();
	timer_set_handler(&handle_half_second);
	timer_set_scale(TIMER_SCALE);
	timer_set_overflows(TIMER_OVERFLOWS);
	timer_set_repeating(TRUE);
	timer_set_enabled(TRUE);

	// Initialize I/O
	button_init();
	button_set_handler(0, &handle_button1_initiate);
	button_set_handler(1, &handle_button1_initiate);
	led_init();
	display_init();
	led_set_all(FALSE);

	// Set clock and alarm
	set_clock_time();
	set_alarm_time();

	// Show clock
	show_clock();
}

/**
 * High-priority interrupt routine.
 */
void high_isr(void)
__interrupt (1) {
	// Handle timer interrupts
	timer_handle_interrupt();
}
/*
 * Low-priority interrupt routine
 * uncomment for setting it as the low interrupt
 *
 * @TODO move platform spefic part to button,
 * probably need handler for both buttons
 */
void low_isr(void){ //__interrupt (2)
	button_handle_intterupt();


}

void handle_button0_initiate(){
	if (button0_dblpressed()) {
		set_clock_time();
		button_set_handler(&handle_button0_during());
	}
}

void handle_button1_initiate(){
	if (button1_dblpressed()) {
		set_alarm_time();
		button_set_handler(&handle_button0_during());
	}
}
//This switch case could be removed as well by splitting in 3 functions
void handle_button0_during(){
	if(&current_set_time){
		if(button0_pressed()){
			display_time(1,8,&current_set_time); //Display where it changes
			switch (state) {
			case hours:
				time_cycle_hours(&current_set_time);
				break;
			case minutes:
				time_cycle_minutes(&current_set_time);
				break;
			case seconds:
				time_cycle_seconds(&current_set_time);
				break;
			}

			//TRY TO CLEANUP THE LINE NUMBER/colum,...
			//fix it to be after previous?
			//add parameter determining to clear current display or to continue...
		}
	}
}

void handle_button1_during(){
	if(&current_set_time){
		static BYTE arrow_column = 8;
		if(button1_pressed()){
			display_string(0, arrow_column, "  ");
			arrow_column +=3;
			switch (state) {
			case hours:
				state = minutes;
				break;
			case minutes:
				state = seconds;
				break;
			case seconds:
				arrow_column = 8;
				button_set_handler(0,&handle_button0_initiate);
				button_set_handler(0,&handle_button1_initiate);
				current_set_time = NULL;
				// Return to caller
				return;
			}

			//TRY TO CLEANUP THE LINE NUMBER/colum,...
			//fix it to be after previous? Seems complex...
			//add parameter determining to clear current display or to continue...
		}
	}
}


/**
 * Timer interrupt handler.
 */
void handle_half_second() {
	static BOOL at_second = FALSE;

	if (at_second) {
		// Next second
		uptime++;
		if (!time_equals(&current_set_time,&clock_time)) {
			time_increment(&clock_time);
		}
	}

	// Tick alarm
	alarm_run_tick();

	if (! &current_set_time) {
		// Start alarm
		if (time_equals(&clock_time, &alarm_time)) {
			alarm_start();
		}
	}

	at_second = !at_second;
}

/*
 * Alarm
 */

void alarm_start() {
	alarm_remaining = ALARM_DURATION;
}

void alarm_stop() {
	alarm_remaining = 0;
	alarm_run_tick();
}

BOOL alarm_is_running() {
	return alarm_remaining > 0;
}

void alarm_run_tick() {
	if (alarm_is_running()) {
		led_toggle_all();
		alarm_remaining--;
	} else {
		led_set_all(FALSE);
	}
}

/*
 * Time display
 */
void display_time(BYTE line, BYTE column, struct time * ptime) {
	char buffer[TIME_STRING_SIZE] = { 0 };
	time_to_string(ptime, buffer);
	display_string(line, column, buffer);
}

/**
 * Show clock mode.
 */
void show_clock() {
	while (TRUE) {
		if(display_changed()){
			display_update();

		}
		// reduce number of calls to update!
		if(clock_time_changed()){
			// Display clock time
			display_time(0, 0, &clock_time);
		}


	}
}

BOOL clock_time_changed(){
	static long lastUpTime = uptime;
	if(lastUpTime != uptime){
		lastUpTime = uptime;
		return TRUE;
	}
	return FALSE;
}
/**
 * Set clock time mode.
 * Enige verschil is de string en welke clock-time SAMENVOEGEN TODO
 */
void set_clock_time() {
	current_set_time = &clock_time;
	// Stop alarm
	alarm_stop();
	// Set time
	display_string(1, 0, "Clock: ");
	display_string(0, 8, "VV");


}

/**
 * Set alarm time mode.
 */
void set_alarm_time() {
	current_set_time = &alarm_time;
	// Stop alarm
	alarm_stop();
	// Set time
	display_string(1, 0, "Alarm: ");
	display_string(0, 8, "VV");

}
/*
 * change loop to using iterrupts
 */
void set_time_run_loop(BYTE column, struct time *ptime) {
	enum set_time_state state = hours;
	BYTE arrow_column = column;
	while (TRUE) {
		// Display current time
		display_time(0, column, ptime);
		// Draw arrow
		display_string(1, arrow_column, "^^");
		if (button0_pressed()) {
			// Increment value
			switch (state) {
			case hours:
				time_cycle_hours(ptime);
				break;
			case minutes:
				time_cycle_minutes(ptime);
				break;
			case seconds:
				time_cycle_seconds(ptime);
				break;
			}
		}
		if (button1_pressed()) {
			// Move arrow
			display_string(1, arrow_column, "  ");
			arrow_column += 3; // 2 digits, 1 colon
			// Next state
			switch (state) {
			case hours:
				state = minutes;
				break;
			case minutes:
				state = seconds;
				break;
			case seconds:
				// Return to caller
				return;
			}
		}
	}
}
