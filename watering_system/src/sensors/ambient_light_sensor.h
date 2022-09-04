#pragma once

#include <MAX44009.h>

#include "base_sensors.h"
#include "sensor_type.h"

class AmbientLightSensor : public I2CSensor {

private:
	MAX44009 light;

public:
	AmbientLightSensor(SensorHandler* sensorHandler);
	virtual SensorType getSensorType() {return SensorType::AMBIENT_LIGHT;};
	virtual float readValue();
};