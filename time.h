/*
 * time.h
 */

#ifndef ALARM_TIME_H_
#define ALARM_TIME_H_

#include "Include/GenericTypeDefs.h"

struct time {
	BYTE hours;
	BYTE minutes;
	BYTE seconds;
};

long uptime_in_seconds(void);

BOOL time_equals(struct time *ptime1, struct time *ptime2);
long time_in_seconds(struct time *ptime);

void time_increment(struct time *ptime);
BOOL time_cycle_seconds(struct time *ptime);
BOOL time_cycle_minutes(struct time *ptime);
BOOL time_cycle_hours(struct time *ptime);
void time_clear(struct time *ptime);

#define TIME_STRING_SIZE 9
void time_to_string(struct time *ptime, char* buffer);

#endif /* ALARM_TIME_H_ */
