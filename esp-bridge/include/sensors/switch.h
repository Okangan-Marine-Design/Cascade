#pragma once
#include "sensor.h"

class SwitchSensor : public Sensor
{
public:
    using Sensor::Sensor; // Constructor

    bool getReading(); // Method
};