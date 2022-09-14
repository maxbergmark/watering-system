#pragma once

#include <string>

#include "sensor_type.h"

class SensorHandler;

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
	virtual void update();
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