#include "../include/time_utils.h"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdio>

std::string get_iso_timestamp()
{
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    if (gmtime_s(&tm, &t) != 0)
    {
        fprintf(stderr, "[WARN] gmtime_s failed, using epoch fallback.\n");
        return "1970-01-01T00:00:00Z";
    }
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}
