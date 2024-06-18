#include <Arduino.h>
#include <ArduinoJson.h>
#include <logging.h>

// create a buffer for the incoming data

JsonDocument input_comms()
{
    // get the time this function takes
    unsigned long start = millis();
    static StaticJsonDocument<256> json_doc;

    const auto deser_err = deserializeJson(json_doc, Serial);
    if (deser_err)
    {
        Serial.print(F("Failed to deserialize, reason: \""));
        Serial.print(deser_err.c_str());
        Serial.println('"');
    }
    else
    {
        Serial.print(F("Recevied valid json document with "));
        Serial.print(json_doc.size());
        Serial.println(F(" elements."));
        Serial.println(F("Pretty printed back at you:"));
        serializeJsonPretty(json_doc, Serial);
        Serial.println();
    }
    unsigned long delta = millis() - start;
    Serial.print(F("Time taken to process incoming data: "));

    return json_doc;
}