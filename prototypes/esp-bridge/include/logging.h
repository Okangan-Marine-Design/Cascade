#include <ArduinoLog.h>

void configureLogging(int log_level, Print &output);
void printPrefix(Print *_logOutput, int logLevel);
void printTimestamp(Print *_logOutput);
void printLogLevel(Print *_logOutput, int logLevel);
void printSuffix(Print *_logOutput, int logLevel);