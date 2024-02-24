
#include <Arduino.h>

class Sensor
{
public:
    Sensor(String name, int pin);
    virtual ~Sensor();

    template <typename T>
    T getReading();

    String name;

protected:
    int _pin;
};