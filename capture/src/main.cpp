#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "../include/ac_shared_memory.h"
#include "../include/session_context.h"

#define PHYSICS_MAPPING  0
#define GRAPHICS_MAPPING 1
#define STATICS_MAPPING  2

void output_speed(const SPageFilePhysics *physics, const SPageFileGraphic *graphics, const SessionContext &ctx)
{
    printf("speed=%.2f rpms=%d\n", physics->speedKmh, physics->rpms);
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
    void (*func)(const SPageFilePhysics *, const SPageFileGraphic *, const SessionContext &)
)
{
    auto const *physics  { static_cast<SPageFilePhysics const *>(buffer[PHYSICS_MAPPING]) };
    auto const *graphics { static_cast<SPageFileGraphic const *>(buffer[GRAPHICS_MAPPING]) };
    auto const *statics  { static_cast<SPageFileStatic const *>(buffer[STATICS_MAPPING]) };

    while(true)
    {
        while(graphics->status != AC_LIVE) { Sleep(500); }

        std::wcout << L"Car Model: " << statics->carModel << std::endl;
        std::wcout << L"Track: " << statics->track << std::endl;

        SessionContext ctx;
        ctx.track = statics->track;
        ctx.car = statics->carModel;
        ctx.session_id = -1;

        int64_t last_packet_id { -1 };
        while(graphics->status == AC_LIVE)
        {
            if(physics->packetId != last_packet_id)
            {
                func(physics, graphics, ctx);
                last_packet_id = physics->packetId;
            }
            Sleep(5);
        }
    }
}

int main()
{
    void *buffer[3];
    setup(buffer);

    update(buffer, output_speed);
    return 0;
}
