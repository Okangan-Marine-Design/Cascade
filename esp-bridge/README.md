# ESP Bridge

This code is the bridge between the electronics and the Jetson Orin Nano. It passes data 2 ways.

## Features

- Logging ✅ Implemented
- Sensor Interface ✅ Implemented
- Wifi Debug Portal ❌ Not yet implemented
- Serial Communication ❌ Not yet implemented
- Temperature Sensor ⚠️ Half implemented (sends fake data)
- Leak Sensor ⚠️ Half implemented (sends fake data)
- Current Sensor ❌ Not yet implemented
- Hydrophone ❌ Not yet implemented
- Kill Switch ⚠️ Half implemented (sends fake data)
- Motor Control ❌ Not yet implemented
- LED Status ❌ Not yet implemented
- LED Control ❌ Not yet implemented

## General Program Structure

Program Start

1. Initialize Serial Communication

2. Initialize Logging

3. Initialize Sensors
   Sensors are contained in individual files that are imported into the main program. Each sensor has its own class and methods. The general structure of the sensor class is as follows:

```c++
class Sensor
{
public:
    Sensor(int pin);   // Constructor
    void getReading(); // Method

private:
    int _pin; // Member variable
};

Sensor::Sensor(int pin) // Constructor
{
    _pin = pin;
}
```

We use polymorphism to create a common interface for all sensors. This allows us to easily add more sensors in the future.

4. Build the JSON Object
   The JSON object is built from the sensor data. The JSON object is then sent to the Jetson Orin Nano.
   This means that we can add more sensors easily by adding more data to the JSON object.

5. Send the JSON Object
   The JSON object is sent to the Jetson Orin Nano over serial communication.

6. Receive the JSON Object
   The JSON object is received from the Jetson Orin Nano over serial communication.

7. Parse the JSON Object
   The JSON object is parsed and the data is used to control the motors and LEDs.

8. Heartbeat
   The program checks to see when the last time it received a JSON object from the Jetson Orin Nano. If it has been too long, the program will trigger an emergency mode.

## Naming Conventions

- Class names are capitalized
- Member variables are prefixed with an underscore
- Method names are camel case
- Constants are all caps
- File names are all lowercase (except for README.md)
- For sensor files, the file name is the same as the sensor name without the word "sensor" (e.g. temperature.cpp) or (e.g. leak.cpp)
