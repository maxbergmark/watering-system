#pragma once

#include <Adafruit_BMP085.h>

// #include "../handlers/sensor_handler.h"
#include "base_sensors.h"
// class I2CSensor;


class TemperatureSensor : public I2CSensor {

private:
	Adafruit_BMP085 barometer;

public:
	TemperatureSensor(SensorHandler *sensorHandler); 
	virtual SensorType getSensorType() {return SensorType::TEMPERATURE;};
	virtual float readValue();
};