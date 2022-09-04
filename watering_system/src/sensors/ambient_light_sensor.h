#pragma once

#include <MAX44009.h>

// #include "../handlers/sensor_handler.h"
#include "base_sensors.h"
#include "sensor_type.h"

// class I2CSensor;
// class SensorHandler;

class AmbientLightSensor : public I2CSensor {

private:
	MAX44009 light;

public:
	AmbientLightSensor(SensorHandler* sensorHandler);
	virtual SensorType getSensorType() {return SensorType::AMBIENT_LIGHT;};
	virtual float readValue();
};