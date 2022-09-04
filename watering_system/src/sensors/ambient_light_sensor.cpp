#include "ambient_light_sensor.h"

#include "../helpers/logger.h"

// AmbientLightSensor

AmbientLightSensor::AmbientLightSensor(SensorHandler *sensorHandler) 
		: I2CSensor(sensorHandler, "ambient_light", 0, 0x4A) {
	
	if(light.begin()) {
		Logger::log("Could not find a valid MAX44009 sensor, check wiring!");
		// delay(1000);
		// ESP.restart();
	} else {
		enabled = true;
	}
}

float AmbientLightSensor::readValue() {
	if (enabled) {
		return light.get_lux();
	}
	return 0;
}