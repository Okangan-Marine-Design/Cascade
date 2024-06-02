#include <Arduino.h>
#include <ArduinoJson.h>

// create a buffer for the incoming data

JsonDocument input_comms()
{
    // read the incoming data from the serial port and store it in the buffer
    // we only want to read the data if it is available
    // we don't want to block the program if there is no data available
    char buffer[2048];
    JsonDocument doc;

    if (Serial.available() > 0)
    {
        // read the incoming data from the serial port and store it in the buffer
        Serial.readBytesUntil('\n', buffer, sizeof(buffer));
        deserializeJson(doc, buffer);

        // print the incoming data to the serial port
        serializeJsonPretty(doc, Serial);
        Serial.println();
    }
    return doc;
}