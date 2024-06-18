#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <ArduinoLog.h>

void control_actuators(JsonDocument &actuators, JsonDocument &config)
{
    Log.infoln("Controlling actuators");
    serializeJsonPretty(actuators, Serial);
    Serial.println();
    // first get the thrusters object
    JsonArray thrusters = actuators["actuators"]["thrusters"];
    JsonArray thrusters_config = config["actuators"]["thrusters"];

    // next get the motors object
    JsonArray motors = actuators["actuators"]["motors"];
    JsonArray motors_config = config["actuators"]["motors"];

    // then get the servos object
    JsonArray servos = actuators["actuators"]["servos"];
    JsonArray servos_config = config["actuators"]["servos"];

    // determine how many objects are in the thruster array
    int thruster_count = thrusters.size();

    // determine how many objects are in the motor array
    int motor_count = motors.size();

    // determine how many objects are in the servo array
    int servo_count = servos.size();

    Log.infoln("Found %d thrusters, %d motors, and %d servos in json data", thruster_count, motor_count, servo_count);

    // right now we only care about the thrusters
    for (int i = 0; i < thruster_count; i++)
    {
        // get the thruster config object
        JsonObject thruster = thrusters[i];
        JsonObject thruster_config = thrusters_config[i];

        Log.infoln("Setting thruster %s", thruster["name"]);

        int max_pulse = thruster_config["max_pulse"];
        int min_pulse = thruster_config["min_pulse"];
        int dead_zone = thruster_config["dead_zone"];

        // get the speed from the thruster object
        int speed = thruster["speed"];
        int id = thruster["id"];

        // log info
        Log.infoln("Thruster %s has speed %d and id %d", thruster["name"], speed, id);

        int mapped_pulse = map(speed, -100, 100, min_pulse, max_pulse);
        // Don't ask me how this works it's completely magic and nobody understands why it works
        int magic_pulse = map(mapped_pulse, 1000, 2000, 3275, 6553);
        Log.infoln("Mapped pulse: %d", mapped_pulse);
        ledcWrite(id, magic_pulse);
    }
}
