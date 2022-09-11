#include "watering_handler.h"

#include "../sensors/soil_moisture_sensor.h"
#include "../sensors/sensor_type.h"
#include "sensor_handler.h"

WateringHandler::WateringHandler(SensorHandler* sensorHandler, MotorHandler* motorHandler) {
	_sensorHandler = sensorHandler;
	_motorHandler = motorHandler;
}

WateringController* WateringHandler::createWateringController(int soilIndex, int motorIndex) {
	WateringController* w = new WateringController();
	SoilMoistureSensor* s = (SoilMoistureSensor*) _sensorHandler
		->getSensor(soilIndex, SensorType::SOIL_MOISTURE);
	w->setSensor(s);
	w->setMotor(_motorHandler->getMotor(motorIndex));
	return w;
}

void WateringHandler::begin() {
}

void WateringHandler::readConfig(JsonArray& wateringControllerArray) {
	for (WateringController* w : wateringControllers) {
		delete w;
	}
	wateringControllers.clear();
	return;

	for (JsonObject o : wateringControllerArray) {
		int sensor_index = o["sensor_index"];
		int motor_index = o["motor_index"];
		wateringControllers.push_back(
			createWateringController(sensor_index, motor_index));
	}
}


void WateringHandler::update() {
	for (auto& controller : wateringControllers) {
		controller->update();
	}
}

int WateringHandler::printConfiguration(char* buffer) {
	buffer[0] = '[';
	int idx = 1;
	for (auto& controller : wateringControllers) {
		idx += sprintf(&buffer[idx], 
			"{\"type\":\"watering_controller\",\"motor_index\":%d,\"sensor_index\":%d},",
			controller->getMotor()->getMotorPin(), 
			controller->getSensor()->getIndex());
	}
	buffer[idx-1] = ']';
	return idx;
}

void WateringHandler::generateConfiguration(JsonArray* json) {
	for (auto& controller : wateringControllers) {
		JsonObject o = json->createNestedObject();
		o["sensor_index"] = controller->getSensor()->getIndex();
		o["motor_index"] = controller->getMotor()->getMotorPin();
		o["type"] = "watering_controller";
	}
}

// WateringController

WateringController::WateringController() {
	lastActivate = 0;
	consecutiveMeasurements = 0;
}

SoilMoistureSensor* WateringController::getSensor() {
	return sensor;
}

Motor* WateringController::getMotor() {
	return motor;
}


void WateringController::setSensor(SoilMoistureSensor* sensor) {
	this->sensor = sensor;
}

void WateringController::setMotor(Motor* motor) {
	this->motor = motor;
}

void WateringController::update() {
	unsigned long current = millis();
	unsigned long t = current - lastActivate;
	int h = TimeHandler::getHours();

	bool isDry = sensor->readValue() < 0.30;
	bool isDay = 8 <= h && h < 20;
	bool isOtherMotorActive = motor->getMotorHandler()->isAnyMotorActive();

	if (t > DELAY_TIME &&  isDry && isDay) {		
		consecutiveMeasurements++;
		bool enoughMeasurements = consecutiveMeasurements >= REQUIRED_MEASUREMENTS;
		if (!isOtherMotorActive && enoughMeasurements) {
			lastActivate = current;
			motor->turnOnFor(80.0);
			consecutiveMeasurements = 0;
		}
	} else {
		consecutiveMeasurements = 0;
	}
}
