#include <sensor.h>

class TemperatureSensor : public Sensor
{
public:
    TemperatureSensor(int pin, String name); // Constructor
    double getReading();                     // Method
};
