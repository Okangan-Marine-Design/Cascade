#pragma once
#include <Arduino.h>
class Sensor
{
public:
    Sensor(int pin, String name) : _pin(pin), name(name) {}

    template <typename T>
    T getReading();

    String name;

protected:
    int _pin;
};

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
    virtual void getValue(int &i) override;
    virtual void getValue(double &d) override;
    virtual void getValue(bool &b) override;

private:
    int value;
};

class SensorReturnDblType : public SensorReturnBaseType
{
public:
    virtual void getValue(int &i) override;
    virtual void getValue(double &d) override;
    virtual void getValue(bool &b) override;

private:
    double value;
};

class SensorReturnBoolType : public SensorReturnBaseType
{
public:
    virtual void getValue(int &i) override;
    virtual void getValue(double &d) override;
    virtual void getValue(bool &b) override;

private:
    bool value;
};