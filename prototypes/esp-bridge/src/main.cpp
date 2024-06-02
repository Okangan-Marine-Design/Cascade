#include <Arduino.h>
#include <logging.h>
#include <vector>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "actuators/motors/motor_setup.h"
#include "comms/data_receiver.h"

// create global variables for bridge id, software version, and log level
const char *BRIDGE_ID;
const char *BRIDGE_SOFTWARE_VERSION;
int BRIDGE_LOG_LEVEL;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("ESP Bridge by Andre Cox");

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    Serial.println("This happened before logging was configured");
    Serial.println("Please check the SPIFFS configuration");
    return;
  }

  if (!SPIFFS.exists("/config.json"))
  {
    Serial.println("Config file does not exist");
    Serial.println("This happened before logging was configured");
    Serial.println("Please check if the config file exists in the SPIFFS filesystem");
    return;
  }

  File configFile = SPIFFS.open("/config.json", "r");

  if (!configFile)
  {
    Serial.println("Failed to open config file");
    Serial.println("This happened before logging was configured");
    Serial.println("Please check if the config file can be opened in the SPIFFS filesystem");
    return;
  }

  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  configFile.close();

  char json[size];

  for (int i = 0; i < size; i++)
  {
    json[i] = buf.get()[i];
  }

  JsonDocument config;
  deserializeJson(config, json);

  // get bridge id nested value under espbridge: { bridge_id: "1234" }
  JsonObject bridge = config["espbridge"];
  BRIDGE_ID = bridge["bridge_id"];

  // get bridge software version nested value under espbridge: { bridge_software_version: "1.0.0" }
  BRIDGE_SOFTWARE_VERSION = bridge["bridge_software_version"];

  // get bridge log level nested value under espbridge: { log_level: 1 }
  BRIDGE_LOG_LEVEL = bridge["log_level"];

  configureLogging(BRIDGE_LOG_LEVEL, Serial);

  Log.noticeln("Bridge ID: %s", BRIDGE_ID);
  Log.noticeln("Bridge Software Version: %s", BRIDGE_SOFTWARE_VERSION);
  Log.noticeln("Bridge Log Level: %d", BRIDGE_LOG_LEVEL);

  // setup the motor
  motor_setup(config);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // get input from the serial port
  JsonDocument input = input_comms();
  // check if there is data
  if (!input.isNull())
  {
    // print input to the serial port for debugging
  }
}