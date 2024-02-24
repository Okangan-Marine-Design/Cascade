#include <sensor.h>
#include <logging.h>

class LeakSensor : public Sensor
{
public:
    LeakSensor(int pin, String name); // Constructor
    bool getReading();                // Method
};

// set up getReading method
bool LeakSensor::getReading()
{
    Log.infoln("Getting leak sensor reading...");
    // for now, we don't have a leak sensor so we'll just return false as there is no leak under normal conditions
    return false;
}
