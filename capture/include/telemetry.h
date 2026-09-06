#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "../include/ac_shared_memory.h"
#include "../include/session_context.h"

void buffer_telemetry(const SPageFilePhysics *physics, const SPageFileGraphic *graphics, SessionContext *ctx);

#endif
