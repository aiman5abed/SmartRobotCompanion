// Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <string>

class Logger {
public:
    Logger();
    /// Record an event with a timestamp (in seconds since startup)
    void logEvent(const std::string& event);

    /// Retrieve all logged entries
    const std::vector<std::string>& getLogs() const;

    /// Clear the log buffer
    void clearLogs();

private:
    std::vector<std::string> logs;

    /// Helper to get current timestamp as “seconds.milliseconds s”
    std::string getTimestamp() const;
};

#endif // LOGGER_H
