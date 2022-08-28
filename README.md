# Watering System


## Setup

### Libraries

- Esp.h (included when building for ESP8266)
- [ArduinoJson](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)
- [Adafruit MCP23017 Arduino Library](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library)
- [PCF8575](https://github.com/RobTillaart/PCF8575)
- [MAX44009 library](https://github.com/dantudose/MAX44009)
- DHT sensor library for ESPx
- [Adafruit BMP085 Library](https://github.com/adafruit/Adafruit-BMP085-Library)
- [base64](https://github.com/Densaugeo/base64_arduino)

### WiFi connection

Add a file under `watering_system/src/handlers/wifi_pass.h` containing the following:

    const char* ssid = "...";
	const char* password = "...";

where the correct SSID and password is substituted. 