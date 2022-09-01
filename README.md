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

### Sensor configuration

The configuration of the watering system is set by posting a JSON to the `/configuration` endpoint. There's currently a known bug where the system might hang when large json post bodies are sent to the watering system using POST requests. To circumvent this issue, minify the JSON before sending it. In the future, there might be separate endpoints for updating sensors, motors and watering controllers to avoid this issue completely. 

#### Format

    {
    	"sensors" [...],
    	"motors": [...],
    	"watering_controllers": [...]
    }

Each sensor object has a `type` and an `index`. The type can be one of the following:

- `soil_moisture`
- `temperature`
- `dht_temperature`
- `humidity`
- `ambient_light`
- `barometric`
- `heat_index`

The sensor types are defined in `sensor_handler.cpp`. The index value is used to look up sensors using the `/sensor` endpoint, but might be removed in the future. For simplicity, it should be a unique number **per sensor type**, so the sensors will have index 0, 1, 2... for each type of sensor that is added. 

##### Soil moisture sensors

Soil moisture sensors also include the following fields:

- `name`: The name of the plant that the soil moisture sensor belongs to.
- `minLimit`: The soil moisture reading that should correspond to 0% soil moisture
- `maxLimit`: The soil moisture reading that should correspond to 100% soil moisture

`minLimit` and `maxLimit` are needed to calibrate individual soil moisture sensors. In the future, they might be dynamically calculated, but for now they are manually entered. 

##### Motors

Each motor has the following attributes:

- `index`: The motor pin, indexed from 0-5 (inclusive) corresponding to the PCB motor connections going from top to bottom
- `type`: should always be `"motor"`. Should be removed in the future and automatically added
- `activations`: A number specifying how many times the specific motor has been activated. Only useful for tracking statistics if you need to update the configuration down the line. Set it to 0 otherwise

##### Watering controllers

Each watering controller has the following attributes:

- `sensor_index`: The index of the soil moisture sensor (as defined above)
- `motor_index`: The index of the motor (as defined above)
- `type`: Should always be `"watering_controller"`. Should be removed in the future and automatically added

A watering controller connects a single soil moisture sensor and a pump (motor). The logic is quite simple, once the soil moisture gets low, it activates the pump for a period of time (a little over a minute). It is currently not possible to specify the thresholds for when the pump should activate, but that could be an interesting feature.

