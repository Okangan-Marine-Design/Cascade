#include <Arduino.h>

// Set up the SensorReturnBaseType class
class SensorReturnBaseType
{
public:
    virtual void getValue(int &i) { i = 0; };
    virtual void getValue(double &d) { d = 0.0; };
    virtual void getValue(bool &b) { b = false; };
};

class SensorReturnIntType : public SensorReturnBaseType
{
public:
    virtual void getValue(int &i) { i = value; };
    virtual void getValue(double &d) { d = static_cast<double>(value); };
    virtual void getValue(bool &b) { b = value; };

private:
    int value;
};

class SensorReturnDblType : public SensorReturnBaseType
{
public:
    virtual void getValue(int &i) { i = static_cast<int>(value); };
    virtual void getValue(double &d) { d = value; };
    virtual void getValue(bool &b) { b = value; };

private:
    double value;
};

class SensorReturnBoolType : public SensorReturnBaseType
{
public:
    virtual void getValue(int &i) { i = static_cast<int>(value); };
    virtual void getValue(double &d) { d = value; };
    virtual void getValue(bool &b) { b = value; };

private:
    bool value;
};

class SensorReturnBaseType; // Add missing class declaration

class Sensor
{
public:
    Sensor(int pin, String name) : _pin(pin), name(name)
    {
        pinMode(_pin, INPUT);
    }

    SensorReturnBaseType getReading();

    String name;

protected:
    int _pin;
};
