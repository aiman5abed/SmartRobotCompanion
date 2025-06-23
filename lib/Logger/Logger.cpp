// Logger.cpp
#include "Logger.h"
#include <Arduino.h>    // for millis()
#include <string>

Logger::Logger() {
    logs.reserve(100);  // optional: pre-allocate for performance
}

std::string Logger::getTimestamp() const {
    unsigned long ms = millis();
    unsigned long secs = ms / 1000;
    unsigned long rem = ms % 1000;

    // zero-pad milliseconds to three digits
    char buf[16];
    snprintf(buf, sizeof(buf), "%lu.%03lus", secs, rem);
    return std::string(buf);
}

void Logger::logEvent(const std::string& event) {
    std::string entry = getTimestamp() + " - " + event;
    logs.push_back(entry);
}

const std::vector<std::string>& Logger::getLogs() const {
    return logs;
}

void Logger::clearLogs() {
    logs.clear();
}
