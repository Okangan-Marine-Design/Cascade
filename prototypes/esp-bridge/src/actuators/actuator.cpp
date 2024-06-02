#include <ArduinoJson.h>
#include <ESP32Servo.h>

void control_actuators(JsonDocument actuators, JsonDocument actuator_config)
{
    // first get the thrusters object
    JsonArray thrusters = actuators["thrusters"];
    JsonArray thrusters_config = actuator_config["thrusters"];

    // next get the motors object
    JsonArray motors = actuators["motors"];
    JsonArray motors_config = actuator_config["motors"];

    // then get the servos object
    JsonArray servos = actuators["servos"];
    JsonArray servos_config = actuator_config["servos"];

    // determine how many objects are in the thruster array
    int thruster_count = thrusters.size();

    // determine how many objects are in the motor array
    int motor_count = motors.size();

    // determine how many objects are in the servo array
    int servo_count = servos.size();

    // right now we only care about the thrusters
    for (int i = 0; i < thruster_count; i++)
    {
        // get the thruster config object
        JsonObject thruster = thrusters[i];
        JsonObject thruster_config = thrusters_config[i];

        int max_pulse = thruster_config["max_pulse"];
        int min_pulse = thruster_config["min_pulse"];
        int dead_zone = thruster_config["dead_zone"];

        // get the speed from the thruster object
        int speed = thruster["speed"];
        int id = thruster["id"];

        int mapped_pulse = map(speed, -100, 100, min_pulse, max_pulse);
        ledcWrite(id, mapped_pulse);
    }
}
