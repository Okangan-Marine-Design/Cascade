#include <sensor.h>
#include <logging.h>

class SwitchSensor : public Sensor
{
public:
    using Sensor::Sensor; // Constructor
    bool getReading();    // Method
};

// set up getReading method
bool SwitchSensor::getReading()
{
    // for now, we don't have a leak sensor, so we'll just return false
    return false;
}