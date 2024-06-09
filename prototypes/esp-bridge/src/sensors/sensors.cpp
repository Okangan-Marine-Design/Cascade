#include <ArduinoJson.h>
#include <logging.h>
#include <sensors/temperature_lm35.h>

bool shouldLog();

JsonDocument readSensors(JsonDocument &config)
{
    // Create a new JSON document to store the sensor data
    JsonDocument sensorData;

    // Get the sensor data from the config
    JsonObject sensors = config["sensors"];

    // get lm35 sensors defined in the config

    JsonArray lm35Sensors = sensors["temperature_lm35"];

    // iterate over the lm35 sensors
    for (JsonVariant sensor : lm35Sensors)
    {
        // get the pin from the sensor
        int pin = sensor["pin"];
        int id = sensor["id"];

        // get the temperature from the sensor
        float temperature = getTemperatureLM35(pin);

        // add the temperature to the sensor data
        sensorData["temperature_lm35"][id] = temperature;

        // log the temperature if needed
        if (shouldLog())
        {
            Log.infoln("Temperature LM35: %f on pin %d", temperature, pin);
        }
    }

    return sensorData;
}

bool shouldLog()
{
    if (millis() % 1000 == 0)
    {
        return true;
    }
    return false;
}

void setupSensors(JsonDocument &config)
{
    // Create a new JSON document to store the sensor data
    JsonDocument sensorData;

    // Get the sensor data from the config
    JsonObject sensors = config["sensors"];

    // get lm35 sensors defined in the config

    JsonArray lm35Sensors = sensors["temperature_lm35"];

    // iterate over the lm35 sensors
    for (JsonVariant sensor : lm35Sensors)
    {
        // get the pin from the sensor
        int pin = sensor["pin"];
        int id = sensor["id"];

        // setup the sensor
        pinMode(pin, INPUT);
        Log.infoln("LM35 sensor %d setup on pin %d", id, pin);
    }

    Log.infoln("Sensors setup complete");
    return;
}