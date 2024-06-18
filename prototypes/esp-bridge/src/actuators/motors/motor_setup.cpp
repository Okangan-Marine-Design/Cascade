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
        String name = thruster["name"];
        int id = thruster["id"];
        int pin = thruster["pin"];

        bool success = ledcSetup(id, 50, 16);
        ledcAttachPin(pin, id);

        // determine midpoints for the thruster
        int max_pulse = thruster["max_pulse"];
        int min_pulse = thruster["min_pulse"];

        int zero_pulse = (max_pulse + min_pulse) / 2;
        // Don't ask me how this works it's completely magic and nobody understands why it works
        int magic_pulse = map(zero_pulse, 1000, 2000, 3275, 6553);
        ledcWrite(id, magic_pulse);

        // setup ledc channel to output PWM signal to the thruster
        if (!success)
        {
            Log.errorln("Failed to setup LEDC channel for thruster %d", id);
            return;
        }
        else
        {
            Log.noticeln("Configured %S on pin %d with id %d", name, pin, id);
        }
    }
}