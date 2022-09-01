#pragma once

#include <ArduinoJson.h>

#include "../helpers/io_expander.h"
#include "../helpers/logger.h"


#define MOTOR_PIN_0 0
#define MOTOR_PIN_1 1
#define MOTOR_PIN_2 2
#define MOTOR_PIN_3 3
#define MOTOR_PIN_4 4
#define MOTOR_PIN_5 5

class Motor;

class MotorHandler {

private:
	IOExpander* _ioExpander;

	std::vector<Motor*> motors;

public:
	MotorHandler(IOExpander* ioExpander);
	void begin();
	void readConfig(JsonArray& motorArray);
	void update();
	String getMetricsString();
	int printMetrics(char* buffer);
	Motor* getMotor(int idx);
	bool isAnyMotorActive();
	void toggleMotor(int idx);
	void turnOnMotor(int idx);
	void turnOffMotor(int idx);
	void turnOnMotorFor(int idx, float seconds);
	int printConfiguration(char* buffer);
	void generateConfiguration(JsonArray* json);
};

class Motor {

private:
	MotorHandler* _motorHandler;
	IOExpander* _ioExpander;
	int motorPin;
	bool state;
	unsigned long startTime;
	unsigned long stopTime;
	int count;
	float seconds;

public:
	Motor(MotorHandler* motorHandler, IOExpander* ioExpander, int pin);
	String getMetricsString();
	int printMetrics(char* buffer);
	void update();
	void toggle();
	void turnOn();
	void turnOff();
	void turnOnFor(float seconds);
	bool getState();
	MotorHandler* getMotorHandler();
	int getActivations();
	void setActivations(int value);
	int getMotorPin();
};