#include <Arduino.h>

class Sensor
{
public:
    Sensor(int pin, String name) : _pin(pin), name(name)
    {
        pinMode(_pin, INPUT);
    }

    template <typename T>
    T getReading(); // Sensor reading method with a template so we can return different types of data

    String name;

protected:
    int _pin;
};