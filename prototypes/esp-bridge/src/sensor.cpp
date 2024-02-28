#include <Arduino.h>

// This is the base class for all sensors SwitchSensor

enum class DataType
{
    BOOLEAN,
    INTEGER,
    FLOAT,
};
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
    // variable to hold data type for the sensor reading
    DataType dataType;

protected:
    int _pin;
};