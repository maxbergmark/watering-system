#pragma once

#include "sensor_handler.h"
#include "motor_handler.h"

#include <string>

class MetricsHandler {

private:
	SensorHandler *_sensorHandler;
	MotorHandler *_motorHandler;
public:
	MetricsHandler(SensorHandler *sensorHandler, MotorHandler *motorHandler);
	int printMetrics(char* buffer);
	void update();
};