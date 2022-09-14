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
	int n = sprintf(buffer, "%s_gauge{index=\"%d\", name=\"%s\", type=\"normalized\"} %.5f\n", 
		metricName.c_str(), idx, plantName.c_str(), gauge);
	n += sprintf(&buffer[n], "%s_gauge{index=\"%d\", name=\"%s\", type=\"raw\"} %.5f\n", 
		metricName.c_str(), idx, plantName.c_str(), rawGauge);
	return n;
}

float SoilMoistureSensor::getRawValue() {
	_sensorHandler->switchActiveSensor(idx);
	int soilMoisture = analogRead(A0);
	return 1.0 - soilMoisture / 1024.0;
}

float SoilMoistureSensor::readValue() {
	float rawValue = getRawValue();
	maxLimit = max(maxLimit, rawValue);
	minLimit = min(minLimit, rawValue);
	return (rawValue - minLimit) / max(0.001f, maxLimit - minLimit);
}

void SoilMoistureSensor::update() {
	rawGauge = getRawValue();
	gauge = readValue();
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
