#include <Wire.h>
#include <Esp.h>

#include "src/handlers/config_handler.h"
#include "src/handlers/motor_handler.h"
#include "src/handlers/sensor_handler.h"
#include "src/handlers/server_handler.h"
#include "src/handlers/watering_handler.h"
#include "src/helpers/helper_functions.h"
#include "src/handlers/time_handler.h"
#include "src/helpers/logger.h"
#include "src/handlers/crash_handler.h"


#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#define timer0_preload 40161290*4
#define loop_delay 1

PCFExpander ioExpander;
MotorHandler motorHandler(&ioExpander);
SensorHandler sensorHandler(&ioExpander);
WateringHandler wateringHandler(&sensorHandler, &motorHandler);
ConfigHandler configHandler(&sensorHandler, &motorHandler, &wateringHandler);
ServerHandler serverHandler(&sensorHandler, 
	&motorHandler, &wateringHandler, &configHandler);

void IRAM_ATTR updateHandlers() {
	motorHandler.update();
	wateringHandler.update();
	TimeHandler::update();
	timer0_write(ESP.getCycleCount() + timer0_preload * loop_delay);
}

void setupIOExpander() {
	Logger::log("setupIOExpander");
	if (!ioExpander.begin()) {
		Logger::log("Error setting up IO expander.");
		delay(1000);
		ESP.restart();
	}
}

void setupHandlerLoop() {
	noInterrupts();
	timer0_isr_init();
	timer0_attachInterrupt(updateHandlers);
	timer0_write(ESP.getCycleCount() + timer0_preload * loop_delay);
	interrupts();
}

void setupHandlers() {
	sensorHandler.begin();
	serverHandler.setupServer();
	motorHandler.begin();
	wateringHandler.begin();
	configHandler.begin();
	serverHandler.begin();
	TimeHandler::begin();
}

void logResetReason() {
	rst_info* ri = system_get_rst_info();
	Logger::log(ESP.getResetReason());
	Logger::log(String(ri->reason));
	Logger::log(String(ri->exccause));
	Logger::log(String(ri->epc1));
	Logger::log(String(ri->epc2));
	Logger::log(String(ri->epc3));
	Logger::log(String(ri->excvaddr));
	Logger::log(String(ri->depc));
}

void setup() {
	Wire.begin();
	EEPROM.begin(EEPROM_MAX_ADDR);
	Serial.begin(115200);
	while (!Serial);
	// i2cScan();
	setupIOExpander();
	setupHandlers();
	setupHandlerLoop();
	logResetReason();
}

void loop(){
	serverHandler.serverLoop();
}
