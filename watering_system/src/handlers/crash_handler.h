#pragma once

#include <cont.h>
#include <Arduino.h>

#include "../helpers/eeprom.h"

String loadStack();
String loadResetInfo();
int getStack(uint32_t starter, uint32_t ender, char* buffer);

extern "C" void custom_crash_callback(struct rst_info* rst_info, 
	uint32_t stack, uint32_t stack_end);