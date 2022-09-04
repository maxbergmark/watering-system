#include "soil_moisture_sensor.h"

#include "../handlers/sensor_handler.h"

SoilMoistureSensor::SoilMoistureSensor(SensorHandler *sensorHandler, int idx, 
	float minLimit, float maxLimit, std::string plantName) 
		: AnalogSensor(sensorHandler, "soil_moisture", idx) {

	this->maxLimit = maxLimit;
	this->minLimit = minLimit;
	this->plantName = plantName;
};

int SoilMoistureSensor::printMetrics(char* buffer) {
	return sprintf(buffer, "%s_gauge{index=\"%d\", name=\"%s\"} %.5f\n", 
		metricName.c_str(), idx, plantName.c_str(), gauge);
}

float SoilMoistureSensor::readValue() {
	_sensorHandler->switchActiveSensor(idx);
	int soilMoisture = analogRead(A0);
	float rawValue = 1.0 - soilMoisture / 1024.0;
	maxLimit = max(maxLimit, rawValue);
	minLimit = min(minLimit, rawValue);
	return (rawValue - minLimit) / (maxLimit - minLimit);
}

std::string SoilMoistureSensor::getName() {
	return plantName;
}

float SoilMoistureSensor::getMinLimit() {
	return minLimit;	
}

float SoilMoistureSensor::getMaxLimit() {
	return maxLimit;	
}