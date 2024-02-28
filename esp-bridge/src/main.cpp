#include <Arduino.h>
#include <logging.h>
#include <sensors/temperature.h>
#include <sensors/switch.h>
#include <vector>
#include <ArduinoJson.h>
#include <config/configuration.h>

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

void dataSender(Sensor *sensors[], int size)
{

  JsonDocument doc;
  doc["bridge_id"] = BRIDGE_ID;
  doc["bridge_software_version"] = BRIDGE_SOFTWARE_VERSION;
  doc["bridge_log_level"] = BRIDGE_LOG_LEVEL;

  for (int i = 0; i < size; i++)
  {
    JsonObject sensor = doc.createNestedObject(sensors[i]->name);
    sensor["value"] = sensors[i]->getReading<double>();
  }

  return;
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
  Sensor *sensors[] = {&tempSensor1, &leakSensor1, &killSwitch};
  dataSender(sensors, 3);
}
