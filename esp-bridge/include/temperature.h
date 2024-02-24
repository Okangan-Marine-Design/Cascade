#include <sensor.h>

class TemperatureSensor : public Sensor
{
public:
    TemperatureSensor(int pin, String name);

    double getReading();
};