#include "io_expander.h"
#include "logger.h"

IOExpander::IOExpander() {}

MCPExpander::MCPExpander() : IOExpander() {

}

bool MCPExpander::begin() {
	return mcp.begin_I2C();
}

void MCPExpander::pinMode(int pin, bool mode) {
	mcp.pinMode(pin, mode);
}

void IRAM_ATTR MCPExpander::digitalWrite(int pin, bool state) {
	mcp.digitalWrite(pin, state);
}


PCFExpander::PCFExpander() : IOExpander() {

}

bool PCFExpander::begin() {
	bool b = pcf.begin();
	pcf.write16(0);
	return b;
}

void PCFExpander::pinMode(__attribute__((unused)) int pin, __attribute__((unused)) bool mode) {
	// char str[50];
	// sprintf(str, "Setting pin %d to %d", pin, mode);
	// Logger::log(str);
}

void IRAM_ATTR PCFExpander::digitalWrite(int pin, bool state) {
	pcf.write(pin, state);
	// char s[50];
	// sprintf(s, "Writing %d to %d", state, pin);
	// Logger::log(s);
}

