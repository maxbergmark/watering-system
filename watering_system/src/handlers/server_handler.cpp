#include "server_handler.h"

#include <Arduino.h>
#include <base64.hpp>
#include <ArduinoJson.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "../html/control_page.html.h"
#include "../html/watering_system.css.h"
#include "wifi_pass.h"

ESP8266WebServer server(80);

char logBuffer[5100];
char responseBuffer[2048];
char uriBuffer[256];

DynamicJsonDocument responseJson(2048);


ServerHandler::ServerHandler(SensorHandler* sensorHandler, 
	MotorHandler* motorHandler, WateringHandler* wateringHandler, 
	ConfigHandler* configHandler) {
	
	_sensorHandler = sensorHandler;
	_wateringHandler = wateringHandler;
	_motorHandler = motorHandler;
	_configHandler = configHandler;

	_updateManager = new UpdateManager();
	_metricsHandler = new MetricsHandler(_sensorHandler, _motorHandler);
}

void ServerHandler::begin() {
	server.on("/ping", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getPing, this));
	server.on("/reset", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getReset, this));
	server.on("/metrics", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getMetrics, this));
	server.on("/sensor", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getSensorValue, this));
	server.on("/logs", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getLogs, this));
	server.on("/motor", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getActivateMotor, this));
	server.on("/control", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getMotorControl, this));
	
	server.on("/configuration", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getConfiguration, this));
	server.on("/configuration", HTTPMethod::HTTP_POST, 
		std::bind(&ServerHandler::postConfiguration, this));

	server.on("/favicon.png", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getFavicon, this));
	server.on("/watering_system.css", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getCss, this));

	server.on("/crash", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::crash, this));
	server.on("/crashlog", HTTPMethod::HTTP_GET, 
		std::bind(&ServerHandler::getCrashLog, this));
	
	server.onNotFound(std::bind(&ServerHandler::sendNotFoundResponse, this));

	server.begin();
	_updateManager->begin();
	Logger::log("server started");

}

void ServerHandler::serverLoop() {
	_updateManager->handle();
	server.handleClient();
}

void makeUri(char* buffer) {
	const char* uri = server.uri().c_str();
	int n = strlen(uri);
	int i = n;
	memcpy(&buffer[0], uri, n);
	for (int j = 0; j < server.args(); j++) {
		const char* argName = server.argName(j).c_str();
		int argNameN = strlen(argName);
		const char* arg = server.arg(j).c_str();
		int argN = strlen(arg);
		buffer[i] = j == 0 ? '?' : '&';
		memcpy(&buffer[i+1], argName, argNameN);
		buffer[i + argNameN + 1] = '=';
		memcpy(&buffer[i + argNameN + 2], arg, argN);
		i += argNameN + argN + 2;
	}
	buffer[i] = '\0';
}

void ServerHandler::getMetrics() {
	// makeUri(uriBuffer);
	// Logger::log(uriBuffer);
	_metricsHandler->update();
	_metricsHandler->printMetrics(responseBuffer);
	server.send(200, "text/plain", responseBuffer);
}

char timeBuffer[24];

void ServerHandler::getPing() {
	makeUri(uriBuffer);
	Logger::log(uriBuffer);
	responseJson.clear();
	TimeHandler::print(timeBuffer);
	responseJson["response"] = "pong";
	responseJson["millis"] = millis();
	responseJson["time"] = timeBuffer;
	serializeJson(responseJson, responseBuffer);
	server.send(200, "application/json", responseBuffer);
}

void ServerHandler::getReset() {
	makeUri(uriBuffer);
	Logger::log(uriBuffer);

	responseJson.clear();
	TimeHandler::print(timeBuffer);
	responseJson["response"] = "resetting";
	responseJson["millis"] = millis();
	responseJson["time"] = timeBuffer;
	serializeJson(responseJson, responseBuffer);

	server.send(200, "application/json", responseBuffer);
	delay(100);
	ESP.restart();
}

void ServerHandler::getLogs() {
	makeUri(uriBuffer);
	Logger::log(uriBuffer);
	Logger::print(logBuffer, 1024);
	server.send(200, "text/plain", logBuffer);
}

