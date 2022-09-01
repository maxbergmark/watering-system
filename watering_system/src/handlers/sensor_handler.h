#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <MAX44009.h>
#include <DHTesp.h>
#include <Adafruit_BMP085.h>

#include <unordered_map>
#include <string>

#include "../helpers/io_expander.h"
#include "../helpers/eeprom.h"
#include "../helpers/logger.h"


#define IO_ENABLE_PIN 8
#define IO_ADDRESS_PIN_0 9
#define IO_ADDRESS_PIN_1 10
#define IO_ADDRESS_PIN_2 11

#define TEMP_SENSOR_POWER_PIN 12
#define TEMP_SENSOR_DATA_PIN 13


enum SensorType {
	SOIL_MOISTURE,
	TEMPERATURE,
	DHT_TEMPERATURE,
	HUMIDITY,
	AMBIENT_LIGHT,
	BAROMETRIC,
	HEAT_INDEX
};

class Sensor;
class SoilMoistureSensor;

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


class Sensor {

protected:
	bool enabled = false;
	int idx;
	float gauge;
	std::string metricName;
	SensorHandler *_sensorHandler;

public:
	Sensor(SensorHandler *sensorHandler, std::string name);
	virtual SensorType getSensorType() = 0;
	virtual int printMetrics(char* buffer);
	int getIndex() {return idx;};
	virtual float readValue() = 0;
	void update();
};


class AnalogSensor : public Sensor {

public:
	AnalogSensor(SensorHandler *sensorHandler, std::string name, int idx);
};

class DigitalSensor : public Sensor {

protected:
	int multiplexerPort;

public:
	DigitalSensor(SensorHandler *sensorHandler, std::string name, int idx, int port);
};

class I2CSensor : public Sensor {

protected:
	int i2cAddress;

public:
	I2CSensor(SensorHandler *sensorHandler, std::string name, int idx, int address);
};

class SoilMoistureSensor : public AnalogSensor {

private:

	float maxLimit = 1.0;
	float minLimit = 0.0;
	std::string plantName;

public:
	SoilMoistureSensor(SensorHandler *sensorHandler, 
		int idx, float minLimit, float maxLimit, std::string plantName);
	int printMetrics(char* buffer);
	virtual SensorType getSensorType() {return SensorType::SOIL_MOISTURE;};
	virtual float readValue();
	virtual std::string getName();
	virtual float getMinLimit();
	virtual float getMaxLimit();
};

class AmbientLightSensor : public I2CSensor {

private:
	MAX44009 light;

public:
	AmbientLightSensor(SensorHandler *sensorHandler);
	virtual SensorType getSensorType() {return SensorType::AMBIENT_LIGHT;};
	virtual float readValue();
};

class BarometricSensor : public I2CSensor {

private:
	Adafruit_BMP085 barometer;

public:
	BarometricSensor(SensorHandler *sensorHandler);
	virtual SensorType getSensorType() {return SensorType::BAROMETRIC;};
	virtual float readValue();
};

class TemperatureSensor : public I2CSensor {

private:
	Adafruit_BMP085 barometer;

public:
	TemperatureSensor(SensorHandler *sensorHandler); 
	virtual SensorType getSensorType() {return SensorType::TEMPERATURE;};
	virtual float readValue();
};

class HumiditySensor : public DigitalSensor {

private:
	DHTesp* dht;

public:
	HumiditySensor(SensorHandler *sensorHandler, DHTesp* dht, int idx, int port); 
	virtual SensorType getSensorType() {return SensorType::HUMIDITY;};
	virtual float readValue();
};

class DHTTemperatureSensor : public DigitalSensor {

private:
	DHTesp* dht;

public:
	DHTTemperatureSensor(SensorHandler *sensorHandler, DHTesp* dht, int idx, int port); 
	virtual SensorType getSensorType() {return SensorType::DHT_TEMPERATURE;};
	virtual float readValue();
};

class HeatIndexSensor : public DigitalSensor {

private:
	DHTesp* dht;

public:
	HeatIndexSensor(SensorHandler *sensorHandler, DHTesp* dht, int idx, int port); 
	virtual SensorType getSensorType() {return SensorType::HEAT_INDEX;};
	virtual float readValue();
};