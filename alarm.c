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

#include "time.c"

#define LOW(a)     (a & 0xFF)
#define HIGH(a)    ((a>>8) & 0xFF)

// Time
struct time clock_time = { 0, 0, 0 };
struct time alarm_time = { 7, 0, 0 };

// Timer
#define TIMER_TIMEOUT 500
void handle_half_second(void);

// Seconds remaining until alarm stops
// <= 0 : alarm stopped
int alarm_remaining;

// Program mode
enum mode {
	mode_show_clock, mode_set_clock_time, mode_set_alarm_time
};
enum mode current_mode;
typedef void (*MODE_HANDLER)();
MODE_HANDLER current_mode_func;
MODE_HANDLER next_mode_start;

void show_clock_start(void);
void set_clock_time_start(void);
void set_alarm_time_start(void);

// Set time (clock/alarm) mode
enum set_time_state {
	hours, minutes, seconds
};
enum set_time_state set_time_current_state;
struct time *set_time_current = NULL;
BYTE set_time_column;
BYTE set_time_arrow_column;
void set_time_start(BYTE column, struct time *ptime);
void set_time_run(void);

// Display
void display_time(BYTE line, BYTE column, struct time *ptime);

// Alarm
#define ALARM_DURATION (30 * 2)		// 30 * 2 per half-secondvoid alarm_start(void);void alarm_stop(void);BOOL alarm_is_running(void);void alarm_run_tick(void);
// Main routinevoid main(void) {
	// Initialize buttons
	button_init();

	// Initialize timer
	timer_init();
	timer_set_handler(&handle_half_second);
	timer_set_repeating(TRUE);
	timer_set_timeout(TIMER_TIMEOUT);
	timer_set_enabled(TRUE);

	// Initialize display
	led_init();
	display_init();
	led_set_all(FALSE);

	// Set clock, then alarm
	set_clock_time_start();
	next_mode_start = &set_alarm_time_start;

	// Run
	while (TRUE) {
		current_mode_func();
	}
}

/**
 * Low-priority interrupt routine.
 */
void low_isr(void)
__interrupt (2) {
	// Handle button interrupts
	button_handle_interrupt();
}

/**
 * High-priority interrupt routine.
 */
void high_isr(void)
__interrupt (1) {
	// Handle timer interrupts
	timer_handle_interrupt();
}

/**
 * Timer interrupt handler.
 */
void handle_half_second() {
	static BOOL at_second = FALSE;

	if (at_second) {
		// Next second
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
void display_time(BYTE line, BYTE column, struct time * ptime) {
	char buffer[TIME_STRING_SIZE] = { 0 };
	time_to_string(ptime, buffer);
	display_string(line, column, buffer);
}

/**
 * Show clock mode.
 */
void show_clock_run() {
	// Display clock and alarm time
	display_string(0, 0, "Clock: ");
	display_time(0, 7, &clock_time);
	display_string(1, 0, "Alarm: ");
	display_time(1, 7, &alarm_time);
}

void show_clock_start() {
	current_mode = mode_show_clock;
	current_mode_func = &show_clock_run;
	button_set_handler(button0, &set_clock_time_start);
	button_set_handler(button1, &set_alarm_time_start);
}

/**
 * Set clock time mode.
 */
void set_clock_time_run() {
	display_string(0, 0, "Clock: ");
	set_time_run();
}

void set_clock_time_start() {
	current_mode = mode_set_clock_time;
	current_mode_func = &set_clock_time_run;
	next_mode_start = &show_clock_start;
	// Stop alarm
	alarm_stop();
	// Set time
	set_time_start(7, &clock_time);
}

/**
 * Set alarm time mode.
 */
void set_alarm_time_run() {
	display_string(0, 0, "Alarm: ");
	set_time_run();
}

void set_alarm_time_start() {
	current_mode = mode_set_alarm_time;
	current_mode_func = &set_alarm_time_run;
	next_mode_start = &show_clock_start;
	// Stop alarm
	alarm_stop();
	// Set time
	set_time_start(7, &alarm_time);
}

void set_time_button0() {
	// Increment value
	switch (set_time_current_state) {
	case hours:
		time_cycle_hours(set_time_current);
		break;
	case minutes:
		time_cycle_minutes(set_time_current);
		break;
	case seconds:
		time_cycle_seconds(set_time_current);
		break;
	}
}
void set_time_button1() {
	// Move arrow
	set_time_arrow_column += 3; // 2 digits, 1 colon
	// Next state
	switch (set_time_current_state) {
	case hours:
		set_time_current_state = minutes;
		break;
	case minutes:
		set_time_current_state = seconds;
		break;
	case seconds:
		// Next mode
		next_mode_start();
		return;
	}
}

void set_time_start(BYTE column, struct time *ptime) {
	set_time_current_state = hours;
	set_time_current = ptime;
	set_time_column = column;
	set_time_arrow_column = column - 3; // for spaces

	button_set_handler(button0, &set_time_button0);
	button_set_handler(button1, &set_time_button1);
}

void set_time_run() {
	// Display current time
	display_time(0, set_time_column, set_time_current);
	// Draw arrow
	display_string(1, set_time_arrow_column, "   ^^");
}
