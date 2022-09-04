#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <unordered_map>
#include <string>

#include "../helpers/io_expander.h"
#include "../helpers/eeprom.h"
#include "../helpers/logger.h"
#include "../sensors/sensor_type.h"

#define IO_ENABLE_PIN 8
#define IO_ADDRESS_PIN_0 9
#define IO_ADDRESS_PIN_1 10
#define IO_ADDRESS_PIN_2 11

#define TEMP_SENSOR_POWER_PIN 12
#define TEMP_SENSOR_DATA_PIN 13

class Sensor;
// class AnalogSensor;
// class DigitalSensor;
// class I2CSensor;
// class SoilMoistureSensor;

class SensorHandler {

private:
	IOExpander* _ioExpander;
	std::vector<Sensor*> sensors;
	static std::unordered_map<std::string, SensorType> sensorTypeMap;
	static std::unordered_map<SensorType, std::string> sensorNameMap;

public:
	SensorHandler(IOExpander* ioExpander);
	void begin();
	void readConfig(JsonArray& sensorArray);
	Sensor* getSensor(int idx, SensorType sensorType);
	Sensor* getSensor(int idx, std::string sensorStr);
	float getSensorValue(int idx, std::string sensorStr);
	bool hasSensor(int idx, std::string sensorStr);
	int printMetrics(char* buffer);
	int printConfiguration(char* buffer);
	void generateConfiguration(JsonArray* json);
	void switchActiveSensor(int idx);
	void update();
};


// #include "../sensors/base_sensors.h"

