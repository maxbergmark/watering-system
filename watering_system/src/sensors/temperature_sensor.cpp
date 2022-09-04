#include "temperature_sensor.h"

#include "../helpers/logger.h"

// TemperatureSensor

TemperatureSensor::TemperatureSensor(SensorHandler *sensorHandler) 
	: I2CSensor(sensorHandler, "temperature", 0, 0x00) {

	if(!barometer.begin()) {
		Logger::log("Could not find a valid BMP085 sensor, check wiring!");
		// delay(1000);
		// ESP.restart();
	} else {
		enabled = true;
	}
}

float TemperatureSensor::readValue() {
	if (enabled) {
		return barometer.readTemperature();
	}
	return 0;
}