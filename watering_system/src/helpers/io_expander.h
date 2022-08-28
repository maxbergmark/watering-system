#pragma once

#include <Adafruit_MCP23X17.h>
#include <PCF8575.h>

class IOExpander {

private:

public:
	IOExpander();
	virtual bool begin() = 0;
	virtual void pinMode(int pin, bool mode) = 0;
	virtual void digitalWrite(int pin, bool state) = 0;
};

class MCPExpander : public IOExpander {

private:
	Adafruit_MCP23X17 mcp;

public:
	MCPExpander();
	virtual bool begin();
	virtual void pinMode(int pin, bool mode);
	virtual void digitalWrite(int pin, bool state);
};

class PCFExpander : public IOExpander {

private:
	PCF8575 pcf;

public:
	PCFExpander();
	virtual bool begin();
	virtual void pinMode(int pin, bool mode);
	virtual void digitalWrite(int pin, bool state);
};