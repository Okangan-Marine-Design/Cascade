#include <Arduino.h>

class Sensor
{
public:
    Sensor(int pin, String name); // Constructor
    void getReading();            // Method
    String name;                  // The name of the sensor

private:
    int _pin; // Member variable
};

Sensor::Sensor(int pin, String name) // Constructor
{
    _pin = pin;
    name = name;

    pinMode(_pin, INPUT);
}
