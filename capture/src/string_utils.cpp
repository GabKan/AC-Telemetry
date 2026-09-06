#include "../include/string_utils.h"
#include "../include/logging.h"

#include <windows.h>

std::string wstring_to_utf8(const std::wstring &wide)
{
    if (wide.empty()) return "";

    int size = WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), nullptr, 0, nullptr, nullptr);
    if (size <= 0)
    {
        log_line(LogLevel::LOG_WARN, "wstring_to_utf8 size query failed. GetLastError=", GetLastError());
        return "";
    }

    std::string result(size, 0);
    int written = WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), result.data(), size, nullptr, nullptr);
    if (written <= 0)
    {
        log_line(LogLevel::LOG_WARN, "wstring_to_utf8 conversion failed. GetLastError=", GetLastError());
        return "";
    }

    return result;
}
