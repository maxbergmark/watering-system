#include "dht_sensors.h"

// HumiditySensor

HumiditySensor::HumiditySensor(SensorHandler *sensorHandler, DHTesp* dht,
	int idx, int port) : DigitalSensor(sensorHandler, "humidity", idx, port) {
	this->dht = dht;
};

float HumiditySensor::readValue() {
	// return 0;
	return dht->getHumidity();
}


// DHTTemperatureSensor

DHTTemperatureSensor::DHTTemperatureSensor(SensorHandler *sensorHandler, DHTesp* dht,
	int idx, int port) : DigitalSensor(sensorHandler, "dht_temperature", idx, port) {
	this->dht = dht;
};

float DHTTemperatureSensor::readValue() {
	// return 0;
	return dht->getTemperature();
}

// HeatIndexSensor

HeatIndexSensor::HeatIndexSensor(SensorHandler *sensorHandler, DHTesp* dht,
	int idx, int port) : DigitalSensor(sensorHandler, "heat_index", idx, port) {
	this->dht = dht;
};

float HeatIndexSensor::readValue() {
	// return 0;
	float temp = dht->getTemperature();
	float hum = dht->getHumidity();
	return dht->computeHeatIndex(temp, hum, false);
}
