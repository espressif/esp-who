# Code recognition Example

This example demonstrates how to decode 1D/2D barcode. Currectly, the formats that can be decoded by [esp_code_scanner](./../../components/esp-code-scanner) are:
- 1D barcode:
	- code39
	- code128
- 2D barcode:
	- QR code

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-S3 SoC (e.g., ESP-EYE, ESP-WROVER-KIT, ESPS3-EYE etc.)
* A USB cable for Power supply and programming
* A Camera Module: OV2640/OV3660/GC0308/GC032A image sensor(recommended focal range: 5cm-20cm)
* A LCD(optional): ST7789/...

Note: the default OV2640 camera on the development board may not be suitable for this example.

### Configure the Project

Some default settings have been set based on the development board using `sdkconfig.defaults.<chip_name>`


### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Please make sure the code can be clearly captured.
If a qrcode can be succesfully decoded, you will be able to see information displayed as below:

```
I (11164) APP_CODE_SCANNER: Decode time in 70 ms.
I (11164) APP_CODE_SCANNER: Decoded QR-Code symbol "﻿测试"
```


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-who/issues) on GitHub. We will get back to you soon.