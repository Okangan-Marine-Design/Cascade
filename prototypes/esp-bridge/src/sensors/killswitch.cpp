#include <Arduino.h>
#include <logging.h>

bool killswitch(int pin)
{
    bool kill = digitalRead(pin);

    return kill;
}

void setupKillswitch(int pin)
{
    pinMode(pin, INPUT);
    Log.infoln("Killswitch configured on pin %d", pin);
    return;
}