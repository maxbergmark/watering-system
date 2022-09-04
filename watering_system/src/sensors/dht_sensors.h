#pragma once

#include <DHTesp.h>

// #include "../handlers/sensor_handler.h"
#include "base_sensors.h"
// class DigitalSensor;

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