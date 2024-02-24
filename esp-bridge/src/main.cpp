#include <Arduino.h>
#include <logging.h>
#include <temperature.h>

TemperatureSensor tempSensor(2, "temperature_1");

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  configureLogging(LOG_LEVEL_VERBOSE, Serial);

  Log.noticeln("Starting up...");

  Serial.println(tempSensor.name);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(2, LOW);
  delay(1000);
  // get the temperature reading from the sensor
  Log.infoln(F("Temperature reading: %D"), tempSensor.getReading());
}
