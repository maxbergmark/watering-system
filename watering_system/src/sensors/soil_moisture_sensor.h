#pragma once

// #include "../handlers/sensor_handler.h"
#include "base_sensors.h"


class SoilMoistureSensor : public AnalogSensor {

private:

	float maxLimit = 1.0;
	float minLimit = 0.0;
	std::string plantName;

public:
	SoilMoistureSensor(SensorHandler *sensorHandler, 
		int idx, float minLimit, float maxLimit, std::string plantName);
	int printMetrics(char* buffer);
	virtual SensorType getSensorType() {return SensorType::SOIL_MOISTURE;};
	virtual float readValue();
	virtual std::string getName();
	virtual float getMinLimit();
	virtual float getMaxLimit();
};