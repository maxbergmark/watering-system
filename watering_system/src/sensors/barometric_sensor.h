#pragma once

#include <Adafruit_BMP085.h>


// #include "../handlers/sensor_handler.h"
#include "base_sensors.h"
// class I2CSensor;

class BarometricSensor : public I2CSensor {

private:
	Adafruit_BMP085 barometer;

public:
	BarometricSensor(SensorHandler *sensorHandler);
	virtual SensorType getSensorType() {return SensorType::BAROMETRIC;};
	virtual float readValue();
};