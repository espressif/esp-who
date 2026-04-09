# Object Tracking Example

This example demonstrates a 2-axis gimbal object tracking system using a USB camera and servo motors.

## Troubleshooting

If you encounter the error `uvc: Failed to get frame list for uvc_stream_index`, modify the ESP-IDF source file:

```
components/usb/include/usb/usb_types_ch9.h:387
```

Change:
```c
#define USB_IAD_DESC_SIZE    9
```

To:
```c
#define USB_IAD_DESC_SIZE    8
```

## Hardware Support

This example supports servo motors such as SG90, MG90S, MG996R, etc. You may need to adjust PID parameters and other settings based on your specific servo.

## Camera Configuration

This example uses a USB camera (UVC). You may need to configure the following parameters based on your camera:
- Capture width, height, and FPS
- Camera intrinsic parameters (fx, fy)

## Parameter Configuration

All configurable parameters can be adjusted via Kconfig. See `main/Kconfig.projbuild` for available options:

- **Pan/Tilt Servo**: GPIO, direction, PID parameters (Kp, Ki, Kd), initial angle, angle range
- **Camera**: Capture resolution, FPS, focal length (fx, fy)
