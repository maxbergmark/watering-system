#pragma once

#include <Adafruit_BMP085.h>

#include "base_sensors.h"
#include "sensor_type.h"

class TemperatureSensor : public I2CSensor {

private:
	Adafruit_BMP085 barometer;

public:
	TemperatureSensor(SensorHandler *sensorHandler); 
	virtual SensorType getSensorType() {return SensorType::TEMPERATURE;};
	virtual float readValue();
};