#include "../include/network_utils.h"
#include "../include/logging.h"

bool wait_for_backend(httplib::Client &cli, int max_attempts)
{
    for (int attempt { 1 }; attempt <= max_attempts; ++attempt)
    {
        auto res = cli.Get("/health");
        if (res && res->status == 200)
        {
            log_line(LogLevel::LOG_INFO, "Backend connection established.");
            return true;
        }

        log_line(LogLevel::LOG_WARN, "Backend not ready (attempt ", attempt, "/", max_attempts, "). Retrying in 1 second.");
        Sleep(1000);
    }
    return false;
}
