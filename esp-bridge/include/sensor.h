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