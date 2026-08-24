#include <iostream>

#include "../include/ac_shared_memory.h"

#define PHYSICS_MAPPING  0
#define GRAPHICS_MAPPING 1
#define STATICS_MAPPING  2

void output_speed(SPageFilePhysics const *physics, SPageFileGraphic const *graphics)
{
    printf("speed=%.2f rpms=%d\n", physics->speedKmh, physics->rpms);
}

void setup(void **buffer)
{
    HANDLE ac_phys_mapping_handle     = OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_physics");
    HANDLE ac_graphics_mapping_handle = OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_graphics");
    HANDLE ac_static_mapping_handle   = OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_static");

    buffer[PHYSICS_MAPPING]  = MapViewOfFile(ac_phys_mapping_handle, FILE_MAP_READ, 0, 0, 0),
    buffer[GRAPHICS_MAPPING] = MapViewOfFile(ac_graphics_mapping_handle, FILE_MAP_READ, 0, 0, 0),
    buffer[STATICS_MAPPING]  = MapViewOfFile(ac_static_mapping_handle, FILE_MAP_READ, 0, 0, 0),

    CloseHandle(ac_phys_mapping_handle);
    CloseHandle(ac_graphics_mapping_handle);
    CloseHandle(ac_static_mapping_handle);
}

void update(
    void **buffer,
    void (*func)(SPageFilePhysics const *, SPageFileGraphic const *)
)
{
    auto const *physics     = static_cast<SPageFilePhysics const *>(buffer[PHYSICS_MAPPING]);
    auto const *graphics    = static_cast<SPageFileGraphic const *>(buffer[GRAPHICS_MAPPING]);

    long last_packet_id = -1;
    while(1)
    {
        if(physics->packetId != last_packet_id)
        {
            func(physics, graphics);
            last_packet_id = physics->packetId;
        }
        Sleep(5);
    }
}

int main()
{
    void *buffer[3];
    setup(buffer);
    auto const *statics = static_cast<SPageFileStatic const *>(buffer[STATICS_MAPPING]);

    std::wcout << L"SM Version >> " << statics->smVersion << std::endl;
    std::wcout << L"AC Version >> " << statics->acVersion << std::endl;
    
    printf("=================================\n");

    std::wcout << L"Car Model: " << statics->carModel << std::endl;
    std::wcout << L"Track: " << statics->track << std::endl;
    std::wcout << L"Player Name: " << statics->playerName << std::endl;

    printf("=================================\n");
    printf("++ Polled speed and rpms ++\n\n");
    
    update(buffer, output_speed);
    return 0;
}