void ServerHandler::getActivateMotor() {
	makeUri(uriBuffer);
	Logger::log(uriBuffer);
	responseJson.clear();
	if (!server.hasArg("index") || !server.hasArg("seconds")) {
		responseJson["error"] = "bad_arguments";
		responseJson["code"] = 400;
		responseJson["message"] = "required_arguments:[index,seconds]";
		serializeJson(responseJson, responseBuffer);

		server.send(400, "application/json", responseBuffer);
		return;
	}
	int idx = server.arg("index").toInt();
	Motor* motor = _motorHandler->getMotor(idx);
	if (motor == NULL) {
		responseJson["error"] = "bad_arguments";
		responseJson["code"] = 400;
		responseJson["message"] = "Motor not found";

		JsonObject obj = responseJson.createNestedObject("args");
		obj["index"] = idx;
		serializeJson(responseJson, responseBuffer);

		server.send(400, "application/json", responseBuffer);
		return;
	}
	float seconds = server.arg("seconds").toFloat();
	motor->turnOnFor(seconds);

	responseJson["message"] = "activating_motor";
	responseJson["index"] = idx;
	responseJson["time"] = seconds;
	serializeJson(responseJson, responseBuffer);

	server.send(200, "application/json", responseBuffer);
}

void ServerHandler::createConfigJson() {
	responseJson.clear();
	JsonArray sensorJson = responseJson.createNestedArray("sensors");
	_sensorHandler->generateConfiguration(&sensorJson);

	JsonArray motorJson = responseJson.createNestedArray("motors");
	_motorHandler->generateConfiguration(&motorJson);

	JsonArray wateringControllerJson = responseJson
		.createNestedArray("watering_controllers");
	_wateringHandler->generateConfiguration(&wateringControllerJson);	
}

void ServerHandler::getConfiguration() {
	makeUri(uriBuffer);
	Logger::log(uriBuffer);
	createConfigJson();

	serializeJson(responseJson, responseBuffer);
	server.send(200, "application/json", responseBuffer);
}

void ServerHandler::postConfiguration() {
	responseJson.clear();
	deserializeJson(responseJson, server.arg("plain"));
	serializeJson(responseJson, responseBuffer);
	eeprom_write_string(2048, responseBuffer);
	EEPROM.commit();
	_configHandler->createFromJson(responseJson);
	createConfigJson();
	serializeJson(responseJson, responseBuffer);
	server.send(200, "application/json", responseBuffer);
}

void ServerHandler::sendNotFoundResponse() {
	makeUri(uriBuffer);
	Logger::log(uriBuffer);

	responseJson.clear();
	responseJson["error"] = "not_found";
	responseJson["code"] = 404;
	serializeJson(responseJson, responseBuffer);

	server.send(404, "application/json", responseBuffer);
}

void ServerHandler::getSensorValue() {
	makeUri(uriBuffer);
	Logger::log(uriBuffer);
	responseJson.clear();

	if (!server.hasArg("sensor_type") || !server.hasArg("index")) {
		responseJson["error"] = "bad_arguments";
		responseJson["code"] = 400;
		responseJson["message"] = "required_arguments:[sensor_type,index]";
		serializeJson(responseJson, responseBuffer);

		server.send(400, "application/json", responseBuffer);
		return;
	}
	std::string sensorStr = std::string(server.arg("sensor_type").c_str());
	int idx = server.arg("index").toInt();
	if (!_sensorHandler->hasSensor(idx, sensorStr)) {
		responseJson["error"] = "bad_arguments";
		responseJson["code"] = 400;
		responseJson["message"] = "Sensor not found";

		JsonObject obj = responseJson.createNestedObject("args");
		obj["sensor_type"] = sensorStr.c_str();
		obj["index"] = idx;
		serializeJson(responseJson, responseBuffer);
		server.send(400, "application/json", responseBuffer);
		return;
	}
	float value = _sensorHandler->getSensorValue(idx, sensorStr);
	responseJson["sensor_type"] = sensorStr.c_str();
	responseJson["index"] = idx;
	responseJson["value"] = value;
	serializeJson(responseJson, responseBuffer);
	server.send(200, "application/json", responseBuffer);
}

