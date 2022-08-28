#pragma once

#define RTC_UTC_TEST 0

#include <TZ.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval

#include <sntp.h>                       // sntp_servermode_dhcp()
#include <coredecls.h>                  // settimeofday_cb()
#include <Schedule.h>
#include <PolledTimeout.h>

#include "../helpers/logger.h"

typedef esp8266::polledTimeout::periodicMs pollMs;

class TimeHandler {

private:
	static timeval tv;
	static timespec tp;
	static time_t now;
	static pollMs shouldUpdate;

public:
	static void begin();
	static void update();
	static void print(char* buffer);
	static int getHours();
};