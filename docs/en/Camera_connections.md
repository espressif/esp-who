
# Connecting

The table below lists the specific pins used in this example for connecting the ESP32 module and the camera module. 

| Interface | Camera Pin | ESP-EYE |
| :--- | :---: | :---: |
| SCCB Clock | SIOC | IO23 |
| SCCB Data | SIOD | IO18 |
| System Clock | XCLK  | IO4 |
| Vertical Sync | VSYNC | IO5 |
| Horizontal Reference | HREF | IO27 |
| Pixel Clock | PCLK | IO25 |
| Pixel Data Bit 0 | D2 | IO34 |
| Pixel Data Bit 1 | D3 | IO13 |
| Pixel Data Bit 2 | D4 | IO14 |
| Pixel Data Bit 3 | D5 | IO35 |
| Pixel Data Bit 4 | D6 | IO39 |
| Pixel Data Bit 5 | D7 | IO38 |
| Pixel Data Bit 6 | D8 | IO37 |
| Pixel Data Bit 7 | D9 | IO36 |
| Camera Reset | RESET | -1 |
| Camera Power Down | PWDN | -1 |
| Power Supply 3.3V | 3V3 | 3v3 |
| Ground | GND | GND |

> The pin mapping will be slightly different if you use other ESP32 modules. 

