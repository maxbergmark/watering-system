#include "sensor_handler.h"

#include <string>

// SensorHandler

std::unordered_map<std::string, SensorType> SensorHandler::sensorTypeMap = {
	{"soil_moisture", SensorType::SOIL_MOISTURE},
	{"temperature", SensorType::TEMPERATURE},
	{"dht_temperature", SensorType::DHT_TEMPERATURE},
	{"humidity", SensorType::HUMIDITY},
	{"ambient_light", SensorType::AMBIENT_LIGHT},
	{"barometric", SensorType::BAROMETRIC},
	{"heat_index", SensorType::HEAT_INDEX}
};

std::unordered_map<SensorType, std::string> SensorHandler::sensorNameMap = {
	{SensorType::SOIL_MOISTURE, "soil_moisture"},
	{SensorType::TEMPERATURE, "temperature"},
	{SensorType::DHT_TEMPERATURE, "dht_temperature"},
	{SensorType::HUMIDITY, "humidity"},
	{SensorType::AMBIENT_LIGHT, "ambient_light"},
	{SensorType::BAROMETRIC, "barometric"},
	{SensorType::HEAT_INDEX, "heat_index"}
};

SensorHandler::SensorHandler(IOExpander* ioExpander) {
	pinMode(A0, INPUT);
	_ioExpander = ioExpander;
}

DHTesp* createIfNotExists(DHTesp* dht) {
	if (dht == NULL) {
		dht = new DHTesp();
		dht->setup(13, DHTesp::DHT22);
	}
	return dht;
}

void SensorHandler::begin() {
	_ioExpander->pinMode(IO_ENABLE_PIN, OUTPUT);
	_ioExpander->pinMode(IO_ADDRESS_PIN_0, OUTPUT);
	_ioExpander->pinMode(IO_ADDRESS_PIN_1, OUTPUT);
	_ioExpander->pinMode(IO_ADDRESS_PIN_2, OUTPUT);
	_ioExpander->digitalWrite(IO_ENABLE_PIN, LOW);
}

void SensorHandler::readConfig(JsonArray& sensorArray) {

	for (Sensor* s : sensors) {
		delete s;
	}
	sensors.clear();

	DHTesp* dht = NULL;

	for (JsonObject o : sensorArray) {
		std::string typeStr(o["type"]);
		SensorType type = sensorTypeMap.at(typeStr);
		switch (type) {
			case SOIL_MOISTURE:
			{
				int index = o["index"];
				float minLimit = o["minLimit"];
				float maxLimit = o["maxLimit"];
				std::string nameStr(o["name"]); // TODO: Should this be a char pointer?
				sensors.push_back(new SoilMoistureSensor(this, 
					index, minLimit, maxLimit, nameStr));
				break;
			}
			case TEMPERATURE:
				sensors.push_back(new TemperatureSensor(this));
				break;
			case DHT_TEMPERATURE:
				dht = createIfNotExists(dht);
				sensors.push_back(new DHTTemperatureSensor(this, dht, 0, 13));
				break;
			case HUMIDITY:
				dht = createIfNotExists(dht);
				sensors.push_back(new HumiditySensor(this, dht, 0, 13));
				break;
			case AMBIENT_LIGHT:
				sensors.push_back(new AmbientLightSensor(this));
				break;
			case BAROMETRIC:
				sensors.push_back(new BarometricSensor(this));
				break;
			case HEAT_INDEX:
				dht = createIfNotExists(dht);
				sensors.push_back(new HeatIndexSensor(this, dht, 0, 13));
			break;

		}
	}
}

float SensorHandler::getSensorValue(int idx, std::string sensorStr) {
	if (hasSensor(idx, sensorStr)) {
		return getSensor(idx, sensorStr)->readValue();
	}
	return 0;
}

Sensor* SensorHandler::getSensor(int idx, SensorType sensorType) {
	for (auto &sensor : sensors) {
		if (sensor->getSensorType() == sensorType) {
			if (sensor->getIndex() == idx) {
				return sensor;
			}
		}
	}
	return NULL;
}

Sensor* SensorHandler::getSensor(int idx, std::string sensorStr) {
	if (sensorTypeMap.find(sensorStr) == sensorTypeMap.end()) {
		return NULL;
	}
	SensorType sensorType = sensorTypeMap.at(sensorStr);
	return getSensor(idx, sensorType);
}

bool SensorHandler::hasSensor(int idx, std::string sensorStr) {
	return getSensor(idx, sensorStr) != NULL;
}

void SensorHandler::switchActiveSensor(int idx) {
	_ioExpander->digitalWrite(IO_ENABLE_PIN, LOW);
	_ioExpander->digitalWrite(IO_ADDRESS_PIN_0, (idx & 1) > 0);
	_ioExpander->digitalWrite(IO_ADDRESS_PIN_1, (idx & 2) > 0);
	_ioExpander->digitalWrite(IO_ADDRESS_PIN_2, (idx & 4) > 0);
}

int SensorHandler::printMetrics(char* buffer) {
	int idx = 0;
	for (auto &sensor : sensors) {
		idx += sensor->printMetrics(&buffer[idx]);
	}
	return idx;
}

