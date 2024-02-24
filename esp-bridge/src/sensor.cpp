#include <Arduino.h>

class Sensor
{
public:
    Sensor(String name, int pin) : name(name), _pin(pin)
    {
        pinMode(_pin, INPUT);
    }
    virtual ~Sensor() {}

    template <typename T>
    T getReading(); // Sensor reading method with a template so we can return different types of data

    String name;

protected:
    int _pin;
};