#include "motor_handler.h"

MotorHandler::MotorHandler(IOExpander* ioExpander) {
	_ioExpander = ioExpander;
}

void MotorHandler::begin() {
	motors.push_back(new Motor(this, _ioExpander, MOTOR_PIN_0));
	motors.push_back(new Motor(this, _ioExpander, MOTOR_PIN_1));
	motors.push_back(new Motor(this, _ioExpander, MOTOR_PIN_2));
	motors.push_back(new Motor(this, _ioExpander, MOTOR_PIN_3));
	motors.push_back(new Motor(this, _ioExpander, MOTOR_PIN_4));
	motors.push_back(new Motor(this, _ioExpander, MOTOR_PIN_5));
}

void IRAM_ATTR MotorHandler::update() {
	for (auto &motor : motors) {
		motor->update();
	}
}

String MotorHandler::getMetricsString() {
	String s;
	for (auto &motor : motors) {
		s += motor->getMetricsString() + "\n";
	}
	return s;
}

int MotorHandler::printMetrics(char* buffer) {
	int idx = 0;
	for (auto &motor : motors) {
		idx += motor->printMetrics(&buffer[idx]);
	}
	return idx;
}

Motor* MotorHandler::getMotor(int idx) {
	if (idx >= (int) motors.size()) {
		return NULL;
	}
	return motors[idx];
}

bool MotorHandler::isAnyMotorActive() {
	for (auto &motor : motors) {
		if (motor->getState()) {
			return true;
		}
	}
	return false;
}

void MotorHandler::toggleMotor(int idx) {
	motors[idx]->toggle();
}

void MotorHandler::turnOnMotor(int idx) {
	motors[idx]->turnOn();
}

void MotorHandler::turnOffMotor(int idx) {
	motors[idx]->turnOff();
}

void MotorHandler::turnOnMotorFor(int idx, float seconds) {
	motors[idx]->turnOnFor(seconds);
}

int MotorHandler::printConfiguration(char* buffer) {
	buffer[0] = '[';
	int idx = 1;
	for (auto &motor : motors) {
		int n = sprintf(&buffer[idx], 
			"{\"type\":\"motor\",\"index\":%d,\"activations\":%d},", 
			motor->getMotorPin(), motor->getActivations());
		idx += n;
	}
	buffer[idx-1] = ']';
	buffer[idx] = '\0';
	return idx;
}

void MotorHandler::generateConfiguration(JsonArray* json) {
	for (auto &motor : motors) {
		JsonObject o = json->createNestedObject();
		o["index"] = motor->getMotorPin();
		o["type"] = "motor";
		o["activations"] = motor->getActivations();
	}
}

// Motor

Motor::Motor(MotorHandler* motorHandler, IOExpander* ioExpander, int pin) {
	_motorHandler = motorHandler;
	motorPin = pin;
	_ioExpander = ioExpander;
	_ioExpander->pinMode(motorPin, OUTPUT);
	state = LOW;
	startTime = 0;
	stopTime = 0xFFFFFFFF;
	count = 0;
	seconds = 0;
	turnOff();
}

void IRAM_ATTR Motor::update() {
	unsigned long current = millis();
	if ((current > stopTime && state == HIGH) || current < startTime) {
		turnOff();
	}
}

String Motor::getMetricsString() {
	return String("motor_active_count{index=\"") + motorPin + "\"} " + count
		+ "\nmotor_active_sum{index=\"" + motorPin + "\"} " + seconds;
}

int Motor::printMetrics(char* buffer) {
	int idx = sprintf(buffer, 
		"motor_active_count{index=\"%d\"} %d\n", motorPin, count);
	idx += sprintf(&buffer[idx], 
		"motor_active_sum{index=\"%d\"} %f\n", motorPin, seconds);
	return idx;
}


void Motor::toggle() {
	state = !state;
	Logger::log("toggling motor");
	_ioExpander->digitalWrite(motorPin, state);
}

void Motor::turnOn() {
	state = HIGH;
	Logger::log("turning on motor");
	_ioExpander->digitalWrite(motorPin, state);
	count++;
}

void IRAM_ATTR Motor::turnOff() {
	state = LOW;
	Logger::log("turning off motor");
	_ioExpander->digitalWrite(motorPin, state);
}

void Motor::turnOnFor(float seconds) {
	if (state == HIGH) {
		return;
	}
	startTime = millis();
	stopTime = startTime + (unsigned long) (1000 * seconds);
	this->seconds += seconds;
	turnOn();
}

bool Motor::getState() {
	return state;
}

MotorHandler* Motor::getMotorHandler() {
	return _motorHandler;
}

int Motor::getActivations() {
	return count;
}

int Motor::getMotorPin() {
	return motorPin;
}
