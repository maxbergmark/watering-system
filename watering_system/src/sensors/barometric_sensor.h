#pragma once

#include <Adafruit_BMP085.h>

#include "base_sensors.h"
#include "sensor_type.h"

class BarometricSensor : public I2CSensor {

private:
	Adafruit_BMP085 barometer;

public:
	BarometricSensor(SensorHandler *sensorHandler);
	virtual SensorType getSensorType() {return SensorType::BAROMETRIC;};
	virtual float readValue();
};