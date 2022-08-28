#include "logger.h"


std::vector<char*> makeVector() {
	std::vector<char*> v;
	for (int i = 0; i < NUM_LOG_ROWS; i++) {
		char* p = (char*) malloc(LOG_ROW_LENGTH * sizeof(char));
		p[0] = '\0';
		v.push_back(p);
	}
	return v;
}

std::vector<char*> Logger::logRows = makeVector();
int Logger::idx = 0;

void logTime(char* logRow) {
	TimeHandler::print(logRow);
	logRow[NUM_TIME_BYTES-2] = ':';
	logRow[NUM_TIME_BYTES-1] = ' ';
}

void Logger::log(const char* message) {
	int row = idx;
	idx = (idx + 1) % NUM_LOG_ROWS;
	logTime(logRows[row]);
	int n = std::min(
		(size_t) (LOG_ROW_LENGTH - 1 - NUM_TIME_BYTES), 
		strlen(message)
	);
	memcpy(&logRows[row][NUM_TIME_BYTES], message, n);
	logRows[row][n + NUM_TIME_BYTES] = '\0';
	Serial.println(message);
}

void Logger::log(String message) {
	log(message.c_str());
}

void Logger::print(char* output, int maxLen) {
	int currentIdx = 0;
	for (int i = 0; i < NUM_LOG_ROWS; i++) {
		int j = (i + idx) % NUM_LOG_ROWS;
		int n = std::min(strlen(logRows[j]), 
			(size_t) (maxLen - currentIdx - 2));
		if (n > 0) {
			memcpy(&output[currentIdx], logRows[j], n);
			output[currentIdx + n] = '\n';
			currentIdx += n + 1;
		}
	}
	output[currentIdx] = '\0';
}