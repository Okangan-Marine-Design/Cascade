#include <Arduino.h>
#include <logging.h>

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
  delay(1000);
  digitalWrite(2, LOW);
  delay(1000);
  // get the temperature reading from the sensor
  // double tempReading = tempSensor.getReading();
  // Log.infoln(F("Temperature reading: %D"), tempReading);
}
