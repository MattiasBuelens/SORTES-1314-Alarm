/*
 * time.c
 */

#include "Include/GenericTypeDefs.h"

#include <stdio.h>
#include "time.h"

BOOL time_equals(struct time *ptime1, struct time *ptime2) {
	return ptime1->hours == ptime2->hours && ptime1->minutes == ptime2->minutes
			&& ptime1->seconds == ptime2->seconds;
}

long time_in_seconds(struct time *ptime) {
	return (ptime->hours * 60 + ptime->minutes) * 60 + ptime->seconds;
}

void time_increment(struct time *ptime) {
	if (time_cycle_seconds(ptime)) {
		if (time_cycle_minutes(ptime)) {
			time_cycle_hours(ptime);
		}
	}
}

BOOL time_cycle_seconds(struct time *ptime) {
	if (++(ptime->seconds) == 60) {
		ptime->seconds = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL time_cycle_minutes(struct time *ptime) {
	if (++(ptime->minutes) == 60) {
		ptime->minutes = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL time_cycle_hours(struct time *ptime) {
	if (++(ptime->hours) == 24) {
		ptime->hours = 0;
		return TRUE;
	}
	return FALSE;
}

void time_clear(struct time *ptime) {
	ptime->hours = 0;
	ptime->minutes = 0;
	ptime->seconds = 0;
}

void time_to_string(struct time *ptime, char* buffer) {
	sprintf(buffer, "%02d:%02d:%02d", ptime->hours, ptime->minutes,
			ptime->hours);
}
