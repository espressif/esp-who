| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |


# Human Face Detect LVGL Example

### Personalized configuration

You can change these options in menuconfig.

1. USE_PPA_CAM  
    Use pixel-processing accelerator (PPA) to asynchronously resize the camera frame.
    Hardware resize instead of software resize can reduce the cpu load or provide higher
    fps but it will use more psram to store the resized frame.