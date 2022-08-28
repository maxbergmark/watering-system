#include "update_manager.h"

#include <ESP8266WiFi.h>

UpdateManager::UpdateManager() {

}

void UpdateManager::begin() {

	// ArduinoOTA.setPort(8266);
	char s[50];
	sprintf(s, "watering_system (%s)", WiFi.macAddress().c_str());
	ArduinoOTA.setHostname(s);
	// ArduinoOTA.setPassword((const char *)"123");

	ArduinoOTA.onStart([]() {
		Logger::log("Start");
		timer0_detachInterrupt();
	});

	ArduinoOTA.onEnd([]() {
		Logger::log("End");
	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		char s[50];
		sprintf(s, "Progress: %d / %d", progress, total);
		Logger::log(s);
	});

	ArduinoOTA.onError([](ota_error_t error) {
		if (error == OTA_AUTH_ERROR) Logger::log("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Logger::log("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Logger::log("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Logger::log("Receive Failed");
		else if (error == OTA_END_ERROR) Logger::log("End Failed");
	});

	ArduinoOTA.begin();
	Logger::log("UpdateManager is ready");
}

void UpdateManager::handle() {
	ArduinoOTA.handle();
}