#pragma once

#include <ArduinoJson.h>

#include "../helpers/eeprom.h"
#include "sensor_handler.h"
#include "motor_handler.h"
#include "watering_handler.h"

class ConfigHandler {
private:
	SensorHandler* _sensorHandler;
	MotorHandler* _motorHandler;
	WateringHandler* _wateringHandler;

public:
	ConfigHandler(SensorHandler* sensorHandler, 
		MotorHandler* motorHandler, WateringHandler* wateringHandler);
	void begin();
	void createFromJson(DynamicJsonDocument& json);
	void saveConfig();
};
