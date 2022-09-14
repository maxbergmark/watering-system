#include "config_handler.h"

#include "../helpers/logger.h"

char* confBuffer;

ConfigHandler::ConfigHandler(SensorHandler* sensorHandler, 
	MotorHandler* motorHandler, WateringHandler* wateringHandler) {
	_sensorHandler = sensorHandler;
	_motorHandler = motorHandler;
	_wateringHandler = wateringHandler;
}

void ConfigHandler::begin() {
	confBuffer = (char*) malloc(2048 * sizeof(char));
	DynamicJsonDocument json(2048);
	eeprom_read_string(2048, confBuffer, 2048);
	deserializeJson(json, confBuffer);
	createFromJson(json);
	free(confBuffer);
}

void ConfigHandler::createFromJson(DynamicJsonDocument& json) {
	JsonArray sensors = json["sensors"].as<JsonArray>();
	JsonArray motors = json["motors"].as<JsonArray>();
	JsonArray wateringControllers = json["watering_controllers"].as<JsonArray>();
	Logger::log("creating sensors");
	_sensorHandler->readConfig(sensors);
	Logger::log("creating motors");
	_motorHandler->readConfig(motors);
	Logger::log("creating watering controllers");
	_wateringHandler->readConfig(wateringControllers);
}

void ConfigHandler::saveConfig() {
	confBuffer = (char*) malloc(2048 * sizeof(char));
	DynamicJsonDocument json(2048);
	json.clear();

	JsonArray sensorJson = json.createNestedArray("sensors");
	_sensorHandler->generateConfiguration(&sensorJson);

	JsonArray motorJson = json.createNestedArray("motors");
	_motorHandler->generateConfiguration(&motorJson);

	JsonArray wateringControllerJson = json
		.createNestedArray("watering_controllers");
	_wateringHandler->generateConfiguration(&wateringControllerJson);

	serializeJson(json, confBuffer, 2048);
	eeprom_write_string(2048, confBuffer);
	EEPROM.commit();
	free(confBuffer);
}
