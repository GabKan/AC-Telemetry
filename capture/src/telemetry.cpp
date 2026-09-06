#include "../include/telemetry.h"

void buffer_telemetry(const SPageFilePhysics *physics, const SPageFileGraphic *, SessionContext *ctx)
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

    ctx->pending_samples.push_back(sample);
}
