#include <ArduinoJson.h>
#include <logging.h>

void motor_setup(JsonDocument config)
{
    // find the actuators object
    JsonArray thrusters = config["actuators"]["thrusters"];

    // determine how many objects are in the thruster array
    int thruster_count = thrusters.size();

    Log.noticeln("Found %d thrusters in config", thruster_count);

    // loop through each thruster object and configure the output pins
    for (int i = 0; i < thruster_count; i++)
    {
        // index the thruster array to get the current thruster object
        JsonObject thruster = thrusters[i];
        // get the pin number from the current thruster object
        int pin = thruster["pin"];
        // set the pin mode to output
        pinMode(pin, OUTPUT);
        Log.noticeln("Configured thruster %d on pin %d", i, pin);
    }
}