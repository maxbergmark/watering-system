#include "base_sensors.h"

// Sensor

Sensor::Sensor(SensorHandler *sensorHandler, std::string name) {
	_sensorHandler = sensorHandler;
	metricName = name;
	gauge = 0;
}

int Sensor::printMetrics(char* buffer) {
	return sprintf(buffer, "%s_gauge{index=\"%d\"} %.5f\n", 
		metricName.c_str(), idx, gauge);
}

void Sensor::update() {
	gauge = readValue();
}

// AnalogSensor

AnalogSensor::AnalogSensor(SensorHandler *sensorHandler, 
	std::string name, int idx) : Sensor(sensorHandler, name) {

	this->idx = idx;
}

// DigitalSensor

DigitalSensor::DigitalSensor(SensorHandler *sensorHandler, 
	std::string name, int idx, int port) : Sensor(sensorHandler, name) {

	this->idx = idx;
	multiplexerPort = port;
}

// I2CSensor

I2CSensor::I2CSensor(SensorHandler *sensorHandler, 
	std::string name, int idx, int address) : Sensor(sensorHandler, name) {
	this->idx = idx;
	i2cAddress = address;
}