#include <sensor.h>

class LeakSensor : public Sensor
{
public:
    LeakSensor(int pin, String name); // Constructor
    bool getReading();                // Method
};
