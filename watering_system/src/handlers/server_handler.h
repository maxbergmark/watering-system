#pragma once

#include <ESP8266WiFi.h>

#include "config_handler.h"
#include "metrics_handler.h"
#include "update_manager.h"
#include "watering_handler.h"
#include "time_handler.h"
#include "crash_handler.h"
#include "../helpers/logger.h"
#include "../helpers/eeprom.h"

class ServerHandler {

private:
	SensorHandler* _sensorHandler;
	MotorHandler* _motorHandler;
	WateringHandler* _wateringHandler;
	ConfigHandler* _configHandler;

	MetricsHandler *_metricsHandler;
	UpdateManager *_updateManager;
	char* metricsBuffer;

	void getMetrics();
	void getPing();
	void getLogs();
	void getSensorValue();
	void getActivateMotor();
	void getReset();
	void getControl();
	void getMotorControl();

	void getConfiguration();
	void postConfiguration();

	void getFavicon();
	void getCss();

	void crash();
	void getCrashLog();

	void sendNotFoundResponse();
	void createConfigJson();
public:
	ServerHandler(SensorHandler* sensorHandler, MotorHandler* motorHandler, 
		WateringHandler* wateringHandler, ConfigHandler* configHandler);
	void begin();
	void serverLoop();
	void setupServer();
};