#include "config_handler.h"

#include "../helpers/logger.h"

ConfigHandler::ConfigHandler(SensorHandler* sensorHandler, 
	MotorHandler* motorHandler, WateringHandler* wateringHandler) {
	_sensorHandler = sensorHandler;
	_motorHandler = motorHandler;
	_wateringHandler = wateringHandler;
}

void ConfigHandler::begin() {
	char confBuffer[2048];
	DynamicJsonDocument json(2048);
	eeprom_read_string(2048, confBuffer, 2048);
	deserializeJson(json, confBuffer);
	createFromJson(json);
}

void ConfigHandler::createFromJson(DynamicJsonDocument& json) {
	JsonArray sensors = json["sensors"].as<JsonArray>();
	JsonArray motors = json["motors"].as<JsonArray>();
	JsonArray wateringControllers = json["watering_controllers"].as<JsonArray>();
	Logger::log("updating sensors");
	_sensorHandler->readConfig(sensors);
	Logger::log("updating motors");
	_motorHandler->readConfig(motors);
	Logger::log("updating watering controllers");
	_wateringHandler->readConfig(wateringControllers);
}