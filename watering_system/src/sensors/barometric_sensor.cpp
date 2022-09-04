#include "barometric_sensor.h"

#include "../helpers/logger.h"

// BarometricSensor

BarometricSensor::BarometricSensor(SensorHandler *sensorHandler) 
		: I2CSensor(sensorHandler, "barometric", 0, 0x00) {
	
	if(!barometer.begin()) {
		Logger::log("Could not find a valid BMP085 sensor, check wiring!");
		// delay(1000);
		// ESP.restart();
	} else {
		enabled = true;
	}
}

float BarometricSensor::readValue() {
	if (enabled) {
		return barometer.readPressure();
	}
	return 0;
}
