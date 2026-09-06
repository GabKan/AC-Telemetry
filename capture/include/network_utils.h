#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include "../include/httplib.h"

bool wait_for_backend(httplib::Client &cli, int max_attempts = 10);

#endif
