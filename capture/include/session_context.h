#ifndef SESSION_CONTEXT_H
#define SESSION_CONTEXT_H

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

struct TelemetrySample
{
    int64_t packet_id;
    float speed_kmh;
    int rpms;
    int gear;
    float gas;
    float brake;
    float steer_angle;
};

struct SessionContext
{
    uint32_t session_id;
    std::wstring track;
    std::wstring car;

    std::vector<TelemetrySample> pending_samples;
    std::chrono::steady_clock::time_point last_flush_time;
};

#endif