void IRAM_ATTR SensorHandler::update() {
	for (auto &sensor : sensors) {
		sensor->update();
	}
}

int SensorHandler::printConfiguration(char* buffer) {
	buffer[0] = '[';
	int idx = 1;
	for (auto &sensor : sensors) {
		std::string name = sensorNameMap.at(sensor->getSensorType());

		int n = sprintf(&buffer[idx], 
			"{\"index\":%d,\"type\":\"%s\"},",
			sensor->getIndex(), name.c_str());
		idx += n;
	}
	buffer[idx-1] = ']';
	buffer[idx] = '\0';
	return idx;

}

void SensorHandler::generateConfiguration(JsonArray* json) {
	for (auto &sensor : sensors) {
		JsonObject o = json->createNestedObject();
		std::string name = sensorNameMap.at(sensor->getSensorType());
		o["index"] = sensor->getIndex();
		o["type"] = name;
		if (sensor->getSensorType() == SensorType::SOIL_MOISTURE) {
			SoilMoistureSensor* sm = ((SoilMoistureSensor*) sensor);
			o["name"] = sm->getName();
			o["minLimit"] =	sm->getMinLimit(); 
			o["maxLimit"] =	sm->getMaxLimit(); 
		}
	}
}

// Sensor

Sensor::Sensor(SensorHandler *sensorHandler, std::string name) {
	_sensorHandler = sensorHandler;
	metricName = name;
	gauge = 0;
}

int Sensor::printMetrics(char* buffer) {
	return sprintf(buffer, "%s_gauge{index=\"%d\"} %.5f\n", 
		metricName.c_str(), idx, gauge);
}

void IRAM_ATTR Sensor::update() {
	gauge = readValue();
}

// AnalogSensor

AnalogSensor::AnalogSensor(SensorHandler *sensorHandler, 
	std::string name, int idx) : Sensor(sensorHandler, name) {

	this->idx = idx;
}

// DigitalSensor

DigitalSensor::DigitalSensor(SensorHandler *sensorHandler, 
	std::string name, int idx, int port) : Sensor(sensorHandler, name) {

	this->idx = idx;
	multiplexerPort = port;
}

// I2CSensor

I2CSensor::I2CSensor(SensorHandler *sensorHandler, 
	std::string name, int idx, int address) : Sensor(sensorHandler, name) {
	this->idx = idx;
	i2cAddress = address;
}

// SoilMoistureSensor

SoilMoistureSensor::SoilMoistureSensor(SensorHandler *sensorHandler, int idx, 
	float minLimit, float maxLimit, std::string plantName) 
		: AnalogSensor(sensorHandler, "soil_moisture", idx) {

	this->maxLimit = maxLimit;
	this->minLimit = minLimit;
	this->plantName = plantName;
};

int SoilMoistureSensor::printMetrics(char* buffer) {
	return sprintf(buffer, "%s_gauge{index=\"%d\", name=\"%s\"} %.5f\n", 
		metricName.c_str(), idx, plantName.c_str(), gauge);
}



float SoilMoistureSensor::readValue() {
	_sensorHandler->switchActiveSensor(idx);
	int soilMoisture = analogRead(A0);
	float rawValue = 1.0 - soilMoisture / 1024.0;
	return (rawValue - minLimit) / (maxLimit - minLimit);
}

std::string SoilMoistureSensor::getName() {
	return plantName;
}

float SoilMoistureSensor::getMinLimit() {
	return minLimit;	
}

float SoilMoistureSensor::getMaxLimit() {
	return maxLimit;	
}

// AmbientLightSensor

AmbientLightSensor::AmbientLightSensor(SensorHandler *sensorHandler) 
		: I2CSensor(sensorHandler, "ambient_light", 0, 0x4A) {
	
	if(light.begin()) {
		Logger::log("Could not find a valid MAX44009 sensor, check wiring!");
		// delay(1000);
		// ESP.restart();
	} else {
		enabled = true;
	}
}

float AmbientLightSensor::readValue() {
	if (enabled) {
		return light.get_lux();
	}
	return 0;
}

// BarometricSensor

BarometricSensor::BarometricSensor(SensorHandler *sensorHandler) 
		: I2CSensor(sensorHandler, "barometric", 0, 0x00) {
	
	if(!barometer.begin()) {
		Logger::log("Could not find a valid BMP085 sensor, check wiring!");
		// delay(1000);
		// ESP.restart();
	} else {
		enabled = true;
	}
}

float BarometricSensor::readValue() {
	if (enabled) {
		return barometer.readPressure();
	}
	return 0;
}

// TemperatureSensor

TemperatureSensor::TemperatureSensor(SensorHandler *sensorHandler) 
	: I2CSensor(sensorHandler, "temperature", 0, 0x00) {

	if(!barometer.begin()) {
		Logger::log("Could not find a valid BMP085 sensor, check wiring!");
		// delay(1000);
		// ESP.restart();
	} else {
		enabled = true;
	}
}

float TemperatureSensor::readValue() {
	if (enabled) {
		return barometer.readTemperature();
	}
	return 0;
}

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
