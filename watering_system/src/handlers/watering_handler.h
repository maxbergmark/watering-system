#pragma once

#include <vector>
#include <ArduinoJson.h>

#include "sensor_handler.h"
#include "motor_handler.h"
#include "time_handler.h"

class WateringController;

class WateringHandler {

private:

	SensorHandler* _sensorHandler;
	MotorHandler* _motorHandler;
	std::vector<WateringController*> wateringControllers;
	WateringController* createWateringController(int soilIndex, int motorIndex);

public:
	WateringHandler(SensorHandler* sensorHandler, MotorHandler* motorHandler);
	void begin();
	void readConfig(JsonArray& wateringControllerArray);
	void update();
	int printConfiguration(char* buffer);
	void generateConfiguration(JsonArray* json);
};

class WateringController {

private:
	static constexpr unsigned long DELAY_TIME = 300000;
	static constexpr unsigned long REQUIRED_MEASUREMENTS = 20;
	SoilMoistureSensor* sensor;
	Motor* motor;
	unsigned long lastActivate;
	int consecutiveMeasurements;

public:
	WateringController();
	SoilMoistureSensor* getSensor();
	Motor* getMotor();
	void setSensor(SoilMoistureSensor* sensor);
	void setMotor(Motor* motor);
	void update();
};