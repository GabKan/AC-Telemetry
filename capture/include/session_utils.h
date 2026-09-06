#ifndef SESSION_UTILS_H
#define SESSION_UTILS_H

#include "../include/httplib.h"
#include "../include/session_context.h"

std::string session_type_to_string(int type);
SessionContext *create_session(httplib::Client *cli, int type, std::wstring track, std::wstring carModel);
void send_batch(httplib::Client *cli, SessionContext *ctx);

#endif
