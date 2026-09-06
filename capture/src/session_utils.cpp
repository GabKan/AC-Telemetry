#include "../include/session_utils.h"
#include "../include/logging.h"
#include "../include/string_utils.h"
#include "../include/time_utils.h"
#include "../include/ac_shared_memory.h"
#include "../include/json.h"

using json = nlohmann::json;

std::string session_type_to_string(int type)
{
    switch (type)
    {
        case AC_PRACTICE:    return "practice";
        case AC_QUALIFY:     return "qualify";
        case AC_RACE:        return "race";
        case AC_HOTLAP:      return "hotlap";
        case AC_TIME_ATTACK: return "time_attack";
        case AC_DRIFT:       return "drift";
        case AC_DRAG:        return "drag";
        default:             return "unknown";
    }
}

SessionContext *create_session(
    httplib::Client *cli,
    int type,
    std::wstring track,
    std::wstring carModel
)
{
    SessionContext *ctx = new SessionContext();
    ctx->session_type   = type;
    ctx->track          = track;
    ctx->car            = carModel;
    ctx->started_at     = get_iso_timestamp();

    log_line(LogLevel::LOG_INFO, "Creating session: type=", session_type_to_string(type));

    json body = {
        {"session_type", session_type_to_string(ctx->session_type)},
        {"track", wstring_to_utf8(ctx->track)},
        {"car", wstring_to_utf8(ctx->car)},
        {"started_at", ctx->started_at},
    };

    auto res = cli->Post("/sessions", body.dump(), "application/json");
    if(!res)
    {
        log_line(LogLevel::LOG_ERROR, "POST /sessions failed, no response. Reason: ", httplib::to_string(res.error()));
        delete ctx;
        return nullptr;
    }
    if(res->status != 200)
    {
        log_line(LogLevel::LOG_ERROR, "POST /sessions returned status ", res->status, ". Body: ", res->body);
        delete ctx;
        return nullptr;
    }

    try
    {
        json parsed = json::parse(res->body);
        ctx->session_id = parsed["session_id"].get<uint32_t>();
        log_line(LogLevel::LOG_INFO, "Session created. session_id=", ctx->session_id);
    }
    catch(const json::exception &e)
    {
        log_line(LogLevel::LOG_ERROR, "Failed to parse session response: ", e.what(), " body=", res->body);
        delete ctx;
        return nullptr;
    }

    return ctx;
}

void send_batch(httplib::Client *cli, SessionContext *ctx)
{
    // if (ctx->pending_samples.empty())
    //     return;
    //
    // json body = {
    //     {"session_id", ctx->session_id},
    //     {"samples", ctx->pending_samples}
    // };
    //
    // auto res = cli->Post("/telemetry/batch", body.dump(), "application/json");
    //
    // if (!res)
    // {
    //     log_line(LogLevel::LOG_ERROR, "Batch POST failed, no response. Reason: ", httplib::to_string(res.error()),
    //              ". Dropping ", ctx->pending_samples.size(), " samples.");
    // }
    // else if (res->status != 200)
    // {
    //     log_line(LogLevel::LOG_ERROR, "Batch POST returned status ", res->status,
    //              ". Dropping ", ctx->pending_samples.size(), " samples.");
    // }
    // else
    // {
    //     log_line(LogLevel::LOG_INFO, "Flushed ", ctx->pending_samples.size(), " samples.");
    // }
    //
    // ctx->pending_samples.clear();
}
