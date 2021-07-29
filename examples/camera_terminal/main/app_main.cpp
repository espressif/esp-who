#include "app_camera.hpp"
#include "app_dl.h"

extern "C" void app_main()
{
    app_camera_init(FRAMESIZE_QVGA, 12, 2);
    app_dl_init();
}
