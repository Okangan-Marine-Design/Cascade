#include <Arduino.h>

float getTemperatureLM35(int pin)
{
    // Get the pin from the config
    float tempC = (float(analogRead(pin)) * 5 / (1023)) / 0.01;

    return tempC;
}