#include <chrono>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>

#include "../include/httplib.h"
#include "../include/json.h"

#include "../include/ac_shared_memory.h"
#include "../include/session_context.h"

#define PHYSICS_MAPPING  0
#define GRAPHICS_MAPPING 1
#define STATICS_MAPPING  2

const auto FLUSH_INTERVAL = std::chrono::seconds(2);

using json = nlohmann::json;

void buffer_telemetry(const SPageFilePhysics *physics, const SPageFileGraphic *graphics, SessionContext &ctx)
{
    TelemetrySample sample = {
        .packet_id   = physics->packetId,
        .speed_kmh   = physics->speedKmh,
        .rpms        = physics->rpms,
        .gear        = physics->gear,
        .gas         = physics->gas,
        .brake       = physics->brake,
        .steer_angle = physics->steerAngle
    };

    ctx.pending_samples.push_back(sample);
}

void setup(void **buffer)
{
    HANDLE ac_phys_mapping_handle     { OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_physics") };
    HANDLE ac_graphics_mapping_handle { OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_graphics") };
    HANDLE ac_static_mapping_handle   { OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_static") };

    buffer[PHYSICS_MAPPING]  = MapViewOfFile(ac_phys_mapping_handle, FILE_MAP_READ, 0, 0, 0);
    buffer[GRAPHICS_MAPPING] = MapViewOfFile(ac_graphics_mapping_handle, FILE_MAP_READ, 0, 0, 0);
    buffer[STATICS_MAPPING]  = MapViewOfFile(ac_static_mapping_handle, FILE_MAP_READ, 0, 0, 0);

    CloseHandle(ac_phys_mapping_handle);
    CloseHandle(ac_graphics_mapping_handle);
    CloseHandle(ac_static_mapping_handle);
}

void update(
    void **buffer,
    void (*func)(const SPageFilePhysics *, const SPageFileGraphic *, SessionContext &),
    httplib::Client &client
)
{
    // Cast buffer sections to appropriate shared memory structs
    auto const *physics  { static_cast<SPageFilePhysics const *>(buffer[PHYSICS_MAPPING]) };
    auto const *graphics { static_cast<SPageFileGraphic const *>(buffer[GRAPHICS_MAPPING]) };
    auto const *statics  { static_cast<SPageFileStatic const *>(buffer[STATICS_MAPPING]) };

    while(true)
    {
        // Check for if a game session is ACTIVE/LIVE
        // Waits continuously until a session is started
        while(graphics->status != AC_LIVE) { Sleep(500); }

        std::wcout << L"Car Model: " << statics->carModel << std::endl;
        std::wcout << L"Track: " << statics->track << std::endl;

        SessionContext ctx;
        ctx.track = statics->track;
        ctx.car = statics->carModel;
        ctx.session_id = 0;

        // Todo: Post to /session with track and car, parse session_id from response
        ctx.last_flush_time = std::chrono::steady_clock::now();

        int64_t last_packet_id { -1 };
        while(graphics->status == AC_LIVE)
        {
            if(physics->packetId != last_packet_id)
            {
                func(physics, graphics, ctx);
                last_packet_id = physics->packetId;
            }

            auto now = std::chrono::steady_clock::now();
            if (now - ctx.last_flush_time >= FLUSH_INTERVAL && !ctx.pending_samples.empty())
            {
                // TODO: send_batch(cli, ctx) — POST ctx.pending_samples, then ctx.pending_samples.clear()
                ctx.last_flush_time = now;
            }

            Sleep(5);
        }
    }
}

int main()
{
    // Establish connection and set timeouts at (1) second
    // Run health check to ensure backend responds as expected
    httplib::Client cli("127.0.0.1", 8000);
    cli.set_connection_timeout(1);
    cli.set_read_timeout(1);
    cli.set_write_timeout(1);

    auto res = cli.Get("/health");
    if(!res)
    {
        std::cout << "[FAILED CONNECTION] Reason: [" << res.error() << "]" << std::endl;
        return -1;
    }

    if(res->status != 200)
    {
        std::cout << "Status code: [" << res->status << "]" << std::endl;
        std::cout << "Expected [200]" << std::endl;
        return -1;
    }

    // Initialise buffer to hold shared memory structs
    // Loops through shared memory data for seeding the backend
    void *buffer[3];
    setup(buffer);
    update(buffer, buffer_telemetry, cli);
    return 0;
}
