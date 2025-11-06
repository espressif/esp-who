#pragma once
#include "esp_err.h"

namespace who {
namespace cam {
class WhoCam;
}
}

esp_err_t start_http_server(who::cam::WhoCam *cam);
esp_err_t stop_http_server(void);