void ServerHandler::getFavicon() {
	unsigned char* data = (unsigned char*) ("iVBORw0KGgoAAAANSUhEUgAAABAAAAAQE"
		"AYAAABPYyMiAAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdT"
		"AAAOpgAAA6mAAAF3CculE8AAAABmJLR0T///////8JWPfcAAAAB3RJTUUH5gMDCiYOcC2"
		"s9AAAAoJJREFUSMe1lE1IVFEUx3/3zbxEMyh1SNEIjEIsijYZFFQbC0MJxsB0nkGUTmBR"
		"idHCtasgioJoIzGGJb6xRUiLWrrqgxaCCbVIFNR5SgmTlDPvtjhvdNIhx48OPP73nXPvP"
		"b977odijWaF7H4nlvozu9GAMuIoFPpXW6QnGCwKZD+fkXXiRnvAmQQUCgXy1bVgoOByJb"
		"sZZxSskG07M/8BAB8uPq+tA7WC0H5LHHfCjFFKxcEPKDQarAt2r/N1EwCsBjvqTAImahF"
		"AhT5Jo2q/aNkooNDt3zCARE4vWzDJ34wKmGh8wAKa5N6n4gwPesOtv/qq+lZcFP7aKBqF"
		"4W3d7DoArCbbdqaBBOAaJyXB1esS3Xcg86i8adGOKVxAlbzBwMVdA4DVYPc6c17EAPyA7"
		"+hNiYaOr15UgCOHUACtURLMoBmwQvaLpdvzrwqYmCwA4KJym8R5o0M08Co7AB6ItJzGpA"
		"CqwqkTZF2xnzjDGQCs21FmyqTQ6JSeaJRozfMsEy+zkjpAocJdsiBfJ/PsoDhTBSb0S/3"
		"ZW7nrAXDqnQS3Hl4fQMqOxSTXznLvFcm4BQoXMIkzl3IVDm0sccry5Qkjr1p0yfxpbc1P"
		"IEE+21OusYrNAYjJ46y/Dy+PLFYg8ix4rqgY0OmEr0tFpx9vDGBwhALVxohzBnDT677yF"
		"mgPwSWJet8nzrtVovOda0v8dkIO9KNmZvVDKiESCZ4vKvwHQKQnWO918KHdj3Ig73vR1n"
		"7RoW5Rp0v0xx7RL3Wi96IoNOrSNQzAHa/mN0lyViIqVjHrot3nxIAAcVx2McU2/AVxQKH"
		"L5d6oXAHQ4zUowBg7iwZ0sjkSCQbTV7zc/gC/w8FqTspznQAAACV0RVh0ZGF0ZTpjcmVh"
		"dGUAMjAyMi0wMy0wM1QwOTozODozNiswMTowMPAR5IgAAAAldEVYdGRhdGU6bW9kaWZ5A"
		"DIwMjItMDMtMDNUMDk6Mzg6MTQrMDE6MDBU9kpgAAAAAElFTkSuQmCC");
	int l = decode_base64(data, (unsigned char*) responseBuffer);
	server.send_P(200, "image/png", responseBuffer, l);
}

void ServerHandler::getMotorControl() {
	server.send(200, "text/html", controlPageHtml);
}

void ServerHandler::getCss() {
	server.send(200, "text/css", css);
}

void ServerHandler::crash() {
	int a[5];
	int *ptr = a;
	ptr++;
	sprintf(responseBuffer, "Done!");
	int* i = NULL;
	*i = 80;
	server.send(200, "text/plain", responseBuffer);
}

void ServerHandler::getCrashLog() {
	server.send(200, "text/plain", loadStack());

}

void ServerHandler::setupServer() {
	Logger::log(String("Connecting to ") + ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Logger::log("Connection failed. Rebooting...");
		delay(500);
		ESP.restart();
	}
	Logger::log("WiFi connected. IP address: ");
	Logger::log(WiFi.localIP().toString());
	server.begin();
}