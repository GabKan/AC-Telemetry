#include <chrono>
#include <iostream>
#include <cstdint>

#include "../include/httplib.h"

#include "../include/ac_shared_memory.h"
#include "../include/session_context.h"
#include "../include/logging.h"
#include "../include/network_utils.h"
#include "../include/session_utils.h"
#include "../include/shared_memory_utils.h"
#include "../include/telemetry.h"

const auto FLUSH_INTERVAL = std::chrono::seconds(2);

void update(
    void **buffer,
    void (*func)(const SPageFilePhysics *, const SPageFileGraphic *, SessionContext *),
    httplib::Client *cli
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
        log_line(LogLevel::LOG_INFO, "Waiting to establish AC session.");
        while(graphics->status != AC_LIVE) { Sleep(500); }

        log_line(LogLevel::LOG_INFO, "AC session is LIVE.");
        std::wcout << L"\nCar Model: " << statics->carModel << std::endl;
        std::wcout << L"Track: \n" << statics->track << std::endl;

        // Todo: Post to /session with track and car, parse session_id from response
        SessionContext *ctx = create_session(
            cli,
            graphics->session,
            statics->track,
            statics->carModel
        );
        if (!ctx)
        {
            log_line(LogLevel::LOG_WARN, "Session creation failed. Retrying in 2 seconds.");
            Sleep(2000);
            continue;
        }

        ctx->last_flush_time = std::chrono::steady_clock::now();
        int64_t last_packet_id { -1 };
        while(graphics->status == AC_LIVE)
        {
            if(physics->packetId != last_packet_id)
            {
                func(physics, graphics, ctx);
                last_packet_id = physics->packetId;
            }

            auto now = std::chrono::steady_clock::now();
            if (now - ctx->last_flush_time >= FLUSH_INTERVAL && !ctx->pending_samples.empty())
            {
                // TODO: send_batch(cli, ctx) — POST ctx.pending_samples, then ctx.pending_samples.clear()
                send_batch(cli, ctx);
                ctx->last_flush_time = now;
            }

            Sleep(5);
        }

        // Drain on session exit
        if(!ctx->pending_samples.empty())
            send_batch(cli, ctx);
        log_line(LogLevel::LOG_INFO, "Session ended.");
        delete ctx;
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

    wait_for_backend(cli, 10);

    // Initialise buffer to hold shared memory structs
    // Loops through shared memory data for seeding the backend
    void *buffer[3];
    setup(buffer);
    update(buffer, buffer_telemetry, &cli);

    return 0;
}
