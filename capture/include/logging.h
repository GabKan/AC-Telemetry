#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <sstream>
#include <string>

#include "time_utils.h"

enum class LogLevel { LOG_INFO, LOG_WARN, LOG_ERROR };

template<typename... Args>
void log_line(LogLevel level, Args&&... args)
{
    std::ostringstream oss;
    (oss << ... << args);

    const char *tag = level == LogLevel::LOG_INFO  ? "INFO"
                     : level == LogLevel::LOG_WARN  ? "WARN"
                     : "ERROR";

    auto &out = (level == LogLevel::LOG_ERROR) ? std::cerr : std::cout;
    out << "[" << get_iso_timestamp() << "] [" << tag << "] " << oss.str() << std::endl;
}

#endif
