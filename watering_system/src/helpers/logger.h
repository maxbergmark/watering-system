#pragma once

#include <vector>
#include <Arduino.h>
#include <cstdlib>
#include <cstring>

#include "../handlers/time_handler.h"


#define NUM_LOG_ROWS 50
#define LOG_ROW_LENGTH 80
#define NUM_TIME_BYTES 21

class Logger {

	static std::vector<char*> logRows;
	static int idx;

public:
	static void log(const char* message);
	static void log(String message);
	static void print(char* outputBuffer, int maxLen);
};