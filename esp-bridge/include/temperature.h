#include <sensor.h>

class TemperatureSensor : public Sensor
{
public:
        using Sensor::Sensor; // Constructor
    double getReading();
};