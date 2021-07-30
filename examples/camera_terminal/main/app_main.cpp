#include "app_camera.hpp"
#include "app_dl.h"

extern "C" void app_main()
{
    app_camera_init(CAMERA_PIXEL_FORMAT, FRAMESIZE_QVGA, 2);
    app_dl_init();
}
