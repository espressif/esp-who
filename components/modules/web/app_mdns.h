#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

    void app_mdns_main();
    void app_mdns_update_framesize(int size);
    const char *app_mdns_query(size_t *out_len);

#ifdef __cplusplus
}
#endif
