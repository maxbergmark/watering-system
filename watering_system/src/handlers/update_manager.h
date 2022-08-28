#pragma once

#include <ArduinoOTA.h>

#include "../helpers/logger.h"

class UpdateManager {

private:

public:
	UpdateManager();
	void begin();
	void handle();
};