#include <Arduino.h>
#include <logging.h>
#include <sensors/temperature.h>
#include <sensors/switch.h>

TemperatureSensor tempSensor1(2, "temperature_1");
SwitchSensor leakSensor1(3, "leak_1");
SwitchSensor killSwitch(4, "kill_switch");

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  configureLogging(LOG_LEVEL_VERBOSE, Serial);

  Log.noticeln("Starting up...");

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
  // get the temperature reading from the sensor
  Log.infoln(F("Temperature reading: %D"), tempSensor1.getReading());
  // get the leak reading from the sensor
  Log.infoln(F("Leak reading: %T"), leakSensor1.getReading());
  // get the kill switch reading from the sensor
  Log.infoln(F("Kill switch reading: %T"), killSwitch.getReading());
}
