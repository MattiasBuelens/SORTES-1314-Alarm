#define PLATFORM_PIC
//#define PLATFORM_WIN32

#define __SDCC__
#define THIS_INCLUDES_THE_MAIN_FUNCTION

#include "platform/button.h"
#include "platform/display.h"
#include "platform/led.h"

#include "platform.c"
#include "button.c"
#include "time.c"

#define LOW(a)     (a & 0xFF)
#define HIGH(a)    ((a>>8) & 0xFF)

// Times
struct time clock_time = { 0, 0, 0 };
struct time alarm_time = { 7, 0, 0 };

// Seconds remaining until alarm stops
// <= 0 : alarm stopped
int alarm_remaining;

// Program mode
enum mode {
	mode_show_clock, mode_set_clock_time, mode_set_alarm_time
};
enum mode current_mode;
void show_clock();
void set_clock_time();
void set_alarm_time();

// Set time (clock/alarm) mode
enum set_time_state {
	hours, minutes, seconds
};
enum set_time_state current_set_time_state = hours;
void set_time_run_loop(struct time *ptime);

// Display
void display_time(struct time *ptime);

// Alarm
#define ALARM_DURATION 30
void alarm_start();
void alarm_stop();
BOOL alarm_is_running();
void alarm_run_tick();

long uptime = 0;
long uptime_in_seconds(void) {
	// TODO Implement using timer?
	return 0l;
}

/**
 * Main routine.
 */
void main(void) {
	// Initialize I/O
	button_init();
	led_init();

	// Set clock and alarm
	set_clock_time();
	set_alarm_time();

	// Show clock
	show_clock();
}

/**
 * Timer interrupt handler.
 */
void timer_handle_half_second() {
	static BOOL at_second = FALSE;

	if (at_second) {
		// Next second
		uptime++;
		if (current_mode != mode_set_clock_time) {
			time_increment(&clock_time);
		}
	}

	// Tick alarm
	alarm_run_tick();
	if (current_mode == mode_show_clock) {
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
void display_time(struct time * ptime) {
	char buffer[TIME_STRING_SIZE] = { 0 };
	time_to_string(ptime, buffer);
	display_string(0, 0, buffer);
}

/**
 * Show clock mode.
 */
void show_clock() {
	while (TRUE) {
		current_mode = mode_show_clock;

		// Display clock time
		display_time(&clock_time);

		// Set clock on button0 double-press
		if (button0_dblpressed()) {
			set_time_run_loop(&clock_time);
		}

		// Set alarm on button0 double-press
		if (button1_dblpressed()) {
			set_time_run_loop(&alarm_time);
		}
	}
}

/**
 * Set clock time mode.
 */
void set_clock_time() {
	current_mode = mode_set_clock_time;
	// Stop alarm
	alarm_stop();
	// Set time
	set_time_run_loop(&clock_time);
}

/**
 * Set alarm time mode.
 */
void set_alarm_time() {
	current_mode = mode_set_alarm_time;
	// Stop alarm
	alarm_stop();
	// Set time
	set_time_run_loop(&alarm_time);
}

void set_time_run_loop(struct time *ptime) {
	while (TRUE) {
		// Display current time
		display_time(ptime);

		if (button0_pressed()) {
			// Increment value
			switch (current_set_time_state) {
			case hours:
				time_cycle_hours(ptime);
				break;
			case minutes:
				time_cycle_minutes(ptime);
				break;
			case seconds:
				time_cycle_seconds(ptime);
				return;
			}
		}
		if (button1_pressed()) {
			// Next state
			switch (current_set_time_state) {
			case hours:
				current_set_time_state = minutes;
				break;
			case minutes:
				current_set_time_state = seconds;
				break;
			case seconds:
				// Return to caller
				return;
			}
		}
	}
}
