#include "metrics_handler.h"

MetricsHandler::MetricsHandler(SensorHandler *sensorHandler, 
	MotorHandler *motorHandler) {

	_sensorHandler = sensorHandler;
	_motorHandler = motorHandler;
}

int MetricsHandler::printMetrics(char* buffer) {
	int idx = _sensorHandler->printMetrics(buffer);
	idx += _motorHandler->printMetrics(&buffer[idx]);

	idx += sprintf(&buffer[idx], "esp_free_ram %d\n", ESP.getFreeHeap());
	idx += sprintf(&buffer[idx], 
		"esp_heap_fragmentation %d\n", ESP.getHeapFragmentation());
	idx += sprintf(&buffer[idx], 
		"esp_max_free_block_size %d\n", ESP.getMaxFreeBlockSize());
	idx += sprintf(&buffer[idx], "esp_uptime %lu\n", millis());
	return idx;
}

void MetricsHandler::update() {
	_sensorHandler->update();
}