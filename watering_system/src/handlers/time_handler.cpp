#include "time_handler.h"

#include <Arduino.h>

extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

void TimeHandler::begin() {
	TimeHandler::tv = { 0, 0 };
	settimeofday(&TimeHandler::tv, nullptr);

	configTime(TZ_Europe_Stockholm, "pool.ntp.org");
}

void TimeHandler::update() {
	if (TimeHandler::shouldUpdate) {
		gettimeofday(&TimeHandler::tv, nullptr);
		clock_gettime(0, &TimeHandler::tp);
		now = time(nullptr);
	}
}

void TimeHandler::print(char* buffer) {
	gettimeofday(&TimeHandler::tv, nullptr);
	now = time(nullptr);
	tm* tm = localtime(&TimeHandler::now);
	if (tm->tm_year > 100) {
		sprintf(buffer, "%4d-%02d-%02d %02d:%02d:%02d", 
			1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);
	} else {
		memset(buffer, ' ', NUM_TIME_BYTES - 2);
	}
}

int TimeHandler::getHours() {
	gettimeofday(&TimeHandler::tv, nullptr);
	now = time(nullptr);
	tm* tm = localtime(&TimeHandler::now);
	return tm->tm_hour;
}

timeval TimeHandler::tv;
timespec TimeHandler::tp;
time_t TimeHandler::now;
pollMs TimeHandler::shouldUpdate = pollMs(60000);