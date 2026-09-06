#include "../include/shared_memory_utils.h"
#include "../include/logging.h"

#include <windows.h>

static bool try_map_shared_memory(void **buffer)
{
    HANDLE ac_phys_mapping_handle     { OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_physics") };
    HANDLE ac_graphics_mapping_handle { OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_graphics") };
    HANDLE ac_static_mapping_handle   { OpenFileMapping(FILE_MAP_READ, FALSE, "acpmf_static") };

    if (!ac_phys_mapping_handle || !ac_graphics_mapping_handle || !ac_static_mapping_handle)
    {
        if (ac_phys_mapping_handle)     CloseHandle(ac_phys_mapping_handle);
        if (ac_graphics_mapping_handle) CloseHandle(ac_graphics_mapping_handle);
        if (ac_static_mapping_handle)   CloseHandle(ac_static_mapping_handle);
        return false;
    }

    buffer[PHYSICS_MAPPING]  = MapViewOfFile(ac_phys_mapping_handle, FILE_MAP_READ, 0, 0, 0);
    buffer[GRAPHICS_MAPPING] = MapViewOfFile(ac_graphics_mapping_handle, FILE_MAP_READ, 0, 0, 0);
    buffer[STATICS_MAPPING]  = MapViewOfFile(ac_static_mapping_handle, FILE_MAP_READ, 0, 0, 0);

    CloseHandle(ac_phys_mapping_handle);
    CloseHandle(ac_graphics_mapping_handle);
    CloseHandle(ac_static_mapping_handle);

    return buffer[PHYSICS_MAPPING] && buffer[GRAPHICS_MAPPING] && buffer[STATICS_MAPPING];
}

void setup(void **buffer)
{
    int attempt = 0;
    while (!try_map_shared_memory(buffer))
    {
        attempt++;
        log_line(LogLevel::LOG_WARN, "Shared memory not available (attempt ", attempt,
                 "). Waiting for an Assetto Corsa session to go live. Retrying in 2 seconds.");
        Sleep(2000);
    }

    log_line(LogLevel::LOG_INFO, "Shared memory mapped: physics=", buffer[PHYSICS_MAPPING],
             " graphics=", buffer[GRAPHICS_MAPPING], " static=", buffer[STATICS_MAPPING]);
}